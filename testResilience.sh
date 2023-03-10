#!/usr/bin/env bash
set -euo pipefail
PORT=8040

if [ "$#" -ne 1 ]; then
  echo "Usage: $(basename "$0") <URL://filelist.csv>"
  exit 1
fi

# Spawn the coordinator process
cmake-build-debug/coordinator "$1" $PORT &

# Spawn some workers
for _ in {1..4}; do
  cmake-build-debug/worker "localhost" $PORT &
done

# Now we simulate a failing worker.
# This should never lead to incorrect results.
sleep 1
kill -9 $! || true

# And wait for completion
time wait
