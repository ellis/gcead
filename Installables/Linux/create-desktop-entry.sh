#!/bin/bash

echo "[Desktop Entry]
Name=GC-EAD/2009
GenericName=GC-EAD acquisition and analysis
Type=Application
Terminal=false
Encoding=UTF-8
Exec=$1/GcEad.sh
Icon=$1/GcEad.png
Path=$1/
Categories=Education;Science;" > $2
