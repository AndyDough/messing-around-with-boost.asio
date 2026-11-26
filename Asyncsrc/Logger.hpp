#pragma once
#include <fstream>
#include <iostream>
#include <source_location>
#include <string>
class Logger {
private:
  std::ofstream m_logFile;

public:
  Logger(const std::string &fileName) {
    m_logFile.open(fileName, std::ios::app);
    if (!m_logFile.is_open()) {
      std::cerr << "Logfile could not be opened\n";
    }
  }

  ~Logger() { m_logFile.close(); }

  void log(const std::string &message,
           std::source_location location = std::source_location::current());
};
