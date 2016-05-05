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
    glutIdleFunc(funIdle);
    
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
                                 "common/img/cloud1.bmp",
                                 "common/img/rock_floor.bmp",
                                 "common/img/wood.bmp",
                                 "sheep_face_front.bmp",
                                 "snow_cube.bmp"
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
//    GLfloat Ka[] = { 0.2, 0.2, 0.2, 1.0 };
//    GLfloat Kd[] = { 0.7, 0.7, 0.3, 1.0 };
//    GLfloat Ks[] = { 0.5, 0.5, 0.5, 1.0 };
//    glMaterialfv(GL_FRONT, GL_AMBIENT  , Ka);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE  , Kd);
//    glMaterialfv(GL_FRONT, GL_SPECULAR , Ks);
//    glMaterialf (GL_FRONT, GL_SHININESS, 50.0);
    
 // Nubes
    
    glPushMatrix();
        glTranslatef(0.0, 20, -45.0);
        cloud_1();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-30.0, 30, -60.0);
        cloud_1();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(-10.0, 50, -25.0);
        cloud_2();
    glPopMatrix();

    
    //árboles
    glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.0, -2.0, -1.0);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(0.5, -3.0, -1.5);
        tree();
    glPopMatrix();
    glPushMatrix();
        glTranslatef(2.5, -1.5, -2.0);
        tree();
    glPopMatrix();
    
    //Templo
    
    //suelo
    for(int i=1; i<=7; i++){
        for(int j=1; j<=7; j++){
            glPushMatrix();
                glTranslatef(i*2.0, 0.0, j*-3.0);
                floor();
            glPopMatrix();
        }
    }
    
    //pared frontal
    for(int i=4; i<=12; i=i+2){
        if(i!=8){
            glPushMatrix();
                glTranslatef(i, 0.0, -6.0);
                wood();
            glPopMatrix();
        }
    }
    
    //pared izquierda
    for(int i=-6; i>=-18; i=i-2){
        glPushMatrix();
            glTranslatef(4.0, 0.0, i);
            glRotatef(90, 0.0, 1.0, 0.0);
            wood();
        glPopMatrix();
    }
    
    //pared derecha
    for(int i=-6; i>=-18; i=i-2){
        glPushMatrix();
            glTranslatef(14.0, 0.0, i);
            glRotatef(90, 0.0, 1.0, 0.0);
            wood();
        glPopMatrix();
    }
    
    //pared trasera
    for(int i=4; i<=12; i=i+2){
        if(i!=8){
            glPushMatrix();
                glTranslatef(i, 0.0, -20.0);
                wood();
            glPopMatrix();
        }
    }
    
    //setos delanteros
    for(int i=2; i<=16; i=i+2){
        if(i!=8 && i!=10){
            glPushMatrix();
                glTranslatef(i, 1.0, 1.0);
                cube_3();
            glPopMatrix();
        }
    }
    
    //cubos
    glPushMatrix();
        glTranslatef(20.0, 0.0, -15.0);
        for(int i=1; i<=8; i=i+2){
            for(int j=1; j<=8; j=j+2){
                glPushMatrix();
                    glTranslatef(j, 1.0, i);
                    cube_1();
                glPopMatrix();
            }
        }

        for(int i=3; i<6; i=i+2){
            for(int j=3; j<6; j=j+2){
                glPushMatrix();
                    glTranslatef(j, 3.0, i);
                    cube_2();
                glPopMatrix();
            }
        }

        glPushMatrix();
            glTranslatef(4.0, 5.0, 4.0);
            cube_3();
        glPopMatrix();
    glPopMatrix();
    
    //oveja 1
    glPushMatrix();
        if(sheep_mov_1 <= 20.0){
            glTranslatef(sheep_mov_1, 0.5, -1.0);
        }else{
            sheep_mov_1=-10.0;
        }
        sheep();
    glPopMatrix();
    
    //oveja
    glPushMatrix();
        if(switch_sheep_place){
            glTranslatef(23.5, 6.5, -12.0);
            glRotatef(-90.0, 0.0, 1.0, 0.0);
        }else{
            glTranslatef(-3.7, 7.5, -5.5);
            glRotatef(-180.0, 0.0, 1.0, 0.0);
        }
        sheep();
    glPopMatrix();
    
    //oveja
    glPushMatrix();
        glTranslatef(8.5, 0.5, -27.0);
        glRotatef(-90.0, 0.0, 1.0, 0.0);
        sheep();
    glPopMatrix();
    
    //oveja
    glPushMatrix();
        glTranslatef(14.0, sheep_mov_3, 1.5);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        sheep();
    glPopMatrix();
 
}

