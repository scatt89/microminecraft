#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef CUBE_H
#define CUBE_H

class Cube {
public:
    Cube();
    Cube(GLfloat x, GLfloat y, GLfloat z, int cubeType);
    virtual ~Cube();
    
    void setX(GLfloat x){ this->x = x; }
    void setY(GLfloat y){ this->y = y; }
    void setZ(GLfloat z){ this->z = z; }
    void setCubeType(int cubeType){ this->cubeType = cubeType; }
    GLfloat getX(){ return x; }
    GLfloat getY(){ return y; }
    GLfloat getZ(){ return z; }
    GLfloat getCubeType(){ return cubeType; }
    
private:
    GLfloat x, y, z;
    int cubeType;
};

#endif /* CUBE_H */

