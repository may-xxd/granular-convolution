#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <ostream>
#include <vector>

namespace Util {
uint32_t read_u32_le(std::istream &istream);
uint16_t read_u16_le(std::istream &istream);
uint8_t read_u8_le(std::istream &istream);

int32_t read_i32_le(std::istream &istream);
int16_t read_i16_le(std::istream &istream);
int8_t read_i8_le(std::istream &istream);

void write_u32_le(std::ostream &ostream, uint32_t value);
void write_u16_le(std::ostream &ostream, uint16_t value);
void write_u8_le(std::ostream &ostream, uint8_t value);

void write_i32_le(std::ostream &ostream, int32_t value);
void write_i16_le(std::ostream &ostream, int16_t value);
void write_i8_le(std::ostream &ostream, int8_t value);
} // namespace Util
