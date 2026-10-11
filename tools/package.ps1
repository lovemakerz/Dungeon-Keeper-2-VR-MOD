$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$out = Join-Path $root 'DK2VR_V0.1.0'
New-Item -ItemType Directory -Path $out,(Join-Path $out 'payload'),(Join-Path $out 'Licences') -Force | Out-Null
Get-ChildItem -LiteralPath (Join-Path $root 'package') -Force | ForEach-Object { Copy-Item -LiteralPath $_.FullName -Destination $out -Recurse -Force }
Copy-Item -LiteralPath (Join-Path $root 'build\x64\Release\DK2VRHost.exe') -Destination $out -Force
Copy-Item -LiteralPath (Join-Path $root 'build\openxr\src\loader\Release\openxr_loader.dll') -Destination $out -Force
Copy-Item -LiteralPath (Join-Path $root 'build\x86\Release\DK2VR.dll') -Destination (Join-Path $out 'payload') -Force
Copy-Item -LiteralPath (Join-Path $root 'research\dk2-dxwrapper\bin\Release\dxwrapper.dll') -Destination (Join-Path $out 'payload\ddraw.dll') -Force
Copy-Item -LiteralPath (Join-Path $root 'research\dk2-dxwrapper\License.txt') -Destination (Join-Path $out 'Licences\dxwrapper.txt') -Force
Copy-Item -LiteralPath (Join-Path $root 'research\OpenXR-SDK\LICENSE') -Destination (Join-Path $out 'Licences\OpenXR-SDK.txt') -Force
Copy-Item -LiteralPath (Join-Path $root 'research\OpenXR-SDK\src\external\jsoncpp\LICENSE') -Destination (Join-Path $out 'Licences\jsoncpp.txt') -Force
$notices = @{
    'DDrawCompat-0.3.2.txt' = 'DDrawCompat\v0.3.2\LICENSE.txt'
    'DDrawCompat-0.2.1.txt' = 'DDrawCompat\v0.2.1\License.txt'
    'DDrawCompat-0.2.0b.txt' = 'DDrawCompat\v0.2.0b\License.txt'
    'MemoryModule.txt' = 'External\MemoryModule\LICENSE.txt'
    'DirectXMath.txt' = 'External\DirectXMath\LICENSE'
    'detours.txt' = 'External\detours\LICENSE.md'
    'imgui.txt' = 'External\imgui\LICENSE.txt'
    'dinputto8.txt' = 'External\dinputto8\License.txt'
    'Hooking.txt' = 'External\Hooking\License.txt'
    'Hooking-Patterns.txt' = 'External\Hooking.Patterns\LICENSE.md'
    'Logging.txt' = 'External\Logging\License.txt'
    'd3d8to9.txt' = 'External\d3d8to9\LICENSE.md'
}
foreach ($name in $notices.Keys) {
    Copy-Item -LiteralPath (Join-Path $root ('research\dk2-dxwrapper\' + $notices[$name])) -Destination (Join-Path $out ('Licences\' + $name)) -Force
}
Write-Output $out
