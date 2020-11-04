#!/bin/bash

for file in examples/*
do
    echo "Testing" $file
    time ./raytracer $file > "$file"_performance.txt
done


for image_file in *.ppm
do
    echo "Converting" $file
    convert $image_file $image_file.jpg
done