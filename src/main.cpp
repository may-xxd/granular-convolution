#include "Signal.hpp"
#include <fstream>
#include <iostream>
#include <random>

int main() {
  std::ifstream ifstream("res/sitting.wav");
  std::optional<Signal> seed_signal = Signal::parse_from_wave(ifstream);

  if (!seed_signal) {
    std::cerr << "Failed to parse signal from WAVE" << std::endl;
    return 1;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> grain_length_dist(22050, 88200);
  std::uniform_int_distribution<> grain_pos_dist(44100 * 2, 44100 * 30);

  constexpr size_t NUM_GRAINS = 2;

  for (size_t ii = 0; ii < 10; ii++) {
    std::vector<Signal> grains;
    for (size_t ii = 0; ii < NUM_GRAINS; ii++) {
      size_t grain_start = seed_signal->data[0].size() - grain_pos_dist(gen);
      size_t grain_length = grain_length_dist(gen);

      std::cout << "Length: " << seed_signal->data[0].size() << std::endl;
      std::cout << "Start: " << grain_start << ", Size: " << grain_length << std::endl;

      grains.push_back(seed_signal->slice(grain_start, grain_length));
    }

    Signal convolved_grains = grains[0];
    for (size_t ii = 1; ii < grains.size(); ii++) {
      convolved_grains = convolved_grains.convolve(grains[ii]);
    }

    seed_signal->append(convolved_grains);
  }
  std::ofstream ofstream("output.wav");
  seed_signal->write_to_wave(ofstream);
}
