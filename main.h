/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.h
 * Author: scatt
 *
 * Created on 24 de abril de 2016, 0:42
 */

#ifndef MAIN_H
#define MAIN_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include "RgbImage.h"
#include "Cube.h"
#include <list>

//Variables globales

#define SCREEN_SIZE      900, 600
#define SCREEN_POSITION   50,  50
#define SCREEN_COLOR     0.196078,  0.5, 0.8, 0.0

// Animaciones
    GLfloat zoom =  1.0;
    //Angulo de rotación para la dirección de la camera
    GLfloat camAngle = 0.0;
    //Vector que representa la dirección de la camera
    GLfloat lx = 0.0, lz=-1.0;
    //Posición X, Y, Z de la camera
    GLfloat x = 0.0, z = 5.0, y = 3.0;
    //Ratio de aspecto de la escena
    GLsizei current_w, current_h;
// Luces   
    GLfloat PL0[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat PL1[] = {-2.0, 1.0,-4.0, 1.0 };
// Texturas
    #define NT 3
    GLuint textureName[NT];
// VBOs
    #define NB 4
    GLuint  VBO[NB];
    #define vbuffer VBO[0]
    #define nbuffer VBO[1]
    #define tbuffer VBO[2]
    #define ibuffer VBO[3]
// Creacion de objetos
    //Controla si se ha pulsado la tecla de crear cubo
    bool status_createCube = false;
    //Controla si se ha pulsado la tecla de colocar el cubo creado en escena
    bool create_object = false;
    //Coordenadas para ubicar el objeto en escena
    GLfloat xc = x, yc = y, zc = z-4.0;
    //lista de cubos para dibujar en la escena
    std::list<Cube> cubes;

      
void initFunc();
void destroyFunc();
void funReshape(int w, int h);
void funDisplay();
void drawGround();
void drawLights();
void drawInitialObjects();
void cube();
void drawLantern();
void funSpecialKeyboard(int key, int x, int y);
void funKeyboard(unsigned char key, int x, int y);
void funMouse(int button, int state, int x, int y);

inline void destroyFunc() {  
    glDeleteTextures(NT,textureName);
}

inline void drawLights() {
 // Luz 0: Direccional
    glLightfv(GL_LIGHT0, GL_POSITION, PL0);
 // Luz 1: Posicional
    glLightfv(GL_LIGHT1, GL_POSITION, PL1);
}


#endif /* MAIN_H */

