#!/bin/bash

if [ ! -e .current ] ; then echo "1" > .current ; fi
currId=$(cat .current)
folder=$(printf '%.3d' $currId)_$(date +'%Y-%m-%d')_$1
mkdir $folder
echo $((++currId)) > .current
