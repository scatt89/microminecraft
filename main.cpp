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
    glutSpecialFunc(funSpecialKeyboard);
    glutKeyboardFunc(funKeyboard);
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
                                 "common/img/ground_side_1.bmp",
                                 "common/img/grass_dark.bmp",
                                 "common/img/cloud1.bmp"
    };
    
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
    gluPerspective(45*zoom, (GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
 
}

void funDisplay(void) {
    
 // Borramos el buffer de color
    glClearColor(SCREEN_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
 // Activamos la matriz MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
    
 // Posicionamos la cámara (V)
    GLfloat pos[3]    = {x, y, z};
    GLfloat lookat[3] = {x+lx, y, z+lz};
    GLfloat up[3]     = {0.0f,  1.0f,  0.0f};
    gluLookAt(    pos[0],    pos[1],    pos[2],
               lookat[0], lookat[1], lookat[2],
                   up[0],     up[1],     up[2]);

 // Dibujamos la escena (M)  
    drawLights();
    drawGround();
    drawInitialObjects();
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
        glTexCoord2f(0.0, 0.0); glVertex3f(-100.0f, 0.0f, -100.0f);
	glTexCoord2f(100.0, 0.0); glVertex3f(-100.0f, 0.0f,  100.0f);
	glTexCoord2f(100.0, 100.0); glVertex3f( 100.0f, 0.0f,  100.0f);
	glTexCoord2f(0.0, 100.0); glVertex3f( 100.0f, 0.0f, -100.0f);
    glEnd();
    
   
}

void drawInitialObjects() {
    
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
        cube_1();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.0, 3.0, -2.0);
        cube_1();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, 1.0, -2.0);
        cube_1();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0.0, 20, -45.0);
        cloud_1();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-30.0, 30, -60.0);
        glRotatef(90, 0.0, 1.0, 0.0);
        cloud_1();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-10.0, 50, -25.0);
        cloud_2();
    glPopMatrix();
}

void drawObject() {
    
    if(status_createCube1){
        glPushMatrix();
            zc = z-6.0;
            glTranslatef(xc, yc, zc);
            cube_1();
        glPopMatrix();
        if(create_object){
            zc = z-6.0;
            cubes.push_back(Cube(xc, yc, zc, 1));
            status_createCube1 = false;
            create_object = false;
        }
    }
    
    if(status_createCube2){
        glPushMatrix();
            zc = z-6.0;
            glTranslatef(xc, yc, zc);
            cube_2();
        glPopMatrix();
        if(create_object){
            zc = z-6.0;
            cubes.push_back(Cube(xc, yc, zc, 2));
            status_createCube2 = false;
            create_object = false;
        }
    }
    
    if(status_createCube3){
        glPushMatrix();
            zc = z-6.0;
            glTranslatef(xc, yc, zc);
            cube_3();
        glPopMatrix();
        if(create_object){
            zc = z-6.0;
            cubes.push_back(Cube(xc, yc, zc, 3));
            status_createCube3 = false;
            create_object = false;
        }
    }
    
    for(std::list<Cube>::iterator i = cubes.begin(); i != cubes.end(); i++){
        glPushMatrix();
            glTranslatef(i->getX(),i->getY(),i->getZ());
            switch((int)i->getCubeType()){
                case 1:
                    cube_1();
                    break;
                case 2:
                    cube_2();
                    break;
                case 3:
                    cube_3();
                    break;
            }
        glPopMatrix();
    }
}

void cube_1(){
    
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

void cube_2(){
    glBindTexture(GL_TEXTURE_2D, textureName[2]);  
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.0,  1.0, -1.0);
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f(-1.0,  1.0, -1.0); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
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
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.0, -1.0,  1.0);    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0,  1.0);
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
    glEnd();
}

void cube_3(){
    glBindTexture(GL_TEXTURE_2D, textureName[3]);  
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.0,  1.0, -1.0);
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f(-1.0,  1.0, -1.0); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
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
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.0, -1.0,  1.0);    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, -1.0,  1.0);
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
    glEnd();
}

void cloud_1(){
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        //Cara frontal
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f( 0.0, 0.5, 0.0);
        glVertex3f( 3.0, 0.5, 0.0);
        glVertex3f( 3.0, 0.0, 0.0);
        //Cara arriba
        glVertex3f( 0.0, 0.5, 0.0);
        glVertex3f( 0.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.5, 0.0);
        //Cara izquierda
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f( 0.0, 0.5, 0.0);
        glVertex3f( 0.0, 0.5, -10.0);
        glVertex3f( 0.0, 0.0, -10.0);
        //Cara derecha
        glVertex3f( 3.0, 0.0, 0.0);
        glVertex3f( 3.0, 0.5, 0.0);
        glVertex3f( 3.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.0, -10.0);
        //Cara fondo
        glVertex3f( 0.0, 0.0, -10.0);
        glVertex3f( 0.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.0, -10.0);
        //Cara abajo
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f( 0.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.5, -10.0);
        glVertex3f( 3.0, 0.0, 0.0);
    glEnd();
}

void cloud_2(){
    glColor3f(0.9, 0.9, 0.9);
    glBegin(GL_QUADS);
        //cara abajo
        glVertex3f( 0.0, 0.0, 0.0);
        glVertex3f( 0.0, 0.0, -2.0);
        glVertex3f( -2.0, 0.0, -2.0);
        glVertex3f( -2.0, 0.0, -6.0);
        glVertex3f( -4.0, 0.0, -6.0);
        glVertex3f( -4.0, 0.0, -10.0);
        glVertex3f( 4.0, 0.0, -10.0);
        glVertex3f( 4.0, 0.0, -6.0);
        glVertex3f( 4.0, 0.0, 0.0);
        //cara arriba
        glVertex3f( 0.0, 1.0, 0.0);
        glVertex3f( 0.0, 1.0, -2.0);
        glVertex3f( -2.0, 1.0, -2.0);
        glVertex3f( -2.0, 1.0, -6.0);
        glVertex3f( -4.0, 1.0, -6.0);
        glVertex3f( -4.0, 2.0, -10.0);
        glVertex3f( 4.0, 1.0, -10.0);
        glVertex3f( 4.0, 1.0, -6.0);
        glVertex3f( 4.0, 2.0, 0.0);
    glEnd();
}

void drawLantern(){
    
}

void funSpecialKeyboard(int key, int xx, int yy) {
    
    switch(key) {
       case GLUT_KEY_UP:
           x += lx * 0.1;
           z += lz * 0.1;
           break;
       case GLUT_KEY_DOWN:
           x -= lx * 0.1;
           z -= lz * 0.1;
           break;
       case GLUT_KEY_LEFT:
           camAngle -= 0.01;
           lx = sin(camAngle);
           lz = -cos(camAngle);
           break;
       case GLUT_KEY_RIGHT:
           camAngle += 0.01f;
           lx = sin(camAngle);
           lz = -cos(camAngle);
           break;
    }
   
   glutPostRedisplay();

}

void funKeyboard(unsigned char key, int x, int y) {
    
    switch(key) {
        case 'a':
            xc -= 0.5;
            break;
        case 'w':
            yc += 0.5;
            break;
        case 'd':
            xc += 0.5;
            break;
        case 's':
            yc -= 0.5;
            break;
        case '1':
            status_createCube1 = true;
            break;
        case '2':
            status_createCube2 = true;
            break;
        case '3':
            status_createCube3 = true;
            break;
        case 'i':
            create_object = true;
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
