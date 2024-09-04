#include "processData.h"

using namespace std;

int main()
{
    rapidjson::Document doc1;
    readConfig("config.json", doc1);
    spdlog::debug("Configuration loaded: spotBase={}, usdtFutureBase={}, coinFutureBase={}", spotBase, usdtFutureBase, coinFutureBase);

    ostringstream json_stream;
    auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto json_sink = make_shared<spdlog::sinks::ostream_sink_mt>(json_stream);
    auto logger = make_shared<spdlog::logger>("logger", spdlog::sinks_init_list{console_sink, json_sink});

    logger->set_level(spdlog::level::from_str(logLevel));
    spdlog::set_default_logger(logger);
    spdlog::flush_every(chrono::seconds(1));

    spdlog::debug("Logger initialized with level: {}", logLevel);

    boost::asio::io_context ioc;

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);

    boost::asio::steady_timer fetchTimer(ioc, boost::asio::chrono::seconds(request_interval));
    fetchTimer.async_wait(boost::bind(fetchEndpoints, boost::asio::placeholders::error, &fetchTimer, ref(ioc), ref(ctx)));

    thread queryThread(readQueryFileContinuously, "query.json", ref(ioc));
    ioc.run();

    queryThread.join();
    return 0;
}