#!/bin/bash

rm .gitignore
git status | sed -e 's/#\t*//' | grep libusb > .gitignore
