
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

#ifndef KEY_CALLBACK
#define KEY_CALLBACK
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

    mp::Program program;

    // VBO. 점 데이터(위치, 속성) 저장?
    GLuint vertexBufferObject = 0;
    GLuint vertexNormalBufferObject = 0;
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

    float lightIntensity = 120;
    float intensityDelta = 10;
    bool lightTurn = false;
    glm::vec3 lightPosition(10, 10, 5);

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        {
            lightIntensity += intensityDelta;
        }
        else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        {
            lightIntensity -= intensityDelta;
        }
        else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            lightTurn = !lightTurn;
        }
    }

    ObjData object;

    void init(GLFWwindow *window)
    {
        program.loadShader("shaders/utah.vert", "shaders/utah.frag");

        object = loadObject("./newell_teaset/", "teapot.obj");

        // VAO
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // VBO for vertex data
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        // vertices
        glBufferData(GL_ARRAY_BUFFER, object.nVertices * sizeof(glm::vec3), object.vertices.data(), GL_STATIC_DRAW);

        // VAO attrib
        // index 0 - first 3 elements of each vertex = position
        glEnableVertexAttribArray(0);
        // index "0", "3" components per each attrib data = x, y, z, which is "GL_FLOAT".
        // Normalize "GL_FALSE", jump "3" per each data, start with "0"-th
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        // normal
        glGenBuffers(1, &vertexNormalBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferObject);
        glBufferData(GL_ARRAY_BUFFER, object.normals.size() * sizeof(glm::vec3), object.normals.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        glGenBuffers(1, &element3Buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element3Buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec3) * object.nElements3, object.elements3.data(), GL_STATIC_DRAW);
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
                                        {0, 1, 0, 0},
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

        // Fragment Shader
        GLuint eyePositionLoc = glGetUniformLocation(program.programID, "eyePosition");
        GLuint lightPositionLoc = glGetUniformLocation(program.programID, "lightPosition");
        GLuint lightColorLoc = glGetUniformLocation(program.programID, "lightColor");
        GLuint ambientColorLoc = glGetUniformLocation(program.programID, "ambientColor");
        GLuint specularColorLoc = glGetUniformLocation(program.programID, "specularColor");
        GLuint diffuseColorLoc = glGetUniformLocation(program.programID, "diffuseColor");
        GLuint shininessLoc = glGetUniformLocation(program.programID, "shininess");

        glm::vec3 lightColor(lightIntensity);
        if (lightTurn)
        {
            glm::mat4 lightRotation = glm::rotate(rotateFactor, glm::vec3(0, 1, 0));
            lightPosition = lightRotation * glm::vec4(lightPosition, 1);
        }
        // glm::vec3 lightDirection = glm::normalize(lightPosition - glm::vec3(0, 0, 0));
        glm::vec3 ambientColor = object.materialData.begin()->ambientColor;
        glm::vec3 specularColor = object.materialData.begin()->specularColor;
        glm::vec3 diffuseColor = object.materialData.begin()->diffuseColor;
        float shininess = 1.4;

        glUniform3fv(eyePositionLoc, 1, glm::value_ptr(eyePosition));
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        glUniform3fv(lightPositionLoc, 1, glm::value_ptr(lightPosition));
        glUniform3fv(ambientColorLoc, 1, glm::value_ptr(ambientColor));
        glUniform3fv(specularColorLoc, 1, glm::value_ptr(specularColor));
        glUniform3fv(diffuseColorLoc, 1, glm::value_ptr(diffuseColor));
        glUniform1f(shininessLoc, shininess);

        // elements.size() * elements[0].size();
        glDrawElements(GL_TRIANGLES, object.nElements3 * 3, GL_UNSIGNED_SHORT, 0);

        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element4Buffer);
        // glDrawElements(GL_TRIANGLE_FAN, object.nElements4 * 4, GL_UNSIGNED_SHORT, 0);
        glfwSwapBuffers(window);
    }
}

#endif