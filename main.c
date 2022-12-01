#include <GL/glew.h>
#include <GL/freeglut.h>
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

GLuint vertexArray;

GLuint vertexBuffer;

GLuint indexBuffer;

GLuint posAttr, texCoordAttr;

GLuint carTexture;

GLuint texUniform;

GLuint frameBuffer;

GLuint textures[2];

GLuint mouse_pos_uUniform;

GLuint mouse_leftButton_uUniform;

GLuint sand_mode_uUniform;

GLuint wall_mode_uUniform;

GLuint eraser_mode_uUniform;

GLuint widthUniform, heightUniform;

int mousePosX, mousePosY, leftMouseIsPressed;

int useSand, useEraser, useWall;
int screenWidth, screenHeight;
int textureWidth = 75, textureHeight = 50;

//===============================================================================================================================================

void setUniformLocations()
{
    texUniform = glGetUniformLocation(prog, "tex");

    mouse_pos_uUniform = glGetUniformLocation(prog, "mouse_pos_u");

    mouse_leftButton_uUniform = glGetUniformLocation(prog, "mouse_leftButton_u");

    sand_mode_uUniform = glGetUniformLocation(prog, "sand_mode_u"); //

    wall_mode_uUniform = glGetUniformLocation(prog, "wall_mode_u");//       WALL, SAND, AND, ERASER were set in keyboardfuc

    eraser_mode_uUniform = glGetUniformLocation(prog, "eraser_mode_u");//

    widthUniform = glGetUniformLocation(prog, "width");
    heightUniform = glGetUniformLocation(prog, "height");

    posAttr = glGetAttribLocation(prog, "pos");

    texCoordAttr = glGetAttribLocation(prog, "texCoord");
}



void createBuffer();

void initVao();



void init()
{
    createBuffer();

    createProgFromDefaultShaders();

    setUniformLocations();

    initVao();

    // carTexture = im_load("car.jpg");
    // unsigned char color[] = { 255, 255, 0, 255 };
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    textures[0] = createEmptyTexture(textureWidth, textureHeight, /*color, */borderColor);

    textures[1] = createEmptyTexture(textureWidth, textureHeight, /*color, */borderColor);
   // im_load("black.png");
    // glUseProgram(prog);
    // glActiveTexture(GL_TEXTURE0 + 0);
    // glBindTexture(GL_TEXTURE_2D, textures[0]);
    // glUniform1i(texUniform, 0);
    // glUseProgram(0);


    glGenFramebuffers(1, &frameBuffer); // Create Buffer

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); // OK, let's do something with this buffer

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[0], 0); // Attach 1st texture to the enter0

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures[1], 0); //Attach second to the enter1

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
/*
  if (status != GL_FRAMEBUFFER_COMPLETE)

        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
     else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)

            printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");//} else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
            {    printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n");}

        //else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");}

        else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {printf("GL_FRAMEBUFFER_UNSUPPORTED\n");}


        else {printf("Unknown error =(");}


   // glBindFramebuffer(GL_FRAMEBUFFER, 0); // turn off the buffer


    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LEQUAL);
    //glDepthRange(0.0f, 1.0f);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);
*/
    }

void createBuffer() {
    GLushort indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    float vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        // 1.0f, 0.0f, 0.0f, 1.0f,
        // 0.0f, 1.0f, 0.0f, 1.0f,
        // 0.0f, 0.0f, 1.0f, 1.0f,
        // 1.0f, 1.0f, 0.0f, 1.0f,

        0.0f, 0.f,
        1.0f, 0.f,
        1.0f, 1.f,
        0.0f, 1.f
    };

    createArrayBufferAndFill(&vertexBuffer, vertices, sizeof(vertices), GL_STATIC_DRAW);

    createIndexBufferAndFill(&indexBuffer, indices, sizeof(indices), GL_STATIC_DRAW);
}

