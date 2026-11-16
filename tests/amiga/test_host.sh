#!/bin/sh
# The port's host tests: amiga/noise and amiga/amiga_c99.hxx.
# MIT, Copyright (c) 2026 AmiCradle Team.
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
sh "$ROOT/tests/amiga/test_noise.sh"
T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
${HOSTCXX:-c++} -std=c++17 -O1 -Wall -I"$ROOT/amiga" "$ROOT/tests/amiga/test_c99.cxx" -o "$T/test_c99"
"$T/test_c99"
