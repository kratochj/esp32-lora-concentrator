#pragma once


#include "Arduino.h"

#if defined(ARDUINO_ARCH_AVR)
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

class Logger
{
public:
    enum Level
    {
        VERBOSE = 0,
        NOTICE,
        WARNING,
        ERROR,
        FATAL,
        SILENT
    };

    typedef void (*LoggerOutputFunction)(Level level,
                                         const char* module,
                                         const char* message);

    static void setLogLevel(Level level);

    static Level getLogLevel();

    static void verbose(const char* message);
    static void verbose(const String message);
    static void notice(const char* message);
    static void notice(const String message);
    static void warning(const char* message);
    static void warning(const String message);
    static void error(const char* message);
    static void error(const String message);
    static void fatal(const char* message);
    static void fatal(const String message);

    static void verbose(const char* module, const char* message);
    static void verbose(const String module, const String message);
    static void notice(const char* module, const char* message);
    static void notice(const String module, const String message);
    static void warning(const char* module, const char* message);
    static void warning(const String module, const String message);
    static void error(const char* module, const char* message);
    static void error(const String module, const String message);
    static void fatal(const char* module, const char* message);
    static void fatal(const String module, const String message);

    static void log(Level level, const char* message);
    static void log(Level level, const String message);

    static void log(Level level, const char* module, const char* message);
    static void log(Level level, const String module, const String message);

    static void setOutputFunction(LoggerOutputFunction loggerOutputFunction);

    static Logger& getInstance();

    static const char* asString(Level level);

private:
    Logger();
    Logger(const Logger&);
    void operator = (const Logger&);

    static void defaultLog(Level level, const char* module, const char* message);

    Level _level;

    LoggerOutputFunction _loggerOutputFunction;

};
