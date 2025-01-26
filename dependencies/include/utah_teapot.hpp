
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
#include "objreader.hpp"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#else
#include "GL/glew.h"
#endif

#ifndef CURSOR_POS_CALLBACK
#define CURSOR_POS_CALLBACK
#endif

#ifndef SCROLL_CALLBACK
#define SCROLL_CALLBACK
#endif

namespace utah
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

    GLuint element3Buffer = 0;
    GLuint element4Buffer = 0;

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

    float fovy = 40.0f * PI / 180;

    void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        fovy -= yoffset / 10;
        fovy = comparator::max(0.01f, comparator::min(fovy, PI - 0.01f));
    }

    ObjData object;

    void init(GLFWwindow *window)
    {
        program.loadShader("utah.vert", "utah.frag");

        // std::vector<glm::vec3> vertices = {{0.2000, 0.0000, 2.70000}, {0.2000, -0.1120, 2.70000}, {0.1120, -0.2000, 2.70000}, {0.0000, -0.2000, 2.70000}, {1.3375, 0.0000, 2.53125}, {1.3375, -0.7490, 2.53125}, {0.7490, -1.3375, 2.53125}, {0.0000, -1.3375, 2.53125}, {1.4375, 0.0000, 2.53125}, {1.4375, -0.8050, 2.53125}, {0.8050, -1.4375, 2.53125}, {0.0000, -1.4375, 2.53125}, {1.5000, 0.0000, 2.40000}, {1.5000, -0.8400, 2.40000}, {0.8400, -1.5000, 2.40000}, {0.0000, -1.5000, 2.40000}, {1.7500, 0.0000, 1.87500}, {1.7500, -0.9800, 1.87500}, {0.9800, -1.7500, 1.87500}, {0.0000, -1.7500, 1.87500}, {2.0000, 0.0000, 1.35000}, {2.0000, -1.1200, 1.35000}, {1.1200, -2.0000, 1.35000}, {0.0000, -2.0000, 1.35000}, {2.0000, 0.0000, 0.90000}, {2.0000, -1.1200, 0.90000}, {1.1200, -2.0000, 0.90000}, {0.0000, -2.0000, 0.90000}, {-2.0000, 0.0000, 0.90000}, {2.0000, 0.0000, 0.45000}, {2.0000, -1.1200, 0.45000}, {1.1200, -2.0000, 0.45000}, {0.0000, -2.0000, 0.45000}, {1.5000, 0.0000, 0.22500}, {1.5000, -0.8400, 0.22500}, {0.8400, -1.5000, 0.22500}, {0.0000, -1.5000, 0.22500}, {1.5000, 0.0000, 0.15000}, {1.5000, -0.8400, 0.15000}, {0.8400, -1.5000, 0.15000}, {0.0000, -1.5000, 0.15000}, {-1.6000, 0.0000, 2.02500}, {-1.6000, -0.3000, 2.02500}, {-1.5000, -0.3000, 2.25000}, {-1.5000, 0.0000, 2.25000}, {-2.3000, 0.0000, 2.02500}, {-2.3000, -0.3000, 2.02500}, {-2.5000, -0.3000, 2.25000}, {-2.5000, 0.0000, 2.25000}, {-2.7000, 0.0000, 2.02500}, {-2.7000, -0.3000, 2.02500}, {-3.0000, -0.3000, 2.25000}, {-3.0000, 0.0000, 2.25000}, {-2.7000, 0.0000, 1.80000}, {-2.7000, -0.3000, 1.80000}, {-3.0000, -0.3000, 1.80000}, {-3.0000, 0.0000, 1.80000}, {-2.7000, 0.0000, 1.57500}, {-2.7000, -0.3000, 1.57500}, {-3.0000, -0.3000, 1.35000}, {-3.0000, 0.0000, 1.35000}, {-2.5000, 0.0000, 1.12500}, {-2.5000, -0.3000, 1.12500}, {-2.6500, -0.3000, 0.93750}, {-2.6500, 0.0000, 0.93750}, {-2.0000, -0.3000, 0.90000}, {-1.9000, -0.3000, 0.60000}, {-1.9000, 0.0000, 0.60000}, {1.7000, 0.0000, 1.42500}, {1.7000, -0.6600, 1.42500}, {1.7000, -0.6600, 0.60000}, {1.7000, 0.0000, 0.60000}, {2.6000, 0.0000, 1.42500}, {2.6000, -0.6600, 1.42500}, {3.1000, -0.6600, 0.82500}, {3.1000, 0.0000, 0.82500}, {2.3000, 0.0000, 2.10000}, {2.3000, -0.2500, 2.10000}, {2.4000, -0.2500, 2.02500}, {2.4000, 0.0000, 2.02500}, {2.7000, 0.0000, 2.40000}, {2.7000, -0.2500, 2.40000}, {3.3000, -0.2500, 2.40000}, {3.3000, 0.0000, 2.40000}, {2.8000, 0.0000, 2.47500}, {2.8000, -0.2500, 2.47500}, {3.5250, -0.2500, 2.49375}, {3.5250, 0.0000, 2.49375}, {2.9000, 0.0000, 2.47500}, {2.9000, -0.1500, 2.47500}, {3.4500, -0.1500, 2.51250}, {3.4500, 0.0000, 2.51250}, {2.8000, 0.0000, 2.40000}, {2.8000, -0.1500, 2.40000}, {3.2000, -0.1500, 2.40000}, {3.2000, 0.0000, 2.40000}, {0.0000, 0.0000, 3.15000}, {0.8000, 0.0000, 3.15000}, {0.8000, -0.4500, 3.15000}, {0.4500, -0.8000, 3.15000}, {0.0000, -0.8000, 3.15000}, {0.0000, 0.0000, 2.85000}, {1.4000, 0.0000, 2.40000}, {1.4000, -0.7840, 2.40000}, {0.7840, -1.4000, 2.40000}, {0.0000, -1.4000, 2.40000}, {0.4000, 0.0000, 2.55000}, {0.4000, -0.2240, 2.55000}, {0.2240, -0.4000, 2.55000}, {0.0000, -0.4000, 2.55000}, {1.3000, 0.0000, 2.55000}, {1.3000, -0.7280, 2.55000}, {0.7280, -1.3000, 2.55000}, {0.0000, -1.3000, 2.55000}, {1.3000, 0.0000, 2.40000}, {1.3000, -0.7280, 2.40000}, {0.7280, -1.3000, 2.40000}, {0.0000, -1.3000, 2.40000}};

        // std::vector<glm::u16vec3> elements;

        object = loadObject("./newell_teaset/teapot.obj");

        // VAO
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // VBO for vertex data
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        // 24 vertices
        glBufferData(GL_ARRAY_BUFFER, object.nVertices * sizeof(glm::vec3), object.vertices.data(), GL_STATIC_DRAW);

        // VAO attrib
        // index 0 - first 3 elements of each vertex = position
        glEnableVertexAttribArray(0);
        // index "0", "3" components per each attrib data = x, y, z, which is "GL_FLOAT".
        // Normalize "GL_FALSE", jump "3" per each data, start with "0"-th
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        glGenBuffers(1, &element3Buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element3Buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec3) * object.nElements3, object.elements3.data(), GL_STATIC_DRAW);
        // glGenBuffers(1, &element4Buffer);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element4Buffer);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec4) * object.nElements4, object.elements4.data(), GL_STATIC_DRAW);
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element3Buffer);

        // glm vector is column-major - so it should be transposed if i wanna expected translation operation.
        glm::mat4 modelMat = glm::mat4({{1, 0, 0, 0},
                                        {0, 1, 0, -1},
                                        {0, 0, 1, 0},
                                        {0, 0, 0, 1}});
        GLuint modelMatLoc = glGetUniformLocation(program.programID, "modelMat");
        // Allow transpose because above expression is transposed.
        glUniformMatrix4fv(modelMatLoc, 1, GL_TRUE, glm::value_ptr(modelMat));

        glm::mat4 rotateX = glm::rotate(cameraPhi, glm::vec3(1, 0, 0));
        glm::mat4 rotateY = glm::rotate(cameraTheta, glm::vec3(0, 1, 0));
        glm::vec3 eyePosition = rotateY * rotateX * glm::vec4(eye, 1);
        glm::mat4 viewMat = glm::lookAt(eyePosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        GLuint viewMatLoc = glGetUniformLocation(program.programID, "viewMat");
        glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
        // std::cout << eye.x << ", " << eye.y << ", " << eye.z << std::endl;

        glm::mat4 projMat = glm::perspective(fovy, w / (float)h, 0.01f, 1000.0f);
        GLuint projMatLoc = glGetUniformLocation(program.programID, "projMat");
        glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

        // elements.size() * elements[0].size();
        glDrawElements(GL_TRIANGLES, object.nElements3 * 3, GL_UNSIGNED_SHORT, 0);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element4Buffer);
        // glDrawElements(GL_TRIANGLE_FAN, object.nElements4 * 4, GL_UNSIGNED_SHORT, 0);
        glfwSwapBuffers(window);
    }
}

#endif