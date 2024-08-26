// #ifndef REQUEST_H
// #define REQUEST_H

// #include "example/common/root_certificates.hpp"
// #include <boost/beast/core.hpp>
// #include <boost/beast/http.hpp>
// #include <boost/beast/version.hpp>
// #include <boost/asio/ssl.hpp>
// #include <boost/asio/strand.hpp>
// #include <boost/asio/io_context.hpp>
// #include <boost/asio/ip/tcp.hpp>
// #include <boost/asio.hpp>
// #include <memory>
// #include <rapidjson/document.h>
// #include <map>
// #include <string>
// #include <iostream>

// using namespace std;
// namespace beast = boost::beast;
// namespace http = beast::http;
// namespace net = boost::asio;
// namespace ssl = boost::asio::ssl;
// using tcp = boost::asio::ip::tcp;

// // Define the symbolInfo structure
// struct symbolInfo {
//     std::string symbol;
//     std::string quoteAsset;
//     std::string status;
//     std::string tickSize;
//     std::string stepSize;
// };

// // Define the exchangeInfo structure
// class exchangeInfo {
// public:
//     std::map<std::string, symbolInfo> spotSymbols;
//     std::map<std::string, symbolInfo> usdSymbols;
//     std::map<std::string, symbolInfo> coinSymbols;
// };
// extern exchangeInfo binanceExchange;

// template<typename MapType>

// void parseSymbols(const std::string& responseBody, MapType& symbolsMap) {
//     rapidjson::Document fullData;
//     fullData.Parse(responseBody.c_str());

//     if (!fullData.IsObject() || !fullData.HasMember("symbols") || !fullData["symbols"].IsArray()) {
//         std::cerr << "Invalid JSON format or missing symbols array." << std::endl;
//         std::cerr << "Response Body: " << responseBody << std::endl;
//         return;
//     }

//     const auto& symbolsArray = fullData["symbols"];
//     for (const auto& symbol : symbolsArray.GetArray()) {
//         symbolInfo info;
//         info.symbol = symbol["symbol"].GetString();
//         info.quoteAsset = symbol["quoteAsset"].GetString();
//         if (symbol.HasMember("status")) {
//             info.status = symbol["status"].GetString();
//         } else if (symbol.HasMember("contractStatus")) {
//             info.status = symbol["contractStatus"].GetString();
//         }
//         for (const auto& filter : symbol["filters"].GetArray()) {
//             std::string filterType = filter["filterType"].GetString();
//             if (filterType == "PRICE_FILTER") {
//                 info.tickSize = filter.HasMember("tickSize") ? filter["tickSize"].GetString() : "";
//             } else if (filterType == "LOT_SIZE") {
//                 info.stepSize = filter.HasMember("stepSize") ? filter["stepSize"].GetString() : "";
//             }
//         }
//         symbolsMap[info.symbol] = info;
//     }
// }


// void fail(beast::error_code ec, char const* what);
// void load_root_certificates(boost::asio::ssl::context& ctx);

// class session : public enable_shared_from_this<session> {
//     boost::asio::ip::tcp::resolver resolver_;
//     boost::asio::ssl::stream<boost::beast::tcp_stream> stream_;
//     boost::beast::flat_buffer buffer_;
//     boost::beast::http::request<boost::beast::http::empty_body> req_;
//     boost::beast::http::response<boost::beast::http::string_body> res_;
//     boost::beast::http::response<boost::beast::http::string_body> result_;

// public:
//     explicit session(net::any_io_executor ex, ssl::context& ctx)
//     : resolver_(ex), stream_(ex, ctx) {}

//     boost::beast::http::response<boost::beast::http::string_body> returnResponse() const {
//         return result_;
//     }

//     void run(char const* host, char const* port, char const* target, int version) {
//         if(! SSL_set_tlsext_host_name(stream_.native_handle(), host)) {
//             beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
//             cerr << ec.message() << "\n";
//             return;
//         }
//         req_.version(version);
//         req_.method(http::verb::get);
//         req_.target(target);
//         req_.set(http::field::host, host);
//         req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

//         resolver_.async_resolve(host, port, beast::bind_front_handler(&session::on_resolve, shared_from_this()));
//     }

//     void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
//         if (ec) return fail(ec, "resolve");
//         beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));
//         beast::get_lowest_layer(stream_).async_connect(results, beast::bind_front_handler(&session::on_connect, shared_from_this()));
//     }

//     void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
//         if (ec) return fail(ec, "connect");
//         stream_.async_handshake(ssl::stream_base::client, beast::bind_front_handler(&session::on_handshake, shared_from_this()));
//     }

