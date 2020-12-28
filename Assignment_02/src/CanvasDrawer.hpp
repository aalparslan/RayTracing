#include "helper.hpp"

class CanvasDrawer{
    public:
    // Constructor
    CanvasDrawer(Scene& const scene);
    // Draw canvas and return the image
    void drawCanvas();

    private:
    Scene& const scene;
};