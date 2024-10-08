
#include "fetchData.h"
#include <chrono>
#include "rapidjson/filereadstream.h"
#include "spdlog/spdlog.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "boost/asio/strand.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind/bind.hpp"
#include <cstdio>
#include <fstream>
#include <memory>
#include <map>
#include <iostream>
#include <string>

using namespace std;

void exchangeSymbols::setSpotSymbol(const std::string &symbol, const MarketInfo &info)
{
    spotSymbols_[symbol] = info;
}

void exchangeSymbols::setUsdSymbol(const std::string &symbol, const MarketInfo &info)
{
    usdSymbols_[symbol] = info;
}

void exchangeSymbols::setCoinSymbol(const std::string &symbol, const MarketInfo &info)
{
    coinSymbols_[symbol] = info;
}
exchangeSymbols exchangeData;

string spotBase, usdtFutureBase, coinFutureBase;

void processEndpoints::parseSymbols(string &responseBody, const string &base, exchangeSymbols &exchangeData)
{
    if (responseBody.empty())
    {
        spdlog::error("Error: The response body is empty");
        return;
    }

    rapidjson::Document doc;
    rapidjson::ParseResult parseResult = doc.Parse(responseBody.c_str());

    if (!parseResult)
    {
        spdlog::error("JSON parse error: {} (offset {})", rapidjson::GetParseError_En(parseResult.Code()), parseResult.Offset());

        spdlog::debug("Response Body: {}", responseBody);
        return;
    }

    if (!doc.IsObject() || !doc.HasMember("symbols") || !doc["symbols"].IsArray())
    {
        spdlog::error("Invalid JSON format or missing 'symbols' array.");
        spdlog::debug("Response Body: {}", responseBody);
        return;
    }

    const auto &symbolsArray = doc["symbols"];
    for (const auto &symbol : symbolsArray.GetArray())
    {
        MarketInfo info;
        if (symbol.HasMember("symbol") && symbol["symbol"].IsString())
        {
            info.symbol = symbol["symbol"].GetString();
        }
        else
        {
            spdlog::error("Missing or invalid symbol field in a symbol object.");
            continue;
        }
        if (symbol.HasMember("quoteAsset"))
        {
            if (symbol["quoteAsset"].IsString())
            {
                info.quoteAsset = symbol["quoteAsset"].GetString();
            }
            else
            {
                info.quoteAsset = ""; // Handle unexpected type by setting to empty string
                spdlog::error("Invalid 'quoteAsset' field in symbol: {}", info.symbol);
            }
        }
        else
        {
            info.quoteAsset = ""; // Handle missing field
        }
        if (symbol.HasMember("status") && symbol["status"].IsString())
        {
            info.status = symbol["status"].GetString();
        }
        else if (symbol.HasMember("contractStatus") && symbol["contractStatus"].IsString())
        {
            info.status = symbol["contractStatus"].GetString();
        }
        else
        {
            spdlog::error("Missing 'status' or 'contractStatus' in symbol:{} ", info.symbol);
        }

        if (symbol.HasMember("filters") && symbol["filters"].IsArray())
        {
            for (const auto &filter : symbol["filters"].GetArray())
            {
                if (filter.HasMember("filterType") && filter["filterType"].IsString())
                {
                    string filterType = filter["filterType"].GetString();
                    if (filterType == "PRICE_FILTER")
                    {
                        info.tickSize = filter.HasMember("tickSize") && filter["tickSize"].IsString() ? filter["tickSize"].GetString() : "";
                    }
                    else if (filterType == "LOT_SIZE")
                    {
                        info.stepSize = filter.HasMember("stepSize") && filter["stepSize"].IsString() ? filter["stepSize"].GetString() : "";
                    }
                }
            }
        }
        else
        {
            spdlog::error("Missing or invalid 'filters' array in symbol:{}", info.symbol);
        }
        if (base == spotBase)
        {
            exchangeData.setSpotSymbol(info.symbol, info);
        }
        else if (base == usdtFutureBase)
        {
            exchangeData.setUsdSymbol(info.symbol, info);
        }
        else if (base == coinFutureBase)
        {
            exchangeData.setCoinSymbol(info.symbol, info);
        }
    }
}

void session::fail(beast::error_code ec, char const *what)
{
    cerr << what << ": " << ec.message() << endl;
}

void session::run(const char *host, const char *port, const char *target, int version)
{
    host_ = host;

    if (!SSL_set_tlsext_host_name(stream_.native_handle(), host))
    {
        beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
        spdlog::error("{}", ec.message());
        return;
    }

    req_.version(version);
    req_.method(http::verb::get);
    req_.target(target);
    req_.set(http::field::host, host);
    req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    resolver_.async_resolve(
        host,
        port,
        beast::bind_front_handler(
            &session::onResolve,
            shared_from_this()));
}

void session::onResolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
    {
        return fail(ec, "resolve");
    }
    beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));

    beast::get_lowest_layer(stream_).async_connect(
        results,
        beast::bind_front_handler(
            &session::onConnect,
            shared_from_this()));
}

