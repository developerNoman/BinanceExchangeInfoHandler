// #include "spdlog/spdlog.h"

// int main() 
// {
//     spdlog::info("Welcome to spdlog!");
    
//     spdlog::warn("Easy padding in numbers like {:08d}", 12);
//     spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
//     spdlog::info("Support for floats {:03.2f}", 1.23456);
//     spdlog::info("Positional args are {1} {0}..", "too", "supported");
//     spdlog::info("{:<30}", "left aligned");
    
//     spdlog::set_level(spdlog::level::debug); // Set global log level to debug
//     spdlog::debug("This message should be displayed..");    
    
//     // change log pattern
//     spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    

//     SPDLOG_TRACE("Some trace message with param {}", 42);
//     SPDLOG_DEBUG("Some debug message");
// }

// rapidjson/example/simpledom/simpledom.cpp`
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

using namespace rapidjson;

int main() {
   
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    std::cout << buffer.GetString() << std::endl;
    return 0;
}
