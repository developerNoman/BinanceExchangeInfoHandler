#include "benchmark/benchmark.h"
#include "processData.h"
#include "spdlog/spdlog.h"
#include "rapidjson/document.h"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <map>

using namespace std;

void readQueryFile(const string &queryFile, rapidjson::Document &doc1);
void fetchData(const std::string &baseUrl, const std::string &endpoint, net::io_context &ioc, ssl::context &ctx);

//BenchMark to read query file
static void BenchMark_ReadQuery(benchmark::State& state) {
    rapidjson::Document doc;
    std::string queryFile = "query.json";
    for (auto _ : state) {
        readQueryFile(queryFile, doc);
    }
}
BENCHMARK(BenchMark_ReadQuery);


//BenchMark to read config file
static void BenchMark_ReadConfig(benchmark::State& state) {
    rapidjson::Document doc;
    std::string configFile = "config.json";
    for (auto _ : state) {
        readConfig(configFile, doc);
    }
}
BENCHMARK(BenchMark_ReadConfig);

//benchmark for fetchdata
static void BenchMark_FetchData(benchmark::State& state) {
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx{ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);
    for (auto _ : state) {
        fetchData(spotBase, spotTarget, ioc, ctx);
        ioc.run();
    }
}
BENCHMARK(BenchMark_FetchData);


BENCHMARK_MAIN();
