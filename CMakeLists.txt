cmake_minimum_required(VERSION 3.16)
project(ChessScapeRender VERSION 1.0.0 LANGUAGES C CXX)

# --- Project Settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Useful for VS Code Intellisense

# --- Find Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

# GLM is often header-only; find_package works if installed, 
# otherwise we can rely on system paths or pkg-config.
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLM REQUIRED glm)
endif()

# --- Source Files ---
# Note: Ensure you add model.cpp and ShaderClass.cpp here once you split them!
set(SOURCES
    src/3D-Chess.cpp
    src/mesh.cpp            
    src/model.cpp
    src/stb_image_impl.cpp
    src/ShaderClass.cpp
    src/utility.cpp
    dependencies/glad/glad/glad.c
)

# --- Create Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Target Properties (The "Modern" Way) ---
target_include_directories(${PROJECT_NAME} PRIVATE
    include                 # Your new header folder
    src                     # In case any headers remain in src
    dependencies/glad
    dependencies/stb-master
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OPENGL_LIBRARIES}
    glfw                    # Standard target name for GLFW3
    assimp::assimp           # Standard target name for Assimp
    ${CMAKE_DL_LIBS}        # Cross-platform way to link 'dl'
)

# --- Compiler Configuration ---
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall 
        -Wextra 
        -Wno-unused-parameter
    )
endif()

# --- Definitions ---
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PATH_TO_WD="${CMAKE_SOURCE_DIR}"
)

# --- Resource Management ---
# Optimization: On Linux, symlinking is faster than copying. 
# But for portability, we'll stick to a directory copy.
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    COMMENT "Syncing resources to build directory..."
)