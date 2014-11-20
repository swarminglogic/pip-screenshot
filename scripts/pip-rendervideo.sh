#!/bin/bash

cd $1
lastFrame=$(ls -1 frames/ | tail -n 1)
nframes=$(echo ${lastFrame%.*} | sed 's/^0*//')
echo "$nframes frames."

echo "Converting frames to PPM format"
if [ ! -d prepared ]  ; then mkdir prepared ; fi

for i in frames/* ; do
    newname=prepared/$(basename ${i/.*/.ppm})
    if [ -f "$newname" ] ; then
        continue
    else
        convert $i prepared/$(basename ${i/.*/.ppm})
    fi
done

vidfilename=$(pwd | xargs basename).mp4
time ffmpeg -r 15 -i prepared/%05d.ppm -c:v libx264 -r 15 \
    -pix_fmt yuv420p $vidfilename && \
    echo "Cleaning up PPM frames" &&
    rm prepared/* && rmdir prepared

