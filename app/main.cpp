#include "processData.h"
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
using namespace std;


int main()
{
    rapidjson::Document doc1;
    readConfig("config.json", doc1);
    spdlog::info("Configuration loaded: spotBase={}, usdtFutureBase={}, coinFutureBase={}",
        spotBase, usdtFutureBase, coinFutureBase);

    std::ostringstream json_stream;
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto json_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
    auto logger = std::make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});
    
    logger->set_level(spdlog::level::from_str(logLevel)); 
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(1));

    spdlog::info("Logger initialized with level: {}", logLevel);

    boost::asio::io_context ioc;

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);

    boost::asio::steady_timer t(ioc, boost::asio::chrono::seconds(request_interval));
    t.async_wait(boost::bind(fetchEndpoints, boost::asio::placeholders::error, &t, std::ref(ioc), std::ref(ctx)));

    std::thread queryThread(readQueryFileContinuously, "query.json", std::ref(ioc));
    ioc.run();

    queryThread.join();
    return 0;
}