#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>

class Logger {
 public:
  enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

  Logger(const std::string& filename = "program.log")
      : logFile(filename, std::ios::app) {
    if (!logFile.is_open()) {
      throw std::runtime_error("Failed to open log file");
    }
  };
  ~Logger() = default;

  void log(LogLevel level, const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::lock_guard<std::mutex> lock(logMutex);

    logFile << std::put_time(std::localtime(&time), "%F %T") << " " << "["
            << LevelToString(level) << "] : " << msg << std::endl;
  }

 private:
  std::mutex logMutex;
  std::ofstream logFile;

  std::string LevelToString(LogLevel level) {
    switch (level) {
      case LogLevel::DEBUG:
        return "DEBUG";
      case LogLevel::INFO:
        return "INFO";
      case LogLevel::WARNING:
        return "WARNING";
      case LogLevel::ERROR:
        return "ERROR";
      default:
        return "UNKNOWN";
    }
  }
};