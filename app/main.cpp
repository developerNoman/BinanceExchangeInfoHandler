// #include <rapidjson/document.h>
// #include <rapidjson/filereadstream.h>
// #include <cstdio>
// #include <iostream>
// #include <string>
// #include <vector>
// #include <thread>
// #include <boost/asio.hpp>
// #include <boost/bind/bind.hpp>
// using namespace std;

// void spotData(string, string);
// void usdFutureData(string, string);
// void coinFutureData(string, string);
// void processQueries(const rapidjson::Document &);
// void readQueryFile(const string&, rapidjson::Document &);

// string spotBase, usdtFutureBase, coinFutureBase;
// string spotTarget, usdtFutureTarget, coinFutureTarget;
// int request_interval;

// void readConfig(string configFile, rapidjson::Document &doc) {

//     FILE* fp = fopen(configFile.c_str(), "r");
//     if (!fp) {
//         cerr << "Error: unable to open file" << endl;
//     }

//     char buffer[65536];
//     rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));
//     doc.ParseStream(is);

//     spotBase = doc["exchange_base_url"]["spotBase"].GetString();
//     usdtFutureBase = doc["exchange_base_url"]["usdtFutureBase"].GetString();
//     coinFutureBase = doc["exchange_base_url"]["coinFutureBase"].GetString();
//     spotTarget = doc["exchange_endpoints"]["spotTarget"].GetString();
//     usdtFutureTarget = doc["exchange_endpoints"]["usdtFutureTarget"].GetString();
//     coinFutureTarget = doc["exchange_endpoints"]["coinFutureTarget"].GetString();
//     request_interval = doc["request_interval"].GetInt();
    
//     fclose(fp);
// }

// int main() {

//     rapidjson::Document doc;
//     readConfig("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json", doc);
//     spotData(spotBase,spotTarget);
//     usdFutureData(usdtFutureBase,usdtFutureTarget);
//     coinFutureData(coinFutureBase,coinFutureTarget);
    
//     rapidjson::Document doc1;
//     readQueryFile("/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/query.json", doc1);
//     processQueries(doc1);
    
//     return 0;
// }