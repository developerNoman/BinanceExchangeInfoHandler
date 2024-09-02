
#ifndef MARKETINFO_H
#define MARKETINFO_H

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


#endif


// #ifndef MARKETINFO_H
// #define MARKETINFO_H

// #include "example/common/root_certificates.hpp"
// #include "boost/beast/core.hpp"
// #include "boost/beast/http.hpp"
// #include "boost/beast/version.hpp"
// #include "boost/asio/ssl.hpp"
// #include "boost/asio/strand.hpp"
// #include "boost/asio/io_context.hpp"
// #include "boost/asio/ip/tcp.hpp"
// #include "rapidjson/error/en.h"
// #include "boost/asio.hpp"
// #include "rapidjson/document.h"
// #include <memory>
// #include <map>
// #include <string>
// #include <iostream>
// #include <string>
// #include <mutex>

// namespace beast = boost::beast;
// namespace http = beast::http;
// namespace net = boost::asio;
// namespace ssl = boost::asio::ssl;
// using tcp = boost::asio::ip::tcp;


// // Define the MarketInfo structure
// struct MarketInfo {
//     std::string symbol;
//     std::string quoteAsset;
//     std::string status;
//     std::string tickSize;
//     std::string stepSize;
// };

// // Define the exchangeSymbols structure
// class exchangeSymbols {
// private:
//     std::map<std::string, MarketInfo> spotSymbols;
//     std::map<std::string, MarketInfo> usdSymbols;
//     std::map<std::string, MarketInfo> coinSymbols;

// public:

//     const std::map<std::string, MarketInfo>& getSpotSymbols() const {
//         return spotSymbols;
//     }

//     void setSpotSymbol(const std::string& key, const MarketInfo& value) {
//         spotSymbols[key] = value;
//     }

//     const std::map<std::string, MarketInfo>& getUsdSymbols() const {
//         return usdSymbols;
//     }

//     void setUsdSymbol(const std::string& key, const MarketInfo& value) {
//         usdSymbols[key] = value;
//     }

//     const std::map<std::string, MarketInfo>& getCoinSymbols() const {
//         return coinSymbols;
//     }

//     void setCoinSymbol(const std::string& key, const MarketInfo& value) {
//         coinSymbols[key] = value;
//     }
// };
// extern exchangeSymbols exchangeData;


// #endif
