#ifndef REQUEST_H
#define REQUEST_H

#include "boost/asio.hpp"
#include "boost/asio/ssl.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "rapidjson/document.h"
#include <map>
#include <string>
#include <mutex>

// External variables
extern std::mutex myMutex;

// Structures and classes
struct MarketInfo {
    std::string symbol;
    std::string quoteAsset;
    std::string status;
    std::string tickSize;
    std::string stepSize;
};

class exchangeSymbols {
public:
    std::map<std::string, MarketInfo> spotSymbols;
    std::map<std::string, MarketInfo> usdSymbols;
    std::map<std::string, MarketInfo> coinSymbols;
};

extern exchangeSymbols exchangeData;

// Function declarations
void parseSymbols(std::string &responseBody, std::map<std::string, MarketInfo> *symbolsMap);
void fail(boost::beast::error_code ec, char const *what);

class session : public std::enable_shared_from_this<session> {
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ssl::stream<boost::beast::tcp_stream> stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::empty_body> req_;
    boost::beast::http::response<boost::beast::http::string_body> res_;
    std::string host_;

public:
    explicit session(boost::asio::any_io_executor ex, boost::asio::ssl::context &ctx)
        : resolver_(ex), stream_(ex, ctx) {}

    void run(const char *host, const char *port, const char *target, int version);
    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type);
    void on_handshake(boost::beast::error_code ec);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_shutdown(boost::beast::error_code ec);
};

#endif // REQUEST_H
