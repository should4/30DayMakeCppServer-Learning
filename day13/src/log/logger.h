#ifndef LOGGER_H
#define LOGGER_H

#define DEBUG_MODE

#include <queue>
#include <string>
#include <thread>
#include <condition_variable>
#include <vector>
#include "sink.h"
#include "console_sink.h"
#include "file_sink.h"

// 定义日志级别
enum LOG_LEVEL
{
    DEBUG,
    INFO,
    WARM,
};

class Logger
{
public:
    static Logger &get_intance(); // 获取单例

    void log_message(LOG_LEVEL level,const std::string &message);

    // 设置当前日志系统的日志输出等级
    void set_log_level(LOG_LEVEL level);
    // 添加日志器
    void add_sink(Sink *sink);

    // 设置异步日志 开启与关闭
    void start_async();
    void stop_async();

private:
    Logger();
    ~Logger();

    Logger(const Logger &) = delete;               // 删除拷贝构造函数
    Logger &operator=(const Logger &log) = delete; // 删除赋值函数（隐性调用拷贝构造函数）

    std::thread log_thread;
    std::queue<std::string> msg_queue; // BUG :  std::queue<std::string&> msg_queue;
    std::mutex log_mutex;              // msg 队列的锁 并发控制
    std::condition_variable cv;        // 实现线程同步 如果当前没有任务要处理 则将线程挂起
    std::vector<Sink *> sinks;         // 使用基类日志器 即使用父指针操作子类相同的重写函数

    void process_msg(); // 异步处理日志输出函数 消费者处理线程

    static Logger instance;
    LOG_LEVEL log_level = LOG_LEVEL::DEBUG; // 默认为 DEBUG
    bool is_async = false;                  // 默认为同步
    bool async_running = false;             // 用来标志异步单线程是否正在运行 , 初始未运行
};

#endif