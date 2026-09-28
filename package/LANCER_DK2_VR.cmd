@echo off
cd /d "%~dp0"
rem Prepare la copie du jeu la premiere fois, puis met a jour les composants VR a chaque lancement.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Prepare-Game.ps1"
if errorlevel 1 (
  echo.
  echo La preparation a echoue. Lisez le message ci-dessus.
  pause
  exit /b 1
)
echo.
echo Connectez le casque au PC, puis patientez : le jeu va se lancer.
echo F8 ou clic du stick : recentrer. 1 poignee : glisser la carte. 2 poignees : zoom et table. F6 : ecran.
"%~dp0DK2VRHost.exe"
if errorlevel 1 (
  echo.
  echo Un probleme est survenu. Lancez COLLECTER_RAPPORT.cmd et envoyez le fichier cree.
  pause
)
