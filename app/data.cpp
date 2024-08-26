// #include "request.h"
// #include <rapidjson/document.h>
// #include <rapidjson/filereadstream.h>
// #include <boost/asio.hpp>
// #include <boost/asio/steady_timer.hpp>
// #include <boost/bind/bind.hpp>
// #include <rapidjson/writer.h>
// #include <rapidjson/stringbuffer.h>
// #include <thread>
// #include <chrono>
// #include <cstdio>
// #include <string>
// #include <vector>
// #include <map>
// #include <iostream>
// #include <fstream>
// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/ostream_sink.h>

// using namespace std;
// namespace net = boost::asio;


// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);

// exchangeInfo myExchangeData;

// string spotBase, usdtFutureBase, coinFutureBase;
// string spotTarget, usdtFutureTarget, coinFutureTarget;
// int request_interval;

// void readConfig(string configFile, rapidjson::Document &doc1) {

//     FILE* fp = fopen(configFile.c_str(), "r");
//     if (!fp) {
//         cerr << "Error: unable to open file" << endl;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);

//     spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
//     usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
//     coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
//     spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
//     usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
//     coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
//     request_interval = doc1["request_interval"].GetInt();
    
//     fclose(fp);
// }

// void writeToFile(const rapidjson::Document &document) {
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     document.Accept(writer);

//     std::ofstream ofs("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/answers.json");
//     if (ofs.is_open()) {
//         ofs << buffer.GetString();
//         ofs.close();
//     } else {
//         std::cerr << "Failed to open file for writing!" << std::endl;
//     }
// }


// map<string, symbolInfo> spotMarketSymbols;
// void spotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
    
//     fetchData(spotBaseUrl, spotEndpoint, spotMarketSymbols, ioc, ctx);
//     myExchangeData.spotMarketSymbols = spotMarketSymbols;
// }

// map<string, symbolInfo> usdMarketSymbols;
// void usdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
   
//     fetchData(usdFutureBaseUrl, usdFutureEndpoint, usdMarketSymbols, ioc, ctx);
//          myExchangeData.usdMarketSymbols = usdMarketSymbols;
// }

// map<string, symbolInfo> coinMarketSymbols;
// void coinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
 
//     fetchData(coinFutureBaseUrl, coinFutureEndpoint, coinMarketSymbols, ioc, ctx);
//         myExchangeData.coinMarketSymbols = coinMarketSymbols;

// }

// void display(const string &marketType, const string &instrumentName, const symbolInfo &symbolInfo) {
//     spdlog::trace("{} Market - Symbol: {}", marketType, symbolInfo.symbol);
//     spdlog::trace("Quote Asset: {}", symbolInfo.quoteAsset);
//     spdlog::trace("Status: {}", symbolInfo.status);
//     spdlog::trace("Tick Size: {}", symbolInfo.tickSize);
//     spdlog::trace("Step Size: {}", symbolInfo.stepSize);

// }



// void fail(beast::error_code ec, char const *what)
// {
//     cerr << what << ": " << ec.message() << "\n";
// }

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx)
// {
//     auto const host = baseUrl.c_str();
//     auto const port = "443";
//     auto const target = endpoint.c_str();
//     int version = 11;

//     auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
//     sessionPtr->run(host, port, target, version);

//     ioc.run();
//     http::response<http::string_body> exchangeData = sessionPtr->returnResponse();
//     parseSymbols(exchangeData.body(), symbolsMap);
// }


// void readQueryFile(const string &queryFile, rapidjson::Document &doc1)
// {
//     FILE *fp = fopen(queryFile.c_str(), "r");
//     if (!fp){
//         cerr << "Error: unable to open file" << endl;
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);
//     if (doc1.HasParseError())
//     {
//         cerr << "Error parsing JSON file!" << endl;
//     }

//     fclose(fp);
// }


// void processQueries(const rapidjson::Document &doc)
// {
//     static int prevID = -1;  

//     if (!doc.HasMember("query") || !doc["query"].IsArray()) {
//         cerr << "Invalid JSON format!" << endl;
//         return;
//     }
    
//     rapidjson::Document resultDoc;
//     resultDoc.SetObject();
//     rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();
//     rapidjson::Value resultArray(rapidjson::kArrayType);

//     const rapidjson::Value &queries = doc["query"];
//     for (int i = 0; i < queries.Size(); ++i)
//     {
//         const rapidjson::Value &query = queries[i];

//         if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name"))
//         {
//             cerr << "Missing query details!" << endl;
//             continue;
//         }
        
//         int queryID = query["id"].GetInt();

    
//         bool flag = false;
//         if (prevID != queryID) {
//             prevID = queryID;  
//             flag = true;      
//         } else {
//             flag = false;   
//         }
       

//         if (flag) {
//             string queryType = query["query_type"].GetString();
//             string marketType = query["market_type"].GetString();
//             string instrumentName = query["instrument_name"].GetString();

//             rapidjson::Value resultObj(rapidjson::kObjectType);
//             resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
//             resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

//             if (queryType == "GET") {
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         display("SPOT", instrumentName, data->second);
//                     } 
//                 }
//                 else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         display("USD Futures", instrumentName, data->second);
//                     }
//                 }
//                 else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         display("Coin Futures", instrumentName, data->second);
//                     } 
//                 }
//             }
//             else if (queryType == "UPDATE") {
//                 string newStatus = query["data"]["status"].GetString();
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("SPOT", instrumentName, symbolInfo);
//                     } 
//                 }
//                 else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("USD Futures", instrumentName, symbolInfo);
//                     } 
//                 }
//                 else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("Coin Futures", instrumentName, symbolInfo);
//                     } 
//                 }
//             }
//             else if (queryType == "DELETE") {
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         cout << "Symbol " << instrumentName << " is deleted from SPOT market." << endl;
//                         myExchangeData.spotMarketSymbols.erase(data);
                        
