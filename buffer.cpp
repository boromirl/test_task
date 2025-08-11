#include "buffer.h"

std::string Buffer::Read() const {
  std::stringstream ss;

  // output format:
  // [0]: hello
  // [1]: world
  for (size_t i = 0; i < m_buffer.size(); i++) {
    ss << "[" << i << "]: " << m_buffer[i] << "\n";
  }

  return ss.str();
}

void Buffer::Insert(int index, int element) {
  if (index < 0 || index > m_buffer.size()) {
    throw std::out_of_range("Index " + std::to_string(index) +
                            " is out of bounds for buffer size " +
                            std::to_string(m_buffer.size()));
  }

  m_buffer.insert(m_buffer.begin() + index, element);
}

void Buffer::Delete(int index) {
  if (index < 0 || index >= m_buffer.size()) {
    throw std::out_of_range("Index " + std::to_string(index) +
                            " is out of bounds for buffer size " +
                            std::to_string(m_buffer.size()));
    return;
  }
  m_buffer.erase(m_buffer.begin() + index);
}

void Buffer::Reverse() { std::reverse(m_buffer.begin(), m_buffer.end()); }

void Buffer::Sort(bool desc) {
  std::sort(m_buffer.begin(), m_buffer.end());

  if (desc) {
    Reverse();
  }
}

Buffer::CountResult Buffer::Count() const {
  size_t even = 0;
  size_t odd = 0;

  for (size_t i = 0; i < m_buffer.size(); i++) {
    if (i % 2 == 0)
      even++;
    else
      odd++;
  }

  return {even, odd};

  /* можно заменить на:
   even = (m_buffer.size() + 1) / 2;
   odd = (m_buffer.size() / 2);

   return {even, odd};
  */
}