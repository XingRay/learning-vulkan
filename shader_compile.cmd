@echo off

:: 检查并创建 output/shader 目录
if not exist "output\shader" (
    mkdir "output\shader"
)

:: 编译着色器
glslc -fshader-stage=vertex shader/vertex.glsl -o output/shader/vertex.spv
glslc -fshader-stage=fragment shader/fragment.glsl -o output/shader/fragment.spv
