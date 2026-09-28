@echo off
cd /d "%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Prepare-Game.ps1"
if errorlevel 1 (
  echo La preparation a echoue. Consultez le message ci-dessus.
  pause
  exit /b 1
)
echo Preparation terminee.
pause
