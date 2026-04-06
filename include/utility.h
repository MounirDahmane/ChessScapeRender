/**
 * @file utility.h
 * @brief Global state, constants, and helper function declarations.
 */

#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "../dependencies/stb-master/stb_image.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

// --- Paths & Constants ---
namespace Paths
{
inline constexpr const char *FS_MODEL = "resources/Shaders/chess_fs.txt";
inline constexpr const char *VS_MODEL = "resources/Shaders/chess_vs.txt";
inline constexpr const char *VS_SKY = "resources/Shaders/skybox_vs.txt";
inline constexpr const char *FS_SKY = "resources/Shaders/skybox_fs.txt";

inline constexpr const char *CHESS_BOARD = "resources/models/chessBoard/chessBoard.obj";
inline constexpr const char *BLACK_BISHOP = "resources/models/bishop/bishop.obj";
inline constexpr const char *GRAY_BISHOP = "resources/models/bishop_gray/bishop.obj";
inline constexpr const char *BLACK_KING = "resources/models/king/king.obj";
inline constexpr const char *GRAY_KING = "resources/models/king_gray/king.obj";
inline constexpr const char *BLACK_KNIGHT = "resources/models/knight/knight.obj";
inline constexpr const char *GRAY_KNIGHT = "resources/models/knight_gray/knight.obj";
inline constexpr const char *BLACK_PAWN = "resources/models/pawn/pawn.obj";
inline constexpr const char *GRAY_PAWN = "resources/models/pawn_gray/pawn.obj";
inline constexpr const char *BLACK_QUEEN = "resources/models/queen/queen.obj";
inline constexpr const char *GRAY_QUEEN = "resources/models/queen_gray/queen.obj";
inline constexpr const char *BLACK_ROOK = "resources/models/rook/rook.obj";
inline constexpr const char *GRAY_ROOK = "resources/models/rook_gray/rook.obj";
} // namespace Paths

// --- Global Application State ---
namespace State
{
extern unsigned int width;
extern unsigned int height;
extern float deltaTime;
extern float lastTime;
} // namespace State

// --- Camera & Input State ---
namespace Camera
{
extern glm::vec3 pos;
extern glm::vec3 front;
extern glm::vec3 up;
extern float yaw;
extern float pitch;
extern float last_x;
extern float last_y;
extern float fov;
extern bool firstMouse;
extern bool lockCamera;
extern bool mouseLook;
} // namespace Camera

// --- Geometry Data ---
extern const float CULLING[288];
extern const float skyboxVertices[108];
extern const std::vector<std::string> faces;

// --- Function Declarations ---
void FrameBuffer_size_callback(GLFWwindow *window, int newWidth, int newHeight);
void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void ScrollCallback(GLFWwindow *window, double x_offset, double y_offset);

int GLFW_SETUP(GLFWwindow *&window, unsigned int &width_ref, unsigned int &height_ref);
void ProcessInput(GLFWwindow *window);

void configureGLState();
unsigned int loadCubemap(const std::vector<std::string> &facePaths);
void setupCullingVAO(unsigned int &VAO, unsigned int &VBO);
void setupSkyboxVAO(unsigned int &VAO, unsigned int &VBO);
void cleanup(
    GLFWwindow *window,
    unsigned int &cVAO,
    unsigned int &cVBO,
    unsigned int &sVAO,
    unsigned int &sVBO,
    unsigned int &cubemap);
