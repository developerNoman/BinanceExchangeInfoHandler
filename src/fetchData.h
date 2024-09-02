#ifndef FETCHDATA_H
#define FETCHDATA_H

#include "req.h"
#include "rapidjson/document.h"
#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "rapidjson/filereadstream.h"
#include "boost/asio.hpp"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind/bind.hpp"
#include "spdlog/spdlog.h"         
#include "rapidjson/document.h"    
#include "rapidjson/writer.h"      
#include "rapidjson/stringbuffer.h" 
#include "rapidjson/istreamwrapper.h" 
#include "rapidjson/ostreamwrapper.h"  
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ostream_sink.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <map>

namespace net = boost::asio;
// using boost::asio::ssl;

void parseSymbols(std::string &responseBody, std::map<std::string, MarketInfo> *symbolsMap);

void readConfig(const std::string &configFile, rapidjson::Document &doc);

void fetchData(const std::string &baseUrl, const std::string &endpoint, net::io_context &ioc, ssl::context &ctx);

void fetchSpotData(net::io_context &ioc, ssl::context &ctx);

void fetchUsdFutureData(net::io_context &ioc, ssl::context &ctx);

void fetchCoinFutureData(net::io_context &ioc, ssl::context &ctx);

void fetchEndpoints(const boost::system::error_code&, boost::asio::steady_timer *t, boost::asio::io_context &ioc, ssl::context &ctx);

#endif // FETCHDATA_H

