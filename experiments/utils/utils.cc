#include <filesystem>
#include <fstream>
#include <limits>
#include <vector>

#include "spdlog/spdlog.h"
#include "utils/utils.h"

namespace pr {

size_t CompressFile(const std::string &file_path, void *dst,
                    std::size_t dst_capacity, int compression_level) {
  namespace fs = std::filesystem;

  const fs::path source_path = fs::u8path(file_path);
  const std::string source_display = source_path.string();
  spdlog::info("Compressing file {} with level {}", source_display,
               compression_level);

  if (dst == nullptr && dst_capacity > 0) {
    spdlog::error("Destination buffer is null (capacity {}) for {}",
                  dst_capacity, source_display);
    return -5;
  }

  std::ifstream input(source_path, std::ios::binary | std::ios::ate);
  if (!input) {
    spdlog::error("Failed to open {} for compression", source_display);
    return -1;
  }

  const std::streamsize file_size_signed = input.tellg();
  if (file_size_signed < 0) {
    spdlog::error("tellg failed for {}", source_display);
    return -1;
  }

  const std::size_t file_size = static_cast<std::size_t>(file_size_signed);
  input.seekg(0, std::ios::beg);

  std::vector<char> buffer(file_size);
  if (file_size > 0) {
    input.read(buffer.data(), file_size);
    if (input.gcount() != static_cast<std::streamsize>(file_size)) {
      spdlog::error("Read {} bytes but expected {} while loading {}",
                    input.gcount(), file_size, source_display);
      return -2;
    }
  }

  const std::size_t compress_bound = ZSTD_compressBound(file_size);
  if (dst_capacity < compress_bound) {
    spdlog::error("Destination capacity {} too small, need >= {} for {}",
                  dst_capacity, compress_bound, source_display);
    return -3;
  }

  const std::size_t compressed_size = ZSTD_compress(
      dst, dst_capacity, buffer.data(), file_size, compression_level);
  if (ZSTD_isError(compressed_size)) {
    spdlog::error("ZSTD_compress failed for {}: {}", source_display,
                  ZSTD_getErrorName(compressed_size));
    return -4;
  }

  if (file_size == 0) {
    spdlog::info("Compressed {} (empty input) -> {} bytes", source_display,
                 compressed_size);
  } else {
    const double ratio =
        static_cast<double>(compressed_size) / static_cast<double>(file_size);
    spdlog::info("Compressed {} ({} bytes) -> {} bytes (ratio {:.3f})",
                 source_display, file_size, compressed_size, ratio);
  }

  return compressed_size;
}

} // namespace pr