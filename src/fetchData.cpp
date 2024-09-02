
#include "fetchData.h"

using namespace std;

namespace net = boost::asio;

exchangeSymbols exchangeData;
std::string logLevel, spotBase, usdtFutureBase, coinFutureBase;
std::string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;


void parseSymbols(std::string &responseBody, const std::string &base, exchangeSymbols &exchangeData) {
    if (responseBody.empty()) {
        cerr << "Error: The response body is empty." << endl;
        return;
    }

    rapidjson::Document fullData;
    rapidjson::ParseResult parseResult = fullData.Parse(responseBody.c_str());

    if (!parseResult) {
        cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code())
             << " (offset " << parseResult.Offset() << ")" << endl;
        cerr << "Response Body: " << responseBody << endl;
        return;
    }

    if (!fullData.IsObject() || !fullData.HasMember("symbols") || !fullData["symbols"].IsArray()) {
        cerr << "Invalid JSON format or missing 'symbols' array." << endl;
        cerr << "Response Body: " << responseBody << endl;
        return;
    }

    const auto &symbolsArray = fullData["symbols"];
    for (const auto &symbol : symbolsArray.GetArray()) {
        MarketInfo info;
        if (symbol.HasMember("symbol") && symbol["symbol"].IsString()) {
            info.symbol = symbol["symbol"].GetString();
        } else {
            cerr << "Missing or invalid 'symbol' field in a symbol object." << endl;
            continue;
        }
        if (symbol.HasMember("quoteAsset")) {
            if (symbol["quoteAsset"].IsString()) {
                info.quoteAsset = symbol["quoteAsset"].GetString();
            } else {
                info.quoteAsset = ""; 
                cerr << "Invalid 'quoteAsset' field in symbol: " << info.symbol << endl;
            }
        } else {
            info.quoteAsset = "";
        }
        if (symbol.HasMember("status") && symbol["status"].IsString()) {
            info.status = symbol["status"].GetString();
        } else if (symbol.HasMember("contractStatus") && symbol["contractStatus"].IsString()) {
            info.status = symbol["contractStatus"].GetString();
        } else {
            cerr << "Missing 'status' or 'contractStatus' in symbol: " << info.symbol << endl;
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
            cerr << "Missing or invalid 'filters' array in symbol: " << info.symbol << endl;
        }
        if (base == spotBase) {
            exchangeData.setSpotSymbol(info.symbol, info);
        } else if (base == usdtFutureBase) {
            exchangeData.setUsdSymbol(info.symbol, info);
        } else if (base == coinFutureBase) {
            exchangeData.setCoinSymbol(info.symbol, info);
        }
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
    parseSymbols(responseBody, host_, exchangeData);

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

// Function to fetch data from the API
void fetchData(const std::string &baseUrl, const std::string &endpoint, net::io_context &ioc, ssl::context &ctx) {
    spdlog::info("Fetching data from baseUrl: {}, endpoint: {}", baseUrl, endpoint);
    auto sessionPtr = std::make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(baseUrl.c_str(), "443", endpoint.c_str(), 11);
}

// Function to handle spot data
void fetchSpotData(net::io_context &ioc, ssl::context &ctx) {
    fetchData(spotBase, spotTarget, ioc, ctx);
}

// Function to handle USD future data
void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx) {
    fetchData(usdtFutureBase, usdtFutureTarget, ioc, ctx);
}

// Function to handle coin future data
void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx) {
    fetchData(coinFutureBase, coinFutureTarget, ioc, ctx);
}

// Callback function for the timer
void fetchEndpoints(const boost::system::error_code&, boost::asio::steady_timer *t, boost::asio::io_context &ioc, ssl::context &ctx) {
    fetchSpotData(ioc, ctx);
    fetchUsdFutureData(ioc, ctx);
    fetchCoinFutureData(ioc, ctx);
    t->expires_at(t->expiry() + boost::asio::chrono::seconds(request_interval));
    t->async_wait(boost::bind(fetchEndpoints, boost::asio::placeholders::error, t, std::ref(ioc), std::ref(ctx)));
}
