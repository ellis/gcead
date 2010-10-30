#!/bin/bash

FILES=$(ldd ../../GcEad-build-desktop/release/GcEad | grep -e Qt -e libstd -e libgcc | sed -e 's#[^/]*\(/.*\) .*#\1#')
# Use release libraries instead of debug libraries
FILES=$(echo "$FILES" | sed -e 's#/qt/lib/#/lib/#')
mkdir -p libs
rm -f libs/*
rsync -v --copy-links $FILES libs
