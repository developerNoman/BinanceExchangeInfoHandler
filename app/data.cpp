#include "request.h"
#include <set>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "boost/asio.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind/bind.hpp"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "spdlog/spdlog.h"         
#include "rapidjson/document.h"    
#include "rapidjson/writer.h"      
#include "rapidjson/stringbuffer.h" 
#include "rapidjson/istreamwrapper.h" 
#include "rapidjson/ostreamwrapper.h" 
#include "spdlog/spdlog.h" 
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ostream_sink.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
namespace net = boost::asio;

mutex myMutex;

set<int> processedIds;
void fetchData(const string &baseUrl, const string &endpoint, map<string, MarketInfo> &symbolsMap, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);

string logLevel;
string spotBase, usdtFutureBase, coinFutureBase;
string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;

map<string, MarketInfo> symbolsMap;
//object of class is created
exchangeSymbols exchangeData;

//method to read the config.json file and store in variables
void readConfig(string configFile, rapidjson::Document &doc1) {

    FILE* fp = fopen(configFile.c_str(), "r");
    if (!fp) {
        cerr << "Error: unable to open file" << endl;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc1.ParseStream(is);

    logLevel=doc1["logging"]["level"].GetString();
    spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
    usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
    coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
    spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
    usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
    coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
    request_interval = doc1["request_interval"].GetInt();
    
    fclose(fp);
}

//for passing and storing the spot endpoint data in the map 
void spotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
    
    fetchData(spotBaseUrl, spotEndpoint, symbolsMap, ioc, ctx);
}

//for passing and storing the usd Future endpoint data in the map
void usdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
    fetchData(usdFutureBaseUrl, usdFutureEndpoint, symbolsMap, ioc, ctx);
}

//for passing and storing the coin future endpoint data in the map 
void coinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
    fetchData(coinFutureBaseUrl, coinFutureEndpoint, symbolsMap, ioc, ctx);
}

//function to display the extracted data from market
void display(const string &marketType, const string &instrumentName, const MarketInfo &MarketInfo) {
    spdlog::info("{} Market - Symbol: {}", marketType, MarketInfo.symbol);
    spdlog::info("Quote Asset: {}", MarketInfo.quoteAsset);
    spdlog::info("Status: {}", MarketInfo.status);
    spdlog::info("Tick Size: {}", MarketInfo.tickSize);
    spdlog::info("Step Size: {}", MarketInfo.stepSize);

}


void fail(beast::error_code ec, char const *what)
{
    cerr << what << ": " << ec.message() << "\n";
}

//function to make http calls by providing the host and target points
void fetchData(const string &baseUrl, const string &endpoint, map<string, MarketInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx)
{
    spdlog::info("Fetching data from baseUrl: {}, endpoint: {}", baseUrl, endpoint);
    auto const host = baseUrl.c_str();
    auto const port = "443";
    auto const target = endpoint.c_str();
    int version = 11;

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);
    }

//map to store previous ids    
map<string, int> lastSeenIds;

