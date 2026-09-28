# Validation — DK2VR 0.4.5

Base : 0.4.4 (casque 26/09 : couleur du fond en bois correcte ; trous tout autour du plateau ; textures du jeu floues par endroits).

## Causes
- Trous : le fond en bois était 1 mm au-dessus du cadre, dont le bord intérieur est chanfreiné sur 6 mm ; en vue rasante on voyait sous la table par cette fente. Reproduit en aperçu.
- Textures floues : cache de textures du moteur (SurfHashList) limité à 24 blocs de 256×256 (static_MyDirectDraw_devTexture_init, 0x592EA0). Quand la vue demande plus, calcHandleCountToFitHolder (0x593E10) fait utiliser les versions réduites (MM1–MM3). Le projet Flame multiplie ce nombre par 4 pour les grandes résolutions.

## Changements
- Fond en bois posé au pied du chanfrein intérieur et prolongé de 5 mm sous le cadre : plus aucune fente (vérifié en aperçu, même vue rasante).
- Cache de textures du jeu : 'push 0x18' (0x592FAC) → TextureCacheBlocks (96 par défaut, 24 = origine, max 120), signature vérifiée. Seuil de recherche du bloc le moins utilisé 0x10000 → 0x1000000 (0x594A17), comme Flame. Appliqué avant l'initialisation des textures 3D si possible (journal : « Texture cache: … »).

## Préservé
- Tout le reste de la 0.4.4.

## Vérifié ici
- Compilation x86/x64 ; 5 suites de tests ; aperçu du bord du plateau.
- Octets du jeu vérifiés sur l'exécutable local.

## Non vérifié (casque)
- Netteté des textures du jeu ; moment d'application du patch du cache (voir le journal).

## 0.4.6
- Qualité textures : signatures vérifiées 0x566641 (setTextureReductionLevel → g_ReductionLevel 0x7932DC), 0x5948FC (_probablySort lit 0x7932DC et devTexture.reductionLevel 0x792D8C), 0x59D968 (textures haute résolution 0x759AF0). Valeurs >0 remises à 0 à chaque image, haute résolution = 1. Journal : « Texture quality … ».
- Table : hauteur = sol STAGE + TableTopHeight (0.75), min 0.30 m sous les yeux ; replacée automatiquement à la première mesure du sol. Non testé en casque.

## 0.4.7
- Rapport 0.4.6 : qualité textures du jeu déjà au maximum (setting=0 reduction=0 devTexture=0 highRes=1) → le flou ne vient pas du choix des textures par le jeu.
- VR : MINFILTER LINEAR → ANISOTROPIC x16, LOD bias forcé à VrLodBias. Journal « VR textures: » = filtres du jeu et tailles des textures (diagnostic).
- TableView=1 : rectangle circonscrit à l'empreinte écran (makeTabletop circumscribe), élargi au format 0.75 ; ViewExpand calculé à chaque image (x, y, distance lointaine), journal « TableView screen ». Test hors casque : caméra inclinée 0.9 rad → table 17,6 x 13,2 unités (contre 8,2 x 10,4 inscrit), expansion x2,6 / x1,75.
- Non testé en casque : performances du moteur avec la zone agrandie.

## 0.5.0 (base 0.4.7)
- Thread VR : cache des états (RS/TSS/SS/textures/shader) par passe du donjon ; MIN/MAXANISOTROPY/LODBIAS remplacés gérés dans le cache.
- Résolution : ResolutionScale 1.0, MaxEyeDimension 4096 (hôte et INI).
- ViewExpand : côtés (x) et bas (côté joueur) seulement ; haut = ViewExpand, far inchangé. Table : bord lointain = bord lointain écran.
- TableCover : crochet 0x5750EC (signatures doMat 0x575180 et linkVec 0x575130 vérifiées). Coins de table en pixels écran (projection plugin) → sol monde z=0 par la caméra du jeu ; sens y écran choisi (bas plus proche) ; contrôle : coins bas du rectangle moteur présents dans la liste du jeu (< 0,3) ; remplacement après 10 contrôles réussis ; marge 2,2 unités ; points devant la caméra ; nœuds en trop déplacés à l'intérieur (pas de doublons) ; chien de garde (120 images de table puis 45 images sans donjon). Journal : « Table corners: … ».
- Plafond de découpe (5e plan) : haut des murs + 0,4 unité.
- Tests : compilation x86/x64, 5 suites. Non testé en casque ; formule rayon écran→monde du jeu vérifiée seulement par le contrôle automatique en jeu.

## 0.5.1
- Rapport 0.5.0 : thread VR 0,2–0,7 ms, casque 78–85 i/s (corrigé) ; résolution recommandée OK ; jeu ~30–44 i/s (proche de son rythme sans agrandissement). Table corners : jamais actif, contrôle échoué à chaque image avec erreur constante 0,46 (tolérance 0,3) → côtés vides (la table 0,75 est plus large que la vue écran, plus profonde que large). Tolérance → 0,8.

