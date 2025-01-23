// openglstudy: my first selfmade opengl program
// 20250121

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "myprogram.h"

#include "basic_triangle.h"
#include "colored_triangle.h"

#include <iostream>
#include <vector>

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
    // btri::init(window);
    ctri::triangleInit(window);

    // 창 닫기 전까지,
    while (!glfwWindowShouldClose(window))
    {
        // 렌더링
        // btri::render(window);
        ctri::triangleRender(window);

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