//                     } 
//                 }
//                 else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         cout << "Symbol " << instrumentName << " is deleted from USD Future market." << endl;
//                         myExchangeData.usdMarketSymbols.erase(data);
                        
//                     } 
//                 }
//                 else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         cout << "Symbol " << instrumentName << " is deleted from Coin Futures market." << endl;
//                         myExchangeData.coinMarketSymbols.erase(data);
                        
//                     } 
//                 }
//             }
//             resultArray.PushBack(resultObj, allocator);
//         }
//     }
//     resultDoc.AddMember("results", resultArray, allocator);
//     writeToFile(resultDoc);
// }



// void readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
// {
//     while (true)
//     {
//         rapidjson::Document doc;
//         readQueryFile(queryFile, doc);
//         processQueries(doc);

//         this_thread::sleep_for(chrono::seconds(10)); 
//     }
// }


// void fetchSpotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
//     while (true) {
//         spotData(ioc, ctx, spotBaseUrl, spotEndpoint);
//         this_thread::sleep_for(chrono::seconds(60)); 
//     }
// }

// void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
//     while (true) {
//         usdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint);
//         this_thread::sleep_for(chrono::seconds(60)); 
//     }
// }

// void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
//     while (true) {
//         coinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint);
//         this_thread::sleep_for(chrono::seconds(60)); 
//     }
// }

// int main()
// {
//     ostringstream json_stream;
//     auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
//     auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
//     auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});
    
//     logger->set_level(spdlog::level::trace); 
//     spdlog::set_default_logger(logger);
//     spdlog::flush_every(chrono::seconds(1));

//     net::io_context ioc;
//     ssl::context ctx{ssl::context::tlsv12_client};
//     load_root_certificates(ctx);
//     ctx.set_verify_mode(ssl::verify_peer);

    

//     rapidjson::Document doc1;
//     readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc1);

//     thread spotThread(fetchSpotData, ref(ioc), ref(ctx), ref(spotBase), ref(spotTarget));
//     thread usdFutureThread(fetchUsdFutureData, ref(ioc), ref(ctx), ref(usdtFutureBase), ref(usdtFutureTarget));
//     thread coinFutureThread(fetchCoinFutureData, ref(ioc), ref(ctx), ref(coinFutureBase), ref(coinFutureTarget));
    
  
//     thread queryThread(readQueryFileContinuously, "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/query.json", ref(ioc));


//     spotThread.join();
//     usdFutureThread.join();
//     coinFutureThread.join();
//     queryThread.join();


//     return 0;
// }
// #include "request.h"
// #include <rapidjson/document.h>
// #include <rapidjson/filereadstream.h>
// #include <boost/asio.hpp>
// #include <boost/asio/steady_timer.hpp>
// #include <boost/bind/bind.hpp>
// #include <rapidjson/writer.h>
// #include <rapidjson/stringbuffer.h>
// #include <thread>
// #include <chrono>
// #include <cstdio>
// #include <string>
// #include <vector>
// #include <map>
// #include <iostream>
// #include <fstream>
// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/ostream_sink.h>

// using namespace std;
// namespace net = boost::asio;

// // extern string level1;

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);

// exchangeInfo myExchangeData;

// string spotBase, usdtFutureBase, coinFutureBase;
// string spotTarget, usdtFutureTarget, coinFutureTarget;
// int request_interval;

// void readConfig(string configFile, rapidjson::Document &doc1) {
//     FILE* fp = fopen(configFile.c_str(), "r");
//     if (!fp) {
//         spdlog::error("Error: unable to open config file {}", configFile);
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);

//     // level1=doc1["logging"]["level"].GetString();
//     spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
//     usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
//     coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
//     spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
//     usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
//     coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
//     request_interval = doc1["request_interval"].GetInt();
    
//     spdlog::info("Config file {} read successfully", configFile);
//     fclose(fp);
// }

// void writeToFile(const rapidjson::Document &document) {
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     document.Accept(writer);

//     std::ofstream ofs("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/answers.json");
//     if (ofs.is_open()) {
//         ofs << buffer.GetString();
//         ofs.close();
//         spdlog::info("Results written to file successfully");
//     } else {
//         spdlog::error("Failed to open file for writing!");
//     }
// }

// map<string, symbolInfo> spotMarketSymbols;
// void spotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
//     spdlog::info("Fetching SPOT data from {}", spotBaseUrl + spotEndpoint);
//     fetchData(spotBaseUrl, spotEndpoint, spotMarketSymbols, ioc, ctx);
//     myExchangeData.spotMarketSymbols = spotMarketSymbols;
// }

// map<string, symbolInfo> usdMarketSymbols;
// void usdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
//     spdlog::info("Fetching USD Futures data from {}", usdFutureBaseUrl + usdFutureEndpoint);
//     fetchData(usdFutureBaseUrl, usdFutureEndpoint, usdMarketSymbols, ioc, ctx);
//     myExchangeData.usdMarketSymbols = usdMarketSymbols;
// }

// map<string, symbolInfo> coinMarketSymbols;
// void coinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
//     spdlog::info("Fetching Coin Futures data from {}", coinFutureBaseUrl + coinFutureEndpoint);
//     fetchData(coinFutureBaseUrl, coinFutureEndpoint, coinMarketSymbols, ioc, ctx);
//     myExchangeData.coinMarketSymbols = coinMarketSymbols;
// }