//method to process the queries from the query file
void processQueries(const rapidjson::Document &doc)
{
    if (!doc.HasMember("query") || !doc["query"].IsArray()) {
        spdlog::error("Invalid JSON format in query file!");
        return;
    }

    // Created a new document to store the results
    rapidjson::Document resultDoc;
    resultDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();

    rapidjson::Value resultArray(rapidjson::kArrayType);

    // Try to open and parse the existing file to append data
    ifstream ifs("answers.json");
    if (ifs.is_open()) {
        rapidjson::IStreamWrapper isw(ifs);
        resultDoc.ParseStream(isw);
        if (resultDoc.HasMember("results") && resultDoc["results"].IsArray()) {
            resultArray = resultDoc["results"];
        }
        ifs.close();
    }

    const rapidjson::Value &queries = doc["query"];

    int length=queries.Size();
    myMutex.lock();
    for (int i = 0; i < length; ++i)
    {
        const rapidjson::Value &query = queries[i];

        if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name"))
        {
            spdlog::warn("Missing query details in query index: {}", i);
            continue;
        }

        int queryID = query["id"].GetInt();
        string marketType = query["market_type"].GetString();
        spdlog::info("Processing query ID: {}, Market Type: {}", queryID, marketType);

        if (lastSeenIds[marketType] != queryID) {
            lastSeenIds[marketType] = queryID;  

            string queryType = query["query_type"].GetString();
            string instrumentName = query["instrument_name"].GetString();

            rapidjson::Value resultObj(rapidjson::kObjectType);
            resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
            resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

            // Process the queries based on their type
                     if (queryType == "GET") {
                if (marketType == "SPOT") {
                    auto data = exchangeData.spotSymbols.find(instrumentName);
                    if (data != exchangeData.spotSymbols.end()) {
                        display("SPOT", instrumentName, data->second);
                    } else {
                        spdlog::warn("SPOT symbol {} not found", instrumentName);
                    }
                } else if (marketType == "usd_futures") {
                    auto data = exchangeData.usdSymbols.find(instrumentName);
                    if (data != exchangeData.usdSymbols.end()) {
                        display("USD Futures", instrumentName, data->second);
                    } else {
                        spdlog::warn("USD Futures symbol {} not found", instrumentName);
                    }
                } else if (marketType == "coin_futures") {
                    auto data = exchangeData.coinSymbols.find(instrumentName);
                    if (data != exchangeData.coinSymbols.end()) {
                        display("Coin Futures", instrumentName, data->second);
                    } else {
                        spdlog::warn("Coin Futures symbol {} not found", instrumentName);
                    }
                }
            } else if (queryType == "UPDATE") {
                string newStatus = query["data"]["status"].GetString();
                if (marketType == "SPOT") {
                    auto data = exchangeData.spotSymbols.find(instrumentName);
                    if (data != exchangeData.spotSymbols.end()) {
                        auto &MarketInfo = data->second;
                        MarketInfo.status = newStatus;
                        display("SPOT", instrumentName, MarketInfo);
                    } else {
                        spdlog::warn("SPOT symbol {} not found for update", instrumentName);
                    }
                } else if (marketType == "usd_futures") {
                    auto data = exchangeData.usdSymbols.find(instrumentName);
                    if (data != exchangeData.usdSymbols.end()) {
                        auto &MarketInfo = data->second;
                        MarketInfo.status = newStatus;
                        display("USD Futures", instrumentName, MarketInfo);
                    } else {
                        spdlog::warn("USD Futures symbol {} not found for update", instrumentName);
                    }
                } else if (marketType == "coin_futures") {
                    auto data = exchangeData.coinSymbols.find(instrumentName);
                    if (data != exchangeData.coinSymbols.end()) {
                        auto &MarketInfo = data->second;
                        MarketInfo.status = newStatus;
                        display("Coin Futures", instrumentName, MarketInfo);
                    } else {
                        spdlog::warn("Coin Futures symbol {} not found for update", instrumentName);
                    }
                }
            } else if (queryType == "DELETE") {
                if (marketType == "SPOT") {
                    auto data = exchangeData.spotSymbols.find(instrumentName);
                    if (data != exchangeData.spotSymbols.end()) {
                        spdlog::info("Removing SPOT symbol: {}", instrumentName);
                        exchangeData.spotSymbols.erase(data);
                    } else {
                        spdlog::warn("SPOT symbol {} not found for deletion", instrumentName);
                    }
                } else if (marketType == "usd_futures") {
                    auto data = exchangeData.usdSymbols.find(instrumentName);
                    if (data != exchangeData.usdSymbols.end()) {
                        spdlog::info("Removing USD Futures symbol: {}", instrumentName);
                        exchangeData.usdSymbols.erase(data);
                    } else {
                        spdlog::warn("USD Futures symbol {} not found for deletion", instrumentName);
                    }
                } else if (marketType == "coin_futures") {
                    auto data = exchangeData.coinSymbols.find(instrumentName);
                    if (data != exchangeData.coinSymbols.end()) {
                        spdlog::info("Removing Coin Futures symbol: {}", instrumentName);
                        exchangeData.coinSymbols.erase(data);
                    } else {
                        spdlog::warn("Coin Futures symbol {} not found for deletion", instrumentName);
                    }
                }
            }

            resultArray.PushBack(resultObj, allocator);
        } else {
            spdlog::warn("IDs are the same for market type: {}", marketType);
        }
    }
    myMutex.unlock();

    resultDoc.RemoveMember("results");
    resultDoc.AddMember("results", resultArray, allocator);
    ofstream ofs("answers.json");
    if (!ofs.is_open()) {
        spdlog::error("Failed to open answer.json for writing!");
        return;
    }
    rapidjson::OStreamWrapper osw(ofs);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    resultDoc.Accept(writer);
    ofs.close();

    spdlog::info("Processed queries and appended results to file.");
}

//reading the query.json file
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

//read the process the query continuously
void readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
{
    while (true)
    {
        rapidjson::Document doc;
        readQueryFile(queryFile, doc);
        processQueries(doc);

        this_thread::sleep_for(chrono::seconds(2)); 
    }
}

void fetchSpotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
        spotData(ioc, ctx, spotBaseUrl, spotEndpoint);
}

void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
       usdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint);
}

void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
    coinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint);
}

//the function which call all endpoints function and then use this function (fetchEndpoints) as a callBack Function in timer
void fetchEndpoints(const boost::system::error_code& /*e*/, boost::asio::steady_timer* t, boost::asio::io_context& ioc, boost::asio::ssl::context& ctx){
    fetchSpotData(ioc, ctx, ref(spotBase), ref(spotTarget));
    fetchUsdFutureData(ioc, ctx, ref(usdtFutureBase), ref(usdtFutureTarget));
    fetchCoinFutureData(ioc, ctx, ref(coinFutureBase), ref(coinFutureTarget));
    t->expires_at(t->expiry() + boost::asio::chrono::seconds(request_interval));
    t->async_wait(boost::bind(fetchEndpoints, boost::asio::placeholders::error, t, ref(ioc), ref(ctx)));
}



int main()
{

    rapidjson::Document doc1;
    readConfig("config.json", doc1);
    spdlog::info("Configuration loaded: spotBase={}, usdtFutureBase={}, coinFutureBase={}",
        spotBase, usdtFutureBase, coinFutureBase);

    ostringstream json_stream;
    auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
    auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});
    
    logger->set_level(spdlog::level::from_str(logLevel)); 
    spdlog::set_default_logger(logger);
    spdlog::flush_every(chrono::seconds(1));

    spdlog::info("Logger initialized with level: {}", logLevel);

    net::io_context ioc;

    ssl::context ctx{ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    boost::asio::steady_timer t(ioc, boost::asio::chrono::seconds(15));
    t.async_wait(boost::bind(fetchEndpoints, boost::asio::placeholders::error, &t, ref(ioc), ref(ctx)));


    thread queryThread(readQueryFileContinuously, "query.json", ref(ioc));
    ioc.run();

    queryThread.join();
    return 0;
}
