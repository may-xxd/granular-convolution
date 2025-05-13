#include "Signal.hpp"
#include "FFTConvolver.h"
#include "Util.hpp"
#include <iostream>

static constexpr uint16_t FORMAT_TAG_WAVE_FORMAT_PCM = 0x0001;

Signal Signal::slice(float start, float size) const {
  Signal ret;
  ret.sample_rate = sample_rate;

  size_t start_samples = start * sample_rate;
  size_t size_samples = size * sample_rate;

  for (const auto &channel : data) {
    ret.data.push_back(std::vector<float>());

    if (start_samples >= channel.size()) {
      continue;
    }

    if (start_samples + size_samples >= channel.size()) {
      size_samples = channel.size() - start_samples;
    }

    ret.data.back() =
        std::vector<float>(channel.begin() + start_samples,
                           channel.begin() + size_samples + start_samples);
  }

  return ret;
}

float Signal::get_length() const {
  return static_cast<float>(data[0].size()) / sample_rate;
}

Signal Signal::convolve(const Signal &other) const {
  Signal ret;
  ret.sample_rate = sample_rate;

  if (other.data.size() != data.size()) {
    std::cerr << "Unable to compute convolution, signals differ in number of "
                 "channels ("
              << data.size() << " and " << other.data.size() << ")"
              << std::endl;
    return ret;
  }

  for (size_t channel_ii = 0; channel_ii < data.size(); channel_ii++) {
    const std::vector<float> &channel = data[channel_ii];
    const std::vector<float> &other_channel = other.data[channel_ii];

    fftconvolver::FFTConvolver convolver;
    convolver.init(1024, channel.data(), channel.size());
    ret.data.push_back(std::vector<float>(other_channel.size()));
    convolver.process(other_channel.data(), ret.data.back().data(),
                      ret.data.back().size());

    float max = 0;
    for (size_t ii = 0; ii < ret.data.back().size(); ii++) {
      if (std::abs(ret.data.back()[ii]) > max) {
        max = std::abs(ret.data.back()[ii]);
      }
    }

    constexpr size_t ENVELOPE_LENGTH = 100;

    if (max != 0.0f) {
      // normalise to max, and also apply small envelope to start and end to
      // avoid clicks
      for (size_t ii = 0; ii < ret.data.back().size(); ii++) {
        if (ii < ENVELOPE_LENGTH) {
          ret.data.back()[ii] *= static_cast<float>(ii) / ENVELOPE_LENGTH;
        } else if (ret.data.back().size() - ii < ENVELOPE_LENGTH) {
          ret.data.back()[ii] *=
              static_cast<float>(ret.data.back().size() - ii) / ENVELOPE_LENGTH;
        }

        ret.data.back()[ii] /= max;
      }
    }
  }

  return ret;
}

void Signal::append(const Signal &other) {
  if (other.data.size() != data.size()) {
    std::cerr << "Unable to append signal, signals differ in number of "
                 "channels ("
              << data.size() << " and " << other.data.size() << ")"
              << std::endl;
    return;
  }

  for (size_t channel_ii = 0; channel_ii < data.size(); channel_ii++) {
    data[channel_ii].insert(data[channel_ii].end(),
                            other.data[channel_ii].begin(),
                            other.data[channel_ii].end());
  }
}

void Signal::append_crossfade(const Signal &other, float crossfade_time) {
  if (other.data.size() != data.size()) {
    std::cerr << "Unable to append signal, signals differ in number of "
                 "channels ("
              << data.size() << " and " << other.data.size() << ")"
              << std::endl;
    return;
  }

  size_t crossfade_size_samples = crossfade_time * sample_rate;

  for (size_t ii = 0; ii < crossfade_size_samples; ii++) {
    float crossfade_factor =
        static_cast<float>(ii) / static_cast<float>(crossfade_size_samples);

    for (size_t channel_ii = 0; channel_ii < data.size(); channel_ii++) {
      std::vector<float> &channel = data[channel_ii];
      const std::vector<float> &other_channel = other.data[channel_ii];

      if (ii < channel.size() && ii < other_channel.size()) {
        channel[channel.size() - crossfade_size_samples + ii] =
            (1.0 - crossfade_factor) * channel[channel.size() - crossfade_size_samples + ii] +
            crossfade_factor * other_channel[ii];
      }
    }
  }

  for (size_t channel_ii = 0; channel_ii < data.size(); channel_ii++) {
    data[channel_ii].insert(data[channel_ii].end(),
                            other.data[channel_ii].begin() +
                                crossfade_size_samples,
                            other.data[channel_ii].end());
  }
}

