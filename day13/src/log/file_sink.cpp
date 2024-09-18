#include "file_sink.h"
#include <fstream>

FileSink::FileSink(const std::string &file_name, size_t max_size, size_t max_lines)
    : file_name(file_name),
      max_size(max_size),
      max_lines(max_lines)
{
    file.open(file_name, file_open_mode1); // 以 写入 截断 模式打开文件
    if (!file)
    {
        std::perror("file open error");
        exit(-1);
    }
}

void FileSink::log(const std::string &message)
{
    // 文件 I/O 操作需要上锁
    {
        std::unique_lock<std::mutex> lock(file_mutex);
        // 后续需要将此处按照可设定的格式输出
        file << message << std::endl;

        current_size += message.size();
        current_lines++;

        check_file(); // 检查是否需要轮替当前文件
    }
}

void FileSink::check_file()
{
    if (current_lines >= max_lines) // current_size >= max_size ||
    {
        // 关闭原来打开的文件
        file.close();
        file.open(file_name, file_open_mode1); // 当文件满时，直接将源文件内容删除，重新写入
        current_lines = 0;
        current_size = 0;
    }
}