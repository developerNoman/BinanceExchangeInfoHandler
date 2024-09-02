#include "processData.h"
#include "marketInfo.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <fstream>

using namespace std;


void parseSymbols(std::string &responseBody, const std::string &base, exchangeSymbols &exchangeData);

TEST(ReadConfigTest, CorrectlyReadsConfig) {
    rapidjson::Document doc1;
    readConfig("config.json", doc1);

    EXPECT_EQ(spotBase, "api.binance.com");
    EXPECT_EQ(usdtFutureBase, "fapi.binance.com");
    EXPECT_EQ(coinFutureBase, "dapi.binance.com");
    EXPECT_EQ(spotTarget, "/api/v3/exchangeInfo");
    EXPECT_EQ(usdtFutureTarget, "/fapi/v1/exchangeInfo");
    EXPECT_EQ(coinFutureTarget, "/dapi/v1/exchangeInfo");
}




// Test case for empty JSON input
TEST(ParseSymbolsTest, EmptyJSON) {
    std::string emptyJSON = "{}";
    exchangeSymbols exchangeData;
    parseSymbols(emptyJSON, "spot", exchangeData);
    EXPECT_TRUE(exchangeData.getSpotSymbols().empty());
}

// Test case for completely empty input
TEST(ParseSymbolsTest, EmptyInput) {
    std::string emptyInput = "";
    exchangeSymbols exchangeData;
    parseSymbols(emptyInput, "spot", exchangeData);
    EXPECT_TRUE(exchangeData.getSpotSymbols().empty());
}

// Test case for invalid JSON input
TEST(ParseSymbolsTest, InvalidJSON) {
    std::string invalidJSON = R"({"symbols": ["symbol": "BTCUSDT"]})"; // Invalid JSON format
    exchangeSymbols exchangeData;
    parseSymbols(invalidJSON, "spot", exchangeData);
    EXPECT_TRUE(exchangeData.getSpotSymbols().empty());
}
