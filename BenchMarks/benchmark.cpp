#include <benchmark/benchmark.h>
#include "request.h"
#include <spdlog/spdlog.h>
#include <rapidjson/document.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <fstream>
#include <thread>
#include <chrono>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

string spotBase, usdtFutureBase, coinFutureBase;
string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;

void fail(boost::system::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}


void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx)
{
    auto const host = baseUrl.c_str();
    auto const port = "443";
    auto const target = endpoint.c_str();
    int version = 11;

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> exchangeData = sessionPtr->returnResponse();
    parseSymbols(exchangeData.body(), symbolsMap);
}

void readConfig(string configFile, rapidjson::Document &doc1) {

    FILE* fp = fopen(configFile.c_str(), "r");
    if (!fp) {
        cerr << "Error: unable to open file" << endl;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc1.ParseStream(is);

    spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
    usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
    coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
    spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
    usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
    coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
    request_interval = doc1["request_interval"].GetInt();
    
    fclose(fp);
}

//function to test benchmark for the fetch data
static void BM_FetchData(benchmark::State& state) {
    std::string baseUrl = "api.binance.com";
    std::string endpoint = "/api/v3/exchangeInfo";
    std::map<std::string, symbolInfo> symbolsMap;
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

    for (auto _ : state) {
        fetchData(baseUrl, endpoint, symbolsMap, ioc, ctx);
    }
}
BENCHMARK(BM_FetchData);

//function to test benchmark for the read data
static void BM_ReadConfig(benchmark::State& state) {
    rapidjson::Document doc;
    std::string configFile = "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json";
    for (auto _ : state) {
        readConfig(configFile, doc);
    }
}
BENCHMARK(BM_ReadConfig);

BENCHMARK_MAIN();
