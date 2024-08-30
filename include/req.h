#ifndef REQ_H
#define REQ_H

#include "example/common/root_certificates.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/asio/strand.hpp"
#include "boost/asio/io_context.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "rapidjson/error/en.h"
#include "boost/asio.hpp"
#include "rapidjson/document.h"
#include <memory>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <mutex>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;


extern std::string logLevel;
extern std::string spotBase, usdtFutureBase, coinFutureBase;
extern std::string spotTarget, usdtFutureTarget, coinFutureTarget;
extern int request_interval;
extern std::mutex myMutex;

// Define the MarketInfo structure
struct MarketInfo {
    std::string symbol;
    std::string quoteAsset;
    std::string status;
    std::string tickSize;
    std::string stepSize;
};

// Define the exchangeSymbols structure
class exchangeSymbols {
public:
    std::map<std::string, MarketInfo> spotSymbols;
    std::map<std::string, MarketInfo> usdSymbols;
    std::map<std::string, MarketInfo> coinSymbols;
};
extern exchangeSymbols exchangeData;

void parseSymbols(std::string &responseBody, std::map<std::string, MarketInfo> *symbolsMap);


void fail(beast::error_code ec, char const *what);
void load_root_certificates(ssl::context &ctx);

class session : public std::enable_shared_from_this<session> {
    tcp::resolver resolver_;
    ssl::stream<beast::tcp_stream> stream_;
    beast::flat_buffer buffer_;
    http::request<http::empty_body> req_;
    http::response<http::string_body> res_;
    std::string host_;

public:
    explicit session(net::any_io_executor ex, ssl::context &ctx)
        : resolver_(ex), stream_(ex, ctx) {}

    void run(const char *host, const char *port, const char *target, int version);
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type);
    void on_handshake(beast::error_code ec);
    void on_write(beast::error_code ec, std::size_t bytes_transferred);
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_shutdown(beast::error_code ec);
};

#endif
