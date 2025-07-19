
#pragma once
#include <cstdio>
#include <format>
#include <iostream>

class Logger {
  public:
    /* log if input log level is equal or smaller to log level set */
    template <typename... Args>
    static void log(unsigned int logLevel, std::format_string<Args...> fmt,Args&& ... args) {
      if (logLevel <= mLogLevel) {
        auto formatted = std::format(fmt, std::forward<Args>(args)...);
        std::fputs(formatted.c_str(), stdout);
      }
    }

    static void setLogLevel(unsigned int inLogLevel) {
      inLogLevel <= 9 ? mLogLevel = inLogLevel : mLogLevel = 9;
    }

  private:
    inline static unsigned int mLogLevel = 0;
};
