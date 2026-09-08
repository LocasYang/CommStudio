param(
    [string]$QtVersion = "6.6.3",
    [string]$QtRoot = "",
    [string]$QtToolsRoot = "",
    [ValidateSet("mingw", "msvc")]
[string]$Compiler = "msvc"
)

# 发布打包：构建 + 把 exe 与 Qt 运行时部署到 dist/CommStudio，
# 产出自包含文件夹，拷到任何机器（无需 Qt 环境）即可运行。
$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)

$ProjectRoot = (Split-Path -Parent $PSScriptRoot).Replace("\", "/")

$buildArgs = @{ QtVersion = $QtVersion; Compiler = $Compiler }
if ($QtRoot) { $buildArgs.QtRoot = $QtRoot }
if ($QtToolsRoot) { $buildArgs.QtToolsRoot = $QtToolsRoot }
& (Join-Path $PSScriptRoot "build.ps1") @buildArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

. (Join-Path $PSScriptRoot "qt-toolchain.ps1")
$toolchain = Get-QtToolchain -Version $QtVersion -QtRoot $QtRoot -QtToolsRoot $QtToolsRoot -Compiler $Compiler
if ($toolchain.Compiler -eq "mingw") {
    $env:PATH = "$($toolchain.MingwBin);$($toolchain.QtRoot)/bin;$env:PATH"
} else {
    $env:PATH = "$($toolchain.QtRoot)/bin;$env:PATH"
}

$distDir = Join-Path $ProjectRoot "dist/CommStudio"
New-Item -ItemType Directory -Force -Path $distDir | Out-Null

# 切换工具链后清理上一次打包遗留的异构运行库，避免与新构建的 exe 加载冲突。
foreach ($stale in @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll",
                     "vcruntime140.dll", "vcruntime140_1.dll", "msvcp140.dll",
                     "msvcp140_1.dll", "msvcp140_2.dll", "concrt140.dll")) {
    $stalePath = Join-Path $distDir $stale
    if (Test-Path -LiteralPath $stalePath) { Remove-Item -Force $stalePath }
}

Copy-Item -Force (Join-Path $ProjectRoot "README.md") $distDir

# 显式复制最新 exe（构建无改动时 CMake 的 POST_BUILD 复制不会执行）。
$buildExe = if ($Compiler -eq "msvc") {
    Join-Path $ProjectRoot "build-msvc/CommStudio.exe"
} else {
    Join-Path $ProjectRoot "build/CommStudio.exe"
}
if (-not (Test-Path -LiteralPath $buildExe)) {
    throw "未找到构建产物：$buildExe（请先成功运行 build.ps1）"
}
Copy-Item -Force $buildExe (Join-Path $distDir "CommStudio.exe")

$deployArgs = @("--release", "--qmldir", (Join-Path $ProjectRoot "qml"))
if ($Compiler -eq "msvc") { $deployArgs += "--compiler-runtime" }
$deployArgs += (Join-Path $distDir "CommStudio.exe")
& $toolchain.WinDeployQt @deployArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# 编译器运行库：MinGW 需要 libgcc/libstdc++/winpthread；MSVC 优先取
# windeployqt 的 --compiler-runtime 结果，缺漏时从 BuildTools Redist 兜底复制。
if ($Compiler -eq "mingw") {
    foreach ($dll in @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")) {
        $src = Join-Path $toolchain.MingwBin $dll
        if (Test-Path $src) { Copy-Item -Force $src $distDir }
    }
} else {
    $crtDlls = @("vcruntime140.dll", "vcruntime140_1.dll", "msvcp140.dll",
                 "msvcp140_1.dll", "msvcp140_2.dll")
    $missing = @($crtDlls | Where-Object { -not (Test-Path (Join-Path $distDir $_)) })
    if ($missing.Count -gt 0) {
        $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio/Installer/vswhere.exe"
        $vsPath = & $vswhere -latest -products * `
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
            -property installationPath
        $redist = Get-ChildItem (Join-Path $vsPath "VC/Redist/MSVC/*/x64/Microsoft.VC143.CRT") `
            -Directory -ErrorAction SilentlyContinue | Sort-Object FullName | Select-Object -Last 1
        foreach ($dll in $missing) {
            $src = Join-Path $redist.FullName $dll
            if (Test-Path $src) { Copy-Item -Force $src $distDir }
        }
    }
}

Write-Host "发布完成：$distDir"