// void display(const string &marketType, const string &instrumentName, const symbolInfo &symbolInfo) {
//     spdlog::info("{} Market - Symbol: {}", marketType, instrumentName);
//     spdlog::info("Quote Asset: {}", symbolInfo.quoteAsset);
//     spdlog::info("Status: {}", symbolInfo.status);
//     spdlog::info("Tick Size: {}", symbolInfo.tickSize);
//     spdlog::info("Step Size: {}", symbolInfo.stepSize);
// }

// void fail(beast::error_code ec, char const *what) {
//     spdlog::error("{}: {}", what, ec.message());
// }

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx) {
//     auto const host = baseUrl.c_str();
//     auto const port = "443";
//     auto const target = endpoint.c_str();
//     int version = 11;

//     spdlog::info("Initiating session for host: {} target: {}", host, target);

//     auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
//     sessionPtr->run(host, port, target, version);

//     ioc.run();
//     http::response<http::string_body> exchangeData = sessionPtr->returnResponse();
//     parseSymbols(exchangeData.body(), symbolsMap);
// }

// void readQueryFile(const string &queryFile, rapidjson::Document &doc1) {
//     FILE *fp = fopen(queryFile.c_str(), "r");
//     if (!fp) {
//         spdlog::error("Error: unable to open file {}", queryFile);
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);
//     if (doc1.HasParseError()) {
//         spdlog::error("Error parsing JSON file: {}", queryFile);
//     } else {
//         spdlog::info("JSON file {} parsed successfully", queryFile);
//     }

//     fclose(fp);
// }

// void processQueries(const rapidjson::Document &doc) {
//     static int prevID = -1;

//     if (!doc.HasMember("query") || !doc["query"].IsArray()) {
//         spdlog::error("Invalid JSON format in the query document!");
//         return;
//     }

//     rapidjson::Document resultDoc;
//     resultDoc.SetObject();
//     rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();
//     rapidjson::Value resultArray(rapidjson::kArrayType);

//     const rapidjson::Value &queries = doc["query"];
//     for (int i = 0; i < queries.Size(); ++i) {
//         const rapidjson::Value &query = queries[i];

//         if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name")) {
//             spdlog::error("Missing query details in query ID {}", i);
//             continue;
//         }

//         int queryID = query["id"].GetInt();

//         bool flag = false;
//         if (prevID != queryID) {
//             prevID = queryID;
//             flag = true;
//         } else {
//             flag = false;
//         }

//         if (flag) {
//             string queryType = query["query_type"].GetString();
//             string marketType = query["market_type"].GetString();
//             string instrumentName = query["instrument_name"].GetString();

//             rapidjson::Value resultObj(rapidjson::kObjectType);
//             resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
//             resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

//             if (queryType == "GET") {
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         display("SPOT", instrumentName, data->second);
//                     }
//                 } else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         display("USD Futures", instrumentName, data->second);
//                     }
//                 } else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         display("Coin Futures", instrumentName, data->second);
//                     }
//                 }
//             } else if (queryType == "UPDATE") {
//                 string newStatus = query["data"]["status"].GetString();
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("SPOT", instrumentName, symbolInfo);
//                     }
//                 } else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("USD Futures", instrumentName, symbolInfo);
//                     }
//                 } else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("Coin Futures", instrumentName, symbolInfo);
//                     }
//                 }
//             }
//             resultArray.PushBack(resultObj, allocator);
//         }
//     }
//     resultDoc.AddMember("results", resultArray, allocator);
//     writeToFile(resultDoc);
// }



// void readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
// {
//     while (true)
//     {
//         rapidjson::Document doc;
//         readQueryFile(queryFile, doc);
//         processQueries(doc);

//         this_thread::sleep_for(chrono::seconds(10)); 
//     }
// }


// void fetchSpotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
//     while (true) {
//         spotData(ioc, ctx, spotBaseUrl, spotEndpoint);
//         this_thread::sleep_for(chrono::seconds(request_interval)); 
//     }
// }

// void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
//     while (true) {
//         usdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint);
//         this_thread::sleep_for(chrono::seconds(request_interval)); 
//     }
// }

// void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
//     while (true) {
//         coinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint);
//         this_thread::sleep_for(chrono::seconds(request_interval)); 
//     }
// }

// int main()
// {
//     ostringstream json_stream;
//     auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
//     auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
//     auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});
    
//     logger->set_level(spdlog::level::trace); 
//     spdlog::set_default_logger(logger);
//     spdlog::flush_every(chrono::seconds(1));

//     net::io_context ioc;
//     ssl::context ctx{ssl::context::tlsv12_client};
//     load_root_certificates(ctx);
//     ctx.set_verify_mode(ssl::verify_peer);

    

//     rapidjson::Document doc1;
//     readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc1);

//     thread spotThread(fetchSpotData, ref(ioc), ref(ctx), ref(spotBase), ref(spotTarget));
//     thread usdFutureThread(fetchUsdFutureData, ref(ioc), ref(ctx), ref(usdtFutureBase), ref(usdtFutureTarget));
//     thread coinFutureThread(fetchCoinFutureData, ref(ioc), ref(ctx), ref(coinFutureBase), ref(coinFutureTarget));
    
  
//     thread queryThread(readQueryFileContinuously, "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/query.json", ref(ioc));


//     spotThread.join();
//     usdFutureThread.join();
//     coinFutureThread.join();
//     queryThread.join();


//     return 0;
// }

