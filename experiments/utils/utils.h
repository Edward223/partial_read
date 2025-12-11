#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "common/common.h"
#include "zstd.h"

namespace pr {

size_t CompressFile(const std::string &file_path, void *dst,
                    std::size_t dst_capacity, int compression_level);

bool CompressFileInChunks(const std::filesystem::path &input_path,
                          const std::filesystem::path &output_root,
                          const TestParams &params,
                          std::vector<char> &src_buffer,
                          std::vector<char> &dst_buffer);

bool CompressSingleChunk(const char *src, std::size_t src_size,
                         const TestParams &params,
                         std::vector<char> &dst_buffer,
                         std::size_t &compressed_size);

} // namespace pr