void drawObject() {
    
    if(status_createCube1){
        glPushMatrix();
            glTranslatef(x+xc, yc+y-2.0, z-6.0);
            cube_1();
        glPopMatrix();
        if(create_object){
            cubes.push_back(Cube(x+xc, yc+y-2.0, z-6.0, 1));
            status_createCube1 = false;
            create_object = false;
            xc = 0.0; yc=0.0;
        }
    }
    
    if(status_createCube2){
        glPushMatrix();
            glTranslatef(x+xc, yc+y-2.0, z-6.0);
            cube_2();
        glPopMatrix();
        if(create_object){
            cubes.push_back(Cube(x+xc, yc+y-2.0, z-6.0, 2));
            status_createCube2 = false;
            create_object = false;
            xc = 0.0; yc=0.0;
        }
    }
    
    if(status_createCube3){
        glPushMatrix();
            glTranslatef(x+xc, yc+y-2.0, z-6.0);
            cube_3();
        glPopMatrix();
        if(create_object){
            cubes.push_back(Cube(x+xc, yc+y-2.0, z-6.0, 3));
            status_createCube3 = false;
            create_object = false;
            xc = 0.0; yc=0.0;
        }
    }
    
    if(status_createTable1){
        glPushMatrix();
            glTranslatef(x+xc, yc+y-3.0, z-6.0);
            glRotatef(rotateAngle, 0.0, 1.0, 0.0);
            floor();
        glPopMatrix();
        if(create_object){
            cubes.push_back(Cube(x+xc, yc+y-3.0, z-6.0, rotateAngle, 4));
            status_createTable1 = false;
            create_object = false;
            xc = 0.0; yc=0.0;
        }
    }
    
    if(status_createTable2){
        glPushMatrix();
            glTranslatef(x+xc, yc+y-3.0, z-6.0);
            glRotatef(rotateAngle, 0.0, 1.0, 0.0);
            wood();
        glPopMatrix();
        if(create_object){
            cubes.push_back(Cube(x+xc, yc+y-3.0, z-6.0, rotateAngle, 5));
            status_createTable2 = false;
            create_object = false;
            xc = 0.0; yc=0.0;
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
                case 4:
                    glRotatef(i->getRotation(), 0.0, 1.0, 0.0);
                    floor();
                    break;
                case 5:
                    glRotatef(i->getRotation(), 0.0, 1.0, 0.0);
                    wood();
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

void floor(){
    glBindTexture(GL_TEXTURE_2D, textureName[5]);  
    
    GLfloat A[] = { 0.0, 0.0,  0.0 };
    GLfloat B[] = { 0.0, 0.25, 0.0 };
    GLfloat C[] = { 2.0, 0.25, 0.0 };
    GLfloat D[] = { 2.0, 0.0,  0.0 };
    GLfloat E[] = { 0.0, 0.0, -3.0 };
    GLfloat F[] = { 0.0, 0.25,-3.0 };
    GLfloat G[] = { 2.0, 0.0, -3.0 };
    GLfloat H[] = { 2.0, 0.25,-3.0 };
    
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( H[0], F[1], F[2] ); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( F[0], F[1], F[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], F[1], F[2] );  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( G[0], G[1], G[2] );
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] ); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( E[0], E[1], E[2] );   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( D[0], D[1], D[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], F[1], F[2] );   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( G[0], G[1], G[2] );    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
    glEnd();
}


void wood(){
    glBindTexture(GL_TEXTURE_2D, textureName[6]);  
    
    GLfloat A[] = { 0.0, 0.0, 0.0  };
    GLfloat B[] = { 0.0, 0.0, 0.25 };
    GLfloat C[] = { 2.0, 0.0, 0.25 };
    GLfloat D[] = { 2.0, 0.0, 0.0  };
    GLfloat E[] = { 0.0, 3.0, 0.0  };
    GLfloat F[] = { 0.0, 3.0, 0.25 };
    GLfloat G[] = { 2.0, 3.0, 0.0  };
    GLfloat H[] = { 2.0, 3.0, 0.25 };
    
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( H[0], F[1], F[2] ); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( F[0], F[1], F[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], F[1], F[2] );  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( G[0], G[1], G[2] );
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] ); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( E[0], E[1], E[2] );   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( D[0], D[1], D[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], F[1], F[2] );   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( G[0], G[1], G[2] );    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
    glEnd();
}

