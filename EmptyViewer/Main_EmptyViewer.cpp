#define _CRT_SECURE_NO_WARNINGS
#define GLFW_INCLUDE_GLU
#define GLFW_DLL

#include <Windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "load_mesh.h"

int winWidth = 500;
int winHeight = 500;

float totalElapsedTime = 0.0f;
int frameCount = 0;
GLuint timeQuery;

struct MeshVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

std::vector<MeshVertex> meshVertices;
std::vector<int> indexData;
int vertexCount = 0;
int triangleCount = 0;

GLuint meshVAO, meshVBO, meshEBO;

void drawSceneWithVAO() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.1, 0.1, -0.1, 0.1, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

    glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
    float lightPos[] = { lightDirection.x, lightDirection.y, lightDirection.z, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glTranslatef(0.1f, -1.0f, -1.5f);
    glScalef(10.0f, 10.0f, 10.0f);

    glBindVertexArray(meshVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexData.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void framebufferResizeCallback(GLFWwindow*, int width, int height) {
    winWidth = width;
    winHeight = height;
    glViewport(0, 0, width, height);
}

void initializeMeshBuffers() {
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);

    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(MeshVertex), meshVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(int), indexData.data(), GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(MeshVertex), (void*)0);
    glNormalPointer(GL_FLOAT, sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));

    glBindVertexArray(0);
}

int main(int argc, char* argv[]) {
    GLFWwindow* window;

    if (!glfwInit()) return -1;

    window = glfwCreateWindow(winWidth, winHeight, "GL Object Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    float ambientGlobal[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float lightSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientGlobal);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glColor3f(1.0f, 1.0f, 1.0f);
    float matSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    framebufferResizeCallback(window, winWidth, winHeight);

    load_mesh("bunny.obj");
    triangleCount = static_cast<int>(gTriangles.size());
    vertexCount = static_cast<int>(gPositions.size());
    meshVertices.resize(vertexCount);
    indexData.resize(triangleCount * 3);

    for (size_t i = 0; i < vertexCount; ++i) {
        meshVertices[i].position = glm::vec3(gPositions[i].x, gPositions[i].y, gPositions[i].z);
        meshVertices[i].normal = glm::vec3(gNormals[i].x, gNormals[i].y, gNormals[i].z);
    }

    for (size_t i = 0; i < triangleCount; ++i) {
        indexData[i * 3 + 0] = gTriangles[i].indices[0];
        indexData[i * 3 + 1] = gTriangles[i].indices[1];
        indexData[i * 3 + 2] = gTriangles[i].indices[2];
    }

    initializeMeshBuffers();
    glGenQueries(1, &timeQuery);

    while (!glfwWindowShouldClose(window)) {
        glBeginQuery(GL_TIME_ELAPSED, timeQuery);
        drawSceneWithVAO();
        glfwSwapBuffers(window);
        glfwPollEvents();
        glEndQuery(GL_TIME_ELAPSED);

        GLint queryAvailable = GL_FALSE;
        while (!queryAvailable)
            glGetQueryObjectiv(timeQuery, GL_QUERY_RESULT_AVAILABLE, &queryAvailable);

        GLint timeResult;
        glGetQueryObjectiv(timeQuery, GL_QUERY_RESULT, &timeResult);
        float elapsedSec = timeResult / (1000.0f * 1000.0f * 1000.0f);

        frameCount++;
        totalElapsedTime += elapsedSec;
        float fps = frameCount / totalElapsedTime;

        char windowTitle[256];
        std::snprintf(windowTitle, sizeof(windowTitle), "VAO Renderer - FPS: %.2f", fps);
        glfwSetWindowTitle(window, windowTitle);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
