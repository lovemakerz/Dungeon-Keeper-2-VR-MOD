@echo off
cd /d "%~dp0"
"%~dp0DK2VRHost.exe" --probe
echo.
echo Le resultat est aussi dans DK2VR-host.log.
pause
