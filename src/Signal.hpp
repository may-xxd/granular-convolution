#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <vector>
struct Signal {

  Signal(uint32_t sample_rate, uint32_t num_channels);
  uint32_t sample_rate;
  std::vector<std::vector<float>> data;
  Signal slice(float start, float size) const;
  Signal convolve(const Signal &other) const;
  void append(const Signal &other);
  void append_crossfade(const Signal &other, float crossfade_time);

  float get_length() const;

  void write_to_wave(std::ostream &ostream) const;

  static std::optional<Signal> parse_from_wave(std::istream &wave_stream);
};
