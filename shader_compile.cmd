@echo off

:: 检查并创建 output/shader 目录
if not exist "output\shader" (
    mkdir "output\shader"
)

if not exist "output\shader\01_triangle" (
    mkdir "output\shader\01_triangle"
)
glslc -fshader-stage=vertex shader/01_triangle/vertex.glsl -o output/shader/01_triangle/vertex.spv
glslc -fshader-stage=fragment shader/01_triangle/fragment.glsl -o output/shader/01_triangle/fragment.spv

if not exist "output\shader\02_triangle_color" (
    mkdir "output\shader\02_triangle_color"
)
glslc -fshader-stage=vertex shader/02_triangle_color/vertex.glsl -o output/shader/02_triangle_color/vertex.spv
glslc -fshader-stage=fragment shader/02_triangle_color/fragment.glsl -o output/shader/02_triangle_color/fragment.spv

if not exist "output\shader\03_3d_model" (
    mkdir "output\shader\03_3d_model"
)
glslc -fshader-stage=vertex shader/03_3d_model/vertex.glsl -o output/shader/03_3d_model/vertex.spv
glslc -fshader-stage=fragment shader/03_3d_model/fragment.glsl -o output/shader/03_3d_model/fragment.spv
