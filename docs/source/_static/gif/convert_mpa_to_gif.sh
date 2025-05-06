#!/bin/bash

file_name=$1
file_name_without_extension=${file_name%%.*}

ffmpeg -y -i $file_name_without_extension.mp4 -vf palettegen $file_name_without_extension.png
ffmpeg -y -i $file_name_without_extension.mp4 -i $file_name_without_extension.png -lavfi "[0]scale=1080:trunc(ow/a/2)*2[x];[x][1:v] paletteuse" -r 5 $file_name_without_extension.gif
# ffmpeg -i input.gif -ss 00:00:00 -to 00:00:03 -c copy output.gif