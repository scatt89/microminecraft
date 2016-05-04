#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef CUBE_H
#define CUBE_H

class Cube {
public:
    Cube();
    Cube(GLfloat x, GLfloat y, GLfloat z, int cubeType);
    Cube(GLfloat x, GLfloat y, GLfloat z, GLfloat rotation, int cubeType);
    virtual ~Cube();
    
    void setX(GLfloat x){ this->x = x; }
    void setY(GLfloat y){ this->y = y; }
    void setZ(GLfloat z){ this->z = z; }
    void setRotation (GLfloat rotation){ this->rotation; }
    void setCubeType(int cubeType){ this->cubeType = cubeType; }
    GLfloat getX(){ return x; }
    GLfloat getY(){ return y; }
    GLfloat getZ(){ return z; }
    GLfloat getRotation(){ return rotation; }
    int getCubeType(){ return cubeType; }
    
    
private:
    GLfloat x, y, z;
    int cubeType;
    GLfloat rotation;
};

#endif /* CUBE_H */

