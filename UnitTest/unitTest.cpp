#include "processData.h"
#include "marketInfo.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <fstream>

using namespace std;

string spotB, usdtFB, coinFB;
string spotT, usdtFT, coinFT;

void parseSymbols(std::string &responseBody, std::map<std::string, MarketInfo> *symbolsMap);

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


// TestCase to check the data which is get after parsing the symbols
TEST(ParseSymbolsTest, CorrectlyParsesJSON) {
    string jsonString = R"(
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

    map<string, MarketInfo> symbolsMap;
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
    string jsonString = R"({})";

    map<string, MarketInfo> symbolsMap;
    parseSymbols(jsonString, &symbolsMap);

    EXPECT_TRUE(symbolsMap.empty());
}

//Testcase to handle multiple symbols
TEST(ParseSymbolsTest, HandlesMultipleSymbols) {
    string jsonString = R"(
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

    map<string, MarketInfo> symbolsMap;
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
    string jsonString = R"(
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

    map<string, MarketInfo> symbolsMap;
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
