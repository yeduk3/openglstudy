
#ifndef colored_triangle_h
#define colored_triangle_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "myprogram.h"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#else
#include "GL/glew.h"
#endif

namespace ctri
{

    Program program, program2;

    // VBO. 점 데이터(위치, 속성) 저장?
    GLuint vertexBufferObject = 0;
    // VAO. 점
    GLuint vertexArrayObject = 0;
    GLuint elementBuffer;

    GLuint elementBuffer2;

    void triangleInit(GLFWwindow *window)
    {
        program.loadShader("triangle.vert", "triangle.frag");

        float vertices[][6] = {{0.0, 0.5, 0.0, 1.0, 0.0, 0.0},
                               {-0.5, -0.5, 0.0, 0.0, 1.0, 0.0},
                               {0.5, -0.5, 0.0, 0.0, 0.0, 1.0}};
        // element 타입을 glm::vec3으로 하니까 안 그려짐. u16vec3으로 해야 그려짐.
        // 이유는 glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);때문.
        // GL_UNSIGNED_SHORT는 16비트 부호없는 정수이지만, vec3은 32비트 부동소수점이다.
        // 타입은 다음의 정수형 중 하나여야만 한다. GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT.
        std::vector<glm::u16vec3> elements = {{0, 1, 2}};

        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec3) * elements.size(), elements.data(), GL_STATIC_DRAW);
    }

    void triangleRender(GLFWwindow *window)
    {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glClearColor(0.0, 0.0, 0.3, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program.programID);
        glBindVertexArray(vertexArrayObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
        glfwSwapBuffers(window);
    }
}

#endif