void Signal::write_to_wave(std::ostream &ostream) const {
  // TODO: verify
  ostream.write("RIFF", 4);

  uint32_t file_size = 36 + data.size() * data[0].size() * 2;
  Util::write_u32_le(ostream, file_size);

  ostream.write("WAVE", 4);

  ostream.write("fmt ", 4);

  // chunk size
  Util::write_u32_le(ostream, 16);

  Util::write_u16_le(ostream, FORMAT_TAG_WAVE_FORMAT_PCM);
  // num channels
  Util::write_u16_le(ostream, data.size());
  // sample rate
  Util::write_u32_le(ostream, sample_rate);
  // byte rate
  Util::write_u32_le(ostream, sample_rate * 2 * data.size());
  // block size
  Util::write_u16_le(ostream, 2 * data.size());
  // bits per sample
  Util::write_u16_le(ostream, 16);

  ostream.write("data", 4);

  // chunk size
  Util::write_u32_le(ostream, data.size() * data[0].size() * 2);

  for (size_t sample_ii = 0; sample_ii < data[0].size(); sample_ii++) {
    for (size_t channel_ii = 0; channel_ii < data.size(); channel_ii++) {
      float sample_f = data[channel_ii][sample_ii];
      if (sample_f > 1.0f) {
        sample_f = 1.0f;
      }
      if (sample_f < -1.0f) {
        sample_f = -1.0f;
      }
      int32_t sample_32 = static_cast<int32_t>(sample_f * 32768);
      int16_t sample = sample_32 >= 32767    ? 32767
                       : sample_32 <= -32768 ? -32768
                                             : sample_32;
      // std::cout << "sample (f): " << sample_f << ", sample (32): " <<
      // sample_32
      //<< ", sample (16): " << sample << std::endl;
      Util::write_i16_le(ostream, sample);
    }
  }
}

std::optional<Signal> Signal::parse_from_wave(std::istream &wave_stream) {
  Signal ret;
  char riff_chunk_id[4] = {0};
  wave_stream.read(riff_chunk_id, sizeof(riff_chunk_id));

  if (riff_chunk_id[0] != 'R' || riff_chunk_id[1] != 'I' ||
      riff_chunk_id[2] != 'F' || riff_chunk_id[3] != 'F') {
    std::cerr << "Failed to parse WAVE file, does not start with RIFF chunk id"
              << std::endl;
    return std::nullopt;
  }

  uint32_t riff_chunk_size = Util::read_u32_le(wave_stream);

  char wave_id[4] = {0};
  wave_stream.read(wave_id, sizeof(wave_id));
  if (wave_id[0] != 'W' || wave_id[1] != 'A' || wave_id[2] != 'V' ||
      wave_id[3] != 'E') {
    std::cerr << "Failed to parse WAVE file, WAVE id is incorrect" << std::endl;
    return std::nullopt;
  }

  riff_chunk_size -= 4;

  std::optional<uint32_t> block_size;
  std::optional<uint16_t> bits_per_sample;
  std::optional<uint32_t> byte_rate;

  uint32_t data_read = 0;

  while (data_read < riff_chunk_size) {
    char chunk_id[4] = {0};
    wave_stream.read(chunk_id, sizeof(chunk_id));
    uint32_t chunk_size = Util::read_u32_le(wave_stream);
    data_read += sizeof(chunk_id) + chunk_size;
    if (chunk_id[0] == 'f' && chunk_id[1] == 'm' && chunk_id[2] == 't' &&
        chunk_id[3] == ' ') {
      // fmt chunk

      uint16_t format_tag = Util::read_u16_le(wave_stream);
      uint16_t num_channels = Util::read_u16_le(wave_stream);
      uint32_t sample_rate = Util::read_u32_le(wave_stream);
      byte_rate = Util::read_u32_le(wave_stream);
      block_size = Util::read_u16_le(wave_stream);
      if (format_tag != FORMAT_TAG_WAVE_FORMAT_PCM) {
        std::cerr << "Unknown fmt chunk format tag " << format_tag << std::endl;
        return std::nullopt;
      }
      bits_per_sample = Util::read_u16_le(wave_stream);

      ret.sample_rate = sample_rate;
      for (uint32_t channel = 0; channel < num_channels; channel++) {
        ret.data.push_back(std::vector<float>());
      }

      if (chunk_size > 16) {
        wave_stream.ignore(chunk_size - 16);
      }
    } else if (chunk_id[0] == 'd' && chunk_id[1] == 'a' && chunk_id[2] == 't' &&
               chunk_id[3] == 'a') {
      // data chunk
      uint32_t data_remaining = chunk_size;
      while (data_remaining) {
        // for each channel
        for (std::vector<float> &channel : ret.data) {
          if (*bits_per_sample == 32) {
            data_remaining -= 4;
            channel.push_back(
                static_cast<float>(Util::read_i32_le(wave_stream)) /
                static_cast<float>(2147493647.f));
          } else if (*bits_per_sample == 16) {
            data_remaining -= 2;
            channel.push_back(
                static_cast<float>(Util::read_i16_le(wave_stream)) / 32768.0f);
          } else {
            std::cerr << "Unable to handle PCM WAVE data with bit depth "
                      << *bits_per_sample << std::endl;
          }
        }
      }
    } else {
      std::cerr << "Ignoring chunk with unknown id " << chunk_id[0]
                << chunk_id[1] << chunk_id[2] << chunk_id[3] << std::endl;
      wave_stream.ignore(chunk_size);
    }
  }

  return ret;
}
