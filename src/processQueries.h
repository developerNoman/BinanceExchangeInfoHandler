#ifndef PROCESSQUERIES_H
#define PROCESSQUERIES_H

#include "marketInfo.h"
#include <vector>
#include <mutex>


void display(const std::string &, const std::string &, const MarketInfo &);
void processQueries(const rapidjson::Document &);
void readQueryFile(const std::string &, rapidjson::Document &);
void readQueryFileContinuously(const std::string &, boost::asio::io_context &);

#endif