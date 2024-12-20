#include <iostream>
#include "log.h"

namespace xsw{

const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}
Logger::Logger(const std::string& name = "root")
    : m_name(name){
}

void Logger::addAppender(LogAppender::ptr appender){
    m_appenders.push_back(appender);//这里先不考虑线程安全
}
void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        for(auto& i : m_appenders){
            i->log(level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event){
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event){
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event){
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event){
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event){
    log(LogLevel::FATAL, event);
}


void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        std::cout << m_formatter->format(level, event);
    }
}

FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename){

}
void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_formatter->format(level, event);
    }
}

bool FileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

//我们要有个流对象，输入我们想要的输出信息
//然后在析构函数的时候调用log方法
//把所有的信息都输出出来 就相当于有个暂存区
//把我们的内容先暂存起来
//在它的那一行运行结束的时候 会进行析构
//在析构的时候 会把所有的内容给输出出来
//所以还需要个留对象

LogFormatter::LogFormatter(const std::string& pattern)
    : m_pattern(pattern){

}

std::string LogFormatter::format(LogLevel::Level level,LogEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_item){
        i->format(ss, level, event);
    }
    return ss.str();
}

//%xxx %xxx{xxx} %%
void LogFormatter::init(){
    //str, format, type 这样的三元组格式
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for (size_t i = 0; i < m_pattern.size(); ++i){
        if (m_pattern[i] != '%'){
            nstr.append(1, m_pattern[i]);
            continue;
        }
        if((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%'){
            nstr.append(1, '%');
            continue;
        }

        size_t j = i + 1;
        int fmt_status = 0;//format的状态
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while (j < m_pattern.size()){
            if(isspace(m_pattern[j])){
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[j] == '{') {
                    str = m_pattern.substr(i + 1, j - i - 1);
                    fmt_status = 1; //解析格式
                    fmt_begin = j;
                    j++;
                    continue;
                }
            }else if(fmt_status == 1){
                if(m_pattern[j] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, j - fmt_begin - 1);
                    fmt_status = 2;
                    break;
                }
            }
            
        }
        if(fmt_status == 0){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            
            str = m_pattern.substr(i + 1, j - i - 1);
            vec.push_back(std::make_tuple(str, fmt, 1));
             i = j;
        } else if(fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 1));
        } else if( fmt_status == 2){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = j;
        }
    }
    if(!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    //%m -- 输出代码中指定的消息
    //%p -- 输出优先级 level
    //%r -- 输出启动后的时间，毫秒数
    //%c -- 输出日志名称
    //%t -- 输出线程id
    //%n -- 输出一个回车换行符，Windows平台为"\\r\\n"，Unix平台为"\\n"
    //%d -- 输出日志时间点的日期或时间，默认格式为ISO8601，
    //      也可以在其后指定格式，比如：%d{yyy MMM dd HH:mm:ss,SSS}
    //      输出类似：2024年12月20日 22 : 10 : 28, 921
    //%f -- 输出文件名
    //%l -- 输出行号
}

//日志解析的子模块

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};



    const char* m_file = nullptr; //文件名
    int32_t m_line = 0;           //行号
    uint32_t m_elapse = 0;        //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;      //线程id
    uint32_t m_fiberId = 0;       //协程id
    uint64_t m_time = 0;          //时间戳
    std::string m_content;        //消息

}