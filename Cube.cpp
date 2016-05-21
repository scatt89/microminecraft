/* 
 * File:   Cube.cpp
 * Author: Marrarichy Da Silva Garcia
 *
 * Created on 24 de abril de 2016, 0:42
 */

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
    this->rotation = 0.0;
}

Cube::Cube(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation, int cubeType) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->cubeType = cubeType;
    this->rotation = rotation;
}