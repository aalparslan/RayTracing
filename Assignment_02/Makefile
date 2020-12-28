all:
	g++ -I ./src \
		src/IntersectionCalculator.cpp \
		src/MathematicalOperations.cpp \
		src/Transformator.cpp \
		src/ColorCalculator.cpp \
		src/SceneTransformations.cpp \
		src/parser.cpp \
		src/ppm.cpp \
		src/tinyxml2.cpp \
		src/main.cpp \
		src/jpeg.cpp \
		-o raytracer \
		-std=c++11 \
		-pthread \
		-ljpeg \
		-g


transformator_tester:
	g++ -I ./src \
		src/Transformator.cpp \
		src/TransformatorTester.cpp \
		src/MathematicalOperations.cpp \
		src/parser.cpp \
		src/tinyxml2.cpp \
		-o transformator_tester \
		-std=c++11 \
		-g