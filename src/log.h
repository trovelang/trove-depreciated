#pragma once

#include <iostream>

class Logger {
public:

    enum class LogLevel {
        DBG,
        WARN,
        INFO,
        ERR
    };

    const char* RED = "\033[1;31m";
    const char* GREEN = "\033[1;32m";  
    const char* YELLOW = "\033[1;33m"; 
    const char* END = "\033[0m";

    inline Logger& info(){
        std::cout << GREEN << "[INFO] " << END;
        return *this;
    }

    inline Logger& warn(){
        std::cout << YELLOW << "[WARN] " << END;
        return *this;
    }

    inline Logger& errr(){
        std::cout << RED << "[ERRR] " << END;
        return *this;
    }

    template<class T>
    Logger& operator<<(const T& msg) {
        std::cout << msg;
        return *this;
    }

private:

};

extern Logger logger;
