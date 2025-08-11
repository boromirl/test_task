#pragma once

#include <condition_variable>
#include <limits>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

#include "buffer.h"

enum class CommandType { READ, WRITE, EXIT };
enum class Command {
  READ,
  COUNT,
  INSERT,
  DELETE,
  REVERSE,
  SORT_ASC,
  SORT_DESC,
  EXIT
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
  void SafePrint(const std::string &str, bool isError = false);

  // вызывается в начале программы
  void WelcomeMsg();
};