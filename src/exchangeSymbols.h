#ifndef MARKETINFO_H
#define MARKETINFO_H

#include "example/common/root_certificates.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "boost/asio.hpp"
#include "boost/beast/core.hpp"
#include "boost/beast/http.hpp"
#include "boost/beast/version.hpp"
#include "boost/asio/ssl.hpp"
#include <cstdio>
#include <fstream>
#include <memory>
#include <map>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

// Defined the MarketInfo structure
struct MarketInfo
{
    std::string symbol;
    std::string quoteAsset;
    std::string status;
    std::string tickSize;
    std::string stepSize;
};

// Defined the exchangeSymbols structure
class exchangeSymbols
{
public:
    void setSpotSymbol(const std::string &symbol, const MarketInfo &info);

    void setUsdSymbol(const std::string &symbol, const MarketInfo &info);

    void setCoinSymbol(const std::string &symbol, const MarketInfo &info);

    const std::map<std::string, MarketInfo> &getSpotSymbols() const;

    const std::map<std::string, MarketInfo> &getUsdSymbols() const;

    const std::map<std::string, MarketInfo> &getCoinSymbols() const;

    void updateSpotSymbol(const std::string &symbol, const MarketInfo &info);

    void updateUsdSymbol(const std::string &symbol, const MarketInfo &info);

    void updateCoinSymbol(const std::string &symbol, const MarketInfo &info);

    void removeSpotSymbol(const std::string &instrumentName);
    void removeUsdSymbol(const std::string &instrumentName);
    void removeCoinSymbol(const std::string &instrumentName);

private:
    std::map<std::string, MarketInfo> spotSymbols_;
    std::map<std::string, MarketInfo> usdSymbols_;
    std::map<std::string, MarketInfo> coinSymbols_;
};

#endif