#include "processQueries.h"
using namespace std;
namespace net = boost::asio;
mutex myMutex;

void display(const string &marketType, const string &instrumentName, const MarketInfo &MarketInfo) {
    spdlog::info("{} Market - Symbol: {}", marketType, MarketInfo.symbol);
    spdlog::info("Quote Asset: {}", MarketInfo.quoteAsset);
    spdlog::info("Status: {}", MarketInfo.status);
    spdlog::info("Tick Size: {}", MarketInfo.tickSize);
    spdlog::info("Step Size: {}", MarketInfo.stepSize);

}
map<string, map<int, int>> idOccurrences;

//method to process the queries from the query file
void processQueries(const rapidjson::Document &doc) {
    if (!doc.HasMember("query") || !doc["query"].IsArray()) {
        spdlog::error("Invalid JSON format in query file!");
        return;
    }

    rapidjson::Document resultDoc;
    resultDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();

    rapidjson::Value resultArray(rapidjson::kArrayType);

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

    int length = queries.Size();
    myMutex.lock();
    for (int i = 0; i < length; ++i) {
        const rapidjson::Value &query = queries[i];

        if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name")) {
            spdlog::warn("Missing query details in query index: {}", i);
            continue;
        }

        int queryID = query["id"].GetInt();
        string marketType = query["market_type"].GetString();

        // Update occurrences map
        if (idOccurrences[marketType][queryID] == 0) {
            idOccurrences[marketType][queryID] = 1;
        } else {
            spdlog::warn("Ids are same ID: {} for market type: {}", queryID, marketType);
            continue;
        }

        spdlog::info("Processing query ID: {}, Market Type: {}", queryID, marketType);

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
