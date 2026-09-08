# Qt 工具链定位：不写死盘符，按版本号在各个固定盘的 QT\<版本>\<套件> 自动
# 探测（Qt 在线安装器的标准布局，Tools 与版本目录同级）；也可用 QT_ROOT 环境
# 变量或 -QtRoot/-QtToolsRoot 显式指定。
#   -Compiler mingw -> QT\<版本>\mingw_64    + Tools\mingw*_64（Ninja/CMake 来自 Tools）
#   -Compiler msvc  -> QT\<版本>\msvc2019_64 + 通过 vswhere 定位 VS 的 vcvars64.bat
#                      （Ninja/CMake 优先用 VS 自带组件，回退 Tools 目录）

function Get-QtToolchain {
    param(
        [string]$Version = "6.6.3",
        [string]$QtRoot = "",
        [string]$QtToolsRoot = "",
        [ValidateSet("mingw", "msvc")]
        [string]$Compiler = "mingw"
    )

    $kitDir = if ($Compiler -eq "msvc") { "msvc2019_64" } else { "mingw_64" }

    if (-not $QtRoot) {
        if ($env:QT_ROOT -and (Test-Path (Join-Path $env:QT_ROOT "bin\windeployqt.exe"))) {
            $QtRoot = $env:QT_ROOT
        } else {
            foreach ($drive in (Get-PSDrive -PSProvider FileSystem).Root) {
                $candidate = Join-Path $drive "QT\$Version\$kitDir"
                if (Test-Path (Join-Path $candidate "bin\windeployqt.exe")) {
                    $QtRoot = $candidate
                    break
                }
            }
        }
        if (-not $QtRoot) {
            throw "未找到 Qt $Version ($kitDir)：请安装到任意盘的 QT\$Version\$kitDir，" +
                  "或设置 QT_ROOT / 传 -QtRoot 指定"
        }
    }
    $WinDeployQt = Join-Path $QtRoot "bin\windeployqt.exe"
    if (-not (Test-Path -LiteralPath $WinDeployQt)) {
        throw "windeployqt 不存在：$WinDeployQt（检查 -QtRoot / QT_ROOT 是否指向 $kitDir 目录）"
    }

    $toolsBase = Split-Path -Parent (Split-Path -Parent $QtRoot)
    if (-not $QtToolsRoot -and (Test-Path (Join-Path $toolsBase "Tools"))) {
        $QtToolsRoot = Join-Path $toolsBase "Tools"
    }

    if ($Compiler -eq "msvc") {
        # 通过 vswhere 定位最新 VS / Build Tools（含 C++ 工具集），取 vcvars64.bat。
        $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
        if (-not (Test-Path -LiteralPath $vswhere)) {
            throw "未找到 vswhere：请安装 Visual Studio 2022 Build Tools（C++ 工作负载）"
        }
        $vsPath = & $vswhere -latest -products * `
            -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
            -property installationPath
        if (-not $vsPath) {
            throw "未找到含 C++ 工具集 (v142/v143) 的 Visual Studio：`n" +
                  "winget install Microsoft.VisualStudio.2022.BuildTools"
        }
        $vcvars64 = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"
        if (-not (Test-Path -LiteralPath $vcvars64)) {
            throw "vcvars64.bat 不存在：$vcvars64"
        }
        # Ninja/CMake：VS 自带（Microsoft.VisualStudio.Component.VC.Tools 内置）优先。
        $vsNinja = Get-ChildItem (Join-Path $vsPath "Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe") -ErrorAction SilentlyContinue
        $vsCmake = Get-ChildItem (Join-Path $vsPath "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe") -ErrorAction SilentlyContinue
        $fallbackNinja = Join-Path $QtToolsRoot "Ninja\ninja.exe"
        $fallbackCmake = Join-Path $QtToolsRoot "CMake_64\bin\cmake.exe"
        return @{
            Compiler    = "msvc"
            QtRoot      = $QtRoot
            VcVars64    = $vcvars64
            NinjaExe    = if ($vsNinja) { $vsNinja.FullName } elseif (Test-Path $fallbackNinja) { $fallbackNinja } else { "" }
            CmakeExe    = if ($vsCmake) { $vsCmake.FullName } elseif (Test-Path $fallbackCmake) { $fallbackCmake } else { "" }
            WinDeployQt = $WinDeployQt
        }
    }

    # ---- MinGW 分支（原有逻辑）----
    if (-not $QtToolsRoot) {
        $QtToolsRoot = Join-Path $toolsBase "Tools"
    }
    $mingwMatch = Get-Item (Join-Path $QtToolsRoot "mingw*_64\bin") -ErrorAction SilentlyContinue |
        Sort-Object FullName | Select-Object -First 1
    if (-not $mingwMatch) {
        throw "未找到 MinGW 工具链：$QtToolsRoot\mingw*_64\bin"
    }

    return @{
        Compiler    = "mingw"
        QtRoot      = $QtRoot
        QtToolsRoot = $QtToolsRoot
        MingwBin    = $mingwMatch.FullName
        NinjaExe    = Join-Path $QtToolsRoot "Ninja\ninja.exe"
        CmakeExe    = Join-Path $QtToolsRoot "CMake_64\bin\cmake.exe"
        WinDeployQt = $WinDeployQt
        VcVars64    = ""
    }
}
