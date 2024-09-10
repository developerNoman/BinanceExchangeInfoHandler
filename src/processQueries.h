#ifndef PROCESSQUERIES_H
#define PROCESSQUERIES_H

#include <vector>
#include "exchangeSymbols.h"
#include <mutex>

class processResponse
{
public:
    void display(const std::string &, const std::string &, const MarketInfo &, rapidjson::Value &, rapidjson::Document::AllocatorType &);
    void processQueries(const rapidjson::Document &);
    void readQueryFile(const std::string &, rapidjson::Document &);
    void readQueryFileContinuously(const std::string &, boost::asio::io_context &);
};

#endif