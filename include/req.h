#ifndef REQ_H
#define REQ_H

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "example/common/root_certificates.hpp"
#include "rapidjson/document.h"
#include <string>

// External variables used in main.cpp
extern std::string logLevel;
extern std::string spotBase, usdtFutureBase, coinFutureBase;
extern int request_interval;

// Function declarations used in main.cpp
void load_root_certificates(boost::asio::ssl::context &ctx);
void readConfig(const std::string &config_file, rapidjson::Document &doc1);
void fetchEndpoints(const boost::system::error_code &ec, boost::asio::steady_timer *t, boost::asio::io_context &ioc, boost::asio::ssl::context &ctx);
void readQueryFileContinuously(const std::string &query_file, boost::asio::io_context &ioc);

#endif
