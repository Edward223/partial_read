#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common/common.h"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "zstd.h"

namespace {

bool CompressFileInChunks(const std::filesystem::path &input_path,
                          const std::filesystem::path &output_root,
                          const pr::PrParams &params,
                          std::vector<char> &src_buffer,
                          std::vector<char> &dst_buffer) {
  namespace fs = std::filesystem;
  ZSTD_CCtx *cctx = ZSTD_createCCtx();
  PR_Param *pr_params;

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

  std::size_t chunk_index = 0;
  while (input) {
    input.read(src_buffer.data(), params.compress_block_size);
    const std::streamsize read_bytes = input.gcount();
    if (read_bytes <= 0) {
      break;
    }

    const std::size_t src_size = static_cast<std::size_t>(read_bytes);
    const std::size_t required_capacity = ZSTD_compressBound(src_size);
    if (dst_buffer.size() < required_capacity) {
      dst_buffer.resize(required_capacity);
    }

    const std::size_t compressed_size =
        ZSTD_compress(dst_buffer.data(), dst_buffer.size(), src_buffer.data(),
                      src_size, params.compression_level);
    if (ZSTD_isError(compressed_size)) {
      spdlog::error("ZSTD_compress failed for {} chunk {}: {}",
                    input_path.filename().string(), chunk_index,
                    ZSTD_getErrorName(compressed_size));
      return false;
    }

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

} // namespace

int main() {
  namespace fs = std::filesystem;

  const fs::path dataset_dir = pr::kRawDataDir / "silesia";
  if (!fs::exists(dataset_dir) || !fs::is_directory(dataset_dir)) {
    spdlog::error("Dataset directory not found or not a directory: {}",
                  dataset_dir.string());
    return 1;
  }

  const pr::PrParams params{
      .compress_block_size = 1 * pr::MB,
      .compression_level = ZSTD_CLEVEL_DEFAULT,
  };

  std::vector<char> src_buffer(params.compress_block_size);
  std::vector<char> dst_buffer(ZSTD_compressBound(params.compress_block_size));

  const fs::path output_root = pr::kOutputDir / "silesia_chunks";
  std::error_code output_ec;
  fs::create_directories(output_root, output_ec);
  if (output_ec) {
    spdlog::error("Failed to create output root {}: {}", output_root.string(),
                  output_ec.message());
    return 1;
  }

  std::size_t processed_files = 0;
  for (const auto &entry : fs::directory_iterator(dataset_dir)) {
    if (!entry.is_regular_file()) {
      continue;
    }

    spdlog::info("Processing {}", entry.path().string());
    if (CompressFileInChunks(entry.path(), output_root, params, src_buffer,
                             dst_buffer)) {
      ++processed_files;
    }
  }

  spdlog::info("Completed processing {} files from {}", processed_files,
               dataset_dir.string());
  return 0;
}
