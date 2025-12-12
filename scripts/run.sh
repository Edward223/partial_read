#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
RESULT_DIR="${ROOT_DIR}/result"
mkdir -p "${RESULT_DIR}"

ts="$(date +"%Y-%m-%d__%H-%M-%S")"
out="${RESULT_DIR}/${ts}.log"

echo "Writing output to ${out}"

# Use a pseudo-TTY so spdlog keeps ANSI colors; script is commonly available.
# script -q -c "${ROOT_DIR}/build/bin/pr_exam_exe" "${out}"
script -q -c "${ROOT_DIR}/build/bin/pr_exam_exe" "${out}" >/dev/null 2>&1