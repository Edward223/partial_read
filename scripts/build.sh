#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
BUILD_DIR="${ROOT_DIR}/build"
BUILD_TYPE=${BUILD_TYPE:-Debug}

# echo "[build] ensuring libpr artifacts are up to date"
# make -C "${ROOT_DIR}/libpr" lib

# mkdir -p "${BUILD_DIR}"

echo "[build] configuring CMake project (${BUILD_TYPE})"
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
	-DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
	-DZSTD_LIB_PATH="${ROOT_DIR}/libpr/libzstd.a"

echo "[build] building experiments target"
cmake --build "${BUILD_DIR}" --target pr_exam_exe -- -j"$(nproc)"

echo "[build] done. Binaries are under ${BUILD_DIR}/bin"
