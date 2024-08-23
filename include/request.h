#ifndef REQUEST_H
#define REQUEST_H

#include "example/common/root_certificates.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <rapidjson/document.h>
#include <map>
#include <string>
#include <iostream>

using namespace std;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

// Define the symbolInfo structure
struct symbolInfo {
    std::string symbol;
    std::string quoteAsset;
    std::string status;
    std::string tickSize;
    std::string stepSize;
};

// Define the exchangeInfo structure
class exchangeInfo {
public:
    std::map<std::string, symbolInfo> spotSymbols;
    std::map<std::string, symbolInfo> usdSymbols;
    std::map<std::string, symbolInfo> coinSymbols;
};
extern exchangeInfo binanceExchange;

template<typename MapType>
void parseSymbols(const std::string& responseBody, MapType& symbolsMap) {
    rapidjson::Document fullData;
    fullData.Parse(responseBody.c_str());

    if (!fullData.IsObject() || !fullData.HasMember("symbols") || !fullData["symbols"].IsArray()) {
        std::cerr << "Invalid JSON format or missing symbols array." << std::endl;
        return;
    }

    const auto& symbolsArray = fullData["symbols"];
    for (const auto& symbol : symbolsArray.GetArray()) {
        symbolInfo info;
        info.symbol = symbol["symbol"].GetString();
        info.quoteAsset = symbol["quoteAsset"].GetString();
        info.status = symbol.HasMember("status") ? symbol["status"].GetString() : "";
        for (const auto& filter : symbol["filters"].GetArray()) {
            std::string filterType = filter["filterType"].GetString();
            if (filterType == "PRICE_FILTER") {
                info.tickSize = filter["tickSize"].GetString();
            } else if (filterType == "LOT_SIZE") {
                info.stepSize = filter["stepSize"].GetString();
            }
        }
        symbolsMap[info.symbol] = info;
    }
}

void fail(beast::error_code ec, char const* what);
void load_root_certificates(boost::asio::ssl::context& ctx);

class session : public enable_shared_from_this<session> {
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::ssl::stream<boost::beast::tcp_stream> stream_;
    boost::beast::flat_buffer buffer_;
    boost::beast::http::request<boost::beast::http::empty_body> req_;
    boost::beast::http::response<boost::beast::http::string_body> res_;
    boost::beast::http::response<boost::beast::http::string_body> result_;

public:
    explicit session(net::any_io_executor ex, ssl::context& ctx)
    : resolver_(ex), stream_(ex, ctx) {}

    boost::beast::http::response<boost::beast::http::string_body> returnResponse() const {
        return result_;
    }

    void run(char const* host, char const* port, char const* target, int version) {
        if(! SSL_set_tlsext_host_name(stream_.native_handle(), host)) {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            cerr << ec.message() << "\n";
            return;
        }
        req_.version(version);
        req_.method(http::verb::get);
        req_.target(target);
        req_.set(http::field::host, host);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        resolver_.async_resolve(host, port, beast::bind_front_handler(&session::on_resolve, shared_from_this()));
    }

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
        if (ec) return fail(ec, "resolve");
        beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));
        beast::get_lowest_layer(stream_).async_connect(results, beast::bind_front_handler(&session::on_connect, shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
        if (ec) return fail(ec, "connect");
        stream_.async_handshake(ssl::stream_base::client, beast::bind_front_handler(&session::on_handshake, shared_from_this()));
    }

    void on_handshake(beast::error_code ec) {
        if (ec) return fail(ec, "handshake");
        beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));
        http::async_write(stream_, req_, beast::bind_front_handler(&session::on_write, shared_from_this()));
    }

    void on_write(beast::error_code ec, size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec) return fail(ec, "write");
        http::async_read(stream_, buffer_, res_, beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        if (ec) return fail(ec, "read");
        result_ = res_;
        beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));
        stream_.async_shutdown(beast::bind_front_handler(&session::on_shutdown, shared_from_this()));
    }

    void on_shutdown(beast::error_code ec) {
        if (ec != net::ssl::error::stream_truncated) return fail(ec, "shutdown");
    }
};

#endif // REQUEST_H
