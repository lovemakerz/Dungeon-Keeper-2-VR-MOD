$ErrorActionPreference = 'Stop'
$root = Join-Path $PSScriptRoot '..\research\dk2-dxwrapper'
$lines = & git -C $root config -f .gitmodules --get-regexp '^submodule\..*\.path$'
foreach ($line in $lines) {
    $parts = $line -split ' ', 2
    $key = $parts[0] -replace '\.path$', '.url'
    $relative = $parts[1]
    if ($relative -eq 'DirectX') { continue }
    $url = & git -C $root config -f .gitmodules --get $key
    $entry = & git -C $root ls-tree HEAD -- $relative
    $commit = ($entry -split '\s+')[2]
    $destination = Join-Path $root $relative
    if (!(Test-Path (Join-Path $destination '.git'))) {
        New-Item -ItemType Directory -Path $destination -Force | Out-Null
        & git -C $destination init --quiet
        if ($LASTEXITCODE) { throw "git init failed: $relative" }
        & git -C $destination remote add origin $url
    }
    $current = & git -C $destination rev-parse HEAD 2>$null
    if ($current -eq $commit) { Write-Output "$relative already at $commit"; continue }
    & git -c http.sslBackend=openssl -C $destination fetch --depth 1 origin $commit
    if ($LASTEXITCODE) { throw "git fetch failed: $relative" }
    & git -C $destination checkout --quiet --detach FETCH_HEAD
    if ($LASTEXITCODE) { throw "git checkout failed: $relative" }
    Write-Output "$relative $commit"
}
