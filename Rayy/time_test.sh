#!/bin/bash


commit_hash=53ef76c

rm examples/*.txt

for file in examples/*
do
    echo "Testing" $file
    /usr/bin/time -o "$file"_"$commit_hash"_performance.txt -f "%U" ./raytracer $file  &
done

wait

for image_file in *.ppm
do
    echo "Converting" $image_file
    convert $image_file $image_file.jpg
done



# Writing to file...
printf "a = {" > performance_$commit_hash.txt
for file in examples/*.txt
do
    printf "'$file': '$(cat $file)', " >> performance_$commit_hash.txt
done
printf "}" > performance_$commit_hash.txt
echo ""  > performance_$commit_hash.txt

