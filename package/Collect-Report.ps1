# DK2VR - collecte des journaux dans un ZIP a envoyer pour le diagnostic.
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$names = 'DK2VR-host.log','DK2VR.ini','Game\DK2VR-game.log','Game\ddraw-dkii-dx.log','Game\DK2VR-copy.json','DK2VR-oeil-gauche.bmp','DK2VR-oeil-droit.bmp','DK2VR-ecran.bmp'
$files = @($names | ForEach-Object { Join-Path $root $_ } | Where-Object { Test-Path -LiteralPath $_ })
$captures = Join-Path $root 'Captures'
if (Test-Path -LiteralPath $captures) {
    # 0.6.9.3 : captures JPG (legeres) -> les 6 plus recentes (3 images chacune). Les anciens PNG sont ignores.
    $files += @(Get-ChildItem -LiteralPath $captures -File -Filter '*.jpg' | Sort-Object LastWriteTime -Descending | Select-Object -First 18 | ForEach-Object { $_.FullName })
}
# 0.6.9.4 : l'executable du jeu (copie VR, jamais modifie sur disque) pour l'analyse du choix des textures et du crash.
$exe = Join-Path $root 'Game\DKII-DX.exe'
if (Test-Path -LiteralPath $exe) { $files += $exe }
if ($files.Count -eq 0) { Write-Host 'Aucun journal trouve. Lancez d''abord le jeu avec LANCER_DK2_VR.cmd.'; exit 1 }
$out = Join-Path $root ('DK2VR_Rapport_' + (Get-Date -Format 'yyyyMMdd_HHmmss') + '.zip')
Compress-Archive -LiteralPath $files -DestinationPath $out -Force
Write-Host "Rapport cree : $out"
Start-Process explorer.exe -ArgumentList "/select,`"$out`""
