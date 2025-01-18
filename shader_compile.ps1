# 脚本功能：编译指定目录下的 GLSL shader 文件为 SPIR-V 格式
# 使用方法：./compile_shaders.ps1 [-ShaderSrcDir] <shader源目录> [-ShaderOutDir] <目标目录>
# 默认值：
#   shader源目录 = ./shaders
#   目标目录 = ./output/shaders

param (
    [string]$ShaderSrcDir = "./shaders",  # 默认 shader 源目录
    [string]$ShaderOutDir = "./output/shaders"  # 默认目标目录
)

# 检查并创建目标目录
if (-not (Test-Path -Path $ShaderOutDir)) {
    New-Item -ItemType Directory -Path $ShaderOutDir | Out-Null
}

# 定义编译 shader 的函数
function Compile-Shader {
    param (
        [string]$ShaderFile,
        [string]$ShaderOutFile
    )

    # 根据文件后缀确定 shader 阶段
    if ($ShaderFile.EndsWith(".vert")) {
        $ShaderStage = "vertex"
    } elseif ($ShaderFile.EndsWith(".frag")) {
        $ShaderStage = "fragment"
    } else {
        Write-Host "错误：未知的 shader 文件类型 $ShaderFile" -ForegroundColor Red
        exit 1
    }

    # 编译 shader
    Write-Host "正在编译 $ShaderFile ..."
    $glslcCommand = "glslc -fshader-stage=$ShaderStage `"$ShaderFile`" -o `"$ShaderOutFile`""
    Invoke-Expression $glslcCommand
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误：编译 $ShaderFile 失败。" -ForegroundColor Red
        exit 1
    }
    Write-Host "编译成功：$ShaderOutFile" -ForegroundColor Green
}

# 遍历 shader 源目录并编译
Get-ChildItem -Path $ShaderSrcDir -Recurse -Include *.vert, *.frag | ForEach-Object {
    $ShaderFile = $_.FullName
    $ShaderOutFile = Join-Path -Path $ShaderOutDir -ChildPath ($_.BaseName + $_.Extension + ".spv")

    # 调用编译函数
    Compile-Shader -ShaderFile $ShaderFile -ShaderOutFile $ShaderOutFile
}

Write-Host "所有 shader 编译完成！" -ForegroundColor Green