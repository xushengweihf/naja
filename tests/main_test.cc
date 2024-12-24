#include <iostream>
#include "xsw/log.h"


int main(int argc,char** argv){
    xsw::Logger::ptr logger(new xsw::Logger);
    logger->addAppender(xsw::LogAppender::ptr(new xsw::StdoutLogAppender));

    xsw::LogEvent::ptr event(new xsw::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));

    logger->log(xsw::LogLevel::DEBUG, event);

    std::cout <<  "hello xsw log" << std::endl;
    return 0;
}