# Rejeu de contrÃ´le du menu V0.9.24
made by LoVeMaKeRz

NativeMenuReplay.cpp lit les traces21 dans un pÃ©riphÃ©rique D3D9Ex matÃ©riel cachÃ©. Mode0=natif, mode6=correcteurs MenuFaces et MenuMaterials de production, mÃªmes portes d'Ã©tat que Plugin.cpp. tools/build_mingw.sh compile l'outil. Obligatoire : -msse2 -mfpmath=sse comme la DLL ; D3D9 change la prÃ©cision x87 et peut dÃ©placer une clÃ© UV Ã  un seuil d'arrondi. Aucune texture du jeu n'est incluse dans les sources publiques.

MenuMaterials minimum RGB224 pour les faces de drapeau reconnues, 208 pour corps/chaÃ®nes ; alpha et gÃ©omÃ©trie inchangÃ©s. Profils UV complets40/4 pour corps/chaÃ®nes, exacts connus sur les deux vraies captures. Drapeaux : lot dÃ©doublonnÃ© reconnu par MenuFaces + UV tous dans le profil56. Texture/ID de copie/pixels non lus pour classer. Sommets partagÃ©s avec d'autres matÃ©riaux exclus. Avant subdivision, dans recordMenuDraw seulement. Les chaÃ®nes exigent un mÃ©lange standardSRCALPHA/INVSRCALPHA et aucune Ã©critureZ. Ni drawWorld ni code des icÃ´nes modifiÃ©.


V0.9.24 : Mode6 applique ensuite MenuChains.hpp uniquement aux chaînes en mélange standard alpha, sansZWrite. Les copies exactes opposées sont retirées même pour un lot incomplet ; UV/Z/alpha doivent être exactement identiques. RGB255 seulement sur les sommets reconnus, partagés avec un matériau inconnu exclus. OptionsSSE2 inchangées. Aucun fichier du jeu inclus.
