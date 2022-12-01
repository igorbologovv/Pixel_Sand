#include "util.h"
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint prog;

char *readFile(const char *fileName) {
    FILE *f = fopen(fileName, "rb");
    if (!f) {
        perror("fopen failed");
        exit(1);
    }
    if (fseek(f, 0, SEEK_END) == -1) {
        perror("fseek failed");
        exit(1);
    }
    long len = ftell(f);
    if (len == -1) {
        perror("ftell failed");
        exit(1);
    }
    if (fseek(f, 0, SEEK_SET) == -1) {
        perror("fseek failed");
        exit(1);
    }
    char *res = malloc(len + 1);
    if (!res) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    if (fread(res, 1, len, f) != (size_t)len) {
        fprintf(stderr, "fread failed for file %s\n", fileName);
        exit(1);
    }
    if (fclose(f) == EOF) {
        perror("fclose failed");
        exit(1);
    }
    res[len] = 0;
    return res;
}

int getLinesCount(char *content) {
    int res = 0;
    char *p = content;
    while (*p) {
        if (*p == '\n') {
            res++;
        }
        p++;
    }
    return res + 1;
}

void fillLines(char *content, char **lines) {
    char *p = content;
    int ind = 0;
    lines[ind++] = p;
    while (*p) {
        if (*p == '\r') {
            *p = '\0';
        } else if (*p == '\n') {
            *p = '\0';
            lines[ind++] = p + 1;
        }
        p++;
    }
}

GLuint createShader(const char *shaderFile, GLenum shaderType) {
    const char *strShaderType;
    if (shaderType == GL_VERTEX_SHADER) {
        strShaderType = "vertex";
    } else if (shaderType == GL_GEOMETRY_SHADER) {
        strShaderType = "geometry";
    } else if (shaderType == GL_FRAGMENT_SHADER) {
        strShaderType = "fragment";
    } else {
        fprintf(stderr, "Unrecognized shader type\n");
        exit(1);
    }
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        fprintf(stderr, "Error creating shader of type %s\n", strShaderType);
        exit(1);
    }
    //char nothing = '\0';
    char *content = readFile(shaderFile);
    /*
    int linesCount = getLinesCount(content);
    char **lines = (char**) malloc(sizeof(char*) * linesCount);
    for (int i = 0; i < linesCount; i++) {
        lines[i] = &nothing;
    }
    fillLines(content, lines);
    for (int i = 0; i < linesCount; i++) {
        printf("line #%d: %s\n", i, lines[i]);
    }
    */
    // glShaderSource(shader, linesCount, (const GLchar **)lines, NULL);
    glShaderSource(shader, 1, (const GLchar **)&content, NULL);
    // free(lines);
    free(content);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        GLchar *info = malloc(sizeof(GLchar) * (infoLen + 1));
        glGetShaderInfoLog(shader, infoLen, NULL, info);
        fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, info);
        exit(1);
    }
    return shader;
}

void createProg(GLuint *shaders, int len) {
    int i = 0;
    prog = glCreateProgram();
    if (!prog) {
        fprintf(stderr, "Failed to create shader program\n");
        exit(1);
    }
    for (; i < len; i++) {
        glAttachShader(prog, shaders[i]);
    }
    glLinkProgram(prog);
    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLen;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLen);
        GLchar *info = malloc(sizeof(GLchar) * (infoLen + 1));
        glGetProgramInfoLog(prog, infoLen, NULL, info);
        fprintf(stderr, "Linker failure: %s\n", info);
        exit(1);
    }
}

void createArrayBufferAndFill(GLuint *buffer, const void *data, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void createIndexBufferAndFill(GLuint *buffer, const void *data, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void createProgFromDefaultShaders() {
    GLuint shaders[] = {
        createShader("shader.vs", GL_VERTEX_SHADER),
        createShader("shader.fs", GL_FRAGMENT_SHADER)
    };
    int len = sizeof(shaders) / sizeof(shaders[0]);
    createProg(shaders, len);
    int i = 0;
    for (; i < len; i++) {
        glDeleteShader(shaders[i]);
    }
}

void im_init() {
    stbi_set_flip_vertically_on_load(1);
}

GLuint im_load(const char *image_file_path) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(image_file_path, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (!data) {
        fprintf(stderr, "Failed to load image %s\n", image_file_path);
        exit(EXIT_FAILURE);
    }
    if (nrChannels < 3 || nrChannels > 4) {
        fprintf(stderr, "Number of channels should be 3 or 4 for %s\n", image_file_path);
        exit(EXIT_FAILURE);
    }
    // for (int i = 0; i < width*height*4; i++) {
    //     printf("%d ", data[i]);
    // }
    // printf("\n");
    
    glTexImage2D(GL_TEXTURE_2D,
        0, // level of detail
        GL_RGB, // result
        width,
        height,
        0, // should be always 0
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

// color: use [0.0, 1.0f, 0.0f, 1.0f] for green color
GLuint createEmptyTexture(int width, int height, /*unsigned char *color, */float *borderColor) {
    GLuint res;
    glGenTextures(1, &res);
    glBindTexture(GL_TEXTURE_2D, res);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //unsigned char *data = (unsigned char *) malloc(width*height*4);
    //for (int i = 0; i < width*height; i++) {
    //    int start = i * 4;
    //    data[start] = color[0];
    //    data[start + 1] = color[1];
    //    data[start + 2] = color[2];
    //    data[start + 3] = color[3];
    //}

    //int width2, height2, nrChannels;
    //unsigned char *data = stbi_load("black2.png", &width2, &height2, &nrChannels, STBI_rgb_alpha);
    //if (!data) {
    //    fprintf(stderr, "Failed to load image %s\n", "black2.png");
    //    exit(EXIT_FAILURE);
    //}

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    //stbi_image_free(data);
    //free(data);

    glBindTexture(GL_TEXTURE_2D, 0);
    return res;
}

