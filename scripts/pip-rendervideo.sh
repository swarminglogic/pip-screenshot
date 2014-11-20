#!/bin/bash

cd $1
lastFrame=$(ls -1 frames/ | tail -n 1)
nframes=$(echo ${lastFrame%.*} | sed 's/^0*//')
echo "$nframes frames."

vidfilename=$(pwd | xargs basename).mp4
time ffmpeg -r 15 -i frames/%05d.sgi -c:v libx264 -r 15 \
    -pix_fmt yuv420p $vidfilename
