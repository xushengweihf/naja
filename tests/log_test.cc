#include "xsw/log.h"
#include <iostream>
#include <string>


int main(int argc, char** argv){

    xsw::Logger::ptr logger(new xsw::Logger("ROOT"));
    // logger->getSS() << "Hello Naja!";
    logger->setLevel(xsw::LogLevel::Level::DEBUG);

    XSW_LOG_DEBUG(logger) << "DEBUG";
    XSW_LOG_INFO(logger) << "INFO";
    std::cout << "======" << std::endl;


    return 0;
}