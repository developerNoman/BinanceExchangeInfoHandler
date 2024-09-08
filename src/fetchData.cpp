
#include "fetchData.h"

using namespace std;

exchangeSymbols exchangeData;
bool logToFile, logToConsole;
string logLevel, spotBase, usdtFutureBase, coinFutureBase;
string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;

void parseSymbols(string &responseBody, const string &base, exchangeSymbols &exchangeData)
{
    if (responseBody.empty())
    {
        cerr << "Error: The response body is empty." << endl;
        return;
    }

    rapidjson::Document fullData;
    rapidjson::ParseResult parseResult = fullData.Parse(responseBody.c_str());

    if (!parseResult)
    {
        cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code())
             << " (offset " << parseResult.Offset() << ")" << endl;
        cerr << "Response Body: " << responseBody << endl;
        return;
    }

    if (!fullData.IsObject() || !fullData.HasMember("symbols") || !fullData["symbols"].IsArray())
    {
        cerr << "Invalid JSON format or missing 'symbols' array." << endl;
        cerr << "Response Body: " << responseBody << endl;
        return;
    }

    const auto &symbolsArray = fullData["symbols"];
    for (const auto &symbol : symbolsArray.GetArray())
    {
        MarketInfo info;
        if (symbol.HasMember("symbol") && symbol["symbol"].IsString())
        {
            info.symbol = symbol["symbol"].GetString();
        }
        else
        {
            cerr << "Missing or invalid 'symbol' field in a symbol object." << endl;
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
                cerr << "Invalid 'quoteAsset' field in symbol: " << info.symbol << endl;
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
            cerr << "Missing 'status' or 'contractStatus' in symbol: " << info.symbol << endl;
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
            cerr << "Missing or invalid 'filters' array in symbol: " << info.symbol << endl;
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

void fail(beast::error_code ec, char const *what)
{
    cerr << what << ": " << ec.message() << endl;
}

void session::run(const char *host, const char *port, const char *target, int version)
{
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
    parseSymbols(responseBody, host_, exchangeData);

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
        return fail(ec, "shutdown");
}

// Read the config.json file and store values in variables
void readConfig(const string &configFile, rapidjson::Document &doc)
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

    fclose(fp);
}

// Function to fetch data from the API
void fetchData(const string &baseUrl, const string &endpoint, net::io_context &ioc, ssl::context &ctx)
{
    spdlog::info("Fetching data from baseUrl: {}, endpoint: {}", baseUrl, endpoint);
    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(baseUrl.c_str(), "443", endpoint.c_str(), 11);
}

// Function to handle spot data
void fetchSpotData(net::io_context &ioc, ssl::context &ctx)
{
    fetchData(spotBase, spotTarget, ioc, ctx);
}

// Function to handle USD future data
void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx)
{
    fetchData(usdtFutureBase, usdtFutureTarget, ioc, ctx);
}

// Function to handle coin future data
void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx)
{
    fetchData(coinFutureBase, coinFutureTarget, ioc, ctx);
}

// Callback function for the timer
void fetchEndpoints(const boost::system::error_code &, boost::asio::steady_timer *fetchTimer, boost::asio::io_context &ioc, ssl::context &ctx)
{
    fetchSpotData(ioc, ctx);
    fetchUsdFutureData(ioc, ctx);
    fetchCoinFutureData(ioc, ctx);
    fetchTimer->expires_at(fetchTimer->expiry() + boost::asio::chrono::seconds(request_interval));
    fetchTimer->async_wait(boost::bind(fetchEndpoints, boost::asio::placeholders::error, fetchTimer, ref(ioc), ref(ctx)));
}
