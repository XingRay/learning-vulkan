cmake_minimum_required(VERSION 3.28)
project(learning_vulkan)
set(CMAKE_CXX_STANDARD 20)


# glfw
set(GLFW_INSTALL_PATH D:\\develop\\opengl\\glfw\\glfw-3.4.bin.WIN64)
set(GLFW_LIB_PATH ${GLFW_INSTALL_PATH}\\lib-vc2022)
set(GLFW_INCLUDE_PATH ${GLFW_INSTALL_PATH}\\include)

# glm
# https://github.com/g-truc/glm
set(GLM_INSTALL_DIR D:\\develop\\opengl\\glm\\glm-1.0.1-light)

# vulkan
# https://www.vulkan.org/
set(VULKAN_INSTALL_DIR D:\\develop\\vulkan)
set(VULKAN_INCLUDE_DIR ${VULKAN_INSTALL_DIR}/Include)
set(VULKAN_LIB_DIR ${VULKAN_INSTALL_DIR}/Lib)


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
        ${GLFW_INCLUDE_PATH}

#        # STB
#        ${STB_INCLUDE_PATH}

        # src
)


file(GLOB_RECURSE SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
        ${PROJECT_NAME}
        ${GLFW_LIB_PATH}/glfw3.lib
        ${VULKAN_LIB_DIR}/vulkan-1.lib

)