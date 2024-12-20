#ifndef __NAHA_LOG_H__
#define __NAHA_LOG_H__

#include <string>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <list>
#include <fstream>
#include <sstream>
#include <vector>

#define XSW_LOG_LEVEL(logger, level) \
    if(level >= logger->getLevel() ) xsw::LogWrap(logger).getLogger()->getSS()

#define XSW_LOG_DEBUG(logger) XSW_LOG_LEVEL(logger, xsw::LogLevel::Level::DEBUG)
#define XSW_LOG_INFO(logger) XSW_LOG_LEVEL(logger, xsw::LogLevel::Level::INFO)
#define XSW_LOG_WARN(logger) XSW_LOG_LEVEL(logger, xsw::LogLevel::Level::WARN)
#define XSW_LOG_ERROR(logger) XSW_LOG_LEVEL(logger, xsw::LogLevel::Level::ERROR)
#define XSW_LOG_FATAL(logger) XSW_LOG_LEVEL(logger, xsw::LogLevel::Level::FATAL)

namespace xsw{


//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_file = nullptr; //文件名
    int32_t m_line = 0;           //行号
    uint32_t m_elapse = 0;        //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;      //线程id
    uint32_t m_fiberId = 0;       //协程id
    uint64_t m_time = 0;          //时间戳
    std::string m_content;        //消息
};

//日志级别
class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

//日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);

    //%t    %thread_id %m%n
    std::string format(LogEvent::ptr event);
private:
    //日志解析的子模块
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };
    void init();
private:
    std::string m_pattern;
    //要输出多少个项
    std::vector<FormatItem::ptr> m_item;
};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    //因为日志输出的地方有很多，所以设为虚函数
    virtual ~LogAppender();

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;
    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    LogFormatter::ptr getFormatter() const {return m_formatter;}
protected://因为下面可能会用到它的level
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};


//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;
    
    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level;}
    void setLevel(LogLevel::Level level) { m_level = level;}
    
private:
    std::string m_name;                      //日志名称
    LogLevel::Level m_level;                 //日志级别
    std::list<LogAppender::ptr> m_appenders; //Appender是个列表 输出目的地的集合
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};

//定义输出到文件的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，文件打开成功返回true
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;//ofstream为输出文件流，用于将数据写入文件，可以将文本或二进制数据写入到指定的文件中
};

}

#endif