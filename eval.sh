#!/usr/bin/env bash

for f in `ls data`; do
    ./cmake-build-debug/ghc18 < data/$f > d_out/$f
done
