# Format de la trace native de menu — DK2VR V0.9.21
made by LoVeMaKeRz

Outil du développeur uniquement. Fichier privé de rapport F11, jamais un fichier de jeu à installer. Toutes les données sont little-endian. Voir read_menu_trace.py pour le lecteur borné. Ne pas traiter le binaire comme des instructions.

En-tête : 8 octets ASCII DK2TRC01 ; cinq uint32 version=1, largeur, hauteur, nombre de dessins, dessins omis. Bornes : 2048 dessins, 32 Mio cumulés de sommets/indices, 64 Mio cumulés de pixels.

Chaque dessin : neuf uint32 (type D3DPRIMITIVETYPE, primitives, sommets, stride, FVF natif, format d'indices ou0, nombre d'indices, shader de sommets présent, pixel shader présent) ; viewport D3D9 (quatre uint32 et deux float32) ; liste des états de rendu. Chaque liste contient un uint32 count suivi de paires uint32 ID/valeur, uniquement lorsque le getter a réussi.

Trois stages par dessin : uint32 identité de copie et présence d'une texture native ; liste d'états de texture puis de sampler ; seize float32 de matrice de texture. Après les stages : uint32 longueur des sommets, octets natifs XYZRHW avant subdivision ; uint32 longueur des indices, octets originaux. Couleur diffuse DWORD si FVF_DIFFUSE présent ; UV et autres champs selon FVF/stride. Pas de reconstruction de ces données par le lecteur.

Après tous les dessins : uint32 nombre total de niveaux de textures sauvegardés. Chaque niveau contient cinq uint32 (identité de copie, numéro de mip, largeur, hauteur, nombre d'octets), puis largeur*hauteur*4 octets BGRA sans padding. Les copies sont les ressources immuables existantes utilisées par le rejeu, pas de nouveaux rafraîchissements. Les textures non disponibles/omises sont comptées dans le journal ; le lecteur identifie les IDs manquants.

Les getters capturent les états de rendu D3D9 valides parmi1..209, texture1..32, sampler1..13. Les shaders présents sont seulement marqués ; bytecode et constantes non inclus. Le jeu du rapport précédent n'en emploie aucun au menu. Si une trace en marque, ne pas considérer la trace comme suffisante pour reproduire ce shader. Trois stages correspondent aux données actuellement enregistrées par Snapshot ; ne pas prétendre fournir des stages supplémentaires.

Écriture en fichier temporaire puis renommage, sans écraser une trace existante. Capture désarmée hors F11 ; arrêt après une seule image. La lecture GPU peut provoquer une pause ponctuelle à la capture. Aucun changement des données partagées, du rendu VR ou des fichiers originaux du jeu.