// #include "request.h"
// #include <rapidjson/document.h>
// #include <rapidjson/filereadstream.h>
// #include <boost/asio.hpp>
// #include <boost/asio/steady_timer.hpp>
// #include <boost/bind/bind.hpp>
// #include <rapidjson/writer.h>
// #include <rapidjson/stringbuffer.h>
// #include <thread>
// #include <chrono>
// #include <cstdio>
// #include <string>
// #include <vector>
// #include <map>
// #include <iostream>
// #include <fstream>
// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/ostream_sink.h>

// using namespace std;
// namespace net = boost::asio;

// // extern string level1;

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);

// exchangeInfo myExchangeData;

// string spotBase, usdtFutureBase, coinFutureBase;
// string spotTarget, usdtFutureTarget, coinFutureTarget;
// int request_interval;

// void readConfig(string configFile, rapidjson::Document &doc1) {
//     FILE* fp = fopen(configFile.c_str(), "r");
//     if (!fp) {
//         spdlog::error("Error: unable to open config file {}", configFile);
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);

//     // level1=doc1["logging"]["level"].GetString();
//     spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
//     usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
//     coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
//     spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
//     usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
//     coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
//     request_interval = doc1["request_interval"].GetInt();
    
//     spdlog::info("Config file {} read successfully", configFile);
//     fclose(fp);
// }

// void writeToFile(const rapidjson::Document &document) {
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     document.Accept(writer);

//     std::ofstream ofs("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/answers.json");
//     if (ofs.is_open()) {
//         ofs << buffer.GetString();
//         ofs.close();
//         spdlog::info("Results written to file successfully");
//     } else {
//         spdlog::error("Failed to open file for writing!");
//     }
// }

// map<string, symbolInfo> spotMarketSymbols;
// void spotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
//     spdlog::info("Fetching SPOT data from {}", spotBaseUrl + spotEndpoint);
//     fetchData(spotBaseUrl, spotEndpoint, spotMarketSymbols, ioc, ctx);
//     myExchangeData.spotMarketSymbols = spotMarketSymbols;
// }

// map<string, symbolInfo> usdMarketSymbols;
// void usdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
//     spdlog::info("Fetching USD Futures data from {}", usdFutureBaseUrl + usdFutureEndpoint);
//     fetchData(usdFutureBaseUrl, usdFutureEndpoint, usdMarketSymbols, ioc, ctx);
//     myExchangeData.usdMarketSymbols = usdMarketSymbols;
// }

// map<string, symbolInfo> coinMarketSymbols;
// void coinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
//     spdlog::info("Fetching Coin Futures data from {}", coinFutureBaseUrl + coinFutureEndpoint);
//     fetchData(coinFutureBaseUrl, coinFutureEndpoint, coinMarketSymbols, ioc, ctx);
//     myExchangeData.coinMarketSymbols = coinMarketSymbols;
// }

// void display(const string &marketType, const string &instrumentName, const symbolInfo &symbolInfo) {
//     spdlog::info("{} Market - Symbol: {}", marketType, instrumentName);
//     spdlog::info("Quote Asset: {}", symbolInfo.quoteAsset);
//     spdlog::info("Status: {}", symbolInfo.status);
//     spdlog::info("Tick Size: {}", symbolInfo.tickSize);
//     spdlog::info("Step Size: {}", symbolInfo.stepSize);
// }

// void fail(beast::error_code ec, char const *what) {
//     spdlog::error("{}: {}", what, ec.message());
// }

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx) {
//     auto const host = baseUrl.c_str();
//     auto const port = "443";
//     auto const target = endpoint.c_str();
//     int version = 11;

//     spdlog::info("Initiating session for host: {} target: {}", host, target);

//     auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
//     sessionPtr->run(host, port, target, version);

//     ioc.run();
//     http::response<http::string_body> exchangeData = sessionPtr->returnResponse();
//     parseSymbols(exchangeData.body(), symbolsMap);
// }

// void readQueryFile(const string &queryFile, rapidjson::Document &doc1) {
//     FILE *fp = fopen(queryFile.c_str(), "r");
//     if (!fp) {
//         spdlog::error("Error: unable to open file {}", queryFile);
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);
//     if (doc1.HasParseError()) {
//         spdlog::error("Error parsing JSON file: {}", queryFile);
//     } else {
//         spdlog::info("JSON file {} parsed successfully", queryFile);
//     }

//     fclose(fp);
// }

// void processQueries(const rapidjson::Document &doc) {
//     static int prevID = -1;

//     if (!doc.HasMember("query") || !doc["query"].IsArray()) {
//         spdlog::error("Invalid JSON format in the query document!");
//         return;
//     }

//     rapidjson::Document resultDoc;
//     resultDoc.SetObject();
//     rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();
//     rapidjson::Value resultArray(rapidjson::kArrayType);

//     const rapidjson::Value &queries = doc["query"];
//     for (int i = 0; i < queries.Size(); ++i) {
//         const rapidjson::Value &query = queries[i];

//         if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name")) {
//             spdlog::error("Missing query details in query ID {}", i);
//             continue;
//         }

//         int queryID = query["id"].GetInt();

//         bool flag = false;
//         if (prevID != queryID) {
//             prevID = queryID;
//             flag = true;
//         } else {
//             flag = false;
//         }

//         if (flag) {
//             string queryType = query["query_type"].GetString();
//             string marketType = query["market_type"].GetString();
//             string instrumentName = query["instrument_name"].GetString();

//             rapidjson::Value resultObj(rapidjson::kObjectType);
//             resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
//             resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

