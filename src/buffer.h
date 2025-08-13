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

  void Insert(int index, int element);
  void Delete(int index);
  void Sort(bool desc);  // desc = True - сортировка по убыванию, desc = False -
                         // по возрастанию
  void Reverse();
  size_t GetSize() { return m_buffer.size(); };

 private:
  std::vector<int> m_buffer;
};