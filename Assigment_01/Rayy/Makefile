all:
	g++ -O3 -g afeser.cpp helper.cpp parser.cpp ppm.cpp tinyxml2.cpp main.cpp -o raytracer -std=c++11 -lgcov
	g++ -O3 -fprofile-arcs -ftest-coverage -pg afeser.cpp helper.cpp parser.cpp ppm.cpp tinyxml2.cpp main.cpp -o raytracer_profiler -std=c++11 -lgcov
	x86_64-w64-mingw32-g++ afeser.cpp helper.cpp parser.cpp ppm.cpp tinyxml2.cpp main.cpp -o raytracer_windows.exe -std=c++11 -static-libgcc -static-libstdc++ # windows compile...
