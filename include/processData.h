#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include "example/common/root_certificates.hpp"
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
#include <iostream>
#include <fstream>
#include <utils.h>

void load_root_certificates(ssl::context &ctx);

// functions declarations which are needed for the main appilcation.

class processEndpoints
{
public:
    void readConfig(const std::string &, rapidjson::Document &, utils &);
    void fetchEndpoints(const boost::system::error_code &, boost::asio::steady_timer *, boost::asio::io_context &, boost::asio::ssl::context &);
};

class processResponse
{
public:
    void readQueryFileContinuously(const std::string &, boost::asio::io_context &);
};

#endif