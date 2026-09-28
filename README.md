# DK2VR — sources du prototype 0.1.0

Le dossier `DK2VR_V0.1.0` contient le programme prêt à lancer et sa copie locale du jeu. Voir `package/LIRE_MOI_FR.md` pour l'utilisation et les limites.

## Architecture

```text
src/common   Protocole mémoire x86/x64 et calcul des projections
src/plugin   DLL x86, lecture de la caméra DKII et rendu stéréoscopique D3D9Ex
src/host     Application x64, suivi OpenXR et soumission D3D11 au casque
tests        Tests géométriques et transfert GPU entre deux processus
tools        Compilation, préparation, intégration du wrapper et diagnostic
package      Lanceurs, réglages et documentation distribuée
research     Dépendances à révisions fixes et données de diagnostic
```

Le jeu reste 32 bits. Un processus OpenXR 64 bits utilise le runtime actuel et transmet les poses de chaque œil à la DLL. La DLL intercepte les triangles transformés, inverse la projection native, puis les dessine deux fois avec les poses et champs de vision propres à chaque œil. Les textures partagées D3D9Ex sont lues en D3D11 par le processus OpenXR. Chaque œil possède sa propre profondeur ; trois emplacements d'images avec synchronisation GPU protègent les échanges.

La fenêtre du jeu reçoit toujours son dessin d'origine. Une troisième texture transporte son image complète pour les menus et le panneau d'interface. La sélection de la carte graphique suit le LUID demandé par OpenXR. Aucun service, pilote ou changement permanent de registre n'est installé.

Le correctif du 25 septembre ajoute un rectangle inscrit dans l'empreinte de la caméra sur le sol natif. Quatre plans de découpe D3D9, transformés dans les coordonnées de chaque œil, s'appliquent seulement au plateau. Un socle complète le contour. `TableBoundary` et `TableCrop` sont lus au lancement ; l'échelle et l'inclinaison précédentes sont conservées. La DLL restaure l'état D3D du jeu après les dessins supplémentaires. Ce correctif ne récupère pas les triangles déjà éliminés par le moteur.

`tests/TableTests.cpp` vérifie les nouvelles limites, leurs transformations et une capture réelle de caméra DKII avec l'axe vertical natif -Z. `tests/TableGpuTests.cpp` compare le rendu matériel aux intersections géométriques pour deux poses et deux hauteurs. Ces tests ont réussi ; le résultat visuel du nouveau socle dans une partie réelle reste à valider dans le casque. Voir `package/VALIDATION.md`.

## Compilation complète

Prérequis : Windows x64, Git, Python 3, CMake 3.21 ou plus récent, Visual Studio 2022 Build Tools avec C++ x86/x64 et un Windows SDK. Un GPU compatible D3D9Ex et D3D11 est nécessaire à l'exécution. Le casque utilise un runtime OpenXR avec `XR_KHR_D3D11_enable`.

Depuis PowerShell dans ce dossier :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\Build.ps1
```

Le script récupère les révisions épinglées, applique les modifications reproductibles, compile les deux architectures et prépare le dossier distribuable. `-SkipDependencies` réutilise les dépendances présentes. `-Python "C:\chemin\python.exe"` choisit Python. `-RunTests` lance les tests géométriques et le test GPU après compilation ; ce dernier demande une session Windows avec accès à la carte graphique.

Le script de compilation ne lance pas le jeu. `Prepare-Game.ps1` crée une copie séparée depuis l'installation locale. L'empreinte de l'exécutable prise en charge est `A5957E8C52B6DEE80DDF6CC1D3EA5079AAAAD4D11B40E0F067D34FF4157CE9BB`.

## Points vérifiés dans le moteur

- Version prise en charge : DKII-DX.exe 1.70, image de 0x3B6000 octets à partir de 0x400000.
- Signature de projection vérifiée à 0x58A6F0. Focale : 0x77F430 ; centres : 0x77F4F0 et 0x77F930.
- Le RHW contient l'inverse de la profondeur caméra. Les opérations à 0x58ADC0 confirment le calcul.
- `g_pCBridge` : 0x795700 ; caméra à +0x3E5, mode à +0xDA8. Modes 1/2 utilisés pour la possession.
- `MyResources_instance` : 0x758340 ; GameCfg +0x2A07 ; indicateur du menu 3D +0x168.
- Pour le plateau rectangulaire : la projection native construit une matrice à 0x77F458 ; multiplication des vecteurs à 0x594DB0, axe natif +Z à +0x18. Le haut est -Z (caméra observée à Z=-5,879 unités). Translation globale à 0x780940. Les facteurs 0x77F4CC et 0x78093C sont annulés comme lors de la reconstruction des sommets. La capture réelle figure dans les données de diagnostic locales ; sa normale et sa distance au sol sont couvertes par un test de régression.

Les lectures de caméra sont désactivées lorsque la signature native n'est pas reconnue. Aucun code exécutable du jeu n'est modifié sur disque.

Le correctif de souris intercepte en mémoire `MyMouse::updatePos` à 0x5DD630 après vérification de ses six premiers octets. La position Windows est ramenée dans le rectangle natif avec correction des bandes noires. La structure réelle utilise +0x20/+0x24 pour les coordonnées et +0x34 pour les limites ; certains noms de champs de la cartographie portent encore des offsets plus anciens. `MyInputManagerCb_instance` est à 0x79CF90, et son pointeur souris à +0x64. La possession, Alt et le bouton central conservent les mouvements relatifs.

## Corrections locales du wrapper

`tools/patch_wrapper.py` applique les interceptions de dessin, présentation, réinitialisation et sélection d'adaptateur. Le script conserve le routage des sommets transformés sans activer les conversions Remix. Il désactive les hooks facultatifs des fonctions d'attente, responsables d'un conflit avec un thread Windhawk lors du chargement, et initialise directement les exports du convertisseur DirectDraw lorsque le module système n'est pas chargé.

Les limites de sélection de géométrie par la caméra d'origine sont connues : cette version ne constitue pas encore une caméra native VR à visibilité 360°. Le rendu des effets 2D et la transition en possession nécessitent des essais de jeu complémentaires.

## Dépendances exactes

| Projet | Révision |
|---|---|
| DK2-Remix-Predetermined-Hashing | ed737addf743dbd555a46cb25c4d008ef80422f1 |
| OpenXR-SDK | f2448a8797c85814aa892efc1ab8707900fbcc78 |
| Flame, référence de recherche | f6e2a3d385bf2e6e95dc70cab88aa387bb7659d9 |

Les dépôts et leurs sous-modules conservent leurs licences. Les fichiers de diagnostic du dossier `research` sont locaux et ne sont pas nécessaires au lancement.
