#!/usr/bin/env bash

for f in `ls data`; do
    echo "Running: $f"
    ./cmake-build-debug/ghc18 < data/$f > d_out/$f
done