## 0.5.2
- Rapport 0.5.1 : table corners vérifié puis remplacement jamais appliqué (bascule prête/non prête), donjon réduit à une petite zone. TileCover retiré (non installé). Vue de table = 0.4.7 exactement (ViewExpand x/y/far, rectangle circonscrit centré), marge latérale 1,08 au lieu de 1,04.
- Conservés (validés sur casque) : cache d'états VR (0,2–0,7 ms), résolution recommandée, plafond de découpe.

## 0.5.3
- Rapport 0.5.2 : jeu 38–46 i/s ; profil : 54–68 % des attentes du fil du jeu dans lockBuf 0x590AC0 → LockRect texture ; source vérifiée : dxwrapper D3d9to9Ex convertit MANAGED en DEFAULT|DYNAMIC (d3d9/IDirect3DDevice9Ex.cpp CreateTexture) → LockRect sans DISCARD attend le GPU. Casque 71–80 i/s avec attente « fraîche » 6 ms (88–90 i/s sans).
- Ombre CPU des textures : crochet LockRect (vtable 19) + UnlockRect (20) ; jumeau SYSTEMMEM rempli une fois ; envoi par UpdateTexture (original, sans attente) ; invalidé par toute autre écriture (surface, UpdateSurface/Texture, StretchRect, ColorFill, cible de rendu) ; marqueur SetPrivateData contre la réutilisation d'adresse. Journal : « Game texture shadow ».
- FreshFrameWaitMs 0 ; marges ViewExpand x1,15 / y1,10.
- Non testé en casque.

## 0.5.4
- Rapport 0.5.3 : ombre de textures active (attente LockRect texture 1213AB : 41 % → 1 %) mais jeu toujours ~35 i/s ; nouvelles attentes : DDRAW+18764E = D3DXLoadSurfaceFromSurface (37–42 %) et DDRAW+121317 = IDirect3DSurface9::LockRect sur la surface fantôme (29–35 %). Casque : fil VR 65 images/s, hôte 77–88.
- Mesure : vtable CEngineDDSurface 0x6703DC (lockBuf 0x590AC0 / unlockBuf 0x590B20, signature vérifiée) enveloppée ; par appelant : nombre, taille, ms verrou / déverrou. Journal « Surface locks ».
- ViewExpand : bas (côté joueur) ×1,3 supplémentaire.

## 0.5.4.1
- Plantage 0.5.4 (code 0xC0000005 au démarrage) : lockBuf 0x590AC0 se termine par « ret » (aucun argument, Flame indiquait int*) et unlockBuf 0x590B20 par « ret 4 » (1 argument) ; enveloppes corrigées (fastcall sans argument / 1 argument, ret vérifiés à la compilation).

