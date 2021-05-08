//
// Created by swirta on 08.05.2021.
//

#ifndef SCZR_SNAKE_LOG_HPP
#define SCZR_SNAKE_LOG_HPP

#include <iostream>
#include <fstream>

const char separator = ',';

class Log{
public:
    ~Log();
    bool open_file();
    void close_file();
    void convert_and_write(const char* captureData, const char* processData, const char* gameData);

private:
    const std::string logPath = "../tests/Log.txt";
    std::fstream logFile;
};

#endif //SCZR_SNAKE_LOG_HPP
