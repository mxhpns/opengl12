#include <GL/glew.h>
#include <GL/freeglut.h>
#include "util.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

GLuint vao;
GLuint vbo;
GLuint ibo;
GLuint projMatrLoc;
GLuint rotMatrLoc;

void setUniformLocations() {
    projMatrLoc = glGetUniformLocation(prog, "projMatr");
    rotMatrLoc = glGetUniformLocation(prog, "rotMatr");
}

void createBuffer();
void initVao();

void init() {
    createBuffer();
    initVao();

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

    setUniformLocations();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void createBuffer() {
    GLushort indices[] = {
        0, 1, 2,
        0, 2, 3,
        3, 2, 6,
        3, 6, 7,
        4, 5, 1,
        4, 1, 0,
        7, 6, 5,
        7, 5, 4,
        1, 5, 6,
        1, 6, 2,
        4, 0, 3,
        4, 3, 7
    };
    float vertices[] = {
        // vertices
        // front vertices:
        -1.0f, -1.0f, 1.0f,// left-bottom
        -1.0f,  1.0f, 1.0f,// left-top
         1.0f,  1.0f, 1.0f,// right-top
         1.0f, -1.0f, 1.0f,// right-bottom
        // back vertices:
        -1.0f, -1.0f, -1.0f,// left-bottom
        -1.0f,  1.0f, -1.0f,// left-top
         1.0f,  1.0f, -1.0f,// right-top
         1.0f, -1.0f, -1.0f,// right-bottom

         // colors
         1.0f, 0.0f, 0.0f, 1.0f, // red
         0.0f, 1.0f, 0.0f, 1.0f, // green
         1.0f, 0.0f, 1.0f, 1.0f, // purple
         1.0f, 1.0f, 0.0f, 1.0f, // yellow
         0.0f, 1.0f, 1.0f, 1.0f, // aqua
         0.5f, 0.0f, 0.0f, 1.0f, // dark-red
         0.0f, 0.5f, 0.0f, 1.0f, // dark-green
         0.0f, 0.0f, 0.5f, 1.0f  // dark-blue
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initVao() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // vertex array object will store next things:
    // 1. Binding to GL_ELEMENT_ARRAY_BUFFER
    // 2. Enabled vertex attrib arrays
    // 3. Vertex attrib pointer settings

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glVertexAttribPointer will use GL_ARRAY_BUFFER
    // , which was set by glBindBuffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) (8*3*sizeof(float)));
    // Binding to GL_ARRAY_BUFFER will not be stored in
    // the vertex array object,
    // but it was already defined for glVertexAttribPointer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // This will be used by glDrawElements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glBindVertexArray(0);

}

float *rotationMatr(float elapsed) {
    float part = fmodf(elapsed, 5.0f) / 5.0f;
    float angle = 3.14159f*2.0f * part;
    float res[] = {
        cosf(angle), 0, -sinf(angle), 0,
        0, 1, 0, 0,
        sinf(angle), 0, cosf(angle), 0,
        0, 0, 0, 1
    };
    float *matr = malloc(sizeof(float) * 16);
    memcpy(matr, res, sizeof(float) * 16);
    return matr;
}

void display() {
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(vao);
    float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float *matr = rotationMatr(elapsed);
    glUniformMatrix4fv(rotMatrLoc, 1, GL_FALSE, matr);
    free(matr);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    glutSwapBuffers();
    glutPostRedisplay();
}

float toRad(float deg) {
    // 180 deg = pi
    // "deg" deg = x
    return deg * 3.141593f / 180.0f;
}

void reshape(int w, int h) {
    float n = 0.2f;
    float f = 1000.0f;
    float fov = toRad(90);
    float aspect = w / (float) h;
    float t = n * tanf(fov / 2.0f);
    float r = t * aspect;

    float projMatr[] = {
        n/r, 0,   0,              0,
        0,   n/t, 0,              0,
        0,   0,   (f+n)/(n-f),    -1,
        0,   0,   2.0f*n*f/(n-f), 0
    };
    printf("n=%.1f  f=%.1f  fov=%.2f  aspect=%.2f  t=%.2f  r=%.2f\n", n, f, fov, aspect, t, r);
    glUseProgram(prog);
    glUniformMatrix4fv(projMatrLoc, 1, GL_FALSE, projMatr);
    glUseProgram(0);
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard(unsigned char key, int x, int y) {
    (void) x;
    (void) y;
    if (key == 27) {
        glutLeaveMainLoop();
    }
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Game");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(res));
        return 1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    init();
    glutMainLoop();
    return 0;
}
