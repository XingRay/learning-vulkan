#include <iostream>
#include <stdexcept>

#include "TriangleTest.h"
#include "ModelLoaderTest.h"
#include "EngineTest.h"

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

int main() {
//    triangle::TriangleTest triangleTest;
//    triangleTest.main();

//    model_loader_test::ModelLoaderTest modelLoaderTest;
//    modelLoaderTest.main();

    engine::EngineTest engineTest;
    engineTest.main();

    return EXIT_SUCCESS;
}