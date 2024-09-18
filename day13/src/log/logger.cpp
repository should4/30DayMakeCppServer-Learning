#include "logger.h"
#include <iostream>

// 在源文件中进行初始化
Logger Logger::instance; // 使用 饿汉模式 静态成员变量初始化分配内存

Logger::Logger() {}
Logger::~Logger() {}

Logger &Logger::get_intance()
{
    return instance;
}

void Logger::log_message(LOG_LEVEL level, const std::string &message)
{
    // 判断当前日志等级 是否大于 系统日志等级的
    if (level >= log_level)
    {
        if (is_async) // 如果异步模式打开
        {
            {
#ifdef DEBUG_MODE
                std::cout << "async message produce" << std::endl;
#endif
                std::unique_lock<std::mutex> lock(log_mutex);
                msg_queue.push(message);
                cv.notify_one();
#ifdef DEBUG_MODE
                std::cout << "current msg_queue size" << msg_queue.size() << std::endl;
#endif
            }
        }
        else
        {
            // 同步模式直接执行
            for (const auto sink : sinks)
            {
                sink->log(message);
            }
        }
    }
}

void Logger::add_sink(Sink *sink)
{
    sinks.push_back(sink);
}

// 设置当前日志系统的日志输出等级
void Logger::set_log_level(LOG_LEVEL level)
{
    {
        std::unique_lock<std::mutex> lock(log_mutex);
        log_level = level;
    }
}

// 设置异步日志 开启与关闭
void Logger::start_async()
{
#ifdef DEBUG_MODE
    std::cout << "start asynchronous mode" << std::endl;
#endif

    {
        std::unique_lock<std::mutex> lock(log_mutex);
        is_async = true;
        async_running = true;                                 // 设置异步模式标志
        log_thread = std::thread(&Logger::process_msg, this); // 将异步处理线程绑定回调函数
        // log_thread.detach(); // 一旦调用了 detach()，线程将与主线程分离，无法再通过 join() 进行同步。
    }
}

void Logger::stop_async()
{
    {
#ifdef DEBUG_MODE
        std::cout << "stop asynchronous mode" << std::endl;
#endif
        std::unique_lock<std::mutex> lock(log_mutex);
        is_async = false; // 设置关闭异步模式标志
        async_running = false;
        cv.notify_all(); // 当前消费者线程还在被挂起等待时，唤醒线程使其退出
    }

    // 等待消费者线程结束
    if (log_thread.joinable())
    {
#ifdef DEBUG_MODE
        std::cout << "asynchronous thread wait for join" << std::endl;
#endif
        log_thread.join();
    }

#ifdef DEBUG_MODE
    std::cout << "stopped asynchronous mode" << std::endl;
#endif
}

void Logger::process_msg()
{
#ifdef DEBUG_MODE
    std::cout << "start process_msg thread" << std::endl;
#endif

    while (true)
    {
        std::unique_lock<std::mutex> lock(log_mutex);
        cv.wait(lock, [this]()
                { return !async_running || !msg_queue.empty(); });
        // 如果当前正在关闭异步模式 且 消息队列已经消费完成后 则关闭当前线程
        if (!async_running && msg_queue.empty())
        {
#ifdef DEBUG_MODE
            std::cout << "process_msg thread stopped" << std::endl;
#endif
            return;
        }
#ifdef DEBUG_MODE
        std::cout << "process_msg thread consume a msg" << std::endl;
#endif

        auto msg = msg_queue.front();
        msg_queue.pop();

        lock.unlock();
        for (const auto sink : sinks)
        {
            sink->log(msg);
        }

        lock.lock();
    }
}