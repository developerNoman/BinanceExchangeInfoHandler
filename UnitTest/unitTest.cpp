#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include <fstream>
#include <rapidjson/filereadstream.h>
#include "request.h"

string spotB, usdtFB, coinFB;
string spotT, usdtFT, coinFT;

//function to read the config.json file
void readConfig(std::string configFile, rapidjson::Document &doc1) {
    FILE* fp = fopen(configFile.c_str(), "r");
    if (!fp) {
        cerr << "Error: unable to open file" << endl;
        return;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc1.ParseStream(is);

    if (doc1.HasParseError()) {
        cerr << "Error: JSON parse error" << endl;
        fclose(fp);
        return;
    }

    if (!doc1.IsObject()) {
        cerr << "Error: JSON is not an object" << endl;
        fclose(fp);
        return;
    }

    if (!doc1.HasMember("exchange_base_url") || !doc1["exchange_base_url"].IsObject()) {
        cerr << "Error: Missing or invalid 'exchange_base_url' field" << endl;
        fclose(fp);
        return;
    }
    if (!doc1.HasMember("exchange_endpoints") || !doc1["exchange_endpoints"].IsObject()) {
        cerr << "Error: Missing or invalid 'exchange_endpoints' field" << endl;
        fclose(fp);
        return;
    }

    const auto& baseUrls = doc1["exchange_base_url"];
    const auto& endpoints = doc1["exchange_endpoints"];

    if (baseUrls.HasMember("spotBase") && baseUrls["spotBase"].IsString()) {
        spotB = baseUrls["spotBase"].GetString();
    } else {
        cerr << "Error: Missing or invalid 'spotBase' field" << endl;
    }

    if (baseUrls.HasMember("usdtFutureBase") && baseUrls["usdtFutureBase"].IsString()) {
        usdtFB = baseUrls["usdtFutureBase"].GetString();
    } else {
        cerr << "Error: Missing or invalid 'usdtFutureBase' field" << endl;
    }

    if (baseUrls.HasMember("coinFutureBase") && baseUrls["coinFutureBase"].IsString()) {
        coinFB = baseUrls["coinFutureBase"].GetString();
    } else {
        cerr << "Error: Missing or invalid 'coinFutureBase' field" << endl;
    }

    if (endpoints.HasMember("spotTarget") && endpoints["spotTarget"].IsString()) {
        spotT = endpoints["spotTarget"].GetString();
    } else {
        cerr << "Error: Missing or invalid 'spotTarget' field" << endl;
    }

    if (endpoints.HasMember("usdtFutureTarget") && endpoints["usdtFutureTarget"].IsString()) {
        usdtFT = endpoints["usdtFutureTarget"].GetString();
    } else {
        cerr << "Error: Missing or invalid 'usdtFutureTarget' field" << endl;
    }

    if (endpoints.HasMember("coinFutureTarget") && endpoints["coinFutureTarget"].IsString()) {
        coinFT = endpoints["coinFutureTarget"].GetString();
    } else {
        cerr << "Error: Missing or invalid 'coinFutureTarget' field" << endl;
    }

    fclose(fp);
}

//TestCase to check the data which is get after parsing the symbols
TEST(ParseSymbolsTest, CorrectlyParsesJSON) {
    std::string jsonString = R"(
    {
        "symbols": [
            {
                "symbol": "BTCUSDT",
                "quoteAsset": "USDT",
                "status": "TRADING",
                "filters": [
                    {"filterType": "PRICE_FILTER", "tickSize": "0.01"},
                    {"filterType": "LOT_SIZE", "stepSize": "0.0001"}
                ]
            }
        ]
    })";

    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    std::map<std::string, symbolInfo> symbolsMap;
    parseSymbols(jsonString, &symbolsMap);  

    ASSERT_EQ(symbolsMap.size(), 1);

    auto it = symbolsMap.find("BTCUSDT");
    ASSERT_NE(it, symbolsMap.end());

    EXPECT_EQ(it->second.symbol, "BTCUSDT");
    EXPECT_EQ(it->second.quoteAsset, "USDT");
    EXPECT_EQ(it->second.status, "TRADING");
    EXPECT_EQ(it->second.tickSize, "0.01");
    EXPECT_EQ(it->second.stepSize, "0.0001");
}

