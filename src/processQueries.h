#ifndef PROCESSQUERIES_H
#define PROCESSQUERIES_H

#include "marketInfo.h"
#include "rapidjson/document.h"
#include "boost/asio.hpp"
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

// Function to display market information
void display(const std::string &marketType, const std::string &instrumentName, const MarketInfo &MarketInfo);

// Function to process queries from the query file
void processQueries(const rapidjson::Document &doc);

// Function to read the query JSON file
void readQueryFile(const std::string &queryFile, rapidjson::Document &doc1);

// Function to continuously read and process the query file
void readQueryFileContinuously(const std::string &queryFile, boost::asio::io_context &ioc);

#endif 