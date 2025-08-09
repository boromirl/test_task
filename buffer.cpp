#include "buffer.h"

void Buffer::Read() {
  if (m_buffer.empty()) {
    std::cout << "<The buffer is empty>\n";
    return;
  }
  // output format:
  // [0]: hello
  // [1]: world
  for (int i = 0; i < m_buffer.size(); i++) {
    std::cout << "[" << i << "]: " << m_buffer[i] << "\n";
  }
}

void Buffer::Insert(int element) { m_buffer.push_back(element); }

void Buffer::DeleteByIndex(int ind) {
  if (ind >= m_buffer.size()) {
    std::cerr << "Error: index " << ind << " out of bounds\n";
    return;
  }
  m_buffer.erase(m_buffer.begin() + ind);
}

void Buffer::Reverse() { std::reverse(m_buffer.begin(), m_buffer.end()); }

void Buffer::Sort(bool desc) {
  std::sort(m_buffer.begin(), m_buffer.end());

  if (desc) {
    Reverse();
  }
}

void Buffer::Count() {
  int even = 0;
  int odd = 0;

  for (int i = 0; i < m_buffer.size(); i++) {
    if (i % 2 == 0)
      even++;
    else
      odd++;
  }

  std::cout << "Elements in even positions: " << even << "\n";
  std::cout << "Elements in odd positions: " << odd << "\n";
}