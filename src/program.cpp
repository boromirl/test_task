#include "program.h"

void Program::SafePrint(const std::string& str, bool isError) {
  std::lock_guard<std::mutex> lock(outputMutex);
  if (isError) {
    std::cerr << str << std::endl;
  } else {
    std::cout << str << std::endl;
  }
}

void Program::WelcomeMsg() {
  SafePrint(
      "Welcome to the program. Here are your commands:\n"
      "read - read the buffer\n"
      "count - count the odd and even elements in the buffer\n"
      "insert <index> <value> - insert new element in the <index> position\n"
      "delete <index> - delete the element in the <index> position\n"
      "reverse - reverse the order of the elements in the buffer\n"
      "sort <asc/desc> - sort the buffer in the ascending/descending order\n");
}

void Program::Run() {
  WelcomeMsg();

  while (true) {
    std::string input;
    std::getline(std::cin, input);

    Request req = ParseRequest(input);

    if (req.ct == CommandType::EXIT) {
      LOG_INFO("EXIT request received");
      break;
    }

    if (req.ct == CommandType::INVALID) {
      std::string errMsg;
      switch (req.cmd) {
        case Command::INSERT:
          errMsg = "Invalid insert request. Usage: insert <index> <value>";
          break;
        case Command::DELETE:
          errMsg = "Invalid delete request. Usage: delete <index>";
          break;
        case Command::SORT_ASC:
        case Command::SORT_DESC:
          errMsg = "Invalid sort request. Usage: sort asc OR sort desc";
          break;
        default:
          errMsg = "Invalid request: " + input;
      }

      SafePrint(errMsg, true);
      LOG_ERROR(errMsg);
    }

    else if (req.ct == CommandType::READ) {
      AddReaderRequest(req);
    } else if (req.ct == CommandType::WRITE) {
      AddWriterRequest(req);
    }
  }
}

void Program::WriterJob() {
  LOG_INFO("Writer thread started");

  while (true) {
    Request request;
    {
      std::unique_lock<std::mutex> lock(writerMutex);
      writerCV.wait(lock, [this] { return !writerQueue.empty(); });
      request = writerQueue.front();
      writerQueue.pop();
    }

    if (request.ct == CommandType::EXIT)
      break;  // stop the thread on exit command

    std::lock_guard<std::mutex> buf_lock(bufferMutex);

    try {
      switch (request.cmd) {
        case Command::INSERT:
          buffer.Insert(request.index, request.value);
          LOG_INFO("Insert " + std::to_string(request.value) + " at index " +
                   std::to_string(request.index));
          break;
        case Command::DELETE:
          buffer.Delete(request.index);
          LOG_INFO("Deleted element at index " + std::to_string(request.index));
          break;
        case Command::SORT_ASC:
          buffer.Sort(false);
          LOG_INFO("Sorted elements in ascending order");
          break;
        case Command::SORT_DESC:
          buffer.Sort(true);
          LOG_INFO("Sorted elements in descending order");
          break;
        case Command::REVERSE:
          buffer.Reverse();
          LOG_INFO("Reversed the buffer");
          break;
        default:
          break;
      }
    } catch (const std::exception& e) {
      SafePrint(e.what(), true);
      LOG_ERROR(e.what());
    }
  }

  LOG_INFO("Writer thread exiting");
}

void Program::ReaderJob() {
  LOG_INFO("Reader thread started");

  while (true) {
    Request request;
    {
      std::unique_lock<std::mutex> lock(readerMutex);
      readerCV.wait(lock, [this] { return !readerQueue.empty(); });
      request = readerQueue.front();
      readerQueue.pop();
    }

    if (request.ct == CommandType::EXIT)
      break;  // stop the thread on exit command

    std::lock_guard<std::mutex> buf_lock(bufferMutex);
    switch (request.cmd) {
      case Command::READ: {
        SafePrint(buffer.Read());
        break;
      }
      case Command::COUNT: {
        Buffer::CountResult count = buffer.Count();
        SafePrint("Number of elements in odd positions: " +
                  std::to_string(count.odd) +
                  "\nNumber of elements in even positions: " +
                  std::to_string(count.even));
        break;
      }
      default:
        break;
    }
  }
  LOG_INFO("Reader thread exiting");
}

Request Program::ParseRequest(const std::string& line) {
  std::istringstream iss(line);
  std::string commandStr;
  iss >> commandStr;

  Request req{CommandType::INVALID, Command::INVALID, 0, 0};

  if (commandStr.empty()) {
    return req;
  }

  // при неправильном вводе команды, устанавливаем CommandType в INVALID, но
  // сохраняем саму команду Command, чтобы потом выдать сообщение о
  // конкретной ошибке
  if (commandStr == "insert") {
    req.cmd = Command::INSERT;
    if (iss >> req.index >> req.value) {
      req.ct = CommandType::WRITE;
    }

  } else if (commandStr == "delete") {
    req.cmd = Command::DELETE;
    if (iss >> req.index) {
      req.ct = CommandType::WRITE;
    }
  } else if (commandStr == "sort") {
    std::string order;
    req.cmd = Command::SORT_ASC;

    if (iss >> order) {
      if (order == "asc") {
        req.cmd = Command::SORT_ASC;
        req.ct = CommandType::WRITE;
      } else if (order == "desc") {
        req.ct = CommandType::WRITE;
        req.cmd = Command::SORT_DESC;
      }
    }
  } else if (commandStr == "reverse") {
    req.ct = CommandType::WRITE;
    req.cmd = Command::REVERSE;
  } else if (commandStr == "read") {
    req.ct = CommandType::READ;
    req.cmd = Command::READ;
  } else if (commandStr == "count") {
    req.ct = CommandType::READ;
    req.cmd = Command::COUNT;
  } else if (commandStr == "exit") {
    req.cmd = Command::EXIT;
    req.ct = CommandType::EXIT;
  }

  return req;
}

Program::Program() {
  LOG_INFO("Program started");
  writerThread = std::thread(&Program::WriterJob, this);
  readerThread = std::thread(&Program::ReaderJob, this);
}

Program::~Program() {
  // "send exit signal" to threads
  AddWriterRequest({CommandType::EXIT, Command::EXIT, 0, 0});
  AddReaderRequest({CommandType::EXIT, Command::EXIT, 0, 0});

  writerThread.join();
  readerThread.join();
  LOG_INFO("Program exiting");
}

void Program::AddWriterRequest(Request req) {
  std::lock_guard<std::mutex> lock(writerMutex);
  writerQueue.push(req);
  writerCV.notify_one();
  LOG_INFO("Writer request added to queue");
}

void Program::AddReaderRequest(Request req) {
  std::lock_guard<std::mutex> lock(readerMutex);
  readerQueue.push(req);
  readerCV.notify_one();
  LOG_INFO("Reader request added to queue");
}