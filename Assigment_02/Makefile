all:
	g++ -I ./src \
		src/IntersectionCalculator.cpp \
		src/helper.cpp \
		src/parser.cpp \
		src/ppm.cpp \
		src/tinyxml2.cpp \
		src/main.cpp \
		-o raytracer \
		-std=c++11 \
		-lgcov -pthread \
		-O3 -g
