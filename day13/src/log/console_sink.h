#ifndef CONSOLE_SINK_H
#define CONSOLE_SINK_H


#include "sink.h"

class ConsoleSink : public Sink
{
public:
    ConsoleSink();
    ~ConsoleSink();
    void log(const std::string &message) override;
};

#endif