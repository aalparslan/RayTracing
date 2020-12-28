#!/bin/bash

# Time test and correction test

mkdir -p test_results/examples
mkdir -p jpg_output
rm *.ppm

for file in examples/*
do
    echo "Testing" $file
    /usr/bin/time -o test_results/"$file"_performance.txt -f "%U" ./raytracer $file
    echo $file >> test_results/"$file"_performance.txt
done


# Check ppm files and convert original files to jpg
for image_file in *.ppm
do
    echo "Converting" $image_file "to JPG"
    python3 difference.py $image_file
    mv difference_image.jpg test_results/$image_file.jpg
    convert $image_file jpg_output/$image_file.jpg
done



