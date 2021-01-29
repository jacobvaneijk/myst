/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include <cstdlib>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

static GLFWwindow* window = nullptr;

static void errorCallback(int error, const char* description)
{
    std::cerr << "glfw: " << description << std::endl;
}

static bool initGLFW()
{
    if (!glfwInit()) {
        std::cerr << "glfw: initialization failed" << std::endl;
        return false;
    }

    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(640, 480, "Myst", nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "glfw: window creation failed" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(window);

    return true;
}

static bool initGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "glad: initialization failed" << std::endl;
        return false;
    }

    return true;
}

static void update()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(int argc, char* argv[])
{
    if (!initGLFW()) {
        return EXIT_FAILURE;
    }

    if (!initGLAD()) {
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(window)) {
        update();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
