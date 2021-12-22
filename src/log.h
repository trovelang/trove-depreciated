#pragma once

#include <iostream>

class Logger {
public:

    enum class LogLevel {
        DEBUG,
        WARN,
        INFO,
        ERR
    };

    template<class T>
    Logger& operator<<(const T& msg) {
        std::cout << msg;
        return *this;
    }

private:

};

extern Logger logger;
