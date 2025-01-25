
#ifndef colored_triangle_3d_h
#define colored_triangle_3d_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "myprogram.hpp"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#else
#include "GL/glew.h"
#endif

// #ifndef CURSOR_POS_CALLBACK
// #define CURSOR_POS_CALLBACK
// #endif

namespace ctri3d
{

    namespace comparator
    {
        float min(const float &a, const float &b)
        {
            return a > b ? b : a;
        }
        float max(const float &a, const float &b)
        {
            return a > b ? a : b;
        }
    }

    float cameraTheta = 0;
    float cameraPhi = 0;

    Program program;

    // VBO. 점 데이터(위치, 속성) 저장?
    GLuint vertexBufferObject = 0;
    // VAO. 점
    GLuint vertexArrayObject = 0;

    GLuint elementBuffer = 0;

    const float PI = 3.141592f;

    const float rotateFactor = PI / 120.0f;
    glm::vec3 eye = glm::vec3(0, 0, 5);

    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        static double lastX = 0;
        static double lastY = 0;
        // when left mouse button clicked
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            double dx = xpos - lastX;
            double dy = ypos - lastY;
            int w, h;
            glfwGetWindowSize(window, &w, &h);
            // rotate 180 degree per each width/height dragging
            cameraTheta -= dx / w * PI; // related with y-axis rotation
            cameraPhi -= dy / h * PI;   // related with x-axis rotation
            cameraPhi = comparator::max(-PI / 2 + 0.01f, comparator::min(cameraPhi, PI / 2 - 0.01f));
            // printf("%.3f %.3f\n", cameraTheta, cameraPhi);
        }
        // whenever, save current cursor position as previous one
        lastX = xpos;
        lastY = ypos;
    }

    void init(GLFWwindow *window)
    {
        program.loadShader("triangle3d.vert", "triangle3d.frag");

        float vertices[][6] = {{0.0, 0.5, 0.0, 1.0, 0.0, 0.0},    // top, r
                               {0.0, -0.5, 0.5, 0.0, 1.0, 0.0},   // front, g
                               {-0.5, -0.5, -0.5, 0.0, 0.0, 1.0}, // left, b
                               {0.5, -0.5, -0.5, 1.0, 1.0, 1.0}}; // right, w
        // element 타입을 glm::vec3으로 하니까 안 그려짐. u16vec3으로 해야 그려짐.
        // 이유는 glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);때문.
        // GL_UNSIGNED_SHORT는 16비트 부호없는 정수이지만, vec3은 32비트 부동소수점이다.
        // 타입은 다음의 정수형 중 하나여야만 한다. GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
        std::vector<glm::u16vec3> elements = {{0, 1, 2}, {0, 1, 3}, {1, 2, 3}, {0, 2, 3}};

        // VAO
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // VBO for vertex data
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        // 24 vertices
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), vertices, GL_STATIC_DRAW);

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

#ifdef CURSOR_POS_CALLBACK
        glm::mat4 rotateX = glm::rotate(cameraPhi, glm::vec3(1, 0, 0));
        glm::mat4 rotateY = glm::rotate(cameraTheta, glm::vec3(0, 1, 0));
        glm::vec3 eyePosition = rotateY * rotateX * glm::vec4(eye, 1);
        glm::mat4 viewMat = glm::lookAt(eyePosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        GLuint viewMatLoc = glGetUniformLocation(program.programID, "viewMat");
        glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
#else
        eye = glm::rotate(rotateFactor, glm::vec3(1, 0, 0)) * glm::vec4(eye, 1.0f);
        glm::vec3 destination = glm::vec3(0, 0, 0);
        glm::vec3 to(glm::normalize(destination - eye));
        glm::vec3 up(0, 1, 0);
        float flip = glm::cross(to, up).x > 0.0f ? 1.0f : -1.0f;
        glm::mat4 viewMat = glm::lookAt(eye, destination, glm::vec3(0, flip * 1, 0));
        GLuint viewMatLoc = glGetUniformLocation(program.programID, "viewMat");
        glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
#endif
        // std::cout << eye.x << ", " << eye.y << ", " << eye.z << std::endl;

        glm::mat4 projMat = glm::perspective(40 * PI / 180, w / (float)h, 0.01f, 1000.0f);
        GLuint projMatLoc = glGetUniformLocation(program.programID, "projMat");
        glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

        // elements.size() * elements[0].size()
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_SHORT, 0);
        glfwSwapBuffers(window);
    }
}

#endif