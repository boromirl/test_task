#pragma once

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Buffer {
 public:
  Buffer() = default;
  Buffer(std::initializer_list<int> init_list) : m_buffer(init_list) {};
  ~Buffer() = default;

  std::string Read() const;

  struct CountResult {
    size_t even;
    size_t odd;
  };
  CountResult Count() const;

  void Insert(size_t index, int element);
  void Delete(size_t index);
  void Sort(bool desc);  // desc = True - сортировка по убыванию, desc = False -
                         // по возрастанию
  void Reverse();

 private:
  std::vector<int> m_buffer;
};