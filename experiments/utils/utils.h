#pragma once

#include <cstddef>
#include <string>

#include "common/common.h"
#include "zstd.h"
namespace pr {

size_t CompressFile(const std::string &file_path, void *dst,
                    std::size_t dst_capacity, int compression_level);

size_t CompressFile(const std::string &file_path, void *dst,
                    std::size_t dst_capacity, int compression_level);

} // namespace pr