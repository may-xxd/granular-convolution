#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <vector>
struct Signal {
  uint32_t sample_rate;
  std::vector<std::vector<float>> data;
  Signal slice(size_t start, size_t size) const;
  Signal convolve(const Signal &other) const;

  void write_to_wave(std::ostream &ostream) const;

  static std::optional<Signal> parse_from_wave(std::istream &wave_stream);
};
