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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLShader.hpp"
#include "GLTexture.hpp"

#define WIDTH (640)
#define HEIGHT (480)

static GLFWwindow* window = nullptr;
static GLuint VAO, VBO;

static std::unique_ptr<Myst::GLShaderProgram> program;

static const char* vShaderSource =
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "\n"
    "out vec2 TexCoord;\n"
    "\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
    "}\0";

static const char* fShaderSource =
    "#version 460 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec2 TexCoord;\n"
    "\n"
    "uniform sampler2D myTexture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(myTexture, TexCoord);\n"
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

    window = glfwCreateWindow(WIDTH, HEIGHT, "Myst", nullptr, nullptr);

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

    program = std::make_unique<Myst::GLShaderProgram>();

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
        // positions          // texture coordinates
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
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
        0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup `texture coordinate` attribute.
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(1.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    program->SetMat4("model", model);
    program->SetMat4("view", view);
    program->SetMat4("projection", projection);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

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
    glEnable(GL_DEPTH_TEST);
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
