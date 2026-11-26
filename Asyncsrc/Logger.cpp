#include "Logger.hpp"
#include <sstream>
void Logger::log(
    const std::string &message,
    std::source_location location ) {
  std::ostringstream logEntry;
  logEntry << "\nfile: " << location.file_name() << '(' << location.line()
           << ':' << location.column() << ") `" << location.function_name()
           << std::endl;

  if (m_logFile.is_open()) {
    m_logFile << logEntry.str();
    m_logFile.flush();
  }
}
