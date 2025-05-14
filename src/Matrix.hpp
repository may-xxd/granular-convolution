#pragma once

#include "Signal.hpp"
#include "Util.hpp"
#include "hilbert_curves.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
template <size_t WIDTH, size_t HEIGHT> struct Matrix {
  std::array<std::array<float, WIDTH>, HEIGHT> data;

  Signal to_hilbert_signal() const {
    Signal ret(44100, 1);
    size_t order = std::ceil(std::log2(std::max(WIDTH, HEIGHT)));

    if (order >= 16) {
      std::cerr << "Order too large to convert to hilbert signal: " << order
                << std::endl;
      return ret;
    }

    size_t num_samples = 1 << (order * 2);

    ret.data[0].reserve(num_samples);

    for (size_t ii = 0; ii < num_samples; ii++) {
      uint32_t x;
      uint32_t y;
      hilbertIndexToXY(order, ii, x, y);

      if (x >= WIDTH || y >= HEIGHT) {
        ret.data[0][ii] = 0.0f;
      } else {
        ret.data[0][ii] = data[y][x];
      }
    }

    return ret;
  }

  Matrix(const Signal &signal) {
    size_t order = std::ceil(std::log2(std::max(WIDTH, HEIGHT)));
    size_t num_samples = signal.data[0].size();
    size_t max_hilbert_index = hilbertXYToIndex(order, WIDTH - 1, 0) + 1;

    for (size_t yy = 0; yy < HEIGHT; yy++) {
      for (size_t xx = 0; xx < WIDTH; xx++) {
        size_t ii = hilbertXYToIndex(order, xx, yy);
        size_t sample_index =
            (static_cast<float>(ii) / static_cast<float>(max_hilbert_index)) *
            num_samples;

        if (sample_index >= signal.data[0].size()) {
          data[yy][xx] = 0.0f;
        } else {
          data[yy][xx] = signal.data[0][sample_index];
        }
      }
    }
  }

  void write_to_tga(std::ostream &ostream) const {
    Util::write_u8_le(ostream, 0);
    Util::write_u8_le(ostream, 0);
    Util::write_u8_le(ostream, 3);

    Util::write_u16_le(ostream, 0);
    Util::write_u16_le(ostream, 0);
    Util::write_u8_le(ostream, 0);

    Util::write_u16_le(ostream, 0);
    Util::write_u16_le(ostream, 0);
    Util::write_u16_le(ostream, WIDTH);
    Util::write_u16_le(ostream, HEIGHT);
    Util::write_u8_le(ostream, 8);
    Util::write_u8_le(ostream, 0b100000);

    for (size_t yy = 0; yy < HEIGHT; yy++) {
      for (size_t xx = 0; xx < WIDTH; xx++) {
        int32_t sample_32 = static_cast<uint32_t>(data[yy][xx] * 128) + 128;

        if (sample_32 > 255) {
          sample_32 = 255;
        } else if (sample_32 < 0) {
          sample_32 = 0;
        }
        uint8_t sample = sample_32;
        Util::write_u8_le(ostream, sample);
      }
    }
  }
};