void trunk(){
    
    //tronco
    
    GLfloat A[] = { 0.0, 0.0, 0.0 };
    GLfloat B[] = { 0.0, 0.0,-0.5 };
    GLfloat C[] = { 0.5, 0.0,-0.5 };
    GLfloat D[] = { 0.5, 0.0, 0.0 };
    GLfloat E[] = { 0.0, 6.0, 0.0 };
    GLfloat F[] = { 0.0, 6.0,-0.5 };
    GLfloat G[] = { 0.5, 6.0,-0.5 };
    GLfloat H[] = { 0.5, 6.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureName[2]); 
    
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( G[0], G[1], G[2] ); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( H[0], H[1], H[2] );
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( F[0], F[1], F[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( C[0], C[1], C[2] );
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] ); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( E[0], E[1], E[2] );   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( D[0], D[1], D[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( H[0], H[1], H[2] );    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], H[1], H[2] );
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( C[0], C[1], C[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
    glEnd();
    
    
    //copa
    
    glBindTexture(GL_TEXTURE_2D, textureName[3]);
    
    GLfloat AA[] = { 0.0, 0.0, 0.0 };
    GLfloat BB[] = { 0.0, 0.0,-0.5 };
    GLfloat CC[] = { 0.5, 0.0,-0.5 };
    GLfloat DD[] = { 0.5, 0.0, 0.0 };
    GLfloat EE[] = { 0.0, 2.0, 0.0 };
    GLfloat FF[] = { 0.0, 2.0,-0.5 };
    GLfloat GG[] = { 0.5, 2.0,-0.5 };
    GLfloat HH[] = { 0.5, 2.0, 0.0 };
    
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( BB[0], BB[1], BB[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( FF[0], FF[1], FF[2] ); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( GG[0], GG[1], GG[2] ); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( CC[0], CC[1], CC[2] );
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( BB[0], BB[1], BB[2] );
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( FF[0], FF[1], FF[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( GG[0], GG[1], GG[2] );  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( CC[0], CC[1], CC[2] );
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( AA[0], AA[1], AA[2] );
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( BB[0], BB[1], BB[2] ); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( FF[0], FF[1], FF[2] ); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( EE[0], EE[1], EE[2] );   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( DD[0], DD[1], DD[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( CC[0], CC[1], CC[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( GG[0], GG[1], GG[2] );   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( HH[0], HH[1], HH[2] );    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( AA[0], AA[1], AA[2] );
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( EE[0], EE[1], EE[2] );
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( HH[0], HH[1], HH[2] );
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( DD[0], DD[1], DD[2] );
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( AA[0], AA[1], AA[2] );    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( BB[0], BB[1], BB[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( CC[0], CC[1], CC[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( DD[0], DD[1], DD[2] );
    glEnd();
}

void tree(){
    //árbol
    //tronco
    glPushMatrix();
        glTranslatef(-5.0, 0.0, -5.0);
        trunk();
    glPopMatrix();
    //copa
    glPushMatrix();
        glTranslatef(-4.6, 6.0, -5.2);
        cube_3();
    glPopMatrix();
}

void sheep(){
    sheep_body();
    sheep_face();
    sheep_legs();
}

void sheep_body(){
    GLfloat A[] = { 0.0, 0.0, 0.0 };
    GLfloat B[] = { 0.0, 0.0,-1.0 };
    GLfloat C[] = { 1.5, 0.0,-1.0 };
    GLfloat D[] = { 1.5, 0.0, 0.0 };
    GLfloat E[] = { 0.0, 1.0, 0.0 };
    GLfloat F[] = { 0.0, 1.0,-1.0 };
    GLfloat G[] = { 1.5, 1.0,-1.0 };
    GLfloat H[] = { 1.5, 1.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureName[8]); 
    
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( G[0], G[1], G[2] ); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( H[0], H[1], H[2] );
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( F[0], F[1], F[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( C[0], C[1], C[2] );
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] ); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( E[0], E[1], E[2] );   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( D[0], D[1], D[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( H[0], H[1], H[2] );    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], H[1], H[2] );
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( C[0], C[1], C[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
    glEnd();
}

void sheep_face(){
    glBindTexture(GL_TEXTURE_2D, textureName[7]);  
    
    GLfloat A[] = { 1.5, 1.0-0.25, 0.0 };
    GLfloat B[] = { 1.5, 2.0-0.25, 0.0 };
    GLfloat C[] = { 2.5, 2.0-0.25, 0.0 };
    GLfloat D[] = { 2.5, 1.0-0.25, 0.0 };
    GLfloat E[] = { 1.5, 1.0-0.25,-1.0 };
    GLfloat F[] = { 1.5, 2.0-0.25,-1.0 };
    GLfloat G[] = { 2.5, 2.0-0.25,-1.0 };
    GLfloat H[] = { 2.5, 1.0-0.25,-1.0 };
    
    glBegin(GL_QUADS);
        // CARA SUPERIOR
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( G[0], G[1], G[2] ); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( H[0], H[1], H[2] );
        // CARA TRASERA
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( F[0], F[1], F[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( C[0], C[1], C[2] );
        // CARA DERECHA
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] ); 
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( F[0], F[1], F[2] ); 
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( E[0], E[1], E[2] );   
        // CARA IZQUIERDA
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( D[0], D[1], D[2] );
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( C[0], C[1], C[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( G[0], G[1], G[2] );   
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( H[0], H[1], H[2] );    
        // CARA DELANTERA
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( E[0], E[1], E[2] );
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 0.0); glVertex3f( H[0], H[1], H[2] );
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
        // CARA INFERIOR
        glNormal3f( 1.0, -1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( A[0], A[1], A[2] );    
        glNormal3f(-1.0, -1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( B[0], B[1], B[2] );
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( C[0], C[1], C[2] );  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( D[0], D[1], D[2] );
    glEnd();
}

void sheep_legs(){
    glBindTexture(GL_TEXTURE_2D, textureName[7]);  
    
    glBegin(GL_QUADS);
        // pierna 1
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 0.0, 0.0, 0.0);
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 0.25, 0.0, 0.0); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 0.25, -1.0, 0.0); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 0.0, -1.0, 0.0);
        // pierna 2
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.25, 0.0, 0.0);
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.5, 0.0, 0.0);  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.5, -1.0, 0.0);  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.25, -1.0, 0.0);
        // pierna 3
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 0.0, 0.0, -1.0);
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 0.25, 0.0, -1.0); 
        glNormal3f(-1.0,  1.0,  1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 0.25, -1.0, -1.0); 
        glNormal3f( 1.0,  1.0,  1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 0.0, -1.0, -1.0);
        // pierna 4
        glNormal3f(-1.0,  1.0, -1.0); glTexCoord2f(1.0, 1.0); glVertex3f( 1.25, 0.0, -1.0);
        glNormal3f( 1.0,  1.0, -1.0); glTexCoord2f(0.0, 1.0); glVertex3f( 1.5, 0.0, -1.0);  
        glNormal3f( 1.0, -1.0, -1.0); glTexCoord2f(0.0, 0.0); glVertex3f( 1.5, -1.0, -1.0);  
        glNormal3f(-1.0, -1.0, -1.0); glTexCoord2f(1.0, 0.0); glVertex3f( 1.25, -1.0, -1.0);
    glEnd();
}

void drawLantern(){
    
}

void funSpecialKeyboard(int key, int xx, int yy) {
    
    switch(key) {
       case GLUT_KEY_UP:
           x += lx * 0.5;
           z += lz * 0.5;
           break;
       case GLUT_KEY_DOWN:
           x -= lx * 0.5;
           z -= lz * 0.5;
           break;
       case GLUT_KEY_LEFT:
           camAngle -= 0.05;
           lx = sin(camAngle);
           lz = -cos(camAngle);
           break;
       case GLUT_KEY_RIGHT:
           camAngle += 0.05f;
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
        case 'r':
            if(rotateAngle==360.0){
                rotateAngle = 0.0;
            }else{
                rotateAngle+=90.0;   
            }
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
        case '4':
            status_createTable1 = true;
            break;
        case '5':
            status_createTable2 = true;
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

void funIdle() {
    
    sheep_mov_1+=0.03;
    
    if(sheep_mov_2 >= -20.0 && sheep_mov_2 <= 0.0){
        sheep_mov_2+=0.05;
        switch_sheep_place=true;
    }else{
        sheep_mov_2+=0.05;
        switch_sheep_place=false;
        if(sheep_mov_2 > 20.0){
            sheep_mov_2 = -20.0;
        }
    }

    if(sheep_mov_2 >= -20.0 && sheep_mov_2 <= 0.0){
        sheep_mov_2+=0.05;
        switch_sheep_place=true;
    }else{
        sheep_mov_2+=0.05;
        switch_sheep_place=false;
        if(sheep_mov_2 > 20.0){
            sheep_mov_2 = -20.0;
        }
    }
    
    if(switch_sheep_jump){
        sheep_mov_3+=0.03;
        if(sheep_mov_3 > 3.5){
            switch_sheep_jump = false;
        }
    }else{
        sheep_mov_3-=0.05;
        if(sheep_mov_3 < 2.5){
            switch_sheep_jump = true;
        }
    }
    
    glutPostRedisplay();
    
}