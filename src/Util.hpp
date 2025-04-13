#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <vector>

namespace Util {
uint32_t read_u32_le(std::istream &istream);
uint16_t read_u16_le(std::istream &istream);
uint8_t read_u8_le(std::istream &istream);

int32_t read_i32_le(std::istream &istream);
int16_t read_i16_le(std::istream &istream);
int8_t read_i8_le(std::istream &istream);
} // namespace Util
