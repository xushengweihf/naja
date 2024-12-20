#include <iostream>
#include "log.h"

namespace xsw{
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
        std::cout << m_formatter->format(event);
    }
}

FileLogAppender::FileLogAppender(const std::string& filename) : m_filename(filename){

}
void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_formatter->format(event);
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

std::string LogFormatter::format(LogEvent::ptr event){
    std::stringstream ss;
    for(auto& i : m_item){
        i->format(ss, event);
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
                    ++j;
                    continue;
                }
            }
            
        }
    }
}

}