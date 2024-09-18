#pragma once
#include <string>

class Buffer
{
private:
    std::string buf;

public:
    Buffer();
    ~Buffer();

    const char *c_str();
    void getline();
    void append(const char *str, int size);
    int size();
    void clear();
};