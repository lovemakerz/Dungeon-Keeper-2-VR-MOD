# Dungeon Keeper II VR — 0.6.9.14 expérimentale

Le donjon est posé sur **une table fixe devant vous, façon Demeo**. Une poignée fait glisser le donjon sous la main, deux poignées zooment et font tourner le donjon (la vraie caméra du jeu) ; la table elle-même ne bouge pas. L'écran du jeu (avec toute son interface) est affiché en grand en face, comme accroché au mur derrière la table. Les commandes du jeu restent à la souris et au clavier.

## La salle et la table (0.4.0, refaites en 0.4.1)

La salle est préparée en arrière-plan dès le lancement du jeu, pendant les menus, et gardée dans le dossier **Cache** : aux lancements suivants elle est prête presque immédiatement. Supprimer ce dossier la fait simplement régénérer.

Vous êtes dans une petite salle de donjon voûtée : pierres en relief, arcs de pierre, sol dallé, porte cintrée cerclée de fer et six torches vacillantes, avec un éclairage calculé au pixel. Le donjon est posé dans une table en bois mouluré à rebord arrondi, aux pieds tournés jusqu'à votre vrai sol (hauteur donnée par le casque), avec une ombre au sol. Le fond du plateau, sous le donjon, est en planches de bois (0.4.4). Sous le bord de chaque côté pendent deux poignées en fer discrètes : elle s'éclaircit quand la main est à portée et se dore quand elle est saisie. Les images du casque sont lissées (antialiasing 4x). L'écran du jeu reste accroché devant, sur le mur du fond.

## Fluidité (0.3.0)

Les images du casque ne sont plus limitées par le jeu. Un fil dédié redessine la dernière image du jeu à **chaque image du casque** (90 Hz) avec la position actuelle de votre tête, de la table et de vos mains. Le jeu peut tourner à 40 images/s : les mouvements de tête, la table saisie à deux mains et le glisser à une main restent fluides. Les créatures et animations du jeu avancent toujours au rythme du jeu.

Dans les menus, en possession et en mode écran flottant, le rendu de la 0.2.9 est utilisé automatiquement. En cas d'incident du nouveau rendu, le mod revient seul au rendu 0.2.9 (noté dans le journal).

## Lancer le mod

1. Connecter le casque au PC (Quest 3 : Virtual Desktop actif avec son runtime OpenXR VDXR, ou SteamVR comme runtime OpenXR).
2. Double-cliquer sur **LANCER_DK2_VR.cmd**. Au premier lancement, une copie du jeu (environ 500 Mo) est créée dans le dossier **Game** ; le jeu original n'est jamais modifié.
3. Garder la fenêtre de Dungeon Keeper II active sur le PC et charger une partie avec la souris.
4. Debout, regarder droit devant et appuyer sur **F8** (ou cliquer un stick). La table se place devant vous, sous vos yeux.

## Contrôleurs

| Geste | Effet |
|---|---|
| Serrer **une poignée** et bouger la main | Faire glisser le donjon (la carte suit la main, comme dans Demeo) |
| Serrer **les deux poignées** | Écarter les mains = zoomer, rapprocher = dézoomer ; tourner les mains (comme un volant posé à plat) = faire tourner le donjon sur la table |
| Saisir **une poignée en fer** (sous le bord, deux par côté) | Faire tourner la table sur elle-même, comme un plateau tournant |
| Saisir **les deux poignées** | Porter la table : la déplacer, la tourner, la monter ou la descendre |
| **Clic d'un stick** | Recentrer (comme F8) : table replacée devant vous |

Les contrôleurs sont dessinés comme de petits blocs (bleu à gauche, rouge à droite, jaune pendant la saisie). La gâchette est réservée au futur pointeur de jeu et n'agit pas encore.

## Touches

