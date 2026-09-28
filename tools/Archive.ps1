$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$package = Join-Path $root 'DK2VR_V0.1.0'
$output = Join-Path $root 'Livraison'
New-Item -ItemType Directory -Path $output -Force | Out-Null
Add-Type -AssemblyName System.IO.Compression.FileSystem
function Make-Archive([string]$name, [string]$base, [string[]]$paths) {
    $destination = Join-Path $output $name
    # Only replace this generated archive, never game files.
    if (Test-Path -LiteralPath $destination) { Remove-Item -LiteralPath $destination -Force }
    $zip = [IO.Compression.ZipFile]::Open($destination,[IO.Compression.ZipArchiveMode]::Create)
    try {
        foreach ($relative in $paths) {
            $path = Join-Path $base $relative
            $files = if (Test-Path -LiteralPath $path -PathType Container) { Get-ChildItem -LiteralPath $path -File -Recurse } else { Get-Item -LiteralPath $path }
            foreach ($file in $files) {
                $entry = $file.FullName.Substring($base.Length+1).Replace('\','/')
                [IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip,$file.FullName,$entry,[IO.Compression.CompressionLevel]::Optimal) | Out-Null
            }
        }
    } finally { $zip.Dispose() }
    Get-FileHash -LiteralPath $destination -Algorithm SHA256 | Select-Object Path,Hash
}
Make-Archive 'DK2VR_V0.1.0_Mod.zip' $package @('DK2VRHost.exe','openxr_loader.dll','DK2VR.ini','LIRE_MOI_FR.md','VALIDATION.md','Prepare-Game.ps1','LANCER_DK2_VR.cmd','PREPARER_JEU_VR.cmd','DIAGNOSTIC_VR.cmd','payload','Licences')
$sourcePaths = @('CMakeLists.txt','README.md','src','tests','package')
$sourcePaths += Get-ChildItem -LiteralPath (Join-Path $root 'tools') -File | ForEach-Object { 'tools\' + $_.Name }
Make-Archive 'DK2VR_V0.1.0_Sources.zip' $root $sourcePaths
Write-Host 'Archives terminees. Aucun fichier du jeu commercial inclus.'