void initVao()
{
    glGenVertexArrays(1, &vertexArray);

    glBindVertexArray(vertexArray);

    // vertex array object will store next things:
    // 1. Binding to GL_ELEMENT_ARRAY_BUFFER
    // 2. Enabled vertex attrib arrays
    // 3. Vertex attrib pointer settings

    glEnableVertexAttribArray(posAttr);

    glEnableVertexAttribArray(texCoordAttr);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // glVertexAttribPointer will use GL_ARRAY_BUFFER
    // , which was set by glBindBuffer
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttribPointer(texCoordAttr, 2, GL_FLOAT, GL_FALSE, 0, (void*)(4 * 3 * sizeof(float)));

    // Binding to GL_ARRAY_BUFFER will not be stored in
    // the vertex array object,
    // but it was already defined for glVertexAttribPointer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This will be used by glDrawElements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glBindVertexArray(0);

}

int activeBuffer = 0;

int src, dest;

void Swap()
{
    src = activeBuffer;

    dest = 1 - src;

    activeBuffer = dest;
}

void display(void) /// 2
{
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClearDepth(1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(prog);

    glBindVertexArray(vertexArray); /// GOTO initVao

    glUniform1i(mouse_leftButton_uUniform, leftMouseIsPressed); /// GOTO ===void mouse===

    glUniform1i(sand_mode_uUniform, useSand);

    glUniform1i(wall_mode_uUniform, useWall);

    glUniform1i(eraser_mode_uUniform, useEraser);

//========================================================================================================================
    Swap();

    // ------------------- DRAW ON TEXTURE --------------

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer); //DRAW IN TEXTURE
    glViewport(0, 0, (GLsizei)textureWidth, (GLsizei)textureHeight);
    glUniform2f(mouse_pos_uUniform, (float) (mousePosX / (float)screenWidth), (float) ((1.0 - mousePosY / (float)screenHeight)));// push data into shader TRANSFER TO setUniformLocations
    glUniform1f(widthUniform, (float) textureWidth);
    glUniform1f(heightUniform, (float) textureHeight);

    GLuint destinationAttachment[] = { GL_COLOR_ATTACHMENT0 + dest };// DRAWING in destination texture // dest would be either 1or0
    // glDrawBuffers sets where the scene will be drawn
    glDrawBuffers(1, destinationAttachment); // in which texture do we want to draw

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[src]);//drawing based on whch texture//the result AUTOMATCALLY transferred to shader sampler2D texture_u

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);



    // ------------------- DRAW ON SCREEN --------------

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // mousePosX = [0; 600]
    // 300 -> 0.5
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);
    glUniform2f(mouse_pos_uUniform, (float) (mousePosX / (float)screenWidth), (float) (1.0 - mousePosY / (float)screenHeight));// push data into shader TRANSFER TO setUniformLocations
    glUniform1f(widthUniform, (float) screenWidth);
    glUniform1f(heightUniform, (float) screenHeight);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[src]);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

//============================================================================================================================
    glBindVertexArray(0); // After this commandd everything will be shown on the screen

    glUseProgram(0);

    glutSwapBuffers();

    glutPostRedisplay();
}


void reshape(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    // glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}
//===========================================================================================================KEYBOARD AND MOUSE=============================================================================
void keyboard(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    useSand = 0;

    useEraser = 0;

    useWall = 0;

    if (key == 27)
    {
        glutLeaveMainLoop(); // What is this func doing?
    }

        else if (key == 's') {useSand = 1;}

        else if (key == 'e') {useEraser = 1;}

        else if (key == 'w'){useWall = 1;}
}

void mouse(int button, int state, int x, int y) // from where this func is getting data??
{
    leftMouseIsPressed = button == GLUT_LEFT_BUTTON && state == GLUT_DOWN; // ??

    mousePosX = x;

    mousePosY = y;
}

void mouseMove(int x, int y) // ?
{
    mousePosX = x;

    mousePosY = y;
}
//=====================================================================================================================================================================================================================
int main(int argc, char *argv[])
{
    screenWidth = 1000;
    screenHeight = 800;
    im_init();

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);

    glutInitWindowSize(screenWidth, screenHeight);

    glutInitWindowPosition(100, 100);

    glutCreateWindow("Game");

    GLenum res = glewInit();

    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(res));
        return 1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);/// 1

    glutReshapeFunc(reshape);

    glutKeyboardFunc(keyboard);

    glutMouseFunc(mouse);

    glutMotionFunc(mouseMove);

    init();

    glutMainLoop();

    return 0;
}
