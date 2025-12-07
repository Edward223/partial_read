#!/usr/bin/env bash

set -euo pipefail

usage() {
    echo "Usage: $0 [-p|-e|-a]"
    echo "  -p  build libpr only"
    echo "  -e  build experiments only"
    echo "  -a  build both libpr and experiments"
    exit 1
}

BUILD_LIB=0
BUILD_EXP=0

while getopts "pae" opt; do
    case "${opt}" in
        p) BUILD_LIB=1 ;;
        e) BUILD_EXP=1 ;;
        a) BUILD_LIB=1; BUILD_EXP=1 ;;
        *) usage ;;
    esac
done
shift $((OPTIND - 1))

if (( ! BUILD_LIB && ! BUILD_EXP )); then
    BUILD_LIB=1
    BUILD_EXP=1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
BUILD_DIR="${ROOT_DIR}/build"
BUILD_TYPE=${BUILD_TYPE:-Debug}

if (( BUILD_LIB )); then
    echo "[build] ensuring libpr artifacts are up to date"
    make -C "${ROOT_DIR}/libpr" lib -j"$(nproc)"
fi

if (( BUILD_EXP )); then
    mkdir -p "${BUILD_DIR}"

    echo "[build] configuring CMake project (${BUILD_TYPE})"
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

    echo "[build] building experiments target"
    cmake --build "${BUILD_DIR}" --target pr_exam_exe -- -j"$(nproc)"

    echo "[build] done. Binaries are under ${BUILD_DIR}/bin"
fi

if (( BUILD_LIB && ! BUILD_EXP )); then
    echo "[build] libpr build complete."
fi