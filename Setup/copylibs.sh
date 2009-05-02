#!/bin/bash

FILES=$(ldd ../release/GcEad | grep -e Qt -e libstd -e libgcc | sed -e 's#[^/]*\(/.*\) .*#\1#')
mkdir -p libs
rsync -v --copy-links $FILES libs
