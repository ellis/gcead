#!/usr/bin/env bash

mkdir -p temp
git clone https://github.com/libusb/libusb.git
cd libusb
git checkout -b temp v1.0.20