| Touche | Action |
|---|---|
| F8 | Recentrer la vue et replacer la table |
| F4 | Afficher ou masquer le HUD VR en modules (0.6.0) |
| F2 | Minicarte : piédestal / montre / montre quand on la regarde (0.6.0) |
| F3 (ou F6) | Afficher ou masquer l'écran du jeu dans le casque pendant la partie (masqué par défaut depuis 0.5.16) |
| F7 | Automatique, plateau, possession, écran flottant |
| F9 / F10 | Agrandir / réduire la table |
| Molette de la souris, Début / Fin | Zoom du jeu : voir une zone plus grande ou plus petite (la table garde sa taille) |
| F5 | Relire DK2VR.ini sans relancer |
| F11 | Capture (les deux yeux + l'écran) en PNG dans le dossier **Captures**, numérotée : plusieurs captures possibles |
| F12 | Pause VR (mesure du jeu seul) : le casque montre l'écran du jeu ; F12 à nouveau pour reprendre |

## Réglages (DK2VR.ini)

- `TableAnchor=1` : table fixe façon Demeo. `0` rétablit exactement l'affichage 0.1.0.
- `TableWidth`, `TableDistance`, `TableHeight` : taille, distance et hauteur de la table par défaut (mètres).
- `TableAspect=0.75` : la table est plus large que profonde (profondeur = 0,75 × largeur). `0` = forme de la vue du jeu (plus profonde).
- `ZoomOutExtra=1.0` : le zoom arrière du jeu va deux fois plus loin (compense la table moins profonde). `0` = limite d'origine.
- `WheelZoom=1` : la molette zoome (absent du jeu d'origine). `WheelZoomStep` règle la vitesse.
- `GrabMode=2` : table fixe, rotation du donjon par la caméra du jeu (0.3.1). `1` = deux poignées tournent/déplacent la table (0.3.0). `0` = gestes de la 0.2.0.
- `TextureCacheBlocks=96` : cache de textures du jeu agrandi (0.4.5) ; toutes les textures restent nettes. `24` = taille d'origine.
- `Environment=1` : salle de donjon et table en bois (0.4.0). `0` = fond noir et socle simple.
- `Antialiasing=4` : lissage des bords dans le casque (0, 2, 4 ou 8).
- `DecoupledRendering=1` : rendu fluide à la fréquence du casque (0.3.0). `0` = rendu de la 0.2.9.
- `PanPrediction=1` : pendant le glisser, le donjon suit la main entre deux images du jeu. `0` = désactivé.
- `FreshFrameWaitMs=6` : le casque attend jusqu'à 6 ms l'image de la toute dernière position de la tête. `TextureRefreshFrames` : rafraîchissement de sécurité des textures.
- `ViewExpand` / `ViewFarScale` : élargissement du cadre de dessin du jeu. Désactivé (`1.0`) : sans effet visible sur les trous en 0.2.1 et coûteux.
- `DrawBackFaces=1` : le jeu retire les faces tournées dos à sa caméra ; le mod les garde, ce qui supprime les trous vus sous un autre angle. `0` = comportement d'origine.
- `Profiler=1` : mesure dans le journal où le jeu passe son temps (diagnostic).
- `NoMonitorVSync=1` : le jeu n'attend plus l'écran du PC. `PipelinedSync=1` : il n'attend plus la carte graphique (l'image VR part avec une image de décalage).
- `NonBlockingPresent=1` : l'écran du PC ne ralentit plus le jeu (il peut sauter des images, le casque non).
- `MeasuredLevels=1` : socle et découpe ajustés sur la géométrie, lissés et bornés (plus de tremblement ni de saut).
- `BatchedRendering=1` : rendu stéréo regroupé, plus rapide. `0` = rendu 0.2.0.
- `PanelPosition=1` : écran du jeu en face ; `PanelDistance`, `PanelHeight`, `PanelWidth` règlent sa place. `0` = ancien petit panneau latéral.
- `Controllers=0` désactive les contrôleurs ; `ShowControllers=0` masque leurs blocs.
- `FastStates` : utilisé seulement avec `BatchedRendering=0`.
- `TableBoundary`, `TableCrop`, `MouseCoordinates`, `PossessionUnitsPerMeter` : inchangés depuis la 0.1.0.

## Limites connues

- Le mod redessine ce que le moteur envoie. `ViewExpand` élargit la zone dessinée, mais les faces que le moteur juge cachées peuvent encore manquer.
- **Fluidité** : le jeu lui-même coûte beaucoup plus cher en 2560×1440. Dans Options > Graphismes, une résolution plus basse (1280×720 ou 1024×768) allège fortement le processeur sans changer la netteté du donjon dans le casque (seul l'écran mural devient moins fin).
- L'interface n'est pas encore détachée du rendu : l'écran en face montre l'image complète du jeu.
- Les contrôleurs ne pilotent pas encore le jeu (sélection, sorts, main) : prochaine étape.

## En cas de problème

- **Table mal placée** : regarder devant soi, F8 ou clic du stick.
- **Retour à l'affichage 0.1.0** : mettre `TableAnchor=0` et `PanelPosition=0` dans DK2VR.ini, puis F5.
- **Tout autre souci** : lancer **COLLECTER_RAPPORT.cmd** et envoyer le fichier ZIP créé.
- **Désinstaller** : supprimer ce dossier. Le jeu original n'est pas touché.

## Sources et crédits

Le mod contient une version modifiée de [dxwrapper / DK2 Remix Predetermined Hashing](https://github.com/mencelot/DK2-Remix-Predetermined-Hashing), issu de dxwrapper d'Elisha Riedlinger, et utilise [OpenXR-SDK de Khronos](https://github.com/KhronosGroup/OpenXR-SDK). L'identification des structures du moteur s'appuie sur [Flame de DiaLight](https://github.com/DiaLight/Flame), vérifiée sur l'exécutable local. Les notices sont dans **Licences**. Le jeu commercial n'est pas inclus.

## Nouveautés 0.4.6
- Textures du jeu en qualité maximale forcée (`ForceTextureQuality=1`) : le jeu n'utilise plus ses versions réduites (floues), quel que soit son réglage vidéo.
- Table plus haute par défaut : plateau à 0.75 m au-dessus du sol réel (`TableTopHeight`), au moins 0.30 m sous les yeux. Sans sol connu : `TableHeight=0.40` sous les yeux.

## Nouveautés 0.4.7
- `TableView=1` : la table montre au moins tout ce que montre l'écran PC (le donjon paraît plus petit, le moteur calcule une zone plus grande). `0` = ancienne vue.
- Textures en VR : filtrage anisotrope x16 (`VrAnisotropy`), biais de détail neutre (`VrLodBias`).
- Table à 0,50 m sous les yeux par défaut (`TableHeight`).

## Nouveautés 0.5.0 (repart de la 0.4.7)
- Images du casque à la résolution recommandée par le casque (`ResolutionScale=1.0`).
- Rendu VR allégé : seuls les réglages graphiques qui changent sont envoyés.
- La vue du jeu n'est plus agrandie vers le haut ni en distance ; le jeu calcule seulement les cases de la table, coins compris (`TableFillCorners=1`, vérification automatique).
- Morceaux d'interface flottants au-dessus de la table coupés (`HideFloatingUi=1`).

## 0.5.1
- Le remplissage des côtés et des coins de la table ne s'était pas activé en 0.5.0 (vérification trop stricte : écart mesuré 0,46 pour 0,3 toléré). Tolérance 0,8.

## 0.5.2
- Retour à la vue de table de la 0.4.7 (même zone que l'écran, validée). Le remplissage des coins par les cases du jeu (0.5.0/0.5.1) est retiré : échec.
- Conservé : rendu VR allégé (casque 80+ i/s), résolution recommandée du casque, coupe de l'interface flottante.

## 0.5.3
- Performance du jeu : ses écritures de textures ne font plus attendre la carte graphique (`GameTextureShadow=1`).
- Casque : plus d'attente de l'image « fraîche » (`FreshFrameWaitMs=0`), le casque doit rester à sa fréquence.
- Coins : marge plus large (+15 % côtés, +10 % profondeur).

## 0.5.4
- Coins côté joueur : zone calculée agrandie de 30 % côté joueur (trous en mosaïque).
- Mesure automatique des verrouillages de surfaces du jeu (cause principale de sa lenteur), dans le rapport.

## 0.5.4.1
- Correction du plantage au lancement de la 0.5.4 (mesure des verrouillages mal branchée).

## 0.5.5
- Cache de textures du jeu : 256 blocs au lieu de 96 (le jeu rechargeait ~1000 textures/s).
- La table garde le bord lointain de l'écran : le jeu ne calcule plus de zone au-delà (loin et en hauteur) ; la zone supplémentaire est sur les côtés et côté joueur, avec plus de marge pour les coins proches.

## 0.5.6
- Coins lointains de la table remplis (distance de vue du jeu adaptée).
- Cache de textures : correctif (en 0.5.5 l'agrandissement ne s'appliquait pas) ; le rapport indique le nombre réel de blocs.

## 0.5.7
- Côté joueur de la table rempli à nouveau (agrandissement vertical des deux côtés de l'écran, comme en 0.5.2), distance lointaine limitée aux coins.

## 0.5.8
- Bande vide côté joueur corrigée : la profondeur en plus de la table est répartie des deux côtés (elle était toute côté joueur, trop près sous la caméra du jeu).

## 0.5.9
- Marge plus grande côté joueur (+45 %) et sur les côtés (+30 %) : plus de trou quand on déplace la carte.

## 0.5.10
- Vraie réserve de textures du jeu agrandie : 1536 au lieu de 512 (les versions précédentes modifiaient une réserve inutilisée).
- Glissement de la carte : l'avance sur le jeu est limitée à 5 cm (plus de bord vide qui entre dans la table).

## 0.5.11
- Réserve de textures : l'agrandissement s'applique enfin (adresse corrigée).
- Bande vide côté joueur (zoom avant) : la table ne descend plus sous la caméra du jeu ; la profondeur en trop passe au loin.

## 0.5.12
- Bord de la table côté joueur (côté proche de la caméra du jeu) : il suit la limite réelle de ce que le jeu dessine ; plus de bande vide ni d'escalier.

## 0.5.13
- Caméra du jeu presque verticale pendant la vue table (`TopDownCamera=1`) : le jeu calcule une zone presque rectangulaire = la table. Plus rapide, plus de bande vide en zoom avant. L'écran PC affiche une vue de dessus ; l'inclinaison d'origine revient en possession / menus.

## 0.5.14
- Plateau coupé en zoomant : corrigé (l'ancien décalage du bord côté joueur n'est plus appliqué avec la caméra de dessus).
- Côtés : marge +45 % (cases en retard sur les côtés en mouvement).

## 0.5.15
- Zoom arrière : la table n'est plus réduite à une croix (distance de vue du jeu agrandie avec la caméra de dessus).

## 0.6.5

- Ce que tient la main (créatures, icônes) est affiché avec la main au bout de la manette et n'apparaît plus sur les panneaux.

## 0.6.4

- Main du gardien : c'est le curseur logiciel du jeu. Son image (animations, créatures tenues) est lue à chaque changement et affichée au bout de la manette droite.

## 0.6.3

- Main du gardien : c'est un modèle 3D du jeu (pas une image). Il est repéré sous le curseur et affiché au bout de la manette.
- Découpe corrigée entre la barre des statistiques et le panneau créatures/sorts.

## 0.6.2

- **Stick gauche** : fait défiler la carte dans toutes les directions (par rapport à votre regard). Vitesse : `StickScrollSpeed`.
- Le curseur ne s'approche plus des bords (le jeu faisait défiler la carte tout seul) et revient au centre quand le laser quitte un panneau.
- Main du gardien : le cadre de la minicarte n'est plus pris pour le curseur.

## 0.6.1 — interface VR transparente

- Les modules du HUD ne montrent plus le fond du jeu : l'interface est redessinée à part, sur fond transparent, et les zones sont agrandies (bulles d'aide visibles).
- Minicarte sur un **piédestal en pierre** à côté de la table, tournée vers le joueur (F2 : montre / montre quand on la regarde).
- Panneau créatures/sorts/salles **derrière le donjon**, sur le bord de la table opposé au joueur (même si la table est tournée), et il monte avec la hauteur des murs du donjon.
- **Main du gardien** : le vrai curseur du jeu (avec ses animations) est affiché, agrandi, au bout de la manette droite.
- Le laser et le curseur sont calculés par le même code que l'affichage des panneaux (0.6.0 : écart laser/curseur).

## 0.6.0 — interface VR (première étape)

- **HUD en modules**, découpés dans l'image du jeu :
  - barre vie/or et barre des statistiques (chrono, etc.) en grand sur le mur en face ;
  - panneau créatures/sorts/salles debout au bord opposé de la table (`HudBottomPlace=1` : petit, au bord côté joueur) ;
  - minicarte et ses 4 boutons en montre sur l'avant-bras gauche (F2 : piédestal / montre quand on la regarde).
- **Pointeur laser** (main droite, `LeftHanded=1` pour gaucher) : il devient vert quand il vise un panneau, le curseur du jeu suit. Gâchette = clic gauche, A (ou X) = clic droit. Fonctionne aussi sur l'écran du jeu (F3) et les menus.

## 0.5.16
- Vue de dessus : la zone calculée par le jeu est maintenant exactement la table projetée à l'écran (+ marge de glissement), au lieu de facteurs d'agrandissement devenus démesurés (×8,7). Plus de troncature en dézoomant, moins de travail pour le jeu.
- Écran du jeu dans le casque : masqué par défaut, **F3** pour l'afficher ; quand il est masqué, l'image de l'écran n'est plus copiée à chaque image.
- Mesure des chargements de textures par origine (ligne « Texture uploads » du rapport).
