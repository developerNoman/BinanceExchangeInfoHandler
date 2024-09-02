#include "fetchData.h"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/ssl.hpp"
#include "boost/asio/strand.hpp"
#include "boost/asio/ip/tcp.hpp"

using namespace std;
namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

std::mutex myMutex;
exchangeSymbols exchangeData;
std::string logLevel, spotBase, usdtFutureBase, coinFutureBase;
std::string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;

void parseSymbols(const std::string &responseBody, std::map<std::string, MarketInfo> *symbolsMap) {
    if (responseBody.empty()) {
        std::cerr << "Error: The response body is empty." << std::endl;
        return;
    }

    rapidjson::Document fullData;
    rapidjson::ParseResult parseResult = fullData.Parse(responseBody.c_str());

    if (!parseResult) {
        std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code())
            << " (offset " << parseResult.Offset() << ")" << std::endl;
        std::cerr << "Response Body: " << responseBody << std::endl;
        return;
    }

    if (!fullData.IsObject() || !fullData.HasMember("symbols") || !fullData["symbols"].IsArray()) {
        std::cerr << "Invalid JSON format or missing 'symbols' array." << std::endl;
        std::cerr << "Response Body: " << responseBody << std::endl;
        return;
    }

    const auto &symbolsArray = fullData["symbols"];
    for (const auto &symbol : symbolsArray.GetArray()) {
        MarketInfo info;
        if (symbol.HasMember("symbol") && symbol["symbol"].IsString()) {
            info.symbol = symbol["symbol"].GetString();
        } else {
            std::cerr << "Missing or invalid 'symbol' field in a symbol object." << std::endl;
            continue;
        }
        if (symbol.HasMember("quoteAsset")) {
            if (symbol["quoteAsset"].IsString()) {
                info.quoteAsset = symbol["quoteAsset"].GetString();
            } else {
                info.quoteAsset = ""; // Handle unexpected type by setting to empty string
                std::cerr << "Invalid 'quoteAsset' field in symbol: " << info.symbol << std::endl;
            }
        } else {
            info.quoteAsset = ""; // Handle missing field
        }
        if (symbol.HasMember("status") && symbol["status"].IsString()) {
            info.status = symbol["status"].GetString();
        } else if (symbol.HasMember("contractStatus") && symbol["contractStatus"].IsString()) {
            info.status = symbol["contractStatus"].GetString();
        } else {
            std::cerr << "Missing 'status' or 'contractStatus' in symbol: " << info.symbol << std::endl;
        }

        if (symbol.HasMember("filters") && symbol["filters"].IsArray()) {
            for (const auto &filter : symbol["filters"].GetArray()) {
                if (filter.HasMember("filterType") && filter["filterType"].IsString()) {
                    std::string filterType = filter["filterType"].GetString();
                    if (filterType == "PRICE_FILTER") {
                        info.tickSize = filter.HasMember("tickSize") && filter["tickSize"].IsString() ? filter["tickSize"].GetString() : "";
                    } else if (filterType == "LOT_SIZE") {
                        info.stepSize = filter.HasMember("stepSize") && filter["stepSize"].IsString() ? filter["stepSize"].GetString() : "";
                    }
                }
            }
        } else {
            std::cerr << "Missing or invalid 'filters' array in symbol: " << info.symbol << std::endl;
        }

        (*symbolsMap)[info.symbol] = info;
    }
}

void fail(beast::error_code ec, char const *what) {
    std::cerr << what << ": " << ec.message() << std::endl;
}

void session::run(const char *host, const char *port, const char *target, int version) {
    host_ = host;

    req_.version(version);
    req_.method(http::verb::get);
    req_.target(target);
    req_.set(http::field::host, host);
    req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    resolver_.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &session::on_resolve,
            shared_from_this()));
}

void session::on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
    if (ec) return fail(ec, "resolve");

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    beast::get_lowest_layer(stream_).async_connect(
        results,
        beast::bind_front_handler(
            &session::on_connect,
            shared_from_this()));
}

void session::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
    if (ec) return fail(ec, "connect");

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    stream_.async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &session::on_handshake,
            shared_from_this()));
}

void session::on_handshake(beast::error_code ec) {
    if (ec) return fail(ec, "handshake");

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    http::async_write(stream_, req_,
        beast::bind_front_handler(
            &session::on_write,
            shared_from_this()));
}

void session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) return fail(ec, "write");

    http::async_read(stream_, buffer_, res_,
        beast::bind_front_handler(
            &session::on_read,
            shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) return fail(ec, "read");

    auto responseBody = res_.body();
    if (host_ == spotBase) {
        parseSymbols(responseBody, &exchangeData.spotSymbols);
    } else if (host_ == usdtFutureBase) {
        parseSymbols(responseBody, &exchangeData.usdSymbols);
    } else if (host_ == coinFutureBase) {
        parseSymbols(responseBody, &exchangeData.coinSymbols);
    }

    beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

    stream_.async_shutdown(
        beast::bind_front_handler(
            &session::on_shutdown,
            shared_from_this()));
}

void session::on_shutdown(beast::error_code ec) {
    if (ec == net::error::eof) {
        ec.assign(0, ec.category());
    }
    if (ec) return fail(ec, "shutdown");
}

// Read the config.json file and store values in variables
void readConfig(const std::string &configFile, rapidjson::Document &doc) {
    FILE* fp = fopen(configFile.c_str(), "r");
    if (!fp) {
        spdlog::error("Error: unable to open config file.");
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc.ParseStream(is);

    logLevel = doc["logging"]["level"].GetString();
    spotBase = doc["exchange_base_url"]["spotBase"].GetString();
    usdtFutureBase = doc["exchange_base_url"]["usdtFutureBase"].GetString();
    coinFutureBase = doc["exchange_base_url"]["coinFutureBase"].GetString();
    spotTarget = doc["exchange_endpoints"]["spotTarget"].GetString();
    usdtFutureTarget = doc["exchange_endpoints"]["usdtFutureTarget"].GetString();
    coinFutureTarget = doc["exchange_endpoints"]["coinFutureTarget"].GetString();
    request_interval = doc["request_interval"].GetInt();

    fclose(fp);
}

void fetchData(const std::string &baseUrl, const std::string &endpoint, net::io_context &ioc, ssl::context &ctx) {
    std::make_shared<session>(ioc, ctx, baseUrl, endpoint)->run();
}

void fetchSpotData(net::io_context &ioc, ssl::context &ctx) {
    fetchData(spotBase, spotTarget, ioc, ctx);
}

void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx) {
    fetchData(usdtFutureBase, usdtFutureTarget, ioc, ctx);
}

void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx) {
    fetchData(coinFutureBase, coinFutureTarget, ioc, ctx);
}

void fetchEndpoints(const boost::system::error_code&, boost::asio::steady_timer *t, boost::asio::io_context &ioc, ssl::context &ctx) {
    fetchSpotData(ioc, ctx);
    fetchUsdFutureData(ioc, ctx);
    fetchCoinFutureData(ioc, ctx);
    t->expires_after(std::chrono::seconds(request_interval));
    t->async_wait(boost::asio::bind_executor(ioc.get_executor(), 
        [&ioc, &ctx](const boost::system::error_code& e) {
            fetchEndpoints(e, nullptr, ioc, ctx);
        }));
}
