#pragma once

#include <condition_variable>
#include <limits>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

#include "buffer.h"
#include "logger.h"

// logger macros
#define LOG_DEBUG(msg) logger.log(Logger::LogLevel::DEBUG, msg)
#define LOG_INFO(msg) logger.log(Logger::LogLevel::INFO, msg)
#define LOG_ERROR(msg) logger.log(Logger::LogLevel::ERROR, msg)
#define LOG_WARNING(msg) logger.log(Logger::LogLevel::WARNING, msg)

enum class CommandType { READ, WRITE, EXIT, INVALID };
enum class Command {
  READ,
  COUNT,
  INSERT,
  DELETE,
  REVERSE,
  SORT_ASC,
  SORT_DESC,
  EXIT,
  INVALID
};

struct Request {
  CommandType ct;
  Command cmd;
  int index;
  int value;
};

class Program {
 public:
  Program();
  ~Program();

  // main loop
  void Run();

 private:
  Buffer buffer;
  std::mutex bufferMutex;

  std::thread writerThread, readerThread;
  void WriterJob();
  void ReaderJob();

  std::queue<Request> writerQueue;
  std::mutex writerMutex;  // mutex for the queue
  std::condition_variable writerCV;
  void AddWriterRequest(Request req);

  std::queue<Request> readerQueue;
  std::mutex readerMutex;  // mutex for the queue
  std::condition_variable readerCV;
  void AddReaderRequest(Request req);

  std::mutex outputMutex;  // mutex for the standard output

  // thread-safe output to standard output
  void SafePrint(const std::string& str, bool isError = false);

  Request ParseRequest(const std::string& line);

  // вызывается в начале программы
  void WelcomeMsg();

  Logger logger;
};