#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

class Buffer {
 public:
  Buffer() = default;
  Buffer(std::initializer_list<int> init_list) : m_buffer(init_list) {};

  ~Buffer() = default;
  // Чтение. Должно давать копию, или просто выводить в stdout?
  // Я сделаю просто вывод в stdout
  void Read();
  void Insert(int element);
  // void InsertByIndex(std::string str, int ind);
  void DeleteByIndex(int ind);
  // DeleteByValue???
  void Sort(bool desc);  // desc = True - сортировка по убыванию, desc = False -
                         // по возрастанию
  // не ясно, что конкретно должен делать метод. В моём случае он просто выводит
  // информацию.
  void Count();
  void Reverse();

 private:
  // есть небольшое подозрение, что лучше сделать свой массив, а не std::vector
  std::vector<int> m_buffer;
};