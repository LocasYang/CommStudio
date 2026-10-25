param(
    [string]$Executable = 'C:\Users\yangfan\AppData\Local\Temp\commstudio-verify-20261024\CommStudio.exe',
    [string]$OutputDirectory = 'F:\mine\code\qt\com_studio\tmp\ui-acceptance-20261024'
)
$ErrorActionPreference = 'Stop'
$env:PATH = 'E:\QT\6.6.3\msvc2019_64\bin;' + $env:PATH
$env:QT_QPA_PLATFORM = 'offscreen'
$env:QT_QPA_FONTDIR = 'C:\Windows\Fonts'
Add-Type -AssemblyName System.Drawing
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$script:Results = [System.Collections.Generic.List[object]]::new()

function Assert-Ui([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}
function Invoke-UiCase([string]$Name, [string]$Size, [hashtable]$Flags = @{}, [string]$Profile = '', [bool]$Import = $true, [bool]$Popup = $false, [int]$Page = 1) {
    Get-ChildItem Env: | Where-Object { $_.Name -like 'COMMSTUDIO_SCREENSHOT*' } | ForEach-Object { Remove-Item -LiteralPath ('Env:' + $_.Name) }
    if (-not $Profile) { $Profile = Join-Path ([IO.Path]::GetTempPath()) ('commstudio-ui-check-' + [guid]::NewGuid().ToString('N')) }
    $env:COMMSTUDIO_PROFILE_DIR = $Profile
    $env:COMMSTUDIO_SCREENSHOT = Join-Path $OutputDirectory ($Name + '.png')
    $env:COMMSTUDIO_SCREENSHOT_METRICS = '1'
    $env:COMMSTUDIO_SCREENSHOT_SIZE = $Size
    $env:COMMSTUDIO_SCREENSHOT_PAGE = [string]$Page
    $env:COMMSTUDIO_MSG_LOG = Join-Path $OutputDirectory ($Name + '.log')
    Set-Content -LiteralPath $env:COMMSTUDIO_MSG_LOG -Value ''
    if ($Import) { $env:COMMSTUDIO_SCREENSHOT_TEMPLATE = 'F:\mine\code\qt\com_studio\tests\data\modbus-device.dtmthings' }
    foreach ($key in $Flags.Keys) { Set-Item -LiteralPath ('Env:COMMSTUDIO_SCREENSHOT_' + $key) -Value $Flags[$key] }
    $process = Start-Process -FilePath $Executable -WindowStyle Hidden -Wait -PassThru
    Assert-Ui ($process.ExitCode -eq 0) "$Name failed to start ($($process.ExitCode))"
    $log = Get-Content -LiteralPath $env:COMMSTUDIO_MSG_LOG -Raw
    Assert-Ui (-not ($log -match 'WARN:|CRITICAL:|FATAL:')) "$Name runtime diagnostics: $log"
    $metrics = Get-Content -LiteralPath ($env:COMMSTUDIO_SCREENSHOT + '.json') -Raw | ConvertFrom-Json
    Assert-Ui (-not $metrics.channelOpen -and -not $metrics.polling) "$Name unexpectedly opened hardware or started polling"
    if ($Page -eq 1) {
        Assert-Ui ($metrics.points -eq 3) "$Name altered the fixture point definitions"
        $tree = $metrics.modbusTreePanel
        $panel = $metrics.modbusPointPanel
        $tools = $metrics.modbusPointToolbar
        $last = $metrics.modbusPointDetailsToggle
        Assert-Ui ([math]::Abs($panel.x - $tree.x - $tree.width - 14) -le 1) "${Name}: panel gutter lost alignment"
        Assert-Ui ($last.x -ge $tools.x -and $last.x + $last.width -le $tools.x + $tools.width + 1) "${Name}: last toolbar action clipped"
        Assert-Ui ($metrics.modbusTablePane.height -gt 140) "${Name}: table collapsed"
        if (-not $Popup) {
            $bitmap = [Drawing.Bitmap]::new($env:COMMSTUDIO_SCREENSHOT)
            try {
                $scale = $bitmap.Width / $metrics.width
                $x = [int][math]::Floor(($tree.x + $tree.width + 7) * $scale)
                $expected = if ($Flags.ContainsKey('LIGHT')) { '#eef3f7' } else { '#0b1018' }
                $wrong = 0
                for ($y = [int][math]::Ceiling($tree.y * $scale); $y -lt [int][math]::Floor(($tree.y + $tree.height) * $scale); $y++) {
                    $color = $bitmap.GetPixel($x, $y)
                    $rgb = '#{0:x2}{1:x2}{2:x2}' -f $color.R, $color.G, $color.B
                    if ($rgb -ne $expected) { $wrong++ }
                }
                Assert-Ui ($wrong -eq 0) "${Name}: $wrong pixels cross the clear panel gutter"
            } finally { $bitmap.Dispose() }
        }
    }
    $script:Results.Add([pscustomobject]@{ name=$Name; width=$metrics.width; height=$metrics.height; navigationWidth=$metrics.navigationRail.width; navigationCollapsed=$metrics.navigationCollapsed; passed=$true })
    Write-Host "PASS $Name"
    return @{ metrics=$metrics; profile=$Profile }
}

$wide = Invoke-UiCase 'dark-collapsed' '1840x1120'
Assert-Ui ($wide.metrics.navigationRail.width -eq 72) 'Collapsed navigation width changed'
$min = Invoke-UiCase 'dark-expanded-minimum' '1100x700' @{NAV_EXPANDED='1'}
Assert-Ui ($min.metrics.navigationRail.width -eq 210) 'Expanded navigation width changed'
$null = Invoke-UiCase 'light-collapsed' '1460x900' @{LIGHT='1'}
$null = Invoke-UiCase 'tree-details-minimum' '1100x700' @{NAV_EXPANDED='1'; TREE_DETAILS='1'}
$hidden = Invoke-UiCase 'traffic-hidden-minimum' '1100x700' @{NAV_EXPANDED='1'; TRAFFIC_HIDDEN='1'}
Assert-Ui (-not $hidden.metrics.modbusTrafficDock.visible) 'Traffic dock did not hide'
Assert-Ui ($hidden.metrics.modbusTablePane.height -gt $min.metrics.modbusTablePane.height + 150) 'Hidden dock did not return space to the table'
$null = Invoke-UiCase 'traffic-expanded' '1100x700' @{TRAFFIC_EXPANDED='1'} -Popup $true
$null = Invoke-UiCase 'statistics' '1100x700' @{STATISTICS='1'} -Popup $true
$null = Invoke-UiCase 'nested-channel-configuration' '1100x700' @{CHANNEL_CONFIG='1'} -Popup $true
$null = Invoke-UiCase 'advanced-parameters' '1100x700' @{DEVICE_ADVANCED='1'} -Popup $true
$toggled = Invoke-UiCase 'navigation-toggle' '1100x700' @{NAV_TOGGLE='1'}
Assert-Ui (-not $toggled.metrics.navigationCollapsed) 'Navigation toggle did not expand'
$restored = Invoke-UiCase 'navigation-restored' '1100x700' @{} -Profile $toggled.profile -Import $false
Assert-Ui (-not $restored.metrics.navigationCollapsed -and $restored.metrics.navigationRail.width -eq 210) 'Navigation state did not survive restart'
$null = Invoke-UiCase 'serial-theme' '1100x700' @{} -Page 0
$script:Results | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $OutputDirectory 'results.json')
Write-Output "PASS: $($script:Results.Count) UI scenarios; all profiles isolated; no hardware connection."
