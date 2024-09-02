#ifndef PROCESSQUERIES_H
#define PROCESSQUERIES_H

#include "marketInfo.h"
#include "rapidjson/filereadstream.h"
#include "boost/asio/steady_timer.hpp"
#include "boost/bind/bind.hpp"
#include "spdlog/spdlog.h"            
#include "rapidjson/writer.h"      
#include "rapidjson/stringbuffer.h" 
#include "rapidjson/istreamwrapper.h" 
#include "rapidjson/ostreamwrapper.h"  
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/ostream_sink.h"
#include <thread>
#include <chrono>
#include <cstdio>
#include <vector>
#include <fstream>
#include <set>


void display(const std::string &marketType, const std::string &instrumentName, const MarketInfo &MarketInfo);

void processQueries(const rapidjson::Document &doc);

void readQueryFile(const std::string &queryFile, rapidjson::Document &doc1);

void readQueryFileContinuously(const std::string &queryFile, boost::asio::io_context &ioc);

#endif 