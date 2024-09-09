#include "processData.h"

using namespace std;

int main()
{
    // object creations for method accessing
    processEndpoints pe;
    processResponse pr;

    rapidjson::Document doc1;
    pe.readConfig("config.json", doc1);
    spdlog::debug("Configuration loaded: spotBase={}, usdtFutureBase={}, coinFutureBase={}", spotBase, usdtFutureBase, coinFutureBase);

    vector<spdlog::sink_ptr> sinkData;

    if (logToConsole)
    {
        auto console_sink = make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinkData.push_back(console_sink);
    }

    if (logToFile)
    {
        auto file_sink = make_shared<spdlog::sinks::basic_file_sink_mt>("logfile.log");
        sinkData.push_back(file_sink);
    }

    auto logger = make_shared<spdlog::logger>("BinanceExchangeEndpoints", begin(sinkData), end(sinkData));

    logger->set_level(spdlog::level::from_str(logLevel));
    spdlog::set_default_logger(logger);
    spdlog::flush_every(chrono::seconds(1));

    spdlog::debug("Logger initialized with level: {}", logLevel);

    boost::asio::io_context ioc;

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);

    boost::asio::steady_timer fetchTimer(ioc, boost::asio::chrono::seconds(request_interval));
    fetchTimer.async_wait(boost::bind(&processEndpoints::fetchEndpoints, &pe, boost::asio::placeholders::error, &fetchTimer, ref(ioc), ref(ctx)));

    thread queryThread(&processResponse::readQueryFileContinuously, &pr, "query.json", ref(ioc));
    ioc.run();

    queryThread.join();
    return 0;
}
