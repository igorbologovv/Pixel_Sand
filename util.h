#ifndef UTIL_H
#define UTIL_H

#include <GL/glew.h>
#include <GL/freeglut.h>

extern GLuint prog;

char *readFile(const char *fileName);

GLuint createShader(const char *shaderFile, GLenum shaderType);

void createProg(GLuint *shaders, int len);

void createArrayBufferAndFill(GLuint *buffer, const void *data, GLsizeiptr size, GLenum usage);

void createIndexBufferAndFill(GLuint *buffer, const void *data, GLsizeiptr size, GLenum usage);

void createProgFromDefaultShaders();

void im_init();

GLuint im_load(const char *image_file_path);

GLuint createEmptyTexture(int width, int height, /*unsigned char *color, */float *borderColor);

#endif
