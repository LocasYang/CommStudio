param(
    [string]$QtVersion = "6.6.3",
    [string]$QtRoot = "",
    [string]$QtToolsRoot = "",
    [ValidateSet("mingw", "msvc")]
[string]$Compiler = "msvc",
    [switch]$RunTests
)

$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
$OutputEncoding = [Console]::OutputEncoding
$ProjectRoot = Split-Path -Parent $PSScriptRoot

. (Join-Path $PSScriptRoot "qt-toolchain.ps1")
$toolchain = Get-QtToolchain -Version $QtVersion -QtRoot $QtRoot -QtToolsRoot $QtToolsRoot -Compiler $Compiler
$QtRoot      = $toolchain.QtRoot
$NinjaExe    = $toolchain.NinjaExe
$CmakeExe    = $toolchain.CmakeExe
$WinDeployQt = $toolchain.WinDeployQt

# MSVC 使用独立构建目录，避免与 MinGW 构建产物互相污染。
$buildDir = if ($toolchain.Compiler -eq "msvc") {
    Join-Path $ProjectRoot "build-msvc"
} else {
    Join-Path $ProjectRoot "build"
}

if ($toolchain.Compiler -eq "msvc") {
    # ---- MSVC：所有构建命令在 vcvars64 环境里执行（cl / link / mt 所需）。----
    $env:PATH = "$(Join-Path $QtRoot 'bin');$env:PATH"
    $vcvars = $toolchain.VcVars64
    if (-not $CmakeExe) { throw "MSVC 模式未定位到 CMake（安装 VS 时勾选 C++ CMake 组件或安装 Tools\CMake_64）" }
    if (-not $NinjaExe) { throw "MSVC 模式未定位到 Ninja（安装 VS 时勾选 C++ CMake 组件或安装 Tools\Ninja）" }

    function Invoke-VsCmd([string]$command) {
        cmd /s /c "`"$vcvars`" >nul 2>&1 && $command"
        return $LASTEXITCODE
    }

    if (-not (Test-Path $buildDir)) {
        $exit = Invoke-VsCmd "`"$CmakeExe`" -S `"$ProjectRoot`" -B `"$buildDir`" -G Ninja -DCMAKE_MAKE_PROGRAM=`"$NinjaExe`" -DCMAKE_PREFIX_PATH=`"$QtRoot`" -DCMAKE_BUILD_TYPE=Release"
        if ($exit -ne 0) { exit $exit }
    }

    $exit = Invoke-VsCmd "`"$CmakeExe`" --build `"$buildDir`""
    if ($exit -ne 0) { exit $exit }

    # 部署 Qt 运行时（--compiler-runtime 附带 MSVC 运行库），同样在 vcvars 环境执行。
    $qmlDir = Join-Path $ProjectRoot "qml"
    $exePath = Join-Path $buildDir "CommStudio.exe"
    $exit = Invoke-VsCmd "`"$WinDeployQt`" --release --compiler-runtime --qmldir `"$qmlDir`" `"$exePath`""
    if ($exit -ne 0) { exit $exit }

    if ($RunTests) {
        $exit = Invoke-VsCmd "`"$CmakeExe`" --build `"$buildDir`" --target test"
        if ($exit -ne 0) { exit $exit }
    }
} else {
    # ---- MinGW（原有逻辑）----
    $env:PATH = "$($toolchain.MingwBin);$(Join-Path $QtRoot 'bin');$env:PATH"

    if (-not (Test-Path $buildDir)) {
        & $CmakeExe -S $ProjectRoot -B $buildDir -G Ninja `
            -DCMAKE_MAKE_PROGRAM="$NinjaExe" `
            -DCMAKE_PREFIX_PATH="$QtRoot" `
            -DCMAKE_BUILD_TYPE=Release
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }

    & $CmakeExe --build $buildDir
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    & $WinDeployQt --release --qmldir (Join-Path $ProjectRoot "qml") `
        (Join-Path $buildDir "CommStudio.exe")
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    if ($RunTests) {
        & $CmakeExe --build $buildDir --target test
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
}

$DistExecutable = Join-Path $ProjectRoot "dist\CommStudio\CommStudio.exe"
if (-not (Test-Path -LiteralPath $DistExecutable)) {
    throw "构建完成但发布目录同步失败：$DistExecutable"
}
Write-Host "发布目录已同步：$DistExecutable"