//             if (queryType == "GET") {
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         display("SPOT", instrumentName, data->second);
//                     }
//                 } else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         display("USD Futures", instrumentName, data->second);
//                     }
//                 } else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         display("Coin Futures", instrumentName, data->second);
//                     }
//                 }
//             } else if (queryType == "UPDATE") {
//                 string newStatus = query["data"]["status"].GetString();
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("SPOT", instrumentName, symbolInfo);
//                     }
//                 } else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("USD Futures", instrumentName, symbolInfo);
//                     }
//                 } else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("Coin Futures", instrumentName, symbolInfo);
//                     }
//                 }
//             }
//             resultArray.PushBack(resultObj, allocator);
//         }
//     }
//     resultDoc.AddMember("results", resultArray, allocator);
//     writeToFile(resultDoc);
// }



// void readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
// {
//     while (true)
//     {
//         rapidjson::Document doc;
//         readQueryFile(queryFile, doc);
//         processQueries(doc);

//         this_thread::sleep_for(chrono::seconds(10)); 
//     }
// }


// void fetchSpotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint, boost::asio::steady_timer &t1) {
//     spdlog::info("Fetching SPOT data from {}", spotBaseUrl + spotEndpoint);
//     spotData(ioc, ctx, spotBaseUrl, spotEndpoint);

//     t1.expires_at(t1.expiry() + boost::asio::chrono::seconds(request_interval));
//     t1.async_wait([&, spotBaseUrl, spotEndpoint](const boost::system::error_code& ec) {
//         if (!ec) {
//             fetchSpotData(ioc, ctx, spotBaseUrl, spotEndpoint, t1);
//         } else {
//             spdlog::error("Timer error: {}", ec.message());
//         }
//     });
// }
// void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint, boost::asio::steady_timer &t2) {
//     spdlog::info("Fetching USD Futures data from {}", usdFutureBaseUrl + usdFutureEndpoint);
//     usdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint);

//     t2.expires_at(t2.expiry() + boost::asio::chrono::seconds(request_interval));
//     t2.async_wait([&, usdFutureBaseUrl, usdFutureEndpoint](const boost::system::error_code& ec) {
//         if (!ec) {
//             fetchUsdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint, t2);
//         } else {
//             spdlog::error("Timer error: {}", ec.message());
//         }
//     });
// }

// void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint, boost::asio::steady_timer &t) {
//     spdlog::info("Fetching Coin Futures data from {}", coinFutureBaseUrl + coinFutureEndpoint);
//     coinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint);

//     t.expires_at(t.expiry() + boost::asio::chrono::seconds(request_interval));
//     t.async_wait([&, coinFutureBaseUrl, coinFutureEndpoint](const boost::system::error_code& ec) {
//         if (!ec) {
//             fetchCoinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint, t);
//         } else {
//             spdlog::error("Timer error: {}", ec.message());
//         }
//     });
// }

// int main()
// {
//     ostringstream json_stream;
//     auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
//     auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
//     auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});

//     logger->set_level(spdlog::level::trace); 
//     spdlog::set_default_logger(logger);
//     spdlog::flush_every(chrono::seconds(1));

//     net::io_context ioc;
//     ssl::context ctx{ssl::context::tlsv12_client};
//     load_root_certificates(ctx);
//     ctx.set_verify_mode(ssl::verify_peer);

//     rapidjson::Document doc1;
//     readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc1);

//     boost::asio::steady_timer spotTimer(ioc, boost::asio::chrono::seconds(60));
//     boost::asio::steady_timer usdFutureTimer(ioc, boost::asio::chrono::seconds(60));
//     boost::asio::steady_timer coinFutureTimer(ioc, boost::asio::chrono::seconds(60));

//     thread spotThread(fetchSpotData, ref(ioc), ref(ctx), ref(spotBase), ref(spotTarget),ref(spotTimer));
//     thread usdFutureThread(fetchUsdFutureData, ref(ioc), ref(ctx), ref(usdtFutureBase), ref(usdtFutureTarget),ref(usdFutureTimer));
//     thread coinFutureThread(fetchCoinFutureData, ref(ioc), ref(ctx), ref(coinFutureBase), ref(coinFutureTarget),ref(coinFutureTimer));
    
//     thread queryThread(readQueryFileContinuously, "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/query.json", ref(ioc));
//     spotThread.join();
//     usdFutureThread.join();
//     coinFutureThread.join();
//     queryThread.join();
//     return 0;
// }

// #include "request.h"
// #include <rapidjson/document.h>
// #include <rapidjson/filereadstream.h>
// #include <boost/asio.hpp>
// #include <boost/asio/steady_timer.hpp>
// #include <boost/bind/bind.hpp>
// #include <rapidjson/writer.h>
// #include <rapidjson/stringbuffer.h>
// #include <thread>
// #include <chrono>
// #include <cstdio>
// #include <string>
// #include <vector>
// #include <map>
// #include <iostream>
// #include <fstream>
// #include <spdlog/spdlog.h>
// #include <spdlog/sinks/basic_file_sink.h>
// #include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/ostream_sink.h>

// using namespace std;
// namespace net = boost::asio;

// // extern string level1;

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);

// exchangeInfo myExchangeData;

// string spotBase, usdtFutureBase, coinFutureBase;
// string spotTarget, usdtFutureTarget, coinFutureTarget;
// int request_interval;

// void readConfig(string configFile, rapidjson::Document &doc1) {
//     FILE* fp = fopen(configFile.c_str(), "r");
//     if (!fp) {
//         spdlog::error("Error: unable to open config file {}", configFile);
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);

//     // level1=doc1["logging"]["level"].GetString();
//     spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
//     usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
//     coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
//     spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
//     usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
//     coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
//     request_interval = doc1["request_interval"].GetInt();
    
