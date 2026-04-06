/**
 * @file 3D-Chess.cpp
 * @brief Optimized main entry point for the ChessScapeRender engine.
 */

#include <csignal>
#include <iostream>
#include <vector>

// GLM Headers
#include "model.h"
#include "utility.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- Macros & Debugging ---
#ifdef _WIN32
#define ASSERT(condition)                                                                          \
    if (!(condition))                                                                              \
    __debugbreak()
#else
#define ASSERT(condition)                                                                          \
    if (!(condition))                                                                              \
    raise(SIGTRAP)
#endif

// --- Data Structures ---
struct ChessPiece
{
    const Model *model;
    glm::mat4 transform;
    float reflectivity;
};

/** @brief RAII container for OpenGL resource handles */
struct SceneResources
{
    unsigned int cVAO, cVBO;
    unsigned int sVAO, sVBO;
    unsigned int cubemap;
};

// --- Constants ---
constexpr float PIECE_SCALE = 0.16f;
constexpr int TOTAL_PIECES = 32;

// --- Helper Functions ---

static glm::mat4 CreateTransform(const glm::vec3 &pos, float scale)
{
    return glm::scale(glm::translate(glm::mat4(1.0f), pos), glm::vec3(scale));
}

static GLFWwindow *SetupContext()
{
    GLFWwindow *window;
    if (GLFW_SETUP(window, State::width, State::height) != 0)
        exit(-1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR::GLAD_INIT_FAILED" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    return window;
}

static void PopulateBoard(
    std::vector<ChessPiece> &pieces,
    const Model &bK,
    const Model &gK,
    const Model &bQ,
    const Model &gQ,
    const Model &bB,
    const Model &gB,
    const Model &bN,
    const Model &gN,
    const Model &bR,
    const Model &gR,
    const Model &bP,
    const Model &gP)
{
    pieces.reserve(TOTAL_PIECES);

    // Royalty & Power
    pieces.push_back({&bQ, CreateTransform(glm::vec3(-0.127f, 0.0f, 0.912f), PIECE_SCALE), 0.45f});
    pieces.push_back({&gQ, CreateTransform(glm::vec3(-0.127f, 0.0f, -0.912f), PIECE_SCALE), 0.45f});
    pieces.push_back({&bK, CreateTransform(glm::vec3(0.127f, 0.0f, 0.912f), PIECE_SCALE), 0.45f});
    pieces.push_back({&gK, CreateTransform(glm::vec3(0.127f, 0.0f, -0.912f), PIECE_SCALE), 0.45f});

    // Bishops, Knights, Rooks (Shared logic for efficiency)
    auto addMirror = [&](const Model *b, const Model *g, float x, float z, float refl)
    {
        pieces.push_back({b, CreateTransform(glm::vec3(x, 0.0f, z), PIECE_SCALE), refl});
        pieces.push_back({b, CreateTransform(glm::vec3(-x, 0.0f, z), PIECE_SCALE), refl});
        pieces.push_back({g, CreateTransform(glm::vec3(x, 0.0f, -z), PIECE_SCALE), refl});
        pieces.push_back({g, CreateTransform(glm::vec3(-x, 0.0f, -z), PIECE_SCALE), refl});
    };

    addMirror(&bB, &gB, 0.383f, 0.898f, 0.30f);
    addMirror(&bN, &gN, 0.645f, 0.907f, 0.30f);
    addMirror(&bR, &gR, 0.902f, 0.907f, 0.30f);

    // Pawns
    for (int i = 0; i < 8; ++i)
    {
        float x = -0.905f + i * 0.26f;
        pieces.push_back({&bP, CreateTransform(glm::vec3(x, 0.0f, 0.637f), PIECE_SCALE), 0.15f});
        pieces.push_back({&gP, CreateTransform(glm::vec3(x, 0.0f, -0.637f), PIECE_SCALE), 0.15f});
    }
}

// --- Render Logic ---

static void RenderScene(
    const MyShader &modelShader,
    const MyShader &skyShader,
    const Model &board,
    const std::vector<ChessPiece> &pieces,
    const SceneResources &res,
    const glm::mat4 &proj,
    const glm::mat4 &view)
{
    // 1. Skybox Pass
    glDepthFunc(GL_LEQUAL);
    skyShader.use();
    skyShader.SetUniformMat4fv("view", view);
    skyShader.SetUniformMat4fv("projection", proj);
    glBindVertexArray(res.sVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, res.cubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    // 2. Model Pass Preparation
    modelShader.use();
    modelShader.SetUniformMat4fv("projection", proj);
    modelShader.SetUniformMat4fv("view", view);
    modelShader.SetUniform3f("cameraPos", Camera::pos);

    // Bind environment map once for all models (Slot 10)
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_CUBE_MAP, res.cubemap);
    modelShader.SetUniform1i("skybox", 10);

    // 3. Draw Board
    glm::mat4 boardMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.1f, 0.0f));
    modelShader.SetUniformMat4fv("model", boardMat);
    board.Draw(modelShader, StencilMode::First);

    // 4. Draw Pieces
    for (const auto &piece : pieces)
    {
        modelShader.SetUniformMat4fv("model", piece.transform);
        modelShader.SetUniform1f("reflectivity", piece.reflectivity);
        piece.model->Draw(modelShader, StencilMode::None);
    }
}

int main()
{
    GLFWwindow *window = SetupContext();
    SceneResources res;

    {
        MyShader modelShader(Paths::VS_MODEL, Paths::FS_MODEL);
        MyShader skyShader(Paths::VS_SKY, Paths::FS_SKY);

        Model board(Paths::CHESS_BOARD);
        Model bK(Paths::BLACK_KING);
        Model gK(Paths::GRAY_KING);
        Model bQ(Paths::BLACK_QUEEN);
        Model gQ(Paths::GRAY_QUEEN);
        Model bB(Paths::BLACK_BISHOP);
        Model gB(Paths::GRAY_BISHOP);
        Model bN(Paths::BLACK_KNIGHT);
        Model gN(Paths::GRAY_KNIGHT);
        Model bR(Paths::BLACK_ROOK);
        Model gR(Paths::GRAY_ROOK);
        Model bP(Paths::BLACK_PAWN);
        Model gP(Paths::GRAY_PAWN);

        configureGLState();
        setupCullingVAO(res.cVAO, res.cVBO);
        setupSkyboxVAO(res.sVAO, res.sVBO);
        res.cubemap = loadCubemap(faces);

        std::vector<ChessPiece> pieces;
        PopulateBoard(pieces, bK, gK, bQ, gQ, bB, gB, bN, gN, bR, gR, bP, gP);

        State::lastTime = (float)glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            float currentFrame = (float)glfwGetTime();
            State::deltaTime = currentFrame - State::lastTime;
            State::lastTime = currentFrame;

            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            ProcessInput(window);

            glm::mat4 view = glm::lookAt(Camera::pos, Camera::pos + Camera::front, Camera::up);
            glm::mat4 proj = glm::perspective(
                glm::radians(Camera::fov), (float)State::width / State::height, 0.1f, 100.0f);

            RenderScene(modelShader, skyShader, board, pieces, res, proj, view);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    cleanup(window, res.cVAO, res.cVBO, res.sVAO, res.sVBO, res.cubemap);
    return 0;
}