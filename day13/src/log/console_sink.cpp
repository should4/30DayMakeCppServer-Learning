#include "console_sink.h"
#include <iostream>

ConsoleSink::ConsoleSink() {}
ConsoleSink::~ConsoleSink() {}

void ConsoleSink::log(const std::string &message)
{
    std::cout << message << std::endl;
}