//TestCase which handle Empty json
TEST(ParseSymbolsTest, HandlesEmptyJSON) {
    std::string jsonString = R"({})";

    std::map<std::string, symbolInfo> symbolsMap;
    parseSymbols(jsonString, &symbolsMap);

    EXPECT_TRUE(symbolsMap.empty());
}

//Testcase to handle multiple symbols
TEST(ParseSymbolsTest, HandlesMultipleSymbols) {
    std::string jsonString = R"(
    {
        "symbols": [
            {
                "symbol": "BTCUSDT",
                "quoteAsset": "USDT",
                "status": "TRADING",
                "filters": [
                    {"filterType": "PRICE_FILTER", "tickSize": "0.01"},
                    {"filterType": "LOT_SIZE", "stepSize": "0.0001"}
                ]            
            },
            {
                "symbol": "ETHUSDT",
                "quoteAsset": "USDT",
                "status": "TRADING",
                "filters": [
                    {"filterType": "PRICE_FILTER", "tickSize": "0.01"},
                    {"filterType": "LOT_SIZE", "stepSize": "0.0001"}
                ]            
            }        
        ]
    })";

    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    std::map<std::string, symbolInfo> symbolsMap;
    parseSymbols(jsonString, &symbolsMap);

    ASSERT_EQ(symbolsMap.size(), 2);

    auto itBTC = symbolsMap.find("BTCUSDT");
    ASSERT_NE(itBTC, symbolsMap.end());
    EXPECT_EQ(itBTC->second.symbol, "BTCUSDT");
    EXPECT_EQ(itBTC->second.quoteAsset, "USDT");

    auto itETH = symbolsMap.find("ETHUSDT");
    ASSERT_NE(itETH, symbolsMap.end());
    EXPECT_EQ(itETH->second.symbol, "ETHUSDT");
    EXPECT_EQ(itETH->second.quoteAsset, "USDT");
}

//test case to handle wrong data type
TEST(ParseSymbolsTest, HandlesUnexpectedDataTypes) {
    std::string jsonString = R"(
    {
        "symbols": [
            {
                "symbol": "BTCUSDT",
                "quoteAsset": 123,
                "status": "TRADING",
                "filters": [
                    {"filterType": "PRICE_FILTER", "tickSize": "0.01"},
                    {"filterType": "LOT_SIZE", "stepSize": "0.0001"}
                ]
            }
        ]
    })";

    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    std::map<std::string, symbolInfo> symbolsMap;
    parseSymbols(jsonString, &symbolsMap);

    ASSERT_EQ(symbolsMap.size(), 1);
    auto itBTC = symbolsMap.find("BTCUSDT");
    ASSERT_NE(itBTC, symbolsMap.end());
    EXPECT_EQ(itBTC->second.symbol, "BTCUSDT");
    EXPECT_EQ(itBTC->second.quoteAsset, ""); 
    EXPECT_EQ(itBTC->second.status, "TRADING");
    EXPECT_EQ(itBTC->second.tickSize, "0.01");
    EXPECT_EQ(itBTC->second.stepSize, "0.0001");
}

//TestCase to check the data which is get after reading the files, it is about base urls and endpoints
TEST(ReadConfigTest, CorrectlyReadsConfig) {
    rapidjson::Document doc1;
    readConfig("config.json", doc1);

    EXPECT_EQ(spotB, "api.binance.com");
    EXPECT_EQ(usdtFB, "fapi.binance.com");
    EXPECT_EQ(coinFB, "dapi.binance.com");
    EXPECT_EQ(spotT, "/api/v3/exchangeInfo");
    EXPECT_EQ(usdtFT, "/fapi/v1/exchangeInfo");
    EXPECT_EQ(coinFT, "/dapi/v1/exchangeInfo");
}




