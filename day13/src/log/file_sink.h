#ifndef FILE_SINK_H
#define FILE_SINK_H

#include "sink.h"
#include <fstream>
#include <mutex>

class FileSink : public Sink
{
public:
    FileSink();
    ~FileSink();
    FileSink(const std::string &file_name, size_t max_size, size_t max_lines);

    void log(const std::string &message) override;

    // 检查当前文件是否需要轮替
    void check_file();

    std::ios_base::openmode file_open_mode1 = std::ios::out | std::ios::trunc;

private:
    std::fstream file; // 文件输入输出流
    std::string file_name;
    size_t max_size;  // 文件大小的最大值
    size_t max_lines; // 文件最大行数
    size_t current_size = 0;
    size_t current_lines = 0;

    std::mutex file_mutex;
};

#endif