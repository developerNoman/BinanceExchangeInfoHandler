#ifndef FETCHDATA_H
#define FETCHDATA_H

#include "marketInfo.h"
#include <chrono>

namespace net = boost::asio;

class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    ssl::stream<beast::tcp_stream> stream_;
    beast::flat_buffer buffer_;
    http::request<http::empty_body> req_;
    http::response<http::string_body> res_;
    std::string host_;

public:
    explicit session(net::any_io_executor ex, ssl::context &ctx)
        : resolver_(ex), stream_(ex, ctx) {}

    void fail(beast::error_code ec, char const *what);
    void run(const char *host, const char *port, const char *target, int version);
    void onResolve(beast::error_code ec, tcp::resolver::results_type results);
    void onConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void onHandshake(beast::error_code ec);
    void onWrite(beast::error_code ec, std::size_t bytes_transferred);
    void onRead(beast::error_code ec, std::size_t bytes_transferred);
    void onShutdown(beast::error_code ec);
};

// functions that are used to process the data from endpoints
class processEndpoints
{
public:
    void parseSymbols(std::string &, const std::string &, exchangeSymbols &);
    void readConfig(const std::string &, rapidjson::Document &);
    void fetchData(const std::string &, const std::string &, net::io_context &, ssl::context &);
    void fetchSpotData(net::io_context &, ssl::context &);
    void fetchUsdFutureData(net::io_context &, ssl::context &);
    void fetchCoinFutureData(net::io_context &, ssl::context &);
    void fetchEndpoints(const boost::system::error_code &, boost::asio::steady_timer *, boost::asio::io_context &, ssl::context &);
};

#endif
