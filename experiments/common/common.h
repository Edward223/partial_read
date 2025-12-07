// Common constants shared by experiments.
#pragma once

#include <filesystem>

namespace pr::common {

inline constexpr std::size_t k1KiB = 1024;
inline constexpr std::size_t k4KiB = 4 * k1KiB;
inline constexpr std::size_t k8KiB = 8 * k1KiB;
inline constexpr std::size_t k64KiB = 64 * k1KiB;

inline const std::filesystem::path kProjectRoot{PROJECT_SOURCE_DIR};
inline const std::filesystem::path kCompressedInputDir =
    kProjectRoot / "data" / "compressed";
inline const std::filesystem::path kCompressedOutputDir =
    kProjectRoot / "result" / "compressed";

} // namespace pr::common
