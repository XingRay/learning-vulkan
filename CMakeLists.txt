cmake_minimum_required(VERSION 3.28)
project(learning_vulkan)
set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# glfw
set(GLFW_INSTALL_DIR D:/develop/opengl/glfw/glfw-3.4.bin.WIN64)
set(GLFW_LIB_DIR ${GLFW_INSTALL_DIR}/lib-vc2022)
set(GLFW_INCLUDE_DIR ${GLFW_INSTALL_DIR}/include)

# glm
# https://github.com/g-truc/glm
set(GLM_INSTALL_DIR D:/develop/opengl/glm/glm-1.0.1-light)

# vulkan
# https://www.vulkan.org/
set(VULKAN_INSTALL_DIR D:/develop/vulkan/VulkanSDK/1.3.296.0)
set(VULKAN_INCLUDE_DIR ${VULKAN_INSTALL_DIR}/Include)
set(VULKAN_LIB_DIR ${VULKAN_INSTALL_DIR}/Lib)

# stb
set(STB_INSTALL_DIR D:/develop/opengl/stb)
set(STB_INCLUDE_DIR ${STB_INSTALL_DIR})

# tinyobjloader
set(TINYOBJLOADER_INSTALL_DIR D:/develop/opengl/tinyobjloader)
set(TINYOBJLOADER_INCLUE_DIR ${TINYOBJLOADER_INSTALL_DIR})


include_directories(
        # glm
        ${GLM_INSTALL_DIR}

        # vulkan
        ${VULKAN_INCLUDE_DIR}


        # opencv
#        ${OPENCV_INCLUDE}
#        ${OPENCV_INCLUDE}/opencv2

        # visual studio
#        ${VS_HOME_DIR}/include

        # GLEW
#        ${GLEW_INCLUDE_PATH}

        # GLFW
        ${GLFW_INCLUDE_DIR}

#        # STB
        ${STB_INCLUDE_DIR}

        # tinyobjloader
        ${TINYOBJLOADER_INCLUE_DIR}

        # src
        src
        src/engine
        src/winows
)


file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
        ${PROJECT_NAME}
        ${GLFW_LIB_DIR}/glfw3.lib
        ${VULKAN_LIB_DIR}/vulkan-1.lib
)