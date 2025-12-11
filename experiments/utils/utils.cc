#include <filesystem>
#include <fstream>
#include <limits>
#include <vector>

#include "fmt/format.h"
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

bool CompressFileInChunks(const std::filesystem::path &input_path,
                          const std::filesystem::path &output_root,
                          const TestParams &params,
                          std::vector<char> &src_buffer,
                          std::vector<char> &dst_buffer) {
  namespace fs = std::filesystem;

  std::ifstream input(input_path, std::ios::binary);
  if (!input) {
    spdlog::error("Failed to open {}", input_path.string());
    return false;
  }

  const fs::path file_output_dir = output_root / input_path.stem();
  std::error_code dir_ec;
  fs::create_directories(file_output_dir, dir_ec);
  if (dir_ec) {
    spdlog::error("Failed to create output directory {}: {}",
                  file_output_dir.string(), dir_ec.message());
    return false;
  }

  if (src_buffer.size() < params.compress_block_size) {
    src_buffer.resize(params.compress_block_size);
  }

  std::size_t chunk_index = 0;
  while (input) {
    input.read(src_buffer.data(), params.compress_block_size);
    const std::streamsize read_bytes = input.gcount();
    if (read_bytes <= 0) {
      break;
    }

    const std::size_t src_size = static_cast<std::size_t>(read_bytes);

    std::size_t compressed_size = 0;
    if (!CompressSingleChunk(src_buffer.data(), src_size, params, dst_buffer,
                             compressed_size)) {
      return false;
    }

    if (params.save_compressed_chunks) {
      const std::string chunk_filename = fmt::format(
          "{}_chunk{:04}.zst", input_path.filename().string(), chunk_index);
      const fs::path chunk_path = file_output_dir / chunk_filename;
      std::ofstream chunk_stream(chunk_path, std::ios::binary);
      if (!chunk_stream) {
        spdlog::error("Failed to open chunk file {}", chunk_path.string());
        return false;
      }
      chunk_stream.write(dst_buffer.data(),
                         static_cast<std::streamsize>(compressed_size));
      if (!chunk_stream) {
        spdlog::error("Failed to write compressed data to {}",
                      chunk_path.string());
        return false;
      }
    }

    const double ratio = src_size == 0 ? 0.0
                                       : static_cast<double>(compressed_size) /
                                             static_cast<double>(src_size);
    spdlog::info(
        "Compressed {} chunk {:04} ({} bytes -> {} bytes, ratio {:.3f})",
        input_path.filename().string(), chunk_index, src_size, compressed_size,
        ratio);

    ++chunk_index;
  }

  if (chunk_index == 0) {
    spdlog::warn("{} is empty; no chunks produced", input_path.string());
  } else {
    spdlog::info("Finished {} chunks for {}", chunk_index,
                 input_path.filename().string());
  }

  return true;
}

bool CompressSingleChunk(const char *src, std::size_t src_size,
                         const TestParams &params,
                         std::vector<char> &dst_buffer,
                         std::size_t &compressed_size) {
  if (params.use_partial_read) {
    spdlog::warn("use_partial_read=true is TODO; using full block for now");
    // TODO: implement partial read path using params.inner_block_size
  }

  const std::size_t required_capacity = ZSTD_compressBound(src_size);
  if (dst_buffer.size() < required_capacity) {
    dst_buffer.resize(required_capacity);
  }

  compressed_size = ZSTD_compress(dst_buffer.data(), dst_buffer.size(), src,
                                  src_size, params.compression_level);
  if (ZSTD_isError(compressed_size)) {
    spdlog::error("ZSTD_compress failed for chunk: {}",
                  ZSTD_getErrorName(compressed_size));
    return false;
  }

  return true;
}

} // namespace pr