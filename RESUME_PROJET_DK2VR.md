# DK2VR — Mod VR from-scratch pour Dungeon Keeper II — Prompt de reprise

Ce document est un résumé complet destiné à être collé en premier message d'une nouvelle conversation Claude, pour reprendre ce projet sans perte de contexte. Il inclut aussi ce qu'il faut pour publier le dépôt sur GitHub.

---

## 0. Qui je suis (utilisateur) et comment travailler avec moi

**Je suis un utilisateur NOCODE.** Je ne lis pas de code, je ne l'écris pas, je ne fais pas de diagnostic technique. Mon rôle se limite à : télécharger, lancer, tester dans le jeu/casque, et te décrire ce que j'observe.

Règles à respecter impérativement (persona déjà configurée côté Claude, mais à rappeler) :
- **Jamais** me demander d'écrire/lire/modifier du code, de chercher une ligne, d'appliquer un patch, d'utiliser Git à la main, de choisir une architecture/bibliothèque/méthode de hook, ou de diagnostiquer un problème.
- Quand un choix technique est nécessaire : **décide toi-même**, priorité **fiabilité > préservation de l'existant > compatibilité > simplicité > maintenance > performance**.
- **Toujours livrer un résultat testable** (DLL/EXE compilés, zip prêt), jamais un tutoriel ou une explication de ce que je pourrais faire moi-même.
- Si du code apparaît dans une réponse : fichiers complets uniquement, jamais d'extraits à fusionner (`...`, "reste inchangé", etc. interdits).
- **Anti-régression stricte** : une base déclarée "stable" ou "validée" devient la référence ; ne pas casser une fonction déjà validée pour corriger autre chose sans rapport.
- **Versioning strict** `VX.Y.Z[.W]` incrémental, jamais réutiliser un numéro pour deux builds différents.
- **Format de livraison** systématique en fin de tâche :
  - **VERSION** / **BASE** / **CHANGEMENTS** / **PRÉSERVÉ** / **LIVRABLE** / **TEST** / **RÉSULTAT ATTENDU** / **À ME RETOURNER**
