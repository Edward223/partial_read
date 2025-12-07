// Common constants shared by experiments.
#pragma once

#include <cstddef>
#include <filesystem>

#include "zstd.h"

namespace pr {

inline constexpr std::size_t KB = 1024;
inline constexpr std::size_t MB = 1024 * KB;
inline constexpr std::size_t GB = 1024 * MB;

inline const std::filesystem::path kProjectRoot{PROJECT_SOURCE_DIR};
inline const std::filesystem::path kRawDataDir =
    kProjectRoot / ".." / ".." / "data" / "raw";
inline const std::filesystem::path kCompressedDataDir =
    kProjectRoot / ".." / ".." / "data" / "compressed";
inline const std::filesystem::path kOutputDir =
    kProjectRoot / ".." / ".." / "data" / "output";

struct PrParams {
  std::size_t compress_block_size{1 * MB};
  int compression_level{ZSTD_CLEVEL_DEFAULT};
};

} // namespace pr