//     spdlog::info("Config file {} read successfully", configFile);
//     fclose(fp);
// }

// void writeToFile(const rapidjson::Document &document) {
//     rapidjson::StringBuffer buffer;
//     rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
//     document.Accept(writer);

//     std::ofstream ofs("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/answers.json");
//     if (ofs.is_open()) {
//         ofs << buffer.GetString();
//         ofs.close();
//         spdlog::info("Results written to file successfully");
//     } else {
//         spdlog::error("Failed to open file for writing!");
//     }
// }

// map<string, symbolInfo> spotMarketSymbols;
// void spotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
//     spdlog::info("Fetching SPOT data from {}", spotBaseUrl + spotEndpoint);
//     fetchData(spotBaseUrl, spotEndpoint, spotMarketSymbols, ioc, ctx);
//     myExchangeData.spotMarketSymbols = spotMarketSymbols;
// }

// map<string, symbolInfo> usdMarketSymbols;
// void usdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
//     spdlog::info("Fetching USD Futures data from {}", usdFutureBaseUrl + usdFutureEndpoint);
//     fetchData(usdFutureBaseUrl, usdFutureEndpoint, usdMarketSymbols, ioc, ctx);
//     myExchangeData.usdMarketSymbols = usdMarketSymbols;
// }

// map<string, symbolInfo> coinMarketSymbols;
// void coinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
//     spdlog::info("Fetching Coin Futures data from {}", coinFutureBaseUrl + coinFutureEndpoint);
//     fetchData(coinFutureBaseUrl, coinFutureEndpoint, coinMarketSymbols, ioc, ctx);
//     myExchangeData.coinMarketSymbols = coinMarketSymbols;
// }

// void display(const string &marketType, const string &instrumentName, const symbolInfo &symbolInfo) {
//     spdlog::info("{} Market - Symbol: {}", marketType, instrumentName);
//     spdlog::info("Quote Asset: {}", symbolInfo.quoteAsset);
//     spdlog::info("Status: {}", symbolInfo.status);
//     spdlog::info("Tick Size: {}", symbolInfo.tickSize);
//     spdlog::info("Step Size: {}", symbolInfo.stepSize);
// }

// void fail(beast::error_code ec, char const *what) {
//     spdlog::error("{}: {}", what, ec.message());
// }

// void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx) {
//     auto const host = baseUrl.c_str();
//     auto const port = "443";
//     auto const target = endpoint.c_str();
//     int version = 11;

//     spdlog::info("Initiating session for host: {} target: {}", host, target);

//     auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
//     sessionPtr->run(host, port, target, version);

//     ioc.run();
//     http::response<http::string_body> exchangeData = sessionPtr->returnResponse();
//     parseSymbols(exchangeData.body(), symbolsMap);
// }

// void readQueryFile(const string &queryFile, rapidjson::Document &doc1) {
//     FILE *fp = fopen(queryFile.c_str(), "r");
//     if (!fp) {
//         spdlog::error("Error: unable to open file {}", queryFile);
//         return;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc1.ParseStream(is);
//     if (doc1.HasParseError()) {
//         spdlog::error("Error parsing JSON file: {}", queryFile);
//     } else {
//         spdlog::info("JSON file {} parsed successfully", queryFile);
//     }

//     fclose(fp);
// }

// void processQueries(const rapidjson::Document &doc) {
//     static int prevID = -1;

//     if (!doc.HasMember("query") || !doc["query"].IsArray()) {
//         spdlog::error("Invalid JSON format in the query document!");
//         return;
//     }

//     rapidjson::Document resultDoc;
//     resultDoc.SetObject();
//     rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();
//     rapidjson::Value resultArray(rapidjson::kArrayType);

//     const rapidjson::Value &queries = doc["query"];
//     for (int i = 0; i < queries.Size(); ++i) {
//         const rapidjson::Value &query = queries[i];

//         if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name")) {
//             spdlog::error("Missing query details in query ID {}", i);
//             continue;
//         }

//         int queryID = query["id"].GetInt();

//         bool flag = false;
//         if (prevID != queryID) {
//             prevID = queryID;
//             flag = true;
//         } else {
//             flag = false;
//         }

//         if (flag) {
//             string queryType = query["query_type"].GetString();
//             string marketType = query["market_type"].GetString();
//             string instrumentName = query["instrument_name"].GetString();

//             rapidjson::Value resultObj(rapidjson::kObjectType);
//             resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
//             resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

//             if (queryType == "GET") {
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         display("SPOT", instrumentName, data->second);
//                     }
//                 } else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         display("USD Futures", instrumentName, data->second);
//                     }
//                 } else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         display("Coin Futures", instrumentName, data->second);
//                     }
//                 }
//             } else if (queryType == "UPDATE") {
//                 string newStatus = query["data"]["status"].GetString();
//                 if (marketType == "SPOT") {
//                     auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.spotMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("SPOT", instrumentName, symbolInfo);
//                     }
//                 } else if (marketType == "usd_futures") {
//                     auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.usdMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("USD Futures", instrumentName, symbolInfo);
//                     }
//                 } else if (marketType == "coin_futures") {
//                     auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
//                     if (data != myExchangeData.coinMarketSymbols.end()) {
//                         auto &symbolInfo = data->second;
//                         symbolInfo.status = newStatus;
//                         display("Coin Futures", instrumentName, symbolInfo);
//                     }
//                 }
//             }
//             resultArray.PushBack(resultObj, allocator);
//         }
//     }
//     resultDoc.AddMember("results", resultArray, allocator);
//     writeToFile(resultDoc);
// }