- Toujours inspecter les fichiers réels (sources, logs, captures, l'exécutable du jeu) avant de modifier quoi que ce soit ; ne jamais deviner la structure d'un projet ou le contenu d'un fichier non inspecté ; distinguer explicitement ce qui est vérifié de ce qui est déduit/probable.
- Avant une modification potentiellement destructive sur les fichiers du jeu original : sauvegarde ou solution réversible (le jeu original n'est jamais modifié directement — c'est une copie qui est patchée).
- Toutes les réponses techniques, journaux, commentaires de code et documentation utilisateur sont **en français**.
- **RÈGLE CRITIQUE DE CONTINUITÉ** : chaque session cloud est éphémère et n'a AUCUNE mémoire des sessions précédentes. Un fichier `CONTINUITE_MAITRE.md` (à la racine du dépôt / du dossier projet) est le SEUL fil de continuité — il doit être lu en premier à chaque reprise, et mis à jour après chaque version livrée. De même, **chaque livraison doit déposer une archive complète des sources** (`Sources.zip`), car l'espace de travail d'une session cloud est perdu à la fin de la session.

---

## 1. Objectif global du projet

**DK2VR** est un mod VR entièrement reverse-engineered pour **Dungeon Keeper II** (version DKII-DX.exe 1.70, jeu Windows de 1999, moteur DirectX 8/9 32 bits), sans aucun accès au code source du jeu original.

Le but : jouer à Dungeon Keeper II en réalité virtuelle avec le donjon posé sur **une table physique fixe devant le joueur, façon Demeo** — on regarde le plateau de dessus, on interagit avec les mains/contrôleurs VR, l'écran 2D du jeu (son interface complète : barres, panneaux, menus) est reconstitué et plaqué sur des panneaux virtuels autour de la table plutôt que projeté à plat. Le jeu lui-même continue de tourner normalement en arrière-plan (souris/clavier réels), le mod ne fait qu'intercepter son rendu et injecter une couche VR.

Casque cible : **Meta Quest 3**, connecté via **Virtual Desktop (runtime OpenXR VDXR)** ou **SteamVR** comme runtime OpenXR alternatif.

---

## 2. Architecture technique (contexte indispensable)

- **`DK2VR.dll`** — DLL **32 bits** injectée dans `DKII-DX.exe` via un fork de `dxwrapper`/`ddraw.dll` (DDrawCompat). Intercepte les appels Direct3D9 du jeu (`DrawPrimitiveUP`/`DrawIndexedPrimitiveUP`, format `XYZRHW` = sommets déjà projetés en 2D par le jeu), reconstruit les positions 3D monde par **projection inverse** (`unproject()`), et fait tourner un **second périphérique D3D9Ex** sur un **thread dédié découplé** (rendu à la fréquence du casque, indépendant du framerate du jeu qui peut varier 20-100 i/s).
- **`DK2VRHost.exe`** — hôte **64 bits**, **OpenXR** (extension D3D11), consomme les textures partagées produites par la DLL 32 bits (copies GPU cross-bitness via textures partagées Windows) et les soumet au casque.
- **Communication** DLL↔Hôte : mémoire partagée IPC (`src/common/Ipc.hpp`, struct `Request`/`Shared`, `ProtocolVersion` incrémenté à chaque changement de layout).
- **Reverse engineering** de l'exécutable du jeu : adresses mémoire absolues (base image `0x400000`), signatures d'octets vérifiées avant tout patch/hook (jamais de hook sans vérification de signature — le jeu doit rester patché uniquement s'il correspond exactement à la version 1.70 attendue, SHA256 `A5957E8C52B6DEE80DDF6CC1D3EA5079AAAAD4D11B40E0F067D34FF4157CE9BB`). Outils de désassemblage : scripts Python `capstone`+`pefile` faits sur mesure (pas d'IDA/Ghidra disponible dans l'environnement cloud).
- **Toolchain de build** (cross-compilation depuis Linux, établie et stable) :
  - `mingw-w64` : `i686-w64-mingw32-g++-posix` (DLL 32 bits) et `x86_64-w64-mingw32-g++-posix` (hôte 64 bits), installés via `apt`.
  - **OpenXR-SDK** : cloné depuis `https://github.com/KhronosGroup/OpenXR-SDK.git`, **épinglé au commit `f2448a8797c85814aa892efc1ab8707900fbcc78`** (à recloner à chaque nouvelle session cloud, dans `deps/OpenXR-SDK` à la racine du projet — n'est PAS versionné dans le dépôt).
  - Build : `OPENXR=<chemin>/deps/OpenXR-SDK sh tools/build_mingw.sh` depuis la racine du projet. Ce script compile aussi et **exécute automatiquement 5 binaires de tests natifs** qui doivent tous rapporter PASS/OK avant toute livraison : `MathTests`, `TableTests`, `AnchorTests`, `GrabTests`, `DecoupleTests`.
- **Structure du dépôt** :
  ```
  src/
    plugin/      — DLL 32 bits (Plugin.cpp ~2600 lignes, Decoupled.inl, Environment.inl, TileCover...)
    host/        — Hôte 64 bits OpenXR (Host.cpp)
    common/      — Partagé DLL+Hôte : Ipc.hpp, Math.hpp, Grab.hpp, TableModel.hpp, HudLayout.hpp, Protocol.hpp
  tests/         — 5 suites de tests natifs (MathTests, TableTests, AnchorTests, GrabTests, DecoupleTests) + linux_stub
  tools/         — build_mingw.sh, scripts Windows (Build.ps1, package.ps1, Archive.ps1...), scripts Python RE
  package/       — Tout ce qui est livré à l'utilisateur : DK2VR.ini, Prepare-Game.ps1, LANCER_DK2_VR.cmd,
                   COLLECTER_RAPPORT.cmd/Collect-Report.ps1 (diagnostic), LIRE_MOI_FR.md, payload/ (DK2VR.dll + ddraw.dll fork + ddraw.ini)
  ```
- **Déploiement local (PC utilisateur, Windows)** : dossier `D:\Claude\DK2\` avec un sous-dossier par version (`DK2VR_V0.X.Y`), un dossier `Livraison\` recevant `DK2VR_V<version>_Sources.zip` à chaque livraison, et `CONTINUITE_MAITRE.md` à la racine (copié aussi dans chaque dossier de version et dans le Projet Claude). Le jeu original intact est dans `D:\Claude\DK2\Dungeon Keeper 2\`. `Prepare-Game.ps1` crée une COPIE du jeu dans un sous-dossier `Game\` au premier lancement — le jeu original n'est jamais touché.
- **Diagnostic** : l'utilisateur peut lancer `COLLECTER_RAPPORT.cmd` qui produit un zip contenant `DK2VR-game.log`, `DK2VR-host.log`, `ddraw-dkii-dx.log`, `DK2VR.ini`, `DK2VR-copy.json`, et des triplets de captures numérotées (écran PC + œil gauche + œil droit VR). Aux sessions cloud suivantes, dézipper, réduire les captures VR (souvent >4000×4000 px) avant de les visionner, et grep le log pour les lignes clés (`Timing:`, `TableView`, `Table picking:`, `Wall screen`, `Table pointer:`, `Game (native counters`, `VR thread`).

---

## 3. Décisions d'architecture déjà prises (et pourquoi)

| Décision | Pourquoi |
|---|---|
| Table fixe façon Demeo (`TableAnchor=1`), pas de "flotter" libre | Confort VR, lisibilité, demande explicite dès le début |
| Rendu découplé (thread VR dédié + 2e device D3D9Ex) depuis 0.3.0 | Le jeu tourne à 20-100 i/s variable ; sans découplage le casque était perçu "60 Hz" / saccadé |
| Caméra native du jeu passée en **vue de dessus** (`TopDownCamera`, hook sur `0x449F50`/`0x44D6D0`) | Rend le rectangle de rendu du moteur quasi identique au rectangle de la table (moins de perte, meilleures perfs) — remplace définitivement l'ancienne approche "TileCover : cases du moteur = table" (abandonnée, voir §5) |
| Curseur du Gardien = curseur logiciel du jeu lui-même (hook `CursorDrawer::setCursorImage` 0x5DC120) plutôt que détection 2D/3D des sommets | Les détections précédentes (sommets 2D, sommets 3D à profondeur variable) donnaient trop de faux positifs |
| Objets tenus par la main = fenêtre recadrée de la texture d'interface autour du curseur (pas de classification par draw) | Le cache de textures D3D du jeu partage ses blocs entre UI et monde 3D → toute classification basée sur l'identité de texture ou sur la profondeur produit des faux positifs (LEÇON définitive, voir §5) |
| Pointage table = pick géométrique (Möller-Trumbore sur triangles reconstruits) plutôt qu'intersection avec un plan plat | Un plan plat au niveau du sol donnait un pointage juste uniquement pour les créatures (au niveau du sol), faux partout ailleurs (creuser, construire sur des blocs en hauteur) |
| Écran d'options/menus : **jamais projeté sur le mur du fond** — empilé avec les autres panneaux HUD côté joueur, sous la barre de vie | Décision utilisateur explicite et définitive (« On arrête avec l'écran d'option projeté sur le mur ») après plusieurs itérations de détection ratée sur le mur |
| Runtime OpenXR : préférer automatiquement Virtual Desktop (VDXR) à SteamVR quand disponible | Mesuré dans les journaux : 90 images/s sous VDXR contre 64 images/s sous SteamVR à charge de jeu identique |
| Mains contrôleurs = halo lumineux "lueur magique" discret (visible seulement sous le niveau des murs du donjon), plus de bâtons colorés | Demande esthétique explicite de l'utilisateur |

---

## 4. Ce qui a été fait / livré jusqu'à présent

Historique complet dans `CONTINUITE_MAITRE.md` (0.1.0 → 0.6.9.2). Points marquants, du plus ancien au plus récent :

- **0.1.0–0.2.9** : socle du mod, table Demeo statique, niveau du socle (sol de marche calculé par histogramme des hauteurs), fluidité de base. 0.2.9 = première **base stable** validée.
- **0.3.0** : rendu découplé (2e device D3D9Ex, thread VR dédié à la fréquence du casque) — gain de fluidité majeur, validé casque.
- **0.3.1–0.3.2** : table physique fixe avec poignées de saisie, formes/aspect de la table.
- **0.4.x** : salle de donjon voûtée + table en bois procédurales (`Environment.inl`), éclairage par sommet, sol réel via espace OpenXR STAGE.
- **0.5.x** : longue itération sur la **vue table** (quelle portion du donjon le moteur du jeu doit réellement dessiner pour remplir la table sans trous ni bandes vides) — nombreux essais/échecs documentés (voir §5), jusqu'à **0.5.16** : rectangle moteur = projection exacte des coins de la table (méthode fiable, remplace toutes les heuristiques précédentes).
- **0.6.0–0.6.4** : refonte complète de l'UI VR — panneaux HUD flottants reproduisant l'interface 2D du jeu (barres, minicarte, panneau créatures/sorts), détection et affichage du curseur du Gardien.
- **0.6.5–0.6.6.1** : détection des objets tenus en main (créature/or/sort), pointage laser sur la table piloté par la position souris native du jeu.
- **0.6.7** : **pick géométrique** (Möller-Trumbore) remplaçant le plan plat — pointage enfin précis partout sur le donjon. Empilement des 3 barres HUD (mêmes largeur, +15%).
- **0.6.7.1–0.6.8** : affinage détection écran mural (options/menus), taille de la créature tenue, piédestal de la minicarte (couleur rouille, bouton bascule côté, ordre de dessin corrigé pour ne plus être vu à travers la table).
- **0.6.9** : tentative de rendre **tout le donjon affiché sélectionnable** (jusque-là une zone morte entourait le centre de la table) via un hack élargissant le champ de vision natif du jeu (`LensHack`, hook sur `0x58A6F0`) — **a provoqué une régression sévère** (trous aux bords du donjon, latence, mauvaises perfs) car seule la partie "dessin" utilisait la focale réduite, pas la partie "sélection des cases" du moteur (cause isolée précisément par désassemblage en 0.6.9.2, voir ci-dessous).
- **0.6.9.1** : correctif d'urgence — `LensHack` désactivée par défaut (retour au comportement fiable 0.6.8, avec zone morte qui revient), abandon définitif de l'écran mural pour le menu options (empilé côté joueur à la place).
- **0.6.9.2 (DERNIÈRE VERSION LIVRÉE, NON ENCORE TESTÉE AU CASQUE)** :
  - **Vraie cause des trous de la 0.6.9 trouvée par désassemblage** : la fonction du moteur `0x575A00` copie la focale de la caméra à la fois vers `0x760AE8` (focale utilisée par la **sélection des cases visibles**, fonction `0x574820`) et vers la structure passée à `0x58A6F0` (utilisée pour le **dessin/pointage écran**). Le hack de la 0.6.9 ne patchait que `0x58A6F0` : la sélection des cases gardait l'ancienne focale non réduite pendant que le rectangle de zone était réduit → cases découpées ~3× trop étroites → trous en escalier aux bords, confirmé visuellement sur une capture. **Corrigé** : le hook est maintenant posé sur `0x575A00` (signature vérifiée `8B 44 24 04 53 56 57`), qui alimente cohéremment le dessin ET la sélection des cases. `TablePickAll=1` réactivé, `TablePickScale=0.30`.
  - **Cause des mauvaises perfs identifiée dans les journaux** : le runtime OpenXR utilisé était passé de Virtual Desktop (VDXR, 90 i/s) à SteamVR (64 i/s) entre deux sessions de test, sans changement côté mod. Le mod tente maintenant automatiquement Virtual Desktop en premier (recherche dans le registre `HKLM\SOFTWARE\Khronos\OpenXR\1\AvailableRuntimes`) avec repli automatique sur le runtime système si échec. Réglage `OpenXRRuntime=auto|system` dans `DK2VR.ini`.
  - **Écran d'options réorganisé** : empilé côté joueur, ordre de haut en bas = barre de vie / écran options complet / barre du temps / panneau créatures, écran options collé sans espace à la barre du temps, barre de vie remontée au-dessus.
  - Build mingw propre, 5 tests natifs PASS/OK. **Pas encore validé par un test réel au casque** — c'est la toute prochaine étape.

---

## 5. Erreurs et pistes déjà écartées (ne pas répéter)

- **Filtrage par identité de texture D3D pour distinguer UI vs monde 3D** : ABANDONNÉ définitivement (0.6.6.1). Le cache de textures du jeu partage ses blocs entre les draws d'interface et les draws du monde 3D — près de 100% de faux positifs même avec la main vide. Toute nouvelle idée de filtrage doit se baser sur la géométrie (position à l'écran, profondeur, proximité du curseur), jamais sur l'identité/le contenu de la texture.
- **"TileCover" — forcer les cases du moteur à correspondre au repère de la table** : ABANDONNÉ (0.5.1). Le repère interne du plugin ("tabletop") ne correspond pas de façon fiable au monde du jeu ; approche invalidable hors PC (donjon minuscule au centre, remplacement jamais appliqué correctement).
- **Détection de la main du Gardien par classification de sommets 2D ou 3D** : ABANDONNÉE (0.6.3–0.6.3.1) au profit du hook direct sur la fonction native d'affichage du curseur logiciel du jeu (bien plus fiable).
- **Compteur natif "objets en main" à l'adresse `0x6CF908`** : jugé peu fiable (lit 0 en permanence), abandonné comme signal de détection.
- **Élargir le champ de vision natif du jeu en ne patchant qu'un seul point d'entrée (`0x58A6F0`)** : CAUSE DE LA RÉGRESSION 0.6.9 — le moteur utilise la focale à deux endroits distincts (dessin ET sélection des cases) alimentés par la même fonction en amont (`0x575A00`) ; patcher un seul des deux points aval désynchronise les deux usages. Toujours patcher au point d'alimentation commun le plus en amont, jamais un point de consommation isolé, sauf certitude absolue qu'il est le seul consommateur.
- **Écran d'options/menus projeté sur le mur du fond de la salle** : ABANDONNÉ définitivement sur demande utilisateur explicite, après plusieurs itérations de réglage de la détection (taille du bbox, latence de maintien, exclusion du modèle tenu/tooltips) qui n'ont jamais complètement éliminé les faux déclenchements ni satisfait sur le plan ergonomique. Remplacé par un empilement de panneaux côté joueur.
- **Recherche heuristique de facteurs d'agrandissement de la vue table par tâtonnement** (0.5.0–0.5.15, très nombreuses itérations avec trous/bandes vides/perf variables) : ABANDONNÉE au profit du calcul géométrique exact des coins de la table projetés à l'écran (0.5.16), qui a mis fin à des dizaines d'itérations infructueuses. Toute nouvelle idée touchant la "vue table" doit partir de cette méthode exacte, pas d'un nouveau tâtonnement heuristique.

---

## 6. Ce qui reste à faire (immédiat et à moyen terme)

1. **PRIORITÉ IMMÉDIATE — Valider la 0.6.9.2 au casque** : ce n'est pas fait. Attendre/demander le retour utilisateur (test réel + rapport `COLLECTER_RAPPORT.cmd`) avant toute autre modification. Points à vérifier précisément : plus de trous/latence aux bords de la table, sélection/creuser/prendre-créature fonctionnels jusqu'aux bords, menu options bien empilé, runtime OpenXR effectivement basculé sur Virtual Desktop dans le journal, pas de perte de netteté des textures liée à l'élargissement du champ de vision.
2. Si la 0.6.9.2 est validée : envisager de refaire un vrai nettoyage/relecture du code autour de `LensHack`/`ViewExpand` maintenant qu'il fonctionne, et mettre à jour la documentation utilisateur (`LIRE_MOI_FR.md`) qui est restée assez datée (encore des mentions d'anciens contrôles "bâtons colorés" alors que ce sont des halos lumineux depuis 0.6.9).
3. Reste à faire identifié en 0.6.9/0.6.9.1 (`CONTINUITE_MAITRE.md`, section "Reste à faire (UI)") :
   - Auto-détection de l'état "options ouvertes" plus robuste si besoin.
   - Main du Gardien en 3D (verte, avec griffes, orientation gaucher) — actuellement solution 2D/logicielle.
   - Overlays de créatures en billboard (draws pas encore identifiés).
   - Éventuellement : options du mod affichées sur les murs de la salle (paramètres VR configurables en jeu plutôt que seulement via l'ini).
4. Perf générale du jeu natif lui-même (hors casque) : dernier point noté comme non prioritaire, à revisiter seulement si signalé à nouveau.

---

## 7. Contraintes de style et de format à respecter dans les réponses

- **Toujours répondre en français**, y compris commentaires de code et documentation livrée.
- **Toujours produire un livrable testable** en fin de tâche (DLL/EXE compilés + zip de sources), jamais uniquement une explication.
- **Toujours suivre le format de livraison** : VERSION / BASE / CHANGEMENTS / PRÉSERVÉ / LIVRABLE / TEST / RÉSULTAT ATTENDU / À ME RETOURNER.
- **Ne jamais** demander à l'utilisateur d'ouvrir un terminal, PowerShell, Visual Studio, etc. — tout doit être un exécutable/script à double-cliquer.
- **Toujours** compiler et faire passer les 5 tests natifs (`MathTests`, `TableTests`, `AnchorTests`, `GrabTests`, `DecoupleTests`) avant de livrer.
- **Toujours** mettre à jour `CONTINUITE_MAITRE.md` (racine du dépôt + dossier de la version livrée) à la fin de chaque tâche, avec un résumé technique précis de ce qui a changé et pourquoi.
- **Toujours** déposer une archive de sources complète à chaque livraison (le code écrit dans une session cloud disparaît avec elle).
- Version bump cohérent partout : `Plugin.cpp` (`constexpr const char* Version`), `Host.cpp` (chaîne de log de démarrage), `Prepare-Game.ps1`, `LIRE_MOI_FR.md`.

---

## 8. Prochaine étape suggérée pour la nouvelle conversation

Démarrer par : *« Lis `CONTINUITE_MAITRE.md`, puis attends mon retour de test sur la 0.6.9.2 (perfs, trous aux bords, sélection du donjon, menu options) avant de proposer une nouvelle version. »*

---

## Annexe — Archive jointe

Une archive `DK2VR_Reprise_Complete.zip` accompagne ce résumé. Elle contient :
- Ce fichier (`RESUME_PROJET_DK2VR.md`) et `CONTINUITE_MAITRE.md` complet (historique détaillé 0.1.0 → 0.6.9.2) à la racine.
- Les **sources complètes de la dernière version (0.6.9.2)** : `src/`, `tests/`, `tools/`, `package/`, `CMakeLists.txt`, `README.md`, et les binaires déjà compilés dans `out/` (`DK2VR.dll`, `DK2VRHost.exe`, `openxr_loader.dll`).
- Un fichier `.gitignore` prêt pour GitHub (exclut `deps/OpenXR-SDK/`, les dossiers de build, les logs et captures de test).
- Une proposition de structure de dépôt GitHub inchangée par rapport à l'existant (déjà organisée en `src/`, `tests/`, `tools/`, `package/`).

Pour publier sur GitHub : créer le dépôt, y pousser le contenu de l'archive (hors `out/` si on préfère ne pas versionner de binaires — à la discrétion de qui publie), ajouter dans le README les instructions de build (`OPENXR=<chemin> sh tools/build_mingw.sh`, avec le commit OpenXR-SDK à cloner : `f2448a8797c85814aa892efc1ab8707900fbcc78`).
