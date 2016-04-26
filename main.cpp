#include "main.h"

int main(int argc, char** argv) {
    
 // Inicializamos OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
 // Inicializamos la Ventana
    glutInitWindowSize(SCREEN_SIZE);
    glutInitWindowPosition(SCREEN_POSITION);
    glutCreateWindow("Micro Minecraft");
    
 // Inicializaciones específicas
    initFunc();
    
 // Configuración CallBacks
    glutReshapeFunc(funReshape);
    glutDisplayFunc(funDisplay);
    glutSpecialFunc(funKeyboard);
    glutMouseFunc(funMouse);
            
 // Bucle principal
    glutMainLoop();
    
 // Destrucción de objetos
    destroyFunc();
    
    return(0);
}

void initFunc() {
    
 // Inicializamos GLEW
    GLenum err = glewInit();
    if(GLEW_OK != err) {
        printf("Error: %s\n",glewGetErrorString(err));
    }
    printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("This system supports OpenGL Version %s.\n",glGetString(GL_VERSION));
    
 // Habilitamos z-buffer
    glEnable(GL_DEPTH_TEST);
    
 // Modelo de Iluminación
    glEnable(GL_LIGHTING);
    // Luz ambiente global
    GLfloat IA[]  = { 0.5, 0.5, 0.5, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, IA);
    
 // Parámetros de la Luz 0 (direccional=sol)
    GLfloat Ia0[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat Id0[] = { 0.9, 0.9, 0.9, 1.0 };
    GLfloat Is0[] = { 0.5, 0.5, 0.5, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT , Ia0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE , Id0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Is0);
    glEnable(GL_LIGHT0);
    
 // Parámetros de la Luz 1 (posicional=bombilla)
    GLfloat Ia1[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat Id1[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat Is1[] = { 0.7, 0.7, 0.7, 1.0 };
    glLightfv(GL_LIGHT1, GL_AMBIENT , Ia1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE , Id1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, Is1);
    glLightf (GL_LIGHT1, GL_CONSTANT_ATTENUATION , 0.90);
    glLightf (GL_LIGHT1, GL_LINEAR_ATTENUATION   , 0.05);
    glLightf (GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01);
    glEnable(GL_LIGHT1);
    
 // Modelo de Sombreado
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    
 // Texturas
    glEnable(GL_TEXTURE_2D);
    glGenTextures(NT,textureName);
    
    const char *filename[NT] = { "common/img/grass.bmp",
                                 "common/img/ground_side.bmp",
                                 "common/img/ground_side_1.bmp"};
    
    int textureW, textureH;
    
    for(unsigned i=0; i<NT; i++) {
        
     // Seleccionamos la textura a configurar
	glBindTexture(GL_TEXTURE_2D, textureName[i]);
		
     // Cargamos la textura (RgbImage)
        RgbImage texture(filename[i]);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.GetNumCols(), texture.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE, texture.ImageData());
        
     // Configuramos la textura
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
    }
    
}

void funReshape(int w, int h) {
    
 // Guardamos la anchura y altura actuales en un par de variables globales
    current_w = w;
    current_h = h;
    
 // Configuramos el Viewport
    glViewport(0, 0, w, h);

 // Configuracion del modelo de proyeccion
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
 // Proyeccion en Perspectiva   
    gluPerspective(75*zoom, (GLfloat)w/(GLfloat)h, 0.2, 100);
 
}

void funDisplay(void) {
    
 // Borramos el buffer de color
    glClearColor(SCREEN_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
 // Activamos la matriz MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
    
 // Hace posible el movimiento de rotación de la camara con las flechas izquierda y derecha
    glRotatef(camAngle, 0.0, 1.0, 0.0);
    
 // Posicionamos la cámara (V)
    GLfloat pos[3]    = {0.0,  2.0,  zCamPosition};
    GLfloat lookat[3] = {0.0,  2.0, -10.0};
    GLfloat up[3]     = {0.0,  1.0,  0.0};
    gluLookAt(    pos[0],    pos[1],    pos[2],
               lookat[0], lookat[1], lookat[2],
                   up[0],     up[1],     up[2]);

 // Dibujamos la escena (M)  
    drawLights();
    drawGround();
    drawObject();
    drawLantern();
 // Intercambiamos los buffers   
    glutSwapBuffers();
    
}

void drawGround() {
    
 // Definimos del suelo
    GLfloat Ka[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat Kd[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat Ks[] = { 0.8, 0.8, 0.8, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT  , Ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE  , Kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR , Ks);
    glMaterialf (GL_FRONT, GL_SHININESS, 50.0);
 
 // Césped
    glBindTexture(GL_TEXTURE_2D, textureName[0]);
    glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3f( -10.0,  0.0,  0.0);
        glTexCoord2f(8.0, 0.0); glVertex3f(  10.0,  0.0,  0.0);
	glTexCoord2f(8.0, 16.0); glVertex3f( 10.0,  0.0, -8.0);
        glTexCoord2f(0.0, 16.0); glVertex3f(-10.0,  0.0, -8.0);
    glEnd();
    
 // Tierra
    glBindTexture(GL_TEXTURE_2D, textureName[2]);
    glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, 1.0);
        glTexCoord2f(0.0, 0.0); glVertex3f( -10.0, -10.0, 0.0);
        glTexCoord2f(4.0, 0.0); glVertex3f( 10.0, -10.0, 0.0);
	glTexCoord2f(4.0, 4.0); glVertex3f( 10.0,  0.0,  0.0);
        glTexCoord2f(0.0, 4.0); glVertex3f( -10.0,  0.0,  0.0);
    glEnd();
   
}

void drawObject() {
    
 // Definimos el material del Objeto
    GLfloat Ka[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat Kd[] = { 0.7, 0.7, 0.3, 1.0 };
    GLfloat Ks[] = { 0.5, 0.5, 0.5, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT  , Ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE  , Kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR , Ks);
    glMaterialf (GL_FRONT, GL_SHININESS, 50.0);
    
 // Definimos el Objeto
    glPushMatrix();
        glTranslatef(0.0, 1.0, -2.0);
        cube();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, 3.0, -2.0);
        cube();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, 1.0, -2.0);
        cube();
    glPopMatrix();
}

void cube(){
    glBindTexture(GL_TEXTURE_2D, textureName[0]);  
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.0,  1.0, -1.0);
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f(-1.0,  1.0, -1.0); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, textureName[1]);
    glBegin(GL_QUADS);
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, textureName[2]);  
    glBegin(GL_QUADS);
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.0, -1.0,  1.0);    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0,  1.0);
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
    glEnd();
}


void drawLantern(){
    
}

void funKeyboard(int key, int x, int y) {
    
    switch(key) {
       case GLUT_KEY_UP:
           if(zCamPosition >= -10){
            zCamPosition -= 0.1;
           }
           break;
       case GLUT_KEY_DOWN:
           if(zCamPosition <= 1){
            zCamPosition += 0.1;
           }
           break;
       case GLUT_KEY_LEFT:
           camAngle-=1.0;
           if(camAngle == -360.0){
               camAngle=0.0;
           }
           break;
       case GLUT_KEY_RIGHT:
           camAngle+=1.0;
           if(camAngle == 360.0){
               camAngle=0.0;
           }
           break;
    }
   
   glutPostRedisplay();

}

void funMouse(int button, int state, int x, int y) {
    
    switch(button) {
        case 3:
            zoom -= 0.05;
            if(zoom<0.5) zoom = 0.5;
            funReshape(current_w, current_h);
            break;
        case 4:
            zoom += 0.05;
            if(zoom>1.5) zoom = 1.5;
            funReshape(current_w, current_h);
            break;
    }
   
    glutPostRedisplay();
    
}
