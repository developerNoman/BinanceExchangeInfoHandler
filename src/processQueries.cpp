#include "processQueries.h"
using namespace std;

mutex myMutex;

void exchangeSymbols::removeSpotSymbol(const string &instrumentName)
{
    auto data = _spotSymbols.find(instrumentName);
    if (data != _spotSymbols.end())
    {
        _spotSymbols.erase(data);
    }
}

void exchangeSymbols::removeUsdSymbol(const string &instrumentName)
{
    auto data = _usdSymbols.find(instrumentName);
    if (data != _usdSymbols.end())
    {
        _usdSymbols.erase(data);
    }
}

void exchangeSymbols::removeCoinSymbol(const string &instrumentName)
{
    auto data = _coinSymbols.find(instrumentName);
    if (data != _coinSymbols.end())
    {
        _coinSymbols.erase(data);
    }
}

// function to display the marketData in logs and the answers.json file
void processResponse::display(const string &marketType, const string &instrumentName, const MarketInfo &marketInfo, rapidjson::Value &resultObj, rapidjson::Document::AllocatorType &allocator)
{

    resultObj.AddMember("quote_asset", rapidjson::Value(marketInfo.quoteAsset.c_str(), allocator), allocator);
    resultObj.AddMember("status", rapidjson::Value(marketInfo.status.c_str(), allocator), allocator);
    resultObj.AddMember("tick_size", rapidjson::Value(marketInfo.tickSize.c_str(), allocator), allocator);
    resultObj.AddMember("step_size", rapidjson::Value(marketInfo.stepSize.c_str(), allocator), allocator);

    spdlog::info("{} Market - Symbol: {}", marketType, marketInfo.symbol);
    spdlog::info("Quote Asset: {}", marketInfo.quoteAsset);
    spdlog::info("Status: {}", marketInfo.status);
    spdlog::info("Tick Size: {}", marketInfo.tickSize);
    spdlog::info("Step Size: {}", marketInfo.stepSize);
}

// map to handle the id comparison
map<string, map<int, int>> idOccurrences;

