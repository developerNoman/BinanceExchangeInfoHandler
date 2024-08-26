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

// // void readConfig(string, rapidjson::Document);
// string spotBase, usdtFutureBase, coinFutureBase;
// string spotTarget, usdtFutureTarget, coinFutureTarget;
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
//     fclose(fp);
// }
// void fetchData(const string, const string, map<string, symbolInfo> , boost::asio::io_context, boost::asio::ssl::context);
// void writeToFile(const rapidjson::Document); 
// void spotData(net::io_context, ssl::context, const string, const string); 
// void usdFutureData(net::io_context, ssl::context, const string, const string);
// void coinFutureData(net::io_context, ssl::context, const string, const string);
// void display(const string, const string, const symbolInfo);
// void readQueryFile(const string, rapidjson::Document);
// void processQueries(const rapidjson::Document);
// void readQueryFileContinuously(const string, net::io_context);
// void fetchSpotData(net::io_context, ssl::context , const string, const string); 
// void fetchUsdFutureData(net::io_context, ssl::context, const string, const string); 
// void fetchCoinFutureData(net::io_context, ssl::context, const string, const string); 

