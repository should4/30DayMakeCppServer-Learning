#ifndef SINK_H
#define SINK_H

#include <string>

class Sink
{
public:
    Sink() {};
    ~Sink() {};
    virtual void log(const std::string &message) = 0;
};
#endif