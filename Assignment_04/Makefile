# Get the platform name
UNAME = $(shell uname)

# C++ compiler
CXX = g++

# General flags
CXXFLAGS = -std=c++11 
LDFLAGS = -lpthread -ljpeg
GLFLAGS = -lglfw3  -lGLEW

# Add X flags for X11 systems
ifeq ($(UNAME), Linux)
	LDFLAGS += -lm -ldl -ldrm -lGL -lXdamage -lX11-xcb -lxcb-glx -xcb-dri2 -lXrandr -lXinerama -lXxf86vm -lXext -lXcursor -lXrender -lXfixes -lX11 -lxcb -lXau -lXdmcp
endif

# Add Darwin-specific linking flags
ifeq ($(UNAME), Darwin)
	GLFLAGS += -framework OpenGL
endif

SOURCES = $(wildcard ./src/*.cpp)

.PHONY: all clean

all:
	$(CXX) $(CXXFLAGS) -o hw4 $(SOURCES) -I./src/glm/ \
		$(LDFLAGS) \
		$(GLFLAGS)

clean:
	@rm -f hw4 *.o
