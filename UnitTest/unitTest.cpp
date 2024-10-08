#include "exchangeSymbols.h"
#include "processQueries.h"
#include "fetchData.h"
#include "spdlog/spdlog.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <fstream>
#include <utils.h>

using namespace std;

// object creations
exchangeSymbols myExchangeData;
utils utilsInfo;
processEndpoints pe;
processResponse pr;

void display(const string &marketType, const string &instrumentName, const MarketInfo &MarketInfo)
{
    spdlog::info("{} Market - Symbol: {}", marketType, MarketInfo.symbol);
    spdlog::info("Quote Asset: {}", MarketInfo.quoteAsset);
    spdlog::info("Status: {}", MarketInfo.status);
    spdlog::info("Tick Size: {}", MarketInfo.tickSize);
    spdlog::info("Step Size: {}", MarketInfo.stepSize);
}

// Test case for correctly read Query file
TEST(ReadQueryFileTest, CorrectlyReadQuery)
{
    const string tempFileName = "query.json";
    ofstream tempFile(tempFileName);
    tempFile << R"({"queries": [{"id": 1, "symbol": "BTCUSDT"}]})";
    tempFile.close();

    rapidjson::Document doc1;
    pr.readQueryFile(tempFileName, doc1);

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
    pe.readConfig("config.json", doc1, utilsInfo);

    EXPECT_EQ(utilsInfo.spotBase, "api.binance.com");
    EXPECT_EQ(utilsInfo.usdtFutureBase, "fapi.binance.com");
    EXPECT_EQ(utilsInfo.coinFutureBase, "dapi.binance.com");
    EXPECT_EQ(utilsInfo.spotTarget, "/api/v3/exchangeInfo");
    EXPECT_EQ(utilsInfo.usdtFutureTarget, "/fapi/v1/exchangeInfo");
    EXPECT_EQ(utilsInfo.coinFutureTarget, "/dapi/v1/exchangeInfo");
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
            auto data = myExchangeData.getSpotSymbols().find(instrumentName);
            if (data != myExchangeData.getSpotSymbols().end())
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
    pe.parseSymbols(emptyJSON, "spot", exchangeData);
    EXPECT_TRUE(myExchangeData.getSpotSymbols().empty());
}

// Test case for completely empty input
TEST(ParseSymbolsTest, EmptyInput)
{
    string emptyInput = "";
    exchangeSymbols exchangeData;
    pe.parseSymbols(emptyInput, "spot", exchangeData);
    EXPECT_TRUE(myExchangeData.getSpotSymbols().empty());
}

// Test case for invalid JSON input
TEST(ParseSymbolsTest, InvalidJSON)
{
    string invalidJSON = R"({"symbols": ["symbol": "BTCUSDT"]})";
    exchangeSymbols exchangeData;
    pe.parseSymbols(invalidJSON, "spot", exchangeData);
    EXPECT_TRUE(myExchangeData.getSpotSymbols().empty());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}