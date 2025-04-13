#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <vector>
struct Signal {
  uint32_t sample_rate;
  std::vector<std::vector<int16_t>> data;

  static std::optional<Signal> parse_from_wave(std::istream &wave_stream);
};
