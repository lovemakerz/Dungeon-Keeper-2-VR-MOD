param([string]$Python, [switch]$SkipDependencies, [switch]$RunTests)
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Set-Location -LiteralPath $root
if (!$Python) {
    $bundled = Join-Path $env:USERPROFILE '.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe'
    if (Test-Path -LiteralPath $bundled) { $Python = $bundled }
    else { $Python = (Get-Command python -ErrorAction Stop).Source }
}
$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakeCommand) { $cmake = $cmakeCommand.Source }
else {
    $cmake = Join-Path $env:ProgramFiles 'CMake\bin\cmake.exe'
    if (!(Test-Path -LiteralPath $cmake)) { throw 'Installer CMake et Git avant de compiler.' }
}
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vs) { throw 'Installer Visual Studio 2022 Build Tools avec Developpement Desktop C++ et Windows SDK.' }
$msbuild = Join-Path $vs 'MSBuild\Current\Bin\MSBuild.exe'
function Run-BuildTool {
    & $Python (Join-Path $PSScriptRoot 'run_clean.py') @args
    if ($LASTEXITCODE) { throw "Echec de compilation : $($args -join ' ')" }
}
function Get-PinnedRepo($url, $relative, $commit) {
    $target = Join-Path $root $relative
    if (!(Test-Path -LiteralPath (Join-Path $target '.git'))) {
        New-Item -ItemType Directory -Path $target -Force | Out-Null
        Run-BuildTool git -C $target init --quiet
        Run-BuildTool git -C $target remote add origin $url
        Run-BuildTool git -c http.sslBackend=openssl -C $target fetch --depth 1 origin $commit
        Run-BuildTool git -C $target checkout --detach FETCH_HEAD
    }
    $actual = & git -C $target rev-parse HEAD
    if ($actual -ne $commit) { throw "Revision inattendue dans $relative. Les sources existantes sont conservees." }
}
if (!$SkipDependencies) {
    Get-PinnedRepo 'https://github.com/mencelot/DK2-Remix-Predetermined-Hashing.git' 'research\dk2-dxwrapper' 'ed737addf743dbd555a46cb25c4d008ef80422f1'
    Get-PinnedRepo 'https://github.com/KhronosGroup/OpenXR-SDK.git' 'research\OpenXR-SDK' 'f2448a8797c85814aa892efc1ab8707900fbcc78'
    & (Join-Path $PSScriptRoot 'Fetch-Submodules.ps1')
}
Run-BuildTool $cmake -S research/OpenXR-SDK -B build/openxr -G 'Visual Studio 17 2022' -A x64 "-DCMAKE_GENERATOR_INSTANCE=$vs" -DBUILD_TESTS=OFF -DBUILD_API_LAYERS=OFF -DBUILD_CONFORMANCE_TESTS=OFF -DBUILD_LOADER=ON -DDYNAMIC_LOADER=ON
Run-BuildTool $cmake --build build/openxr --config Release --target openxr_loader --parallel 1
Run-BuildTool $Python tools/patch_wrapper.py
foreach ($arch in @(@('x86','Win32'), @('x64','x64'))) {
    Run-BuildTool $cmake -S . -B "build/$($arch[0])" -G 'Visual Studio 17 2022' -A $arch[1] "-DCMAKE_GENERATOR_INSTANCE=$vs"
    Run-BuildTool $cmake --build "build/$($arch[0])" --config Release --parallel 1
}
Run-BuildTool $msbuild research/dk2-dxwrapper/dxwrapper.sln /t:dxwrapper /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v143 /m:1 /nr:false /verbosity:quiet /nologo
if ($RunTests) {
    Run-BuildTool build/x86/Release/DK2VRTableTests.exe
    Run-BuildTool build/x86/Release/DK2VRTableGpuTests.exe
    Run-BuildTool build/x86/Release/DK2VRMathTests.exe
    Run-BuildTool build/x64/Release/DK2VRMathTests.exe
    Run-BuildTool build/x64/Release/DK2VRGpuTest.exe (Join-Path $root 'build/x86/Release/DK2VRGpuTest.exe')
}
& (Join-Path $PSScriptRoot 'package.ps1')
Write-Host 'Compilation terminee. Executer DK2VR_V0.1.0\LANCER_DK2_VR.cmd.'
