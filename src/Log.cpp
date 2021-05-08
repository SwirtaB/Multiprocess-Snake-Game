//
// Created by swirta on 08.05.2021.
//

#include "Log.hpp"
#include <ctime>

Log::~Log() {
    close_file();
}
bool Log::open_file() {
    logFile.open(logPath, std::ios::out);
    if(!logFile.good()){
        std::cerr << "Blad otwarcia pliku z podanej sciezki: " << logPath << std::endl;
        return true;
    }
    time_t localTime;
    time(&localTime);
    logFile << ctime(&localTime);
    logFile << "Capture time" << separator << "Process time" << separator << "Game time" << std::endl;
    return false;
}
void Log::close_file() {
    logFile.close();
}
void Log::convert_and_write(const char *captureData, const char *processData, const char *gameData) {
    logFile << captureData << separator << processData << separator << gameData << std::endl;
}