// function to process the queries defined in query.json file
void processResponse::processQueries(const rapidjson::Document &doc)
{
    if (!doc.HasMember("query") || !doc["query"].IsArray())
    {
        spdlog::error("Invalid JSON format in query file!");
        return;
    }

    rapidjson::Document resultDoc;
    resultDoc.SetObject();
    rapidjson::Document::AllocatorType &allocator = resultDoc.GetAllocator();

    rapidjson::Value resultArray(rapidjson::kArrayType);

    const rapidjson::Value &queries = doc["query"];
    int length = queries.Size();

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

        if (idOccurrences[marketType][queryID] == 0)
        {
            idOccurrences[marketType][queryID] = 1;
        }
        else
        {
            spdlog::warn("Ids are same ID: {} for market type: {}", queryID, marketType);
            continue;
        }

        spdlog::debug("Processing query ID: {}, Market Type: {}", queryID, marketType);

        string queryType = query["query_type"].GetString();
        string instrumentName = query["instrument_name"].GetString();

        rapidjson::Value resultObj(rapidjson::kObjectType);
        resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
        resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

        MarketInfo marketInfo;

        if (queryType == "GET")
        {
            if (marketType == "SPOT")
            {
                auto data = exchangeData.getSpotSymbols().find(instrumentName);
                if (data != exchangeData.getSpotSymbols().end())
                {
                    marketInfo = data->second;
                    display("SPOT", instrumentName, marketInfo, resultObj, allocator);
                }
                else
                {
                    spdlog::warn("SPOT symbol {} not found", instrumentName);
                }
            }
            else if (marketType == "usd_futures")
            {
                auto data = exchangeData.getUsdSymbols().find(instrumentName);
                if (data != exchangeData.getUsdSymbols().end())
                {
                    marketInfo = data->second;
                    display("USD Futures", instrumentName, marketInfo, resultObj, allocator);
                }
                else
                {
                    spdlog::warn("USD Futures symbol {} not found", instrumentName);
                }
            }
            else if (marketType == "coin_futures")
            {
                auto data = exchangeData.getCoinSymbols().find(instrumentName);
                if (data != exchangeData.getCoinSymbols().end())
                {
                    marketInfo = data->second;
                    display("Coin Futures", instrumentName, marketInfo, resultObj, allocator);
                }
                else
                {
                    spdlog::warn("Coin Futures symbol {} not found", instrumentName);
                }
            }
        }
        else if (queryType == "UPDATE")
        {
            string newStatus = query["data"]["status"].GetString();
            MarketInfo updatedInfo;

            if (marketType == "SPOT")
            {
                auto data = exchangeData.getSpotSymbols().find(instrumentName);
                if (data != exchangeData.getSpotSymbols().end())
                {
                    updatedInfo = data->second;
                    updatedInfo.status = newStatus;
                    exchangeData.setSpotSymbol(instrumentName, updatedInfo);
                    display("SPOT", instrumentName, updatedInfo, resultObj, allocator);
                }
                else
                {
                    spdlog::warn("SPOT symbol {} not found for update", instrumentName);
                }
            }
            else if (marketType == "usd_futures")
            {
                auto data = exchangeData.getUsdSymbols().find(instrumentName);
                if (data != exchangeData.getUsdSymbols().end())
                {
                    updatedInfo = data->second;
                    updatedInfo.status = newStatus;
                    exchangeData.setUsdSymbol(instrumentName, updatedInfo);
                    display("USD Futures", instrumentName, updatedInfo, resultObj, allocator);
                }
                else
                {
                    spdlog::warn("USD Futures symbol {} not found for update", instrumentName);
                }
            }
            else if (marketType == "coin_futures")
            {
                auto data = exchangeData.getCoinSymbols().find(instrumentName);
                if (data != exchangeData.getCoinSymbols().end())
                {
                    updatedInfo = data->second;
                    updatedInfo.status = newStatus;
                    exchangeData.setCoinSymbol(instrumentName, updatedInfo);
                    display("Coin Futures", instrumentName, updatedInfo, resultObj, allocator);
                }
                else
                {
                    spdlog::warn("Coin Futures symbol {} not found for update", instrumentName);
                }
            }
        }
        else if (queryType == "DELETE")
        {
            if (marketType == "SPOT")
            {
                auto data = exchangeData.getSpotSymbols().find(instrumentName);
                if (data != exchangeData.getSpotSymbols().end())
                {
                    spdlog::info("Removing SPOT symbol: {}", instrumentName);
                    exchangeData.removeSpotSymbol(instrumentName);
                }
                else
                {
                    spdlog::warn("SPOT symbol {} not found for deletion", instrumentName);
                }
            }
            else if (marketType == "usd_futures")
            {
                auto data = exchangeData.getUsdSymbols().find(instrumentName);
                if (data != exchangeData.getUsdSymbols().end())
                {
                    spdlog::info("Removing USD Futures symbol: {}", instrumentName);
                    exchangeData.removeUsdSymbol(instrumentName);
                }
                else
                {
                    spdlog::warn("USD Futures symbol {} not found for deletion", instrumentName);
                }
            }
            else if (marketType == "coin_futures")
            {
                auto data = exchangeData.getCoinSymbols().find(instrumentName);
                if (data != exchangeData.getCoinSymbols().end())
                {
                    spdlog::info("Removing Coin Futures symbol: {}", instrumentName);
                    exchangeData.removeCoinSymbol(instrumentName);
                }
                else
                {
                    spdlog::warn("Coin Futures symbol {} not found for deletion", instrumentName);
                }
            }
        }

        resultArray.PushBack(resultObj, allocator);
    }
    myMutex.unlock();

    if (resultArray.Empty())
    {
        spdlog::debug("No new results to append.");
        return;
    }

    ofstream ofs("answers.json", ios::out | ios::app);
    if (!ofs.is_open())
    {
        spdlog::error("Failed to open answers.json for writing!");
        return;
    }

    rapidjson::OStreamWrapper osw(ofs);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    resultDoc.AddMember("results", resultArray, allocator);
    resultDoc.Accept(writer);
    spdlog::info("Results written to answers.json");
}

// reading the query.json file
void processResponse::readQueryFile(const string &queryFile, rapidjson::Document &doc1)
{
    FILE *fp = fopen(queryFile.c_str(), "r");
    if (!fp)
    {
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

// read the process the query continuously
void processResponse::readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
{
    while (true)
    {
        rapidjson::Document doc;
        readQueryFile(queryFile, doc);
        processQueries(doc);

        this_thread::sleep_for(chrono::seconds(2));
    }
}