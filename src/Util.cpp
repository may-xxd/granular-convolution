#include "Util.hpp"
#include <iostream>

namespace Util {

void write_u32_le(std::ostream &ostream, uint32_t value) {
  uint8_t buf[4] = {static_cast<uint8_t>(value & 0x000000ff),
                    static_cast<uint8_t>((value & 0x0000ff00) >> 8),
                    static_cast<uint8_t>((value & 0x00ff0000) >> 16),
                    static_cast<uint8_t>((value & 0xff000000) >> 24)};

  ostream.write(reinterpret_cast<char *>(buf), sizeof(buf));
}

void write_u16_le(std::ostream &ostream, uint16_t value) {
  uint8_t buf[2] = {static_cast<uint8_t>(value & 0x00ff),
                    static_cast<uint8_t>((value & 0xff00) >> 8)};

  ostream.write(reinterpret_cast<char *>(buf), sizeof(buf));
}

void write_i32_le(std::ostream &ostream, int32_t value) {
  uint8_t buf[4] = {static_cast<uint8_t>(value & 0x000000ff),
                    static_cast<uint8_t>((value & 0x0000ff00) >> 8),
                    static_cast<uint8_t>((value & 0x00ff0000) >> 16),
                    static_cast<uint8_t>((value & 0xff000000) >> 24)};

  ostream.write(reinterpret_cast<char *>(buf), sizeof(buf));
}

void write_i16_le(std::ostream &ostream, int16_t value) {
  uint8_t buf[2] = {static_cast<uint8_t>(value & 0x00ff),
                    static_cast<uint8_t>((value & 0xff00) >> 8)};

  ostream.write(reinterpret_cast<char *>(buf), sizeof(buf));
}

void write_i8_le(std::ostream &ostream, int8_t value) {
  ostream.write(reinterpret_cast<char *>(value), sizeof(value));
}

void write_u8_le(std::ostream &ostream, uint8_t value) {
  ostream.write(reinterpret_cast<char *>(value), sizeof(value));
}

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
