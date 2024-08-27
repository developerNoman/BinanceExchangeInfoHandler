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

void readQueryFile(const string &queryFile, rapidjson::Document &doc1)
{
    FILE *fp = fopen(queryFile.c_str(), "r");
    if (!fp){
        cerr << "Error: unable to open file" << endl;
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc1.ParseStream(is);
    if (doc1.HasParseError())
    {
        cerr << "Error parsing JSON file!" << endl;
    }

    fclose(fp);
}

//BenchMark to read query file
static void BenchMark_ReadQuery(benchmark::State& state) {
    rapidjson::Document doc;
    std::string queryFile = "query.json";
    for (auto _ : state) {
        readQueryFile(queryFile, doc);
    }
}
BENCHMARK(BenchMark_ReadQuery);

const std::string dummyResponse = R"({
    "symbols": [
        {
            "symbol": "BTCUSDT",
            "quoteAsset": "USDT",
            "status": "TRADING",
            "filters": [
                {"filterType": "PRICE_FILTER", "tickSize": "0.01"},
                {"filterType": "LOT_SIZE", "stepSize": "0.001"}
            ]
        },
        {
            "symbol": "ETHUSDT",
            "quoteAsset": "USDT",
            "status": "TRADING",
            "filters": [
                {"filterType": "PRICE_FILTER", "tickSize": "0.01"},
                {"filterType": "LOT_SIZE", "stepSize": "0.001"}
            ]
        }
    ]
})";




//BenchMark to read config file
static void BenchMark_ReadConfig(benchmark::State& state) {
    rapidjson::Document doc;
    std::string configFile = "config.json";
    for (auto _ : state) {
        readConfig(configFile, doc);
    }
}
BENCHMARK(BenchMark_ReadConfig);

//Benchmark for the parseSymbols function
static void BenchMark_ParseSymbols(benchmark::State& state) {
    std::map<std::string, symbolInfo> symbolsMap;
    for (auto _ : state) {
        parseSymbols(const_cast<std::string&>(dummyResponse), &symbolsMap);
    }
}
BENCHMARK(BenchMark_ParseSymbols);

BENCHMARK_MAIN();
