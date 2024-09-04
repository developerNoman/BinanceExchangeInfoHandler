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
#include <iostream>
#include <string>
#include <mutex>

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
private:
    std::map<std::string, MarketInfo> spotSymbols;
    std::map<std::string, MarketInfo> usdSymbols;
    std::map<std::string, MarketInfo> coinSymbols;

public:
    void setSpotSymbol(const std::string &symbol, const MarketInfo &info)
    {
        spotSymbols[symbol] = info;
    }

    void setUsdSymbol(const std::string &symbol, const MarketInfo &info)
    {
        usdSymbols[symbol] = info;
    }

    void setCoinSymbol(const std::string &symbol, const MarketInfo &info)
    {
        coinSymbols[symbol] = info;
    }

    const std::map<std::string, MarketInfo> &getSpotSymbols() const
    {
        return spotSymbols;
    }

    const std::map<std::string, MarketInfo> &getUsdSymbols() const
    {
        return usdSymbols;
    }

    const std::map<std::string, MarketInfo> &getCoinSymbols() const
    {
        return coinSymbols;
    }
    void removeSpotSymbol(const std::string &instrumentName)
    {
        auto data = spotSymbols.find(instrumentName);
        if (data != spotSymbols.end())
        {
            spotSymbols.erase(data);
        }
    }

    void removeUsdSymbol(const std::string &instrumentName)
    {
        auto data = usdSymbols.find(instrumentName);
        if (data != usdSymbols.end())
        {
            usdSymbols.erase(data);
        }
    }

    void removeCoinSymbol(const std::string &instrumentName)
    {
        auto data = coinSymbols.find(instrumentName);
        if (data != coinSymbols.end())
        {
            coinSymbols.erase(data);
        }
    }
};
extern exchangeSymbols exchangeData;

#endif