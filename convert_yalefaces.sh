#!/bin/bash -e

cd ../yalefaces

rm -f subject01.glasses.gif # Remove duplicated image

for i in {1..15}; do
    id=$(printf "%02d" $i) # Add leading zero in front of number
    images=`find . -maxdepth 1 -type f -name "subject$id.*"` # Find all images of that subject

    output_name=1
    for image in $images; do
        echo $image
        mkdir -p "s$i" # Create directory
        convert $image "s$i/$output_name.pgm"
        output_name=$((output_name+1))
    done
done
