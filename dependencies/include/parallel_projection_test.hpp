
#ifndef parallel_projection_test_h
#define parallel_projection_test_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/reciprocal.hpp>

#include "myprogram.hpp"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#else
#include "GL/glew.h"
#endif

// #ifndef CURSOR_POS_CALLBACK
// #define CURSOR_POS_CALLBACK
// #endif
#ifndef MOUSE_BUTTON_CALLBACK
#define MOUSE_BUTTON_CALLBACK
#endif

namespace pptest
{
    mp::Program program;

    // VBO. 점 데이터(위치, 속성) 저장?
    GLuint vertexBufferObject = 0;
    // VAO. 점
    GLuint vertexArrayObject = 0;

    GLuint elementBuffer = 0;

    const float PI = 3.141592f;
    const float FOVY = 45.0f * PI / 180;
    // 1/d = tan(FOVY/2) -> parallel green height = perspective green height
    const float d = glm::cot(FOVY * 0.5f);

    bool isParallel = false;

    glm::vec3 eye = glm::vec3(0, 0, d);

    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
        {
            isParallel = !isParallel;
        }
    }

    void init(GLFWwindow *window)
    {
        program.loadShader("triangle3d.vert", "triangle3d.frag");

        float vertices[][6] = {{-0.75, 0.5, 0.5, 1.0, 0.0, 0.0},  // z=0.5 square, red, lt
                               {-0.75, -0.5, 0.5, 1.0, 0.0, 0.0}, // lb
                               {-0.25, -0.5, 0.5, 1.0, 0.0, 0.0}, // rb
                               {-0.25, 0.5, 0.5, 1.0, 0.0, 0.0},  // tr
                               {-0.25, 0.5, 0.0, 0.0, 1.0, 0.0},  // z=0.0 sqaure, green, lt
                               {-0.25, -0.5, 0.0, 0.0, 1.0, 0.0}, // lb
                               {0.25, -0.5, 0.0, 0.0, 1.0, 0.0},  // rb
                               {0.25, 0.5, 0.0, 0.0, 1.0, 0.0},   // rt
                               {0.25, 0.5, -0.5, 0.0, 0.0, 1.0},  // z=-0.5 sqaure, blue, lt
                               {0.25, -0.5, -0.5, 0.0, 0.0, 1.0}, // lb
                               {0.75, -0.5, -0.5, 0.0, 0.0, 1.0}, // rb
                               {0.75, 0.5, -0.5, 0.0, 0.0, 1.0}}; // rt
        // element 타입을 glm::vec3으로 하니까 안 그려짐. u16vec3으로 해야 그려짐.
        // 이유는 glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);때문.
        // GL_UNSIGNED_SHORT는 16비트 부호없는 정수이지만, vec3은 32비트 부동소수점이다.
        // 타입은 다음의 정수형 중 하나여야만 한다. GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
        std::vector<glm::u16vec3> elements = {{0, 1, 2},    // red 1
                                              {0, 2, 3},    // red 2
                                              {4, 5, 6},    // green 1
                                              {4, 6, 7},    // green 2
                                              {8, 9, 10},   // blue 1
                                              {8, 10, 11}}; // blue 2

        // VAO
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // VBO for vertex data
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        // 12 vertices * 6 floats
        glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(float), vertices, GL_STATIC_DRAW);

        // VAO attrib
        // index 0 - first 3 elements of each vertex = position
        glEnableVertexAttribArray(0);
        // index "0", "3" components per each attrib data = x, y, z, which is "GL_FLOAT".
        // Normalize "GL_FALSE", jump "6" per each data, start with "0"-th
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        // index 1 - last 3 elements of each vertex = color
        glEnableVertexAttribArray(1);
        // data = r, g, b
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec3) * elements.size(), elements.data(), GL_STATIC_DRAW);
    }

    void render(GLFWwindow *window)
    {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(0.0, 0.0, 0.3, 0.0);

        // Backface culling 활성화.
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program.programID);
        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

        glm::mat4 modelMat = glm::mat4({{1, 0, 0, 0},
                                        {0, 1, 0, 0},
                                        {0, 0, 1, 0},
                                        {0, 0, 0, 1}});
        GLuint modelMatLoc = glGetUniformLocation(program.programID, "modelMat");
        glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

        glm::mat4 viewMat = glm::lookAt(eye, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        GLuint viewMatLoc = glGetUniformLocation(program.programID, "viewMat");
        glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

        glm::mat4 projMat;
        if (isParallel)
            projMat = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 1000.0f);
        else
            projMat = glm::perspective(FOVY, w / (float)h, 0.01f, 1000.0f);
        GLuint projMatLoc = glGetUniformLocation(program.programID, "projMat");
        glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

        // elements.size() * elements[0].size()
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
        glfwSwapBuffers(window);
    }
}

#endif