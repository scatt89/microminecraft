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
#include "RgbImage.h"

//Variables globales

#define SCREEN_SIZE      900, 600
#define SCREEN_POSITION   50,  50
#define SCREEN_COLOR     0.0, 0.0, 0.0, 0.0

// Animaciones
    GLfloat angX = 0.0, angY = 0.0, zoom =  2.0;
    GLfloat camAngle = 0.0, zCamPosition = 1.0;
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
      
void initFunc();
void destroyFunc();
void funReshape(int w, int h);
void funDisplay();
void drawGround();
void drawLights();
void drawObject();
void cube();
void drawLantern();
void funKeyboard(int key, int x, int y);
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

