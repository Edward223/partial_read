#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="${SCRIPT_DIR}/.."
RESULT_DIR="${ROOT_DIR}/result"
mkdir -p "${RESULT_DIR}"

ts="$(date +"%Y%m%d_%H%M%S")"
out="${RESULT_DIR}/${ts}.txt"

echo "Writing output to ${out}"
"${ROOT_DIR}/build/bin/pr_exam_exe" >"${out}" 2>&1