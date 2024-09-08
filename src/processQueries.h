#ifndef PROCESSQUERIES_H
#define PROCESSQUERIES_H

#include "marketInfo.h"
#include <vector>
#include <mutex>

class processResponse
{
public:
    void display(const std::string &marketType, const std::string &instrumentName, const MarketInfo &marketInfo, rapidjson::Value &resultObj, rapidjson::Document::AllocatorType &allocator);
    void processQueries(const rapidjson::Document &);
    void readQueryFile(const std::string &, rapidjson::Document &);
    void readQueryFileContinuously(const std::string &, boost::asio::io_context &);
};

#endif