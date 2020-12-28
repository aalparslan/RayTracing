for file in input/*
do
	echo "Processing file $file"
	./raytracer $file
	echo

done
