// openglstudy: my first selfmade opengl program
// 20250121

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "myprogram.h"

#include <iostream>
#include <vector>

void render(GLFWwindow *window);
void init(GLFWwindow *window);

void triangleInit(GLFWwindow *window);
void triangleRender(GLFWwindow *window);

int main()
{
    if (!glfwInit())
    {
        // if glfw init fails,
        std::cerr << "GLFW Init Fails" << std::endl;
        return -1;
    }

// If MacOS
#ifdef __APPLE__
    // Get OpenGL Version 410
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // forward compatibility: necessary for OS X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // set as core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL Study", 0, 0);
    glfwMakeContextCurrent(window);

    // 컨텍스트가 있어야 성공.
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW Init Fails" << std::endl;
        return -1;
    }

    // 내가 만들 프로그램 초기 설정.
    // init(window);
    triangleInit(window);

    // 창 닫기 전까지,
    while (!glfwWindowShouldClose(window))
    {
        // 렌더링
        // render(window);
        triangleRender(window);

        // This function processes only those events that have already been received and then returns immediately. Processing events will cause the window and input callbacks associated with those events to be called.
        // 윈도우에 대한 조작도 포함하기에 안 적으면 렌더링 안 됨.
        glfwPollEvents();
    }

    // 생성했던 윈도우 파괴.
    glfwDestroyWindow(window);
    // glfw 종료
    glfwTerminate();

    return 0;
}

Program program, program2;

// VBO. 점 데이터(위치, 속성) 저장?
GLuint vertexBufferObject = 0;
// VAO. 점
GLuint vertexArrayObject = 0;
GLuint elementBuffer;

GLuint elementBuffer2;

void init(GLFWwindow *window)
{
    program.loadShader("shader.vert", "shader.frag");

    // Data of vertices and elements which are composed of 3 vertices.
    std::vector<glm::vec3> vertices = {{-1, -1, 0}, {1, -1, 0}, {0, 1, 0}, {-0.5, 0, 0}, {0.5, 0, 0}, {0, -1, 0}, {-2, 1, 0}};
    // Make sth using 0, 3, 5-th vertex, 1, 2, 3-th vertex and ...
    std::vector<glm::u16vec3> elements = {{0, 3, 5}, {1, 4, 5}, {2, 3, 4}};

    // Create VAO
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Create VBO
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // 선택한 VAO의 0번 Attrib. 활성화 후에 정보 주입. 바인드 된 VBO 정보를 VAO에 넣는 것임.
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    // Create VBO and put element data -> 이제 데이터가 어디로 가는가? -> draw할 때 해당 버퍼를 사용.
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec3) * elements.size(), elements.data(), GL_STATIC_DRAW);

    // new program
    program2.loadShader("shader.vert", "shader2.frag");
    // vertex는 같은 점 사용.
    std::vector<glm::u16vec3> elements2 = {{6, 2, 5}};

    glGenBuffers(1, &elementBuffer2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u16vec3) * elements2.size(), elements2.data(), GL_STATIC_DRAW);
}

void render(GLFWwindow *window)
{
    // get framebuffer size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    // set the area which gonna be drawn
    glViewport(0, 0, w, h);

    // set initial color into framebuffer
    glClearColor(0, 0, 0.3, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // program 1 draw
    glUseProgram(program.programID);
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

    GLuint testMatLoc = glGetUniformLocation(program.programID, "testMat");
    float testMat[4][4] = {{0.5, 0, 0, 0},
                           {0, 0.5, 0, 0},
                           {0, 0, 0.5, 0},
                           {0, 0, 0, 1}};
    glUniformMatrix4fv(testMatLoc, 1, false, &testMat[0][0]);

    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, 0);

    // program 2 draw
    glUseProgram(program2.programID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer2);

    GLuint testMatLoc2 = glGetUniformLocation(program2.programID, "testMat");
    float testMat2[4][4] = {{1, 0, 0, 1},
                            {0, 1, 0, -0.5},
                            {0, 0, 1, 0},
                            {0, 0, 0, 1}};
    glUniformMatrix4fv(testMatLoc2, 1, false, &testMat2[0][0]);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

    // swap front and back buffer
    glfwSwapBuffers(window);
}

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