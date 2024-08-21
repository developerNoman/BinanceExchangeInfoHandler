#include "context.h"
using namespace std;

int main()
{
    string config_file = "/home/noman-shafique/Training/Tasks/BinanceExchangeInfoHandler/config.json";

    rapidjson::Document config;
    try {
        config = session::load_config(config_file);
        cout << "Config loaded successfully." << endl;
    } catch (const exception& e) {
        cerr << "Error loading config: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (!config.HasMember("exchange_endpoints") || !config["exchange_endpoints"].IsArray()) {
        cerr << "Config file is missing 'exchange_endpoints' field or it's not an array." << endl;
        return EXIT_FAILURE;
    }

    auto& endpoints = config["exchange_endpoints"];
    vector<tuple<string, string, string>> endpoint_list;

    int port = 443; 

    for (const auto& endpoint : endpoints.GetArray()) {
        if (endpoint.HasMember("host") && endpoint.HasMember("target") &&
            endpoint["host"].IsString() && endpoint["target"].IsString()) {
            
            string host = endpoint["host"].GetString();
            string target = endpoint["target"].GetString();

            cout << "Endpoint loaded: " << host << ":" << port << target << endl;
            endpoint_list.emplace_back(host, to_string(port), target);
        }
    }

    if (endpoint_list.empty()) {
        cerr << "No valid endpoints found in config file." << endl;
        return EXIT_FAILURE;
    }

    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    for (const auto& [host, port_str, target] : endpoint_list)
    {
        int port = stoi(port_str); 
        cout << "Requesting: " << host << ":" << port << target << endl;
        make_shared<session>(
            net::make_strand(ioc),
            ctx
        )->run(host.c_str(), to_string(port).c_str(), target.c_str(), 11);

        this_thread::sleep_for(chrono::seconds(config["request_interval"].GetInt()));
    }

    ioc.run();

    return EXIT_SUCCESS;
}



