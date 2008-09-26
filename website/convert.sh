#!/bin/bash

BASE=$(basename $1 .png)
convert $1 -resize 100 $BASE-small.jpg
convert $1 -resize 320 $BASE-medium.jpg
convert $1 -resize 640 $BASE-large.jpg
convert $1 $BASE.jpg
