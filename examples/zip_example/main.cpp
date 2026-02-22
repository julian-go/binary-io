#include <stdio.h>

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "binary-io/binary-io.hpp"

/// This example is intentionally minimal and does not implement a full ZIP file
/// parser. It only reads the local file headers and does not support data
/// descriptors, central directory records, or other ZIP features.

static constexpr uint32_t kZipMagicNumber = 0x04034b50;

struct ZipHeader {
  uint32_t magic_number{0};
  uint16_t version{0};
  std::bitset<16> flags{0};
  uint16_t compression_method{0};
  std::string timestamp{};
  uint32_t crc32{0};
  uint32_t compressed_size{0};
  uint32_t uncompressed_size{0};
  std::string file_name{};

  void print() {
    std::cout << "magic_number: " << std::hex << magic_number << std::dec
              << "\nversion: " << version << "\nflags: " << flags
              << "\ncompression_method: " << compression_method
              << "\nlast_modification: " << timestamp << "\ncrc32: " << std::hex
              << crc32 << std::dec << "\ncompressed_size: " << compressed_size
              << "\nuncompressed_size: " << uncompressed_size
              << "\nfile_name: " << file_name << "\n";
  }
};

std::vector<uint8_t> load_zip_file() {
  const auto path{std::filesystem::path{SOURCE_DIR} / "sample-1.zip"};
  auto file = std::ifstream{path, std::ios::binary};
  auto buffer = std::vector<uint8_t>{};
  if (file) {
    buffer = {std::istreambuf_iterator<char>{file},
              std::istreambuf_iterator<char>{}};
  }
  std::cout << "Read " << buffer.size() << " bytes from " << path << "\n";
  return buffer;
}

bool read_date_time(bio::LEReader& reader, std::string& timestamp) {
  uint16_t time = 0;
  uint16_t date = 0;
  bool ok = true;
  ok = ok && reader.read_u16(time);
  ok = ok && reader.read_u16(date);

  if (ok) {
    // MS-DOS date/time format
    const auto hours = (time >> 11) & 0x1F;
    const auto minutes = (time >> 5) & 0x3F;
    const auto seconds = (time & 0x1F) * 2;
    const auto year = ((date >> 9) & 0x7F) + 1980;
    const auto month = (date >> 5) & 0x0F;
    const auto day = date & 0x1F;
    char buf[20];
    std::snprintf(buf, sizeof(buf), "%d-%02d-%02d %02d:%02d:%02d", year, month,
                  day, hours, minutes, seconds);
    timestamp = buf;
  }
  return ok;
}

bool parse_file_header(bio::LEReader& reader, ZipHeader& header) {
  uint16_t flags{0};
  uint16_t file_name_length{0};
  uint16_t extra_field_length{0};

  bool ok = true;

  ok = ok && reader.read_u32(header.magic_number);
  ok = ok && reader.read_u16(header.version);
  ok = ok && reader.read_u16(flags);

  header.flags = std::bitset<16>(flags);

  ok = ok && reader.read_u16(header.compression_method);
  ok = ok && read_date_time(reader, header.timestamp);
  ok = ok && reader.read_u32(header.crc32);
  ok = ok && reader.read_u32(header.compressed_size);
  ok = ok && reader.read_u32(header.uncompressed_size);
  ok = ok && reader.read_u16(file_name_length);
  ok = ok && reader.read_u16(extra_field_length);

  header.file_name.resize(file_name_length);

  ok = ok && reader.read_bytes(header.file_name.data(), file_name_length);
  ok = ok && reader.skip(extra_field_length);

  ok = ok && header.magic_number == kZipMagicNumber;

  return ok;
}

std::optional<ZipHeader> parse_file_entry(bio::LEReader& reader) {
  auto header = ZipHeader{};
  bool ok = parse_file_header(reader, header);
  ok = ok && reader.skip(header.compressed_size);
  if (ok) {
    return header;
  }
  return std::nullopt;
}

void parse_zip_file(const std::vector<uint8_t>& buffer) {
  bio::LEReader reader{buffer.data(), buffer.size()};
  while (auto header = parse_file_entry(reader)) {
    header->print();
  }
}

int main() {
  const auto buffer = load_zip_file();
  parse_zip_file(buffer);
  return 0;
}
