#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
using namespace std;

void spotData(string, string);
void usdFutureData(string, string);
void coinFutureData(string, string);

string spotBase, usdtFutureBase, coinFutureBase;
string spotTarget, usdtFutureTarget, coinFutureTarget;
int request_interval;
void readConfig(string configFile, rapidjson::Document &doc) {
    FILE* fp = fopen(configFile.c_str(), "r");
    if (!fp) {
        cerr << "Error: unable to open file" << endl;
    }
    char buffer[65536];
    rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
    doc.ParseStream(is);
    spotBase = doc["exchange_base_url"]["spotBase"].GetString();
    usdtFutureBase = doc["exchange_base_url"]["usdtFutureBase"].GetString();
    coinFutureBase = doc["exchange_base_url"]["coinFutureBase"].GetString();
    spotTarget = doc["exchange_endpoints"]["spotTarget"].GetString();
    usdtFutureTarget = doc["exchange_endpoints"]["usdtFutureTarget"].GetString();
    coinFutureTarget = doc["exchange_endpoints"]["coinFutureTarget"].GetString();
    request_interval = doc["request_interval"].GetInt();
    fclose(fp);
}

void fetchAll(const boost::system::error_code& /*e*/, boost::asio::steady_timer* t){
    thread spotThread (spotData, spotBase, spotTarget);
    thread usdFutureThread (usdFutureData, usdtFutureBase, usdtFutureTarget);
    thread coinFutureThread (coinFutureData, coinFutureBase, coinFutureTarget);
    spotThread.join();
    usdFutureThread.join();
    coinFutureThread.join();
    t->expires_at(t->expiry() + boost::asio::chrono::seconds(20));
    t->async_wait(boost::bind(fetchAll, boost::asio::placeholders::error, t));
}

int main() {
    rapidjson::Document doc;
    readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc);
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(20));
    t.async_wait(boost::bind(fetchAll, boost::asio::placeholders::error, &t));
    io.run();
    return 0;
}