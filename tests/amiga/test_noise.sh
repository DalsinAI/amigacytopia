#!/bin/sh
# Host test for amiga/noise (see test_noise.cxx). MIT, Copyright (c) 2026 AmiCradle Team.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
${HOSTCXX:-c++} -std=c++17 -O1 -Wall -I"$ROOT/amiga/noise" "$ROOT/amiga/noise/noise.cxx" "$ROOT/tests/amiga/test_noise.cxx" -o "$T/test_noise"
"$T/test_noise"
