#include "Signal.hpp"
#include "Util.hpp"
#include <iostream>

static constexpr uint16_t FORMAT_TAG_WAVE_FORMAT_PCM = 0x0001;

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
        ret.data.push_back(std::vector<int16_t>());
      }
    } else if (chunk_id[0] == 'd' && chunk_id[1] == 'a' && chunk_id[2] == 't' &&
               chunk_id[3] == 'a') {
      // data chunk
      uint32_t data_remaining = chunk_size;
      while (data_remaining) {
        // for each channel
        for (std::vector<int16_t> &channel : ret.data) {
          if (*bits_per_sample != 16) {
            std::cerr << "Unable to parse non 16-bit PCM data" << std::endl;
            return std::nullopt;
          }
          data_remaining -= 2;
          channel.push_back(Util::read_i16_le(wave_stream));
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
