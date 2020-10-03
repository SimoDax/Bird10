#pragma once

#include "logger.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;


// --------------------------------------
// static members initialization
// --------------------------------------

const string Logger::PRIORITY_NAMES[] =
{
    "DEBUG",
    "CONFIG",
    "INFO",
    "WARNING",
    "ERROR"
};

Logger Logger::instance;


// --------------------------------------
// function implementations
// --------------------------------------

Logger::Logger() : active(false) {}

void Logger::Start(Priority minPriority, const string& logFile)
{
    instance.active = true;
    instance.minPriority = minPriority;
    if (logFile != "")
    {
        instance.fileStream.open(logFile.c_str(), std::ofstream::out | std::ofstream::app);
    }
}

void Logger::Stop()
{
    instance.active = false;
    if (instance.fileStream.is_open())
    {
        instance.fileStream.close();
    }
}

void Logger::Write(Priority priority, const QString& message)
{
    if (instance.active && priority >= instance.minPriority)
    {
        // identify current output stream
        ostream& stream
            = instance.fileStream.is_open() ? instance.fileStream : std::cout;

        stream  << PRIORITY_NAMES[priority]
                << ": "
                << message.toStdString()
                << endl;
    }
}
