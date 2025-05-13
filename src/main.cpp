#include "Signal.hpp"
#include <fstream>
#include <iostream>
#include <random>

void print_help() {
  std::cout << "Usage: ./granular-convolution [wav_file]" << std::endl;
}

int main(int argc, char **argv) {

  if (argc != 2) {
    print_help();
    return 1;
  }

  std::ifstream ifstream(argv[1]);
  std::optional<Signal> seed_signal = Signal::parse_from_wave(ifstream);

  if (!seed_signal) {
    std::cerr << "Failed to parse signal from WAVE" << std::endl;
    return 1;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> grain_length_dist(1.0, 10.0);
  std::uniform_real_distribution<> grain_pos_dist(2.0, 70.0);

  constexpr size_t NUM_GRAINS = 2;

  Signal output_audio = *seed_signal;
  Signal convolved_audio(output_audio.sample_rate, output_audio.data.size());
  for (size_t ii = 0; ii < 100; ii++) {
    std::vector<Signal> grains;
    for (size_t ii = 0; ii < NUM_GRAINS; ii++) {
      float grain_start = seed_signal->get_length() - grain_pos_dist(gen);
      float grain_length = grain_length_dist(gen);

      if (grain_start < 0.0) {
        grain_start = 0.0;
      }

      // std::cout << "Length: " << seed_signal->data[0].size() << std::endl;
      // std::cout << "Start: " << grain_start << ", Size: " << grain_length
      //<< std::endl;

      grains.push_back(seed_signal->slice(grain_start, grain_length));
    }

    Signal convolved_grains = grains[0];
    for (size_t ii = 1; ii < grains.size(); ii++) {
      convolved_grains = convolved_grains.convolve(grains[ii]);
    }

    if (convolved_grains.get_length() > 0) {
      seed_signal->append_crossfade(convolved_grains,
                                    convolved_grains.get_length() / 2.0f);
      convolved_audio.append_crossfade(convolved_grains,
                                       convolved_grains.get_length() / 2.0f);
      std::cout << "New length: " << seed_signal->get_length() << std::endl;
    }
  }
  output_audio.append_crossfade(convolved_audio, output_audio.get_length());
  std::ofstream ofstream("output.wav");
  output_audio.write_to_wave(ofstream);
}
