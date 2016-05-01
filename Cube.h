#include <GL/glew.h>
#include <GL/freeglut.h>

#ifndef CUBE_H
#define CUBE_H

class Cube {
public:
    Cube();
    Cube(GLfloat x, GLfloat y, GLfloat z);
    virtual ~Cube();
    
    void setX(GLfloat x){ this->x = x; }
    void setY(GLfloat y){ this->y = y; }
    void setZ(GLfloat z){ this->z = z; }
    GLfloat getX(){ return x; }
    GLfloat getY(){ return y; }
    GLfloat getZ(){ return z; }
    
private:
    GLfloat x, y, z;
};

#endif /* CUBE_H */

