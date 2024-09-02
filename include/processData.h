#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include "example/common/root_certificates.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/asio/strand.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "rapidjson/error/en.h"
#include <memory>
#include <mutex>
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
#include <thread>
#include <chrono>
#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <set>
#include <functional>

namespace net = boost::asio;

extern std::string logLevel;
extern std::string spotBase, usdtFutureBase, coinFutureBase;
extern std::string spotTarget, usdtFutureTarget, coinFutureTarget;
extern int request_interval;

void load_root_certificates(ssl::context &ctx);

void readConfig(const std::string &configFile, rapidjson::Document &doc);
void fetchEndpoints(const boost::system::error_code &ec, boost::asio::steady_timer *t, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);
void readQueryFileContinuously(const std::string &queryFile, boost::asio::io_context &ioc);

#endif 
