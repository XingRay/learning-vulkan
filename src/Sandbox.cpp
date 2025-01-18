#include <iostream>
#include <stdexcept>

#include <GLFW/glfw3.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "test/Test01SimpleTriangle.h"
#include "test/Test02SingleColorTriangle.h"
#include "test/Test03ColoredTriangle.h"
#include "test/Test04MvpMatrix.h"
#include "test/Test05TextureImage.h"
#include "test/Test06Load3dModel.h"

test::TestBase *engineTest;

const int32_t mWidth = 1024;
const int32_t mHeight = 1024;

GLFWwindow *mWindow;

void init();

void mainLoop();

void cleanUp();


int main() {
    init();

    mainLoop();

    cleanUp();

    return EXIT_SUCCESS;
}

void init() {
    glfwInit();

    // Because GLFW was originally designed to create an OpenGL context,
    // we need to tell it to not create an OpenGL context with a subsequent call:
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Because handling resized windows takes special care that we'll look into later, disable it for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // The first three parameters specify the width, height and title of the mWindow.
    // The fourth parameter allows you to optionally specify a monitor to open the mWindow on
    // and the last parameter is only relevant to OpenGL.
    mWindow = glfwCreateWindow(mWidth, mHeight, "engineTest", nullptr, nullptr);
    // 将 this 指针保存到window对象中， 这样可以在callback中取出， 这里使用 lambda， 可以不需要
    glfwSetWindowUserPointer(mWindow, nullptr);
    glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow *window, int width, int height) {
        // auto app = reinterpret_cast<ModelLoaderTest *>(glfwGetWindowUserPointer(window));
        std::cout << "app->mFrameBufferResized, width:" << width << " height:" << height << std::endl;
        // app->mFrameBufferResized = true;
    });


    // engineTest = new test01::Test01SimpleTriangle(mWindow);
    // engineTest = new test02::Test02SingleColorTriangle(mWindow);
    // engineTest = new test03::Test03ColoredTriangle(mWindow);
    // engineTest = new test04::Test04MvpMatrix(mWindow);
    // engineTest = new test05::Test05TextureImage(mWindow);
    engineTest = new test06::Test06Load3dModel(mWindow);
    engineTest->init();
}

void mainLoop() {
    while (!glfwWindowShouldClose(mWindow)) {
        glfwPollEvents();
        engineTest->drawFrame();
    }
}

void cleanUp() {
    engineTest->cleanup();
}
