#include "Signal.hpp"
#include <fstream>
#include <iostream>
int main() {
  std::ifstream ifstream("res/sitting.wav");

  std::optional<Signal> signal = Signal::parse_from_wave(ifstream);

  if (!signal) {
    std::cerr << "Failed to parse signal from WAVE" << std::endl;
    return 1;
  }

  std::cout << "Sample rate: " << signal->sample_rate << std::endl;
  std::cout << "Num channels: " << signal->data.size() << std::endl;
  for (uint32_t ii=0; ii < signal->data.size(); ii++)
  {
    std::cout << "Channel " << ii << " size: " << signal->data[ii].size() << std::endl;
  }
}
