#include "request.h"
#include <rapidjson/document.h>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

exchangeInfo binanceExchange;
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

void spotData(string spotBaseUrl, string spotEndpoint) {
    auto const host = spotBaseUrl.c_str();
    auto const port = "443";
    auto const target = spotEndpoint.c_str();
    int version = 11;
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> spotExchange = sessionPtr->returnResponse();
    parseSymbols(spotExchange.body(), binanceExchange.spotSymbols);
}

void usdFutureData(string usdFutureBaseUrl, string usdFutureEndpoint) {
    auto const host = usdFutureBaseUrl.c_str();
    auto const port = "443";
    auto const target = usdFutureEndpoint.c_str();
    int version = 11;
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> usdFutureExchange = sessionPtr->returnResponse();
    parseSymbols(usdFutureExchange.body(), binanceExchange.usdSymbols);

}

void coinFutureData(string coinFutureBaseUrl, string coinFutureEndpoint) {
    auto const host = coinFutureBaseUrl.c_str();
    auto const port = "443";
    auto const target = coinFutureEndpoint.c_str();
    int version = 11;
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};

    load_root_certificates(ctx);
    ctx.set_verify_mode(ssl::verify_peer);

    auto sessionPtr = make_shared<session>(net::make_strand(ioc), ctx);
    sessionPtr->run(host, port, target, version);

    ioc.run();
    http::response<http::string_body> coinFutureExchange = sessionPtr->returnResponse();
    parseSymbols(coinFutureExchange.body(), binanceExchange.coinSymbols);
             for (const auto& pair : binanceExchange.coinSymbols) {
        const auto& symbolInfo = pair.second;
        cout << "Symbol: " << symbolInfo.symbol << endl;
        cout << "Quote Asset: " << symbolInfo.quoteAsset << endl;
        cout << "Status: " << symbolInfo.status << endl;
        cout << "Tick Size: " << symbolInfo.tickSize << endl;
        cout << "Step Size: " << symbolInfo.stepSize << endl;
    }
}
