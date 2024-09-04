#ifndef PROCESSQUERIES_H
#define PROCESSQUERIES_H

#include "marketInfo.h"
#include <vector>
#include <mutex>

void display(const std::string &marketType, const std::string &instrumentName, const MarketInfo &MarketInfo);

void processQueries(const rapidjson::Document &doc);

void readQueryFile(const std::string &queryFile, rapidjson::Document &doc1);

void readQueryFileContinuously(const std::string &queryFile, boost::asio::io_context &ioc);

#endif