/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include <cstdlib>
#include <iostream>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "GLShader.hpp"
#include "GLTexture.hpp"

static GLFWwindow* window = nullptr;
static GLuint VAO, VBO;

static const char* vShaderSource =
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "\n"
    "out vec4 vertexColor;\n"
    "out vec2 vertexTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.xyz, 1.0);\n"
    "   vertexColor = vec4(aColor.xyz, 1.0);\n"
    "   vertexTexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

static const char* fShaderSource =
    "#version 460 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec4 vertexColor;\n"
    "in vec2 vertexTexCoord;\n"
    "\n"
    "uniform sampler2D myTexture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(myTexture, vertexTexCoord);\n"
    "}\0";

static void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "glfw: " << description << std::endl;
}

static void GLAPIENTRY glMessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    std::cerr << "gl: type=0x" << type << ", severity=0x" << severity
              << ", message=" << message << std::endl;
}

static bool initGLFW()
{
    if (!glfwInit()) {
        std::cerr << "glfw: initialization failed" << std::endl;
        return false;
    }

    glfwSetErrorCallback(glfwErrorCallback);

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

static bool initShaders()
{
    auto vShader = std::make_unique<Myst::GLShader>(GL_VERTEX_SHADER);
    auto fShader = std::make_unique<Myst::GLShader>(GL_FRAGMENT_SHADER);

    if (!vShader->Compile(vShaderSource)) {
        std::cerr << "gl: failed to compile vShader" << std::endl;
        return false;
    }

    if (!fShader->Compile(fShaderSource)) {
        std::cerr << "gl: failed to compile fShader" << std::endl;
        return false;
    }

    auto program = std::make_unique<Myst::GLShaderProgram>();

    program->AttachShader(*vShader);
    program->AttachShader(*fShader);

    if (!program->Link()) {
        std::cerr << "gl: failed to link program" << std::endl;
        return false;
    }

    program->Bind();
    program->SetInt("myTexture", 0);

    return true;
}

static void initBuffers()
{
    // clang-format off
    float vertices[] = {
        // positions        // colors          // texture coordinates
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.5f, 1.0f,
    };

    // Create the vertex array object and the vertex buffer object.
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the vertex array object to it becomes active.
    glBindVertexArray(VAO);

    // Bind the vertex buffer object and move the vertex data into it.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup `position` attribute.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup `color` attribute.
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup `texture coordinate` attribute.
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

static bool initTexture()
{
    auto texture = std::make_unique<Myst::GLTexture>("assets/uv_grid.png", GL_TEXTURE_2D);

    if (!texture->Generate()) {
        std::cerr << "gl: failed to load texture" << std::endl;
        return false;
    }

    texture->Bind();

    return true;
}

static void update()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

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

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glMessageCallback, 0);

    if (!initShaders()) {
        return EXIT_FAILURE;
    }

    initBuffers();
    initTexture();

    while (!glfwWindowShouldClose(window)) {
        update();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
