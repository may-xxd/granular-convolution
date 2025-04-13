#include "Util.hpp"
#include <iostream>

namespace Util {
uint32_t read_u32_le(std::istream &istream) {
  uint8_t buf[4] = {0};
  istream.read(reinterpret_cast<char *>(buf), sizeof(buf));
  return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
}

uint16_t read_u16_le(std::istream &istream) {
  uint8_t buf[2] = {0};
  istream.read(reinterpret_cast<char *>(buf), sizeof(buf));
  return (buf[1] << 8) | (buf[0]);
}

uint8_t read_u8_le(std::istream &istream) {
  uint8_t ret;
  istream.read(reinterpret_cast<char *>(&ret), sizeof(ret));
  return ret;
}

int32_t read_i32_le(std::istream &istream) {
  uint8_t buf[4] = {0};
  istream.read(reinterpret_cast<char *>(buf), sizeof(buf));
  return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | (buf[0]);
}

int16_t read_i16_le(std::istream &istream) {
  uint8_t buf[2] = {0};
  istream.read(reinterpret_cast<char *>(buf), sizeof(buf));
  return (buf[1] << 8) | (buf[0]);
}

int8_t read_i8_le(std::istream &istream) {
  int8_t ret;
  istream.read(reinterpret_cast<char *>(&ret), sizeof(ret));
  return ret;
}
} // namespace Util
