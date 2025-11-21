#!/bin/bash

file_name=$1
file_name_without_extension=${file_name%%.*}

ffmpeg -y -i $file_name_without_extension.mp4 -vf palettegen $file_name_without_extension.png
ffmpeg -y -i $file_name_without_extension.mp4 -i $file_name_without_extension.png -lavfi "[0]scale=1080:trunc(ow/a/2)*2[x];[x][1:v] paletteuse" -r 5 $file_name_without_extension.gif

# --> you canmake several part and then concat them :
# ffmpeg -ss 2 -to 42 -i input.mp4 -c copy output.mp4
# echo "file 'part1.mp4' file 'part2.mp4' file 'part3.mp4' file 'part4.mp4' > list.txt"
# ffmpeg  -f concat -safe 0 -i list.txt  -c copy configure_idf.mp4
