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

  std::string line;
  // main input/output loop
  while (std::getline(std::cin, line)) {
    std::stringstream ss(line);
    std::string command;
    ss >> command;

    if (command == "exit") break;

    if (command == "insert") {
      std::string indexStr, valueStr;
      int index, value;

      // если удаётся считать аргументы
      if (ss >> index >> value) {
        AddWriterRequest({CommandType::WRITE, Command::INSERT, index, value});
      } else {
        SafePrint(
            "Error: wrong arguments for insert. Usage: insert <index> <value>",
            true);
      }
    } else if (command == "delete") {
      int index;
      if (ss >> index) {
        AddWriterRequest({CommandType::WRITE, Command::DELETE, index, 0});
      } else {
        SafePrint("Error: wrong arguments for delete. Usage: delete <index>",
                  true);
      }
    } else if (command == "sort") {
      std::string order;
      if (ss >> order) {
        if (order == "asc") {
          AddWriterRequest({CommandType::WRITE, Command::SORT_ASC, 0, 0});
        } else if (order == "desc") {
          AddWriterRequest({CommandType::WRITE, Command::SORT_DESC, 0, 0});
        } else {
          SafePrint(
              "Error: wrong arguments for sort. Usage: sort asc OR sort desc",
              true);
        }
        writerCV.notify_one();
      } else {
        SafePrint("");
      }
    } else if (command == "reverse") {
      AddWriterRequest({CommandType::WRITE, Command::REVERSE, 0, 0});
    } else if (command == "read") {
      AddReaderRequest({CommandType::READ, Command::READ, 0, 0});
    } else if (command == "count") {
      AddReaderRequest({CommandType::READ, Command::COUNT, 0, 0});
    } else {
      SafePrint("Error: " + command + " is not a command.", true);
    }
  }
}

void Program::WriterJob() {
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
          LOG_INFO("Sort elements in ascending order");
          break;
        case Command::SORT_DESC:
          buffer.Sort(true);
          LOG_INFO("Sort elements in descending order");
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
}

void Program::ReaderJob() {
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
}

Program::Program() {
  writerThread = std::thread(&Program::WriterJob, this);
  readerThread = std::thread(&Program::ReaderJob, this);
}

Program::~Program() {
  // "send exit signal" to threads
  AddWriterRequest({CommandType::EXIT, Command::EXIT, 0, 0});
  AddReaderRequest({CommandType::EXIT, Command::EXIT, 0, 0});

  writerThread.join();
  readerThread.join();
}

void Program::AddWriterRequest(Request req) {
  std::lock_guard<std::mutex> lock(writerMutex);
  writerQueue.push(req);
  writerCV.notify_one();
}

void Program::AddReaderRequest(Request req) {
  std::lock_guard<std::mutex> lock(readerMutex);
  readerQueue.push(req);
  readerCV.notify_one();
}