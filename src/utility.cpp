/**
 * @file utility.cpp
 * @brief Full implementation of window management, input handling, and GL resource helpers.
 */

#include "utility.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// 1. Initialize Global State (Namespaces defined in utility.h)
namespace State
{
unsigned int width = 1280;
unsigned int height = 720;
float deltaTime = 0.0f;
float lastTime = 0.0f;
} // namespace State

namespace Camera
{
glm::vec3 pos = glm::vec3(0.0f, 1.8f, 3.5f);
glm::vec3 front = glm::vec3(0.0f, -0.3f, -1.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = -20.0f;
float last_x = 640.0f;
float last_y = 360.0f;
float fov = 45.0f;
bool firstMouse = true;
bool lockCamera = false;
bool mouseLook = true;
} // namespace Camera

// 2. Static Geometry Data
const float CULLING[] = {
    // back face
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -1.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -1.0f,
    1.0f,
    1.0f,
    0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -1.0f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -1.0f,
    1.0f,
    1.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -1.0f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    -1.0f,
    0.0f,
    1.0f,
    // front face
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    // left face
    -0.5f,
    0.5f,
    0.5f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    -0.5f,
    -0.5f,
    0.5f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    0.5f,
    0.5f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    // right face
    0.5f,
    0.5f,
    0.5f,
    -1.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.5f,
    -0.5f,
    -0.5f,
    -1.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.5f,
    0.5f,
    -0.5f,
    -1.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0.5f,
    -0.5f,
    -0.5f,
    -1.0f,
    0.0f,
    0.0f,
    0.0f,
    1.0f,
    0.5f,
    0.5f,
    0.5f,
    -1.0f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    -1.0f,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    // bottom face
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.5f,
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    0.0f,
    -0.5f,
    -0.5f,
    0.5f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    0.0f,
    -0.5f,
    -0.5f,
    -0.5f,
    0.0f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    // top face
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    -1.0f,
    0.0f,
    0.0f,
    1.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    -1.0f,
    0.0f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    -0.5f,
    0.0f,
    -1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.5f,
    0.5f,
    0.5f,
    0.0f,
    -1.0f,
    0.0f,
    1.0f,
    0.0f,
    -0.5f,
    0.5f,
    -0.5f,
    0.0f,
    -1.0f,
    0.0f,
    0.0f,
    1.0f,
    -0.5f,
    0.5f,
    0.5f,
    0.0f,
    -1.0f,
    0.0f,
    0.0f,
    0.0f};

const float skyboxVertices[] = {
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f};

const std::vector<std::string> faces = {
    "resources/textures/skybox/right.jpg",
    "resources/textures/skybox/left.jpg",
    "resources/textures/skybox/top.jpg",
    "resources/textures/skybox/bottom.jpg",
    "resources/textures/skybox/front.jpg",
    "resources/textures/skybox/back.jpg"};

// 3. Callback Implementations
void FrameBuffer_size_callback(GLFWwindow *window, int newWidth, int newHeight)
{
    (void)window;
    State::width = newWidth;
    State::height = newHeight;
    glViewport(0, 0, State::width, State::height);
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    (void)window;
    if (!Camera::mouseLook)
        return;

    if (Camera::firstMouse)
    {
        Camera::last_x = static_cast<float>(xPos);
        Camera::last_y = static_cast<float>(yPos);
        Camera::firstMouse = false;
    }

    float x_offset = static_cast<float>(xPos) - Camera::last_x;
    float y_offset = Camera::last_y - static_cast<float>(yPos);
    Camera::last_x = static_cast<float>(xPos);
    Camera::last_y = static_cast<float>(yPos);

    const float sensitivity = 0.1f;
    Camera::yaw += x_offset * sensitivity;
    Camera::pitch += y_offset * sensitivity;

    if (Camera::pitch > 89.0f)
        Camera::pitch = 89.0f;
    if (Camera::pitch < -89.0f)
        Camera::pitch = -89.0f;

    if (!Camera::lockCamera)
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(Camera::yaw)) * cos(glm::radians(Camera::pitch));
        direction.y = sin(glm::radians(Camera::pitch));
        direction.z = sin(glm::radians(Camera::yaw)) * cos(glm::radians(Camera::pitch));
        Camera::front = glm::normalize(direction);
    }
}

void ScrollCallback(GLFWwindow *window, double x_offset, double y_offset)
{
    (void)window;
    (void)x_offset;
    Camera::fov -= static_cast<float>(y_offset);
    if (Camera::fov < 1.0f)
        Camera::fov = 1.0f;
    if (Camera::fov > 45.0f)
        Camera::fov = 45.0f;
}

// 4. Setup and Input
int GLFW_SETUP(GLFWwindow *&window, unsigned int &width_ref, unsigned int &height_ref)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width_ref, height_ref, "ChessScapeRender", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, ScrollCallback);

    return 0;
}

void ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float speed = 5.0f * State::deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Camera::pos += speed * Camera::front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Camera::pos -= speed * Camera::front;

    glm::vec3 right = glm::normalize(glm::cross(Camera::front, Camera::up));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Camera::pos -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Camera::pos += right * speed;

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        Camera::lockCamera = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        Camera::lockCamera = true;

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        Camera::pos = glm::vec3(0.0f, 1.8f, 3.5f);
        Camera::yaw = -90.0f;
        Camera::pitch = -20.0f;
        Camera::firstMouse = true;
    }

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        Camera::mouseLook = false;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        Camera::mouseLook = true;
    }
}

// 5. Resource and GL State Helpers
void configureGLState()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int loadCubemap(const std::vector<std::string> &facePaths)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int w, h, nrChannels;
    stbi_set_flip_vertically_on_load(false); // Cubemaps should not be flipped
    for (unsigned int i = 0; i < facePaths.size(); i++)
    {
        unsigned char *data = stbi_load(facePaths[i].c_str(), &w, &h, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data);
            stbi_image_free(data);
        }
        else
        {
            std::cerr << "Cubemap failed to load: " << facePaths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

void setupCullingVAO(unsigned int &VAO, unsigned int &VBO)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CULLING), CULLING, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void setupSkyboxVAO(unsigned int &VAO, unsigned int &VBO)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindVertexArray(0);
}

void cleanup(
    GLFWwindow *window,
    unsigned int &cVAO,
    unsigned int &cVBO,
    unsigned int &sVAO,
    unsigned int &sVBO,
    unsigned int &cubemap)
{
    glDeleteVertexArrays(1, &cVAO);
    glDeleteBuffers(1, &cVBO);
    glDeleteVertexArrays(1, &sVAO);
    glDeleteBuffers(1, &sVBO);
    glDeleteTextures(1, &cubemap);
    if (window)
        glfwDestroyWindow(window);
    glfwTerminate();
}