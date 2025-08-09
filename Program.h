#pragma once

#define DEBUG 1

#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

#include "buffer.h"

enum class CommandExecutor { READER, WRITER };
enum class CommandType { READ, INSERT, DELETE, SORT, COUNT, REVERSE };

struct Command {
  CommandExecutor executor;
  CommandType type;
  int args[2];
};

class Program {
 public:
  Program() {
    commands = {"read",
                "insert <new element>",
                "delete <index>",
                "sort <asc/desc>",
                "count",
                "reverse",
                "exit"};

#if DEBUG
    m_buffer.Insert(1);
    m_buffer.Insert(2);
    m_buffer.Insert(3);
#endif
  };
  ~Program() = default;
  Program(Program &other) = delete;

  // main method
  void Run() {
    WelcomeMessage();

    std::string input = "";
    while (1) {
      std::cin >> input;

      if (input == "exit")
        break;
      else
        ActOnInput(input);
    }
  }

 private:
  Buffer m_buffer;
  std::vector<std::string> commands;

  // Очереди выполнения команд для потокоа
  std::queue<Command> readerQueue;
  std::queue<Command> writerQueue;

  void ActOnInput(std::string input) {
    if (input == "read") {
      std::cout << "\nThe buffer:\n";
      m_buffer.Read();
      std::cout << "\n";
    } else if (input == "insert") {
      int new_element;
      std::cin >> new_element;
      if (!std::cin) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Error: program failed to read a valid element. Element "
                     "should be int.\n";
      } else
        m_buffer.Insert(new_element);
    } else if (input == "delete") {
      int index;
      std::cin >> index;
      if (!std::cin) {
        std::cerr << "Error: program failed to read a valid index\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      } else
        m_buffer.DeleteByIndex(index);
    } else if (input == "sort") {
      std::string order;
      std::cin >> order;
      if (order == "asc")
        m_buffer.Sort(false);
      else if (order == "desc")
        m_buffer.Sort(true);
      else {
        std::cerr << "Error: " << order
                  << " is not a sorting order. Please use asc or desc.\n";
      }
    } else if (input == "count")
      m_buffer.Count();
    else if (input == "reverse")
      m_buffer.Reverse();
    else {
      std::cout << "\n\nERROR: " << input << " is not a command.\n\n";
    }
  }

  void PrintCommandList() {
    for (int i = 0; i < commands.size(); i++) {
      std::cout << "- " << commands[i] << "\n";
    }
  }

  void WelcomeMessage() {
    std::cout << "Welcome to the program. Here are your commands:\n";
    PrintCommandList();
    std::cout << "\n";
  }

  // Первый поток - изменяет буффер
  void WriterJob() {}

  // Второй поток - считывает буффер
  void ReaderJob() {}
};