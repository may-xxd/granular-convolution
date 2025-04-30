#include "Signal.hpp"
#include <fstream>
#include <iostream>
int main() {
  std::ifstream piano_ifstream("res/heavy.wav");
  std::optional<Signal> piano_signal = Signal::parse_from_wave(piano_ifstream);

  std::ifstream heavy_ifstream("res/sitting.wav");
  std::optional<Signal> heavy_signal = Signal::parse_from_wave(heavy_ifstream);

  if (!piano_signal || !heavy_signal) {
    std::cerr << "Failed to parse signal from WAVE" << std::endl;
    return 1;
  }

  std::cout << "piano samples: " << piano_signal->data[0].size() << std::endl;
  std::cout << "heavy samples: " << heavy_signal->data[0].size() << std::endl;

  Signal convolved = piano_signal->convolve(*heavy_signal);

  std::ofstream ofstream("output.wav");
  convolved.write_to_wave(ofstream);
}
