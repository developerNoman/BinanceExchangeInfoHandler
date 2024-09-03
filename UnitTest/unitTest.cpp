#include "processData.h"
#include "marketInfo.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <fstream>

using namespace std;

void parseSymbols(string &responseBody, const string &base, exchangeSymbols &exchangeData);
void readQueryFile(const string &queryFile, rapidjson::Document &doc1);
void display(const string &marketType, const string &instrumentName, const MarketInfo &MarketInfo);

extern exchangeSymbols exchangeData;

// Test case for correctly read Query file
TEST(ReadQueryFileTest, CorrectlyReadQuery)
{
    const string tempFileName = "query.json";
    ofstream tempFile(tempFileName);
    tempFile << R"({"queries": [{"id": 1, "symbol": "BTCUSDT"}]})";
    tempFile.close();

    rapidjson::Document doc1;
    readQueryFile(tempFileName, doc1);

    ASSERT_FALSE(doc1.HasParseError());
    ASSERT_TRUE(doc1.HasMember("queries"));
    ASSERT_TRUE(doc1["queries"].IsArray());
    ASSERT_EQ(doc1["queries"].Size(), 1);
    ASSERT_EQ(doc1["queries"][0]["id"].GetInt(), 1);
    ASSERT_EQ(doc1["queries"][0]["symbol"].GetString(), string("BTCUSDT"));

    remove(tempFileName.c_str());
}

// Test case for correctly read config file
TEST(ReadConfigTest, CorrectlyReadsConfig)
{
    rapidjson::Document doc1;
    readConfig("config.json", doc1);

    EXPECT_EQ(spotBase, "api.binance.com");
    EXPECT_EQ(usdtFutureBase, "fapi.binance.com");
    EXPECT_EQ(coinFutureBase, "dapi.binance.com");
    EXPECT_EQ(spotTarget, "/api/v3/exchangeInfo");
    EXPECT_EQ(usdtFutureTarget, "/fapi/v1/exchangeInfo");
    EXPECT_EQ(coinFutureTarget, "/dapi/v1/exchangeInfo");
}

// test the processQuery for spot data
TEST(ExchangeDataTest, GetSpotSymbol)
{
    std::string instrumentName = "BTCUSDT";
    std::string marketType = "SPOT";
    std::string queryType = "GET";

    if (queryType == "GET")
    {
        if (marketType == "SPOT")
        {
            auto data = exchangeData.getSpotSymbols().find(instrumentName);
            if (data != exchangeData.getSpotSymbols().end())
            {
                display("SPOT", instrumentName, data->second);
            }
            else
            {
                spdlog::warn("SPOT symbol {} not found", instrumentName);
            }
        }
    }
}

// Test case for empty JSON input
TEST(ParseSymbolsTest, EmptyJSON)
{
    string emptyJSON = "{}";
    exchangeSymbols exchangeData;
    parseSymbols(emptyJSON, "spot", exchangeData);
    EXPECT_TRUE(exchangeData.getSpotSymbols().empty());
}

// Test case for completely empty input
TEST(ParseSymbolsTest, EmptyInput)
{
    string emptyInput = "";
    exchangeSymbols exchangeData;
    parseSymbols(emptyInput, "spot", exchangeData);
    EXPECT_TRUE(exchangeData.getSpotSymbols().empty());
}

// Test case for invalid JSON input
TEST(ParseSymbolsTest, InvalidJSON)
{
    string invalidJSON = R"({"symbols": ["symbol": "BTCUSDT"]})";
    exchangeSymbols exchangeData;
    parseSymbols(invalidJSON, "spot", exchangeData);
    EXPECT_TRUE(exchangeData.getSpotSymbols().empty());
}