//     void on_handshake(beast::error_code ec) {
//         if (ec) return fail(ec, "handshake");
//         beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));
//         http::async_write(stream_, req_, beast::bind_front_handler(&session::on_write, shared_from_this()));
//     }

//     void on_write(beast::error_code ec, size_t bytes_transferred) {
//         boost::ignore_unused(bytes_transferred);
//         if (ec) return fail(ec, "write");
//         http::async_read(stream_, buffer_, res_, beast::bind_front_handler(&session::on_read, shared_from_this()));
//     }

//     void on_read(beast::error_code ec, size_t bytes_transferred) {
//         boost::ignore_unused(bytes_transferred);
//         if (ec) return fail(ec, "read");
//         result_ = res_;
//         beast::get_lowest_layer(stream_).expires_after(chrono::seconds(30));
//         stream_.async_shutdown(beast::bind_front_handler(&session::on_shutdown, shared_from_this()));
//     }

//     void on_shutdown(beast::error_code ec) {
//         if (ec != net::ssl::error::stream_truncated) return fail(ec, "shutdown");
//     }
// };
// #endif

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
#include <rapidjson/error/en.h>  
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
    std::map<std::string, symbolInfo> spotMarketSymbols;
    std::map<std::string, symbolInfo> usdMarketSymbols;
    std::map<std::string, symbolInfo> coinMarketSymbols;
};
extern exchangeInfo myExchangeData;

template<typename MapType>


void parseSymbols(const std::string& responseBody, MapType& symbolsMap) {
    // Check if the response body is empty
    if (responseBody.empty()) {
        std::cerr << "Error: The response body is empty." << std::endl;
        return;
    }

    rapidjson::Document fullData;
    rapidjson::ParseResult parseResult = fullData.Parse(responseBody.c_str());

    // Check if JSON is parsed correctly
    if (!parseResult) {
        std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(parseResult.Code())
                  << " (offset " << parseResult.Offset() << ")" << std::endl;
        std::cerr << "Response Body: " << responseBody << std::endl;
        return;
    }

    // Check if the fullData is an object and has the "symbols" array
    if (!fullData.IsObject() || !fullData.HasMember("symbols") || !fullData["symbols"].IsArray()) {
        std::cerr << "Invalid JSON format or missing 'symbols' array." << std::endl;
        std::cerr << "Response Body: " << responseBody << std::endl;
        return;
    }

    const auto& symbolsArray = fullData["symbols"];
    for (const auto& symbol : symbolsArray.GetArray()) {
        symbolInfo info;

        // Safely extract the "symbol" and "quoteAsset" fields
        if (symbol.HasMember("symbol") && symbol["symbol"].IsString()) {
            info.symbol = symbol["symbol"].GetString();
        } else {
            std::cerr << "Missing or invalid 'symbol' field in a symbol object." << std::endl;
            continue;
        }

        if (symbol.HasMember("quoteAsset") && symbol["quoteAsset"].IsString()) {
            info.quoteAsset = symbol["quoteAsset"].GetString();
        } else {
            std::cerr << "Missing or invalid 'quoteAsset' field in symbol: " << info.symbol << std::endl;
            continue;
        }

        // Handle status/contractStatus
        if (symbol.HasMember("status") && symbol["status"].IsString()) {
            info.status = symbol["status"].GetString();
        } else if (symbol.HasMember("contractStatus") && symbol["contractStatus"].IsString()) {
            info.status = symbol["contractStatus"].GetString();
        } else {
            std::cerr << "Missing 'status' or 'contractStatus' in symbol: " << info.symbol << std::endl;
        }

        // Handle filters
        if (symbol.HasMember("filters") && symbol["filters"].IsArray()) {
            for (const auto& filter : symbol["filters"].GetArray()) {
                if (filter.HasMember("filterType") && filter["filterType"].IsString()) {
                    std::string filterType = filter["filterType"].GetString();
                    if (filterType == "PRICE_FILTER") {
                        info.tickSize = filter.HasMember("tickSize") && filter["tickSize"].IsString() ?
                                        filter["tickSize"].GetString() : "";
                    } else if (filterType == "LOT_SIZE") {
                        info.stepSize = filter.HasMember("stepSize") && filter["stepSize"].IsString() ?
                                        filter["stepSize"].GetString() : "";
                    }
                }
            }
        } else {
            std::cerr << "Missing or invalid 'filters' array in symbol: " << info.symbol << std::endl;
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
#endif

