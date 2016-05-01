#include "Cube.h"

Cube::Cube(){
}

Cube::~Cube() {
}

Cube::Cube(GLfloat x, GLfloat y, GLfloat z, int cubeType) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->cubeType = cubeType;
}