## 0.5.5
- Rapport 0.5.4.1 : lockBuf 0x59049B (dans paintSurf 0x590450 = chargement d'une texture dans le cache SurfHashList) ≈ 1060 appels/s, 128x128 ; jeu 15 i/s zoom arrière (780 dessins), attentes wrapper D3DX 18764E + LockRect 121317 → rechargements permanents du cache.
- Cache : appel 0x592FB9 du constructeur SurfHashList 0x592DB0 (thiscall(desc,count), ret 8 vérifié) redirigé, count = TextureCacheBlocks (256).
- Vue : bord lointain de table = bord lointain écran ; ViewExpand côtés/côté joueur seulement (haut et far inchangés), marge ×1,2.

## 0.5.6
- Rapport 0.5.5 : jeu ~60 i/s (bord lointain fixe) ; aucun log du constructeur → cache resté à 24 blocs (3 300 chargements/s). Correctif : octet push (127) + crochet d'appel ; journal « Texture cache: N blocks in use » (SurfHashList+0x74).
- Coins lointains vides : distance de rejet 0x760B04 mesurée depuis la caméra → far × (distance coin / distance milieu du bord lointain) × 1,1.
- « UI flottante » entourée : petit objet près du mur, probablement la main du Gardien (curseur 3D du jeu) projetée là où pointe la souris quand elle est sur le HUD ; traité avec le pointeur manette.

## 0.5.7
- Rapport 0.5.6 : bande centrale seulement, côté joueur vide malgré « x4.14 towards the player » : déplacer seulement 0x760AF8 n'élargit pas le côté joueur (0.4.7–0.5.4 déplaçaient les deux bords et le côté joueur était rempli). Retour à l'agrandissement symétrique, far = ratio coin/milieu ×1,1 (au lieu de ×4 en 0.4.7). Journal cache « -1 » : g_pSurfHashList 0x792D60 nul à la lecture, compteur non fiable.

## 0.5.8
- Rapport 0.5.7 : « mieux », reste une bande vide côté joueur ; jeu 20 i/s zoom arrière (890 dessins). Cause probable : bord proche de la table derrière la limite proche du moteur (profondeur ajoutée entièrement côté joueur depuis 0.5.5). Retour au centrage de 0.4.7 ; far = distance coin lointain de la table / milieu du bord lointain de l'écran ×1,1 (max 4).

## 0.5.9
- Retour 0.5.8 : « presque » ; petit trou coin bas-gauche à l'arrêt, gros manque côté joueur en mouvement (prédiction du glisser en avance sur le jeu). Marges ex ×1,30, eb ×1,45.

## 0.5.10
- Chemin matériel : cache SurfHashList2 pSurfHashList2_2 0x7920D0 construit à 0x593259 (constructeur 0x592BD0(desc,32,512), porteurs 128x128, compteur +0xDC), vérifié dans le désassemblage et la source Flame (MyDirectDraw.cpp). Les correctifs 0x592FAC/0x594A17 (SurfHashList, chemin logiciel) étaient sans effet. push 0x200 @0x593242 → TextureCacheBlocks (1536). Journal « Texture cache: N blocks in use » lit désormais 0x7920D0+0xDC.
- Prédiction du glisser limitée à 0,05 m (PanPredictionLimit ; 0.5.9 : moyenne 87 mm).
- Nombre de dessins (~1100) non réduit dans cette version.

## 0.5.11
- Rapport 0.5.10 : « Texture cache: signature unsupported » : adresse du push fausse (0x593242 au lieu de 0x593247 ; octets vérifiés 68 00020000 6a20 68 982d7900 890dd0207900 e8, cible 0x592BD0). Corrigé.
- Bande vide côté joueur à l'arrêt (zoom avant) : bord proche de table limité à l'empreinte d'un écran agrandi ×2 vers le joueur, décalage vers le loin.

## 0.5.12
- Test utilisateur : rotation physique de 180° (poignées) → bord côté joueur plein ; le manque est donc sur le côté PROCHE DE LA CAMÉRA DU JEU (le moteur n'y produit pas de cases), cohérent avec la limite proche.
- Mesure : distance « avant » minimale des sommets dessinés par le jeu (repère plugin), lissée ; bord proche de table ≥ cette limite + 0,3 ; l'agrandissement moteur reste calculé pour le bord non décalé (pas d'emballement). Journal « Table near edge moved ».

## 0.5.13
- Rapport 0.5.12 : trous côté caméra du jeu croissants avec le zoom avant, jeu 24–28 i/s, 750–870 dessins ; aucun « near edge moved ».
- Inclinaison CCamera +0xD86 (0..0x1CC, rotateRel 0x449D40), reconstruction matrice 0x44D6D0 (thiscall, ret, signature setDirection 0x449F50 vérifiée). Forcée à 0x1CC pendant la vue table ; sens vérifié par la composante descendante de 0x760AC4[8] (sinon 0) ; restaurée hors table. Journal « Top-down camera ».

## 0.5.14
- Rapport 0.5.13 : « bonne voie », perf meilleure, zooms min/max excellents ; tilt 220→460 moins vers le bas (0,159), bascule auto sur 0 (vue de dessus) OK. Plateau coupé : « Table near edge moved 4.6 units » (logique 0.5.11/0.5.12 inutile en vue de dessus) → désactivée quand la caméra de dessus est active. ex ×1,45.

## 0.5.15
- Rapport 0.5.14 (sans commentaire) : capture 1 correcte, capture 2 (zoom arrière) = croix de cases : limite de distance 0x760B04 (sphère autour de la caméra haute). far ≥ ×3 en vue de dessus. Jeu 37–67 i/s, casque 87–90.

## 0.5.16
- Rapport 0.5.15 : tilt 0 = vue exactement verticale (composante descendante 1,000). TableView : « x8.70 towards the player » : la recherche de couverture supposait le bord haut fixe alors que le rectangle est agrandi des deux côtés depuis 0.5.7 → recherche jusqu'à la limite, rectangle ×8,7 vertical, table tronquée au dézoom.
- MyCameraState 0x575A00 (vérifié) : rectangle 0x760AEC..AF8 et centre 0x760AFC/B00 recalculé = milieu du rectangle ; far 0x760B04 plafonné à 32 si drapeau bit0 (avant generate, le multiplicateur s'applique après).
- Vue de dessus : rectangle moteur = union(rectangle d'origine, projection écran des 12 coins de la table (sol, sous le sol, haut des murs) + marge 1,5 + prédiction glisser + 8 %), far ×6 (rayons bornés par le sol). Test hors casque : caméra verticale → rectangle 1024 × 768 (×1,19 vertical), couverture de la table vérifiée ; inclinée 80° → couverte aussi.
- Panneau écran du jeu : F3 (et F6), masqué par défaut ; RequestPanelShown (bit 9) ; copie de l'écran seulement si panneau, menus ou aucun dessin du donjon.
- paintSurf 0x590450 (thiscall 3 arg, ret 0xC) : 7 appelants directs comptés (journal « Texture uploads »).
