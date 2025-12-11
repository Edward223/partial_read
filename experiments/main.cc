#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "common/common.h"
#include "spdlog/spdlog.h"
#include "utils/utils.h"
#include "zstd.h"

int main() {
  namespace fs = std::filesystem;

  const fs::path dataset_dir = pr::kRawDataDir / "silesia";
  if (!fs::exists(dataset_dir) || !fs::is_directory(dataset_dir)) {
    spdlog::error("Dataset directory not found or not a directory: {}",
                  dataset_dir.string());
    return 1;
  }

  const pr::TestParams params{
      .compress_block_size = 1 * pr::MB,
      .inner_block_size = 64 * pr::KB,
      .use_partial_read = false, // TODO: implement partial read path
      .save_compressed_chunks = true,
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
    if (pr::CompressFileInChunks(entry.path(), output_root, params, src_buffer,
                                 dst_buffer)) {
      ++processed_files;
    }
  }

  spdlog::info("Completed processing {} files from {}", processed_files,
               dataset_dir.string());
  return 0;
}
