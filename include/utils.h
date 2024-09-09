#ifndef UTILS_H
#define UTILS_H

#include <iostream>
struct utils
{
    std::string logLevel;
    bool logToFile, logToConsole;
    std::string spotBase, usdtFutureBase, coinFutureBase;
    std::string spotTarget, usdtFutureTarget, coinFutureTarget;
    int request_interval;
};

#endif