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
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "OpenGL/GLShader.hpp"
#include "OpenGL/GLTexture.hpp"
#include "Scene/Camera.hpp"

#define WIDTH (640)
#define HEIGHT (480)

static GLFWwindow* window = nullptr;
static GLuint lightVAO, cubeVAO, VBO;

static std::unique_ptr<Myst::Camera> camera;
static std::unique_ptr<Myst::GLTexture> diffuse;
static std::unique_ptr<Myst::GLTexture> specular;

static bool firstMouseMovement{true};
static float mouseLastX{0};
static float mouseLastY{0};

static float deltaTime{0};
static float lastTime{0};

static void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "glfw: " << description << std::endl;
}

static void glfwCursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouseMovement) {
        firstMouseMovement = false;
        mouseLastX = xPos;
        mouseLastY = yPos;
    }

    double xDelta = xPos - mouseLastX;
    double yDelta = mouseLastY - yPos;

    mouseLastX = xPos;
    mouseLastY = yPos;

    camera->OnMouseMove((float)xDelta, (float)yDelta);
}

static void glfwScrollCallback(
    GLFWwindow* window, double xOffset, double yOffset)
{
    camera->OnMouseScroll((float)yOffset);
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, glfwCursorPosCallback);
    glfwSetScrollCallback(window, glfwScrollCallback);

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

static Myst::GLShaderProgram* createShaderProgram(
    const std::string& vertexShaderFilepath,
    const std::string& fragmentShaderFilepath)
{
    auto vShader = std::make_unique<Myst::GLShader>(
        vertexShaderFilepath, GL_VERTEX_SHADER);
    auto fShader = std::make_unique<Myst::GLShader>(
        fragmentShaderFilepath, GL_FRAGMENT_SHADER);

    if (!vShader->Compile()) {
        std::cerr << "gl: failed to compile vShader" << std::endl;
        return nullptr;
    }

    if (!fShader->Compile()) {
        std::cerr << "gl: failed to compile fShader" << std::endl;
        return nullptr;
    }

    Myst::GLShaderProgram* program = new Myst::GLShaderProgram();

    program->AttachShader(*vShader);
    program->AttachShader(*fShader);

    if (!program->Link()) {
        std::cerr << "gl: failed to link program" << std::endl;
        delete program;
        return nullptr;
    }

    program->Bind();

    return program;
}

static void initBuffers()
{
    // clang-format off
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // Create the cube's vertex array object and the vertex buffer object.
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    // Bind the vertex buffer object and move the vertex data into it.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind the cube's vertex array object to it becomes active.
    glBindVertexArray(cubeVAO);

    // Setup `position` attribute.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Setup `normal` attribute.
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Setup `texture` attribute.
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Create the light's vertex array object.
    glGenVertexArrays(1, &lightVAO);

    // Bind the lights's vertex array object to it becomes active.
    glBindVertexArray(lightVAO);

    // Bind the vertex buffer object. (No need to fill it because it's already
    // filled.)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Setup `position` attribute.
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

static bool initTextures()
{
    diffuse = std::make_unique<Myst::GLTexture>("assets/textures/crate_diffuse.png", GL_TEXTURE_2D);
    if (!diffuse->Generate()) {
        std::cerr << "gl: failed to load diffuse map" << std::endl;
        return false;
    }

    specular = std::make_unique<Myst::GLTexture>("assets/textures/crate_specular.png", GL_TEXTURE_2D);
    if (!specular->Generate()) {
        std::cerr << "gl: failed to load specular map" << std::endl;
        return false;
    }

    diffuse->Bind(0);
    specular->Bind(1);

    return true;
}

static void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->OnKeyPress(GLFW_KEY_W, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->OnKeyPress(GLFW_KEY_A, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->OnKeyPress(GLFW_KEY_S, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->OnKeyPress(GLFW_KEY_D, deltaTime);
    }
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

    initBuffers();

    if (!initTextures()) {
        return EXIT_FAILURE;
    }

    std::unique_ptr<Myst::GLShaderProgram> lightProgram(
            createShaderProgram(
                "assets/shaders/light_vertex.glsl",
                "assets/shaders/light_fragment.glsl"));

    std::unique_ptr<Myst::GLShaderProgram> cubeProgram(
            createShaderProgram(
                "assets/shaders/cube_vertex.glsl",
                "assets/shaders/cube_fragment.glsl"));

    camera = std::make_unique<Myst::Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(window);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera->GetZoom()), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 model(1.0f);

        glm::mat3 modelView(view * model);
        glm::mat3 normal = glm::inverseTranspose(modelView);

        cubeProgram->Bind();

        cubeProgram->SetInt("material.diffuse", 0);
        cubeProgram->SetInt("material.specular", 1);
        cubeProgram->SetFloat("material.shininess", 32.0f);

        cubeProgram->SetVec3("light.position", view * glm::vec4(lightPos, 1.0));
        cubeProgram->SetVec3("light.ambient", glm::vec3(0.2f));
        cubeProgram->SetVec3("light.diffuse", glm::vec3(0.5f));
        cubeProgram->SetVec3("light.specular", glm::vec3(1.0f));

        cubeProgram->SetMat4("projection", projection);
        cubeProgram->SetMat4("view", view);
        cubeProgram->SetMat4("model", model);
        cubeProgram->SetMat3("normal", normal);
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightProgram->Bind();
        lightProgram->SetMat4("projection", projection);
        lightProgram->SetMat4("view", view);
        lightProgram->SetMat4("model", model);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
