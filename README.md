# cg_hw8_Q2 readme

## What you need
You need Visual Studio 2022 and window 11 OS.

And C/C++ should be available in VS2022.

## About
This project is about Immediate Mode.

To view the result image, open Q2_result.png, 

and if you want an explanation of the code, scroll down below.

## How to run

1. Click code and download as zip file.
   
![image](https://github.com/user-attachments/assets/444acd84-7c81-419d-9b90-410cdc4641c7)

2. Unzip a download file

![image](https://github.com/user-attachments/assets/b0642fb7-286a-448a-b45c-bf7a5b292380)

3. Open cg_hw8_Q2-master. Double click cg_hw8_Q2-master and open OpenglViewer

![image](https://github.com/user-attachments/assets/64b5e066-7126-44d1-96fd-de18cfbde9c1)

5. click "F5" on your keybord. Then you will get the result.

![Q2_result](https://github.com/user-attachments/assets/a936a30f-bc03-49c9-a5ee-829610624ab6)


## Code explanation

Other parts are same as cg_hw8_Q1.

```
GLuint meshVAO, meshVBO, meshEBO;

void drawSceneWithVAO() {
...

    glBindVertexArray(meshVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexData.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}


```
Sets up VAO and draws the mesh combine VAO.

-------------


```
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


```
Initializes VAO, VBO, and EBO by uploading vertex and index data to the GPU.


-----------
```

int main(int argc, char* argv[]) {
...

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
```
Main function

Measures GPU rendering time per frame using a timer query.

It renders the scene with VAO, swaps buffers, polls events, waits for the query result, computes the FPS, updates the window title with the FPS.


--------------