void session::onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type)
{
    if (ec)
    {
        return fail(ec, "connect");
    }
    beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));

    stream_.async_handshake(
        ssl::stream_base::client,
        beast::bind_front_handler(
            &session::onHandshake,
            shared_from_this()));
}

void session::onHandshake(beast::error_code ec)
{
    if (ec)
    {
        return fail(ec, "handshake");
    }
    beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));

    http::async_write(stream_, req_,
                      beast::bind_front_handler(
                          &session::onWrite,
                          shared_from_this()));
}

void session::onWrite(beast::error_code ec, size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        return fail(ec, "write");
    }
    http::async_read(stream_, buffer_, res_,
                     beast::bind_front_handler(
                         &session::onRead,
                         shared_from_this()));
}

void session::onRead(beast::error_code ec, size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        return fail(ec, "read");
    }

    auto responseBody = res_.body();
    processEndpoints pe;

    pe.parseSymbols(responseBody, host_, exchangeData);

    beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));

    stream_.async_shutdown(
        beast::bind_front_handler(
            &session::onShutdown,
            shared_from_this()));
}

void session::onShutdown(beast::error_code ec)
{
    if (ec == net::error::eof)
    {
        ec.assign(0, ec.category());
    }
    if (ec)
    {
        return fail(ec, "shutdown");
    }
}
bool logToFile, logToConsole;

string logLevel, spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;

// Read the config.json file and store values in variables
void processEndpoints::readConfig(const string &configFile, rapidjson::Document &doc, utils &utilsInfo)
{
    FILE *fp = fopen(configFile.c_str(), "r");
    if (!fp)
    {
        spdlog::error("Error: unable to open config file.");
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc.ParseStream(is);

    logToFile = doc["logging"]["file"].GetBool();
    logToConsole = doc["logging"]["console"].GetBool();
    logLevel = doc["logging"]["level"].GetString();
    spotBase = doc["exchange_base_url"]["spotBase"].GetString();
    usdtFutureBase = doc["exchange_base_url"]["usdtFutureBase"].GetString();
    coinFutureBase = doc["exchange_base_url"]["coinFutureBase"].GetString();
    spotTarget = doc["exchange_endpoints"]["spotTarget"].GetString();
    usdtFutureTarget = doc["exchange_endpoints"]["usdtFutureTarget"].GetString();
    coinFutureTarget = doc["exchange_endpoints"]["coinFutureTarget"].GetString();
    request_interval = doc["request_interval"].GetInt();

    utilsInfo.logToFile = doc["logging"]["file"].GetBool();
    utilsInfo.logToConsole = doc["logging"]["console"].GetBool();
    utilsInfo.logLevel = doc["logging"]["level"].GetString();
    utilsInfo.spotBase = doc["exchange_base_url"]["spotBase"].GetString();
    utilsInfo.usdtFutureBase = doc["exchange_base_url"]["usdtFutureBase"].GetString();
    utilsInfo.coinFutureBase = doc["exchange_base_url"]["coinFutureBase"].GetString();
    utilsInfo.spotTarget = doc["exchange_endpoints"]["spotTarget"].GetString();
    utilsInfo.usdtFutureTarget = doc["exchange_endpoints"]["usdtFutureTarget"].GetString();
    utilsInfo.coinFutureTarget = doc["exchange_endpoints"]["coinFutureTarget"].GetString();
    utilsInfo.request_interval = doc["request_interval"].GetInt();

    fclose(fp);
}

// Function to fetch data from the API
void processEndpoints::fetchData(const string &baseUrl, const string &endpoint, net::io_context &ioc, ssl::context &ctx)
{
    spdlog::info("Fetching data from baseUrl: {}, endpoint: {}", baseUrl, endpoint);
    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(baseUrl.c_str(), "443", endpoint.c_str(), 11);
}

// Function to handle spot data
void processEndpoints::fetchSpotData(net::io_context &ioc, ssl::context &ctx)
{
    fetchData(spotBase, spotTarget, ioc, ctx);
}

// Function to handle USD future data
void processEndpoints::fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx)
{
    fetchData(usdtFutureBase, usdtFutureTarget, ioc, ctx);
}

// Function to handle coin future data
void processEndpoints::fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx)
{
    fetchData(coinFutureBase, coinFutureTarget, ioc, ctx);
}

// Callback function for the timer
void processEndpoints::fetchEndpoints(const boost::system::error_code &, boost::asio::steady_timer *fetchTimer, boost::asio::io_context &ioc, ssl::context &ctx)
{
    fetchSpotData(ioc, ctx);
    fetchUsdFutureData(ioc, ctx);
    fetchCoinFutureData(ioc, ctx);
    processEndpoints pe;
    fetchTimer->expires_at(fetchTimer->expiry() + boost::asio::chrono::seconds(request_interval));
    fetchTimer->async_wait(boost::bind(&processEndpoints::fetchEndpoints, &pe, boost::asio::placeholders::error, fetchTimer, ref(ioc), ref(ctx)));
}