// void readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
// {
//     while (true)
//     {
//         rapidjson::Document doc;
//         readQueryFile(queryFile, doc);
//         processQueries(doc);

//         this_thread::sleep_for(chrono::seconds(10)); 
//     }
// }


// void fetchSpotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
//     while (true) {
//         spotData(ioc, ctx, spotBaseUrl, spotEndpoint);
//         this_thread::sleep_for(chrono::seconds(request_interval)); 
//     }
// }

// void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
//     while (true) {
//         usdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint);
//         this_thread::sleep_for(chrono::seconds(request_interval)); 
//     }
// }

// void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
//     while (true) {
//         coinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint);
//         this_thread::sleep_for(chrono::seconds(request_interval)); 
//     }
// }

// int main()
// {
//     ostringstream json_stream;
//     auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
//     auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
//     auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});
    
//     logger->set_level(spdlog::level::trace); 
//     spdlog::set_default_logger(logger);
//     spdlog::flush_every(chrono::seconds(1));

//     net::io_context ioc;
//     ssl::context ctx{ssl::context::tlsv12_client};
//     load_root_certificates(ctx);
//     ctx.set_verify_mode(ssl::verify_peer);

    

//     rapidjson::Document doc1;
//     readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc1);

//     thread spotThread(fetchSpotData, ref(ioc), ref(ctx), ref(spotBase), ref(spotTarget));
//     thread usdFutureThread(fetchUsdFutureData, ref(ioc), ref(ctx), ref(usdtFutureBase), ref(usdtFutureTarget));
//     thread coinFutureThread(fetchCoinFutureData, ref(ioc), ref(ctx), ref(coinFutureBase), ref(coinFutureTarget));
    
  
//     thread queryThread(readQueryFileContinuously, "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/query.json", ref(ioc));


//     spotThread.join();
//     usdFutureThread.join();
//     coinFutureThread.join();
//     queryThread.join();


//     return 0;
// }
#include "request.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/bind/bind.hpp>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <thread>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ostream_sink.h>

using namespace std;
namespace net = boost::asio;


void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);

exchangeInfo myExchangeData;

string spotBase, usdtFutureBase, coinFutureBase;
string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;

void readConfig(string configFile, rapidjson::Document &doc1) {

    FILE* fp = fopen(configFile.c_str(), "r");
    if (!fp) {
        cerr << "Error: unable to open file" << endl;
    }

    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc1.ParseStream(is);

    spotBase = doc1["exchange_base_url"]["spotBase"].GetString();
    usdtFutureBase = doc1["exchange_base_url"]["usdtFutureBase"].GetString();
    coinFutureBase = doc1["exchange_base_url"]["coinFutureBase"].GetString();
    spotTarget = doc1["exchange_endpoints"]["spotTarget"].GetString();
    usdtFutureTarget = doc1["exchange_endpoints"]["usdtFutureTarget"].GetString();
    coinFutureTarget = doc1["exchange_endpoints"]["coinFutureTarget"].GetString();
    request_interval = doc1["request_interval"].GetInt();
    
    fclose(fp);
}

void writeToFile(const rapidjson::Document &document) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::ofstream ofs("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/answers.json");
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
    } else {
        std::cerr << "Failed to open file for writing!" << std::endl;
    }
}


map<string, symbolInfo> spotMarketSymbols;
void spotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
    
    fetchData(spotBaseUrl, spotEndpoint, spotMarketSymbols, ioc, ctx);
    myExchangeData.spotMarketSymbols = spotMarketSymbols;
}

map<string, symbolInfo> usdMarketSymbols;
void usdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
   
    fetchData(usdFutureBaseUrl, usdFutureEndpoint, usdMarketSymbols, ioc, ctx);
         myExchangeData.usdMarketSymbols = usdMarketSymbols;
}

map<string, symbolInfo> coinMarketSymbols;
void coinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
 
    fetchData(coinFutureBaseUrl, coinFutureEndpoint, coinMarketSymbols, ioc, ctx);
        myExchangeData.coinMarketSymbols = coinMarketSymbols;

}

void display(const string &marketType, const string &instrumentName, const symbolInfo &symbolInfo) {
    spdlog::trace("{} Market - Symbol: {}", marketType, symbolInfo.symbol);
    spdlog::trace("Quote Asset: {}", symbolInfo.quoteAsset);
    spdlog::trace("Status: {}", symbolInfo.status);
    spdlog::trace("Tick Size: {}", symbolInfo.tickSize);
    spdlog::trace("Step Size: {}", symbolInfo.stepSize);

}



void fail(beast::error_code ec, char const *what)
{
    cerr << what << ": " << ec.message() << "\n";
}

