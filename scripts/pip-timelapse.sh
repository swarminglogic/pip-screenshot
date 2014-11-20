#!/bin/bash

# $1:counter  $2:folderOuput
function doFrameCapture {
    outputfile=${2}/frames/$(printf '%.5d' $1)'.'$image_ext
    pip-screenshot "$outputfile"
}

function showHelp() {
echo "pip-timelapse - calls pip-screenshot at regular intervals

Usage:
$0 OUTPUTDIR [DELAY]

OUTPUTDIR  Directory to store pip screen captures './frames' subdir.
DELAY      Time between each screen capture (default=3.0)

"
}

if [ $# -lt 1 ] ; then
    echo "Error: Missing output dir parameter." >&2
    showHelp
    exit 1
fi


outputFolder=$1
delay=${2:-"3"}
image_ext='sgi'
curr=0

if [ -e $outputFolder/frames ] ; then
    lastFrame=`ls -1 $outputFolder/frames/ | tail -n 1`
    curr=`echo ${lastFrame%.*} | sed 's/^0*//'`
    while true; do
        read -p "Recording in progress (currently $curr frames)
Do you wish to continue at frame "$((curr+1))"? [y/n] " yn
        case $yn in
            [Yy]* ) break;;
            [Nn]* ) exit;;
            * ) echo "Please answer yes or no.";;
        esac
    done
fi

if [ ! -e $outputFolder/frames ]  ; then mkdir $outputFolder/frames ;  fi

while sleep $delay;
do
    ((curr++))
    (doFrameCapture $curr $outputFolder &)
done
