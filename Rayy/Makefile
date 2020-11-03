all:
	g++ -O0 -g afeser.cpp helper.cpp parser.cpp ppm.cpp tinyxml2.cpp main.cpp -o raytracer -std=c++11
	./raytracer examples/simple.xml
	convert test.ppm test.jpg
