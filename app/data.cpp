#include "request.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;

void display(const std::string &marketType, const std::string &instrumentName, const symbolInfo &symbolInfo) {
            cout << marketType << " Market - Symbol: " << symbolInfo.symbol << endl;
            cout << "Quote Asset: " << symbolInfo.quoteAsset << endl;
            cout << "Status: " << symbolInfo.status << endl;
            cout << "Tick Size: " << symbolInfo.tickSize << endl;
            cout << "Step Size: " << symbolInfo.stepSize << endl;
        }

exchangeInfo binanceExchange;

void fail(beast::error_code ec, char const *what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

void spotData(string spotBaseUrl, string spotEndpoint)
{
    auto const host = spotBaseUrl.c_str();
    auto const port = "443";
    auto const target = spotEndpoint.c_str();
    int version = 11;
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> spotExchange = sessionPtr->returnResponse();
    parseSymbols(spotExchange.body(), binanceExchange.spotSymbols);
}

void usdFutureData(string usdFutureBaseUrl, string usdFutureEndpoint)
{
    auto const host = usdFutureBaseUrl.c_str();
    auto const port = "443";
    auto const target = usdFutureEndpoint.c_str();
    int version = 11;
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> usdFutureExchange = sessionPtr->returnResponse();
    parseSymbols(usdFutureExchange.body(), binanceExchange.usdSymbols);
}

void coinFutureData(string coinFutureBaseUrl, string coinFutureEndpoint)
{
    auto const host = coinFutureBaseUrl.c_str();
    auto const port = "443";
    auto const target = coinFutureEndpoint.c_str();
    int version = 11;
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> coinFutureExchange = sessionPtr->returnResponse();
    parseSymbols(coinFutureExchange.body(), binanceExchange.coinSymbols);
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

// Function to process the queries from the JSON document
void processQueries(const rapidjson::Document &doc1)
{
    if (!doc1.HasMember("query") || !doc1["query"].IsArray())  {
        cerr << "Invalid JSON format!" << endl;
        return;
    }

    const rapidjson::Value &queries = doc1["query"];
    int prevID;
    for (int i = 0; i < queries.Size(); ++i)
    {
        const rapidjson::Value &query = queries[i];

        if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name"))
        {
            cerr << "Missing query details!" << endl;
            continue;
        }
        
        int queryID=query["id"].GetInt();
        if(prevID!=queryID){
        prevID = queryID;
        }
        string queryType = query["query_type"].GetString();
        string marketType = query["market_type"].GetString();
        string instrumentName = query["instrument_name"].GetString();

        if (queryType == "GET" && prevID==queryID){
            if (marketType == "SPOT"){
                auto data = binanceExchange.spotSymbols.find(instrumentName);
                if (data != binanceExchange.spotSymbols.end()){
                    display("SPOT", instrumentName, data->second);
                }
                else{
                    cout << "Symbol " << instrumentName << " not found in SPOT market." << endl;
                }
            }
            else if (marketType == "usd_futures") {
                auto data = binanceExchange.usdSymbols.find(instrumentName);
                if (data != binanceExchange.usdSymbols.end()){
                    display("USD Futures", instrumentName, data->second);
                }
                else{
                    cout << "Symbol " << instrumentName << " not found in USD Futures market." << endl;
                }
            }
            else if (marketType == "coin_futures"){
                auto data = binanceExchange.coinSymbols.find(instrumentName);
                if (data != binanceExchange.coinSymbols.end()) {
                    display("Coin Futures", instrumentName, data->second);
                }
                else{
                    cout << "Symbol " << instrumentName << " not found in Coin Futures market." << endl;
                }
            }
        }
        else if (queryType == "UPDATE" && prevID==queryID)   {
            string newStatus = query["data"]["status"].GetString();
            if (marketType == "SPOT"){
                auto data = binanceExchange.spotSymbols.find(instrumentName);
                if (data != binanceExchange.spotSymbols.end()){
                    auto &symbolInfo = data->second;
                    symbolInfo.status = newStatus;
                    display("SPOT", instrumentName, symbolInfo);
                }
                else {
                    cout << "Symbol " << instrumentName << " not found in SPOT market." << endl;
                }
            }
            else if (marketType == "usd_futures") {
                string newStatus = query["data"]["status"].GetString();
                auto data = binanceExchange.usdSymbols.find(instrumentName);
                if (data != binanceExchange.usdSymbols.end())  {
                    auto &symbolInfo = data->second;
                    symbolInfo.status = newStatus;
                    display("USD Futures", instrumentName, symbolInfo);
                }
                else {
                    cout << "Symbol " << instrumentName << " not found in USD Futures market." << endl;
                }
            }
            else if (marketType == "coin_futures"){
                string newStatus = query["data"]["status"].GetString();
                auto data = binanceExchange.coinSymbols.find(instrumentName);
                if (data != binanceExchange.coinSymbols.end()){
                    auto &symbolInfo = data->second;
                    symbolInfo.status = newStatus;
                    display("Coin Futures", instrumentName, symbolInfo);
                }
                else {
                    cout << "Symbol " << instrumentName << " not found in Coin Futures market." << endl;
                }
            }
        }
        else if (queryType == "DELETE" && prevID==queryID){
            if (marketType == "SPOT"){
                auto data = binanceExchange.spotSymbols.find(instrumentName);
                if (data != binanceExchange.spotSymbols.end()){
                    binanceExchange.spotSymbols.erase(data);
                    cout << "Symbol " << instrumentName << " is deleted from SPOT market." << endl;
                }
                else{
                    cout << "Symbol " << instrumentName << " not found in SPOT market." << endl;
                }
            }
            else if (marketType == "usd_futures"){
                auto data = binanceExchange.usdSymbols.find(instrumentName);
                if (data != binanceExchange.usdSymbols.end()){
                    binanceExchange.usdSymbols.erase(data);
                    cout << "Symbol " << instrumentName << " is deleted from USD Future market." << endl;
                }
                else {
                    cout << "Symbol " << instrumentName << " not found in USD Future market." << endl;
                }
            }
            else if (marketType == "coin_futures"){
                auto data = binanceExchange.coinSymbols.find(instrumentName);
                if (data != binanceExchange.coinSymbols.end()){
                    binanceExchange.coinSymbols.erase(data);
                    cout << "Symbol " << instrumentName << " is deleted from coin Future market." << endl;
                }
                else{
                    cout << "Symbol " << instrumentName << " not found in coin Future market." << endl;
                }
            }
        }
    }
}
