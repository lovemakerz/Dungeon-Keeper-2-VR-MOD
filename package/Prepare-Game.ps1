# DK2VR 0.6.9.14 - prepare (first launch) or update (every launch) the VR copy of the game.
param([string]$SourceGame = (Join-Path $PSScriptRoot '..\Dungeon Keeper 2'))
$ErrorActionPreference = 'Stop'
$source = [IO.Path]::GetFullPath($SourceGame)
$target = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'Game'))
if ($source -eq $target -or $source.StartsWith($target + '\', [StringComparison]::OrdinalIgnoreCase)) {
    throw 'Le repertoire original doit etre distinct de la copie VR.'
}
$executable = Join-Path $source 'DKII-DX.exe'
if (!(Test-Path -LiteralPath $executable)) { throw "Jeu introuvable : $source" }
$expected = 'A5957E8C52B6DEE80DDF6CC1D3EA5079AAAAD4D11B40E0F067D34FF4157CE9BB'
$actual = (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
if ($actual -ne $expected) { throw 'Cette version experimentale demande le DKII-DX.exe 1.70 verifie. Le jeu a une autre empreinte : aucune copie modifiee.' }
# 0.6.9.3: the ddraw.dll wrapper (dxwrapper fork) is not rebuilt in the cloud; when missing it is taken from the
# most recent previous DK2VR_V* folder next to this one (read only, the previous version is left intact).
$wrapper = Join-Path $PSScriptRoot 'payload\ddraw.dll'
if (!(Test-Path -LiteralPath $wrapper)) {
    $parentDir = Split-Path -Parent $PSScriptRoot
    $donor = Get-ChildItem -LiteralPath $parentDir -Directory -Filter 'DK2VR_V*' -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -ne $PSScriptRoot -and (Test-Path -LiteralPath (Join-Path $_.FullName 'payload\ddraw.dll')) } |
        Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($donor) {
        Copy-Item -LiteralPath (Join-Path $donor.FullName 'payload\ddraw.dll') -Destination $wrapper -Force
        Write-Host ('Composant ddraw.dll repris depuis ' + $donor.Name)
    } else { throw 'Composant manquant : ddraw.dll. Placez ce dossier a cote de votre dossier DK2VR_V0.6.9.2.' }
}
foreach ($name in 'ddraw.dll','DK2VR.dll','ddraw.ini') {
    if (!(Test-Path -LiteralPath (Join-Path $PSScriptRoot "payload\$name"))) { throw "Composant manquant : $name" }
}
Get-Process -Name 'DKII-DX','DKII','DK2VRHost' -ErrorAction SilentlyContinue | ForEach-Object {
    if ($_.Path -and $_.Path.StartsWith($PSScriptRoot + '\',[StringComparison]::OrdinalIgnoreCase)) { throw 'Fermez la copie VR du jeu avant de la mettre a jour.' }
}
$marker = Join-Path $target 'DK2VR-copy.json'
if (!(Test-Path -LiteralPath $marker)) {
    if (Test-Path -LiteralPath $target) {
        if ((Get-ChildItem -LiteralPath $target -Force | Measure-Object).Count -gt 0) {
            throw 'Le dossier Game existe sans marqueur DK2VR. Renommez-le avant de preparer une nouvelle copie.'
        }
    }
    New-Item -ItemType Directory -Path $target -Force | Out-Null
    Write-Host 'Preparation de la copie VR du jeu (environ 500 Mo, une seule fois)...'
    Get-ChildItem -LiteralPath $source -Force | ForEach-Object { Copy-Item -LiteralPath $_.FullName -Destination $target -Recurse -Force }
    # Reprend les sauvegardes faites dans la version VR precedente (la plus recente), sans toucher a l'original.
    $parent = Split-Path -Parent $PSScriptRoot
    $previous = Get-ChildItem -LiteralPath $parent -Directory -Filter 'DK2VR_V*' -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -ne $PSScriptRoot -and (Test-Path -LiteralPath (Join-Path $_.FullName 'Game\Data\Save')) } |
        Sort-Object LastWriteTime -Descending | Select-Object -First 1
    if ($previous) {
        $saves = Join-Path $previous.FullName 'Game\Data\Save'
        Copy-Item -Path (Join-Path $saves '*') -Destination (Join-Path $target 'Data\Save') -Recurse -Force
        Write-Host ('Sauvegardes reprises depuis ' + $previous.Name)
    }
    [ordered]@{Version='0.6.9.14';Original=$source;ExecutableSHA256=$actual;Created=(Get-Date).ToString('o')} | ConvertTo-Json | Set-Content -LiteralPath $marker -Encoding UTF8
}
foreach ($name in 'ddraw.dll','DK2VR.dll','ddraw.ini') {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot "payload\$name") -Destination (Join-Path $target $name) -Force
}
# Options of the wrapper controlled from DK2VR.ini (applied to the copied ddraw.ini only).
$settings = Join-Path $PSScriptRoot 'DK2VR.ini'
$wrapperIni = Join-Path $target 'ddraw.ini'
if ((Test-Path -LiteralPath $settings) -and (Test-Path -LiteralPath $wrapperIni)) {
    $shadow = '1'
    foreach ($line in Get-Content -LiteralPath $settings) { if ($line -match '^\s*ShadowSurface\s*=\s*([01])') { $shadow = $Matches[1] } }
    $content = Get-Content -LiteralPath $wrapperIni
    $content = $content -replace '^DdrawUseShadowSurface=.*$', ('DdrawUseShadowSurface=' + $shadow)
    Set-Content -LiteralPath $wrapperIni -Value $content -Encoding ASCII
    Write-Host ('Surface miroir : ' + $shadow)
}
Write-Host "Copie VR prete : $target"
Write-Host 'Les fichiers du jeu original sont conserves.'