void fetchData(const string &baseUrl, const string &endpoint, map<string, symbolInfo> &symbolsMap, net::io_context &ioc, ssl::context &ctx)
{
    auto const host = baseUrl.c_str();
    auto const port = "443";
    auto const target = endpoint.c_str();
    int version = 11;

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> exchangeData = sessionPtr->returnResponse();
    parseSymbols(exchangeData.body(), symbolsMap);
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


void processQueries(const rapidjson::Document &doc)
{
    static int prevID = -1;  

    if (!doc.HasMember("query") || !doc["query"].IsArray()) {
        cerr << "Invalid JSON format!" << endl;
        return;
    }
    
    rapidjson::Document resultDoc;
    resultDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = resultDoc.GetAllocator();
    rapidjson::Value resultArray(rapidjson::kArrayType);

    const rapidjson::Value &queries = doc["query"];
    for (int i = 0; i < queries.Size(); ++i)
    {
        const rapidjson::Value &query = queries[i];

        if (!query.HasMember("query_type") || !query.HasMember("market_type") || !query.HasMember("instrument_name"))
        {
            cerr << "Missing query details!" << endl;
            continue;
        }
        
        int queryID = query["id"].GetInt();

    
        bool flag = false;
        if (prevID != queryID) {
            prevID = queryID;  
            flag = true;      
        } else {
            flag = false;   
        }
       

        if (flag) {
            string queryType = query["query_type"].GetString();
            string marketType = query["market_type"].GetString();
            string instrumentName = query["instrument_name"].GetString();

            rapidjson::Value resultObj(rapidjson::kObjectType);
            resultObj.AddMember("instrument_name", rapidjson::Value(instrumentName.c_str(), allocator), allocator);
            resultObj.AddMember("market_type", rapidjson::Value(marketType.c_str(), allocator), allocator);

            if (queryType == "GET") {
                if (marketType == "SPOT") {
                    auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.spotMarketSymbols.end()) {
                        display("SPOT", instrumentName, data->second);
                    } 
                }
                else if (marketType == "usd_futures") {
                    auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.usdMarketSymbols.end()) {
                        display("USD Futures", instrumentName, data->second);
                    }
                }
                else if (marketType == "coin_futures") {
                    auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.coinMarketSymbols.end()) {
                        display("Coin Futures", instrumentName, data->second);
                    } 
                }
            }
            else if (queryType == "UPDATE") {
                string newStatus = query["data"]["status"].GetString();
                if (marketType == "SPOT") {
                    auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.spotMarketSymbols.end()) {
                        auto &symbolInfo = data->second;
                        symbolInfo.status = newStatus;
                        display("SPOT", instrumentName, symbolInfo);
                    } 
                }
                else if (marketType == "usd_futures") {
                    auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.usdMarketSymbols.end()) {
                        auto &symbolInfo = data->second;
                        symbolInfo.status = newStatus;
                        display("USD Futures", instrumentName, symbolInfo);
                    } 
                }
                else if (marketType == "coin_futures") {
                    auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.coinMarketSymbols.end()) {
                        auto &symbolInfo = data->second;
                        symbolInfo.status = newStatus;
                        display("Coin Futures", instrumentName, symbolInfo);
                    } 
                }
            }
            else if (queryType == "DELETE") {
                if (marketType == "SPOT") {
                    auto data = myExchangeData.spotMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.spotMarketSymbols.end()) {
                        cout << "Symbol " << instrumentName << " is deleted from SPOT market." << endl;
                        myExchangeData.spotMarketSymbols.erase(data);
                        
                    } 
                }
                else if (marketType == "usd_futures") {
                    auto data = myExchangeData.usdMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.usdMarketSymbols.end()) {
                        cout << "Symbol " << instrumentName << " is deleted from USD Future market." << endl;
                        myExchangeData.usdMarketSymbols.erase(data);
                        
                    } 
                }
                else if (marketType == "coin_futures") {
                    auto data = myExchangeData.coinMarketSymbols.find(instrumentName);
                    if (data != myExchangeData.coinMarketSymbols.end()) {
                        cout << "Symbol " << instrumentName << " is deleted from Coin Futures market." << endl;
                        myExchangeData.coinMarketSymbols.erase(data);
                        
                    } 
                }
            }
            resultArray.PushBack(resultObj, allocator);
        }
    }
    resultDoc.AddMember("results", resultArray, allocator);
    writeToFile(resultDoc);
}



void readQueryFileContinuously(const string &queryFile, net::io_context &ioc)
{
    while (true)
    {
        rapidjson::Document doc;
        readQueryFile(queryFile, doc);
        processQueries(doc);

        this_thread::sleep_for(chrono::seconds(10)); 
    }
}


void fetchSpotData(net::io_context &ioc, ssl::context &ctx, const string &spotBaseUrl, const string &spotEndpoint) {
    while (true) {
        spotData(ioc, ctx, spotBaseUrl, spotEndpoint);
        this_thread::sleep_for(chrono::seconds(60)); 
    }
}

void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx, const string &usdFutureBaseUrl, const string &usdFutureEndpoint) {
    while (true) {
        usdFutureData(ioc, ctx, usdFutureBaseUrl, usdFutureEndpoint);
        this_thread::sleep_for(chrono::seconds(60)); 
    }
}

void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx, const string &coinFutureBaseUrl, const string &coinFutureEndpoint) {
    while (true) {
        coinFutureData(ioc, ctx, coinFutureBaseUrl, coinFutureEndpoint);
        this_thread::sleep_for(chrono::seconds(60)); 
    }
}

int main()
{
    ostringstream json_stream;
    auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
    auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});
    
    logger->set_level(spdlog::level::trace); 
    spdlog::set_default_logger(logger);
    spdlog::flush_every(chrono::seconds(1));

    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    

    rapidjson::Document doc1;
    readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc1);

    thread spotThread(fetchSpotData, ref(ioc), ref(ctx), ref(spotBase), ref(spotTarget));
    thread usdFutureThread(fetchUsdFutureData, ref(ioc), ref(ctx), ref(usdtFutureBase), ref(usdtFutureTarget));
    thread coinFutureThread(fetchCoinFutureData, ref(ioc), ref(ctx), ref(coinFutureBase), ref(coinFutureTarget));
    
  
    thread queryThread(readQueryFileContinuously, "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/query.json", ref(ioc));


    spotThread.join();
    usdFutureThread.join();
    coinFutureThread.join();
    queryThread.join();


    return 0;
}