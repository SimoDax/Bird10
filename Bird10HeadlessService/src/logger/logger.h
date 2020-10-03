#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <QString>

using namespace std;

class Logger
{
public:
    // log priorities
    enum Priority
    {
        DEBUG,
        CONFIG,
        INFO,
        WARNING,
        ERROR
    };

    // start/stop logging
    // - messages with priority >= minPriority will be written in log
    // - set logFile = "" to write to standard output
    static void Start(Priority minPriority, const string& logFile);
    static void Stop();

    // write message
    static void Write(Priority priority, const QString& message);


private:
    // Logger adheres to the singleton design pattern, hence the private
    // constructor, copy constructor and assignment operator.
    Logger();
    Logger(const Logger& logger) {}
    Logger& operator = (const Logger& logger) {}

    // private instance data
    bool        active;
    ofstream    fileStream;
    Priority    minPriority;

    // names describing the items in enum Priority
    static const string PRIORITY_NAMES[];
    // the sole Logger instance (singleton)
    static Logger instance;
};


#ifdef QT_DEBUG
#define ENABLE_LOGGER
#endif

#ifdef ENABLE_LOGGER

#define LOGGER_START(MIN_PRIORITY, FILE) Logger::Start(MIN_PRIORITY, FILE);
#define LOGGER_STOP() Logger::Stop();
#define LOGGER_WRITE(PRIORITY, MESSAGE) Logger::Write(PRIORITY, MESSAGE);
#define LOG(MESSAGE) Logger::Write(Logger::INFO, MESSAGE);

#else

#define LOGGER_START(MIN_PRIORITY, FILE)
#define LOGGER_STOP()
#define LOGGER_WRITE(PRIORITY, MESSAGE)
#define LOG(MESSAGE)

#endif
