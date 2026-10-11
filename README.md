<div align="center">

# Dungeon Keeper 2 VR

**Play Dungeon Keeper 2 in virtual reality — a fan-made OpenXR mod.**

*Made by LoVeMaKeRz* · Version **0.9.47.63** (public) · [Français](README.fr.md)

[![Watch the demo on YouTube](https://img.youtube.com/vi/0tBOKbF9qJE/maxresdefault.jpg)](https://youtu.be/0tBOKbF9qJE)

▶ **[Watch the video on YouTube](https://youtu.be/0tBOKbF9qJE)**

</div>

---

## What is it?

DK2VR is a VR mod for **Dungeon Keeper 2 (version 1.70)**. Your dungeon sits on a table in a small vaulted stone room: point with a laser, grab and turn the table, **possess creatures in first person with a full 360° view**, and play with your VR controllers.

The game itself is **not modified**: `DKII-DX.exe` is never touched, and the game folder is put back exactly as it was when you quit.

### Features

- The dungeon on a table, with grab / turn / zoom / table-size controls
- First-person **possession with a 360° view** (cube-map rendering done by the game itself)
- Creature skills shown in VR — cycle with **A / B**, attack with the trigger
- Held creatures shown in real **3D**
- Damage numbers rendered upright, readable from any angle
- Virtual **VR keyboard** (to name your saves)
- In-world **OPTIONS** and **CONTROLS** panels on the walls (English / Français)
- Safe install, one-click update, and restore scripts

## Requirements

- Windows 10 / 11 (64-bit) — nothing else to install (PowerShell is built in)
- **Your own copy of Dungeon Keeper 2, version 1.70** (not included). The installer checks the exact `DKII-DX.exe`; if it is another version, nothing is changed.
- A VR headset with an **OpenXR** runtime. Developed and validated on a **Meta Quest 3 through Virtual Desktop**. Other headsets / runtimes: not tested.
- A PC able to run the game and VR at the same time

> [!IMPORTANT]
> **If you change the game's resolution in its options, quit the game and start it again.**
> The mod adapts to the resolution at startup; without a restart, selection lines and the interface are drawn wrongly.

## Install (2 minutes) ( RUNTIME : OPENXR )
Manual: Copy the DK2VR folder & DKII_VR.cmd into your Dungeon Keeper 2 folder, and launch DKII_VR.cmd

Automatic:
1. Download the latest ZIP from the [**Releases**](https://github.com/lovemakerz/Dungeon-Keeper-2-VR-MOD/releases) page and extract it into a **separate** folder (not the game folder).
2. Close the game, then double-click **`INSTALLER.cmd`**. It finds the game (Steam default folders) or asks you to pick the folder containing `DKII-DX.exe`. Accept the administrator prompt if Windows shows one.
3. Start Virtual Desktop (or your OpenXR runtime), put the headset on, and run **`DKII_VR.cmd`** from the game folder.

Your settings and saves are kept on every update. Each update backs up the files it replaces in `DK2VR\_mises_a_jour`.

### Uninstall / go back

| Action | How |
|---|---|
| Return to the previous mod version | `DK2VR\RESTAURER_VERSION_PRECEDENTE.cmd` |
| Game crashed and left VR files behind | `DK2VR\RESTAURER_JEU_ORIGINAL.cmd` (normally automatic when the game closes) |
| Remove everything | Delete the `DK2VR` folder and `DKII_VR.cmd` from the game folder. Your saves are never touched. |

## Controls (Quest 3, default: right hand points)

The same guide is available in-game: click the **CONTROLS** block on the wall opposite the **OPTIONS** block, then pick the English / Français tab.

### Dungeon (table and menus)

| Left hand | Right hand |
|---|---|
| Stick — move the map | Trigger — left click (point and click) |
| Stick click — open / close VR keyboard | A — right click |
| X — Space | Stick L/R — turn the dungeon |
| Y — Escape | Stick U/D — zoom |
| Grip — grab the table | Stick click — recenter |
| | Grip — grab the table |

### Possession

| Left hand | Right hand |
|---|---|
| Stick — move | Trigger — attack / use the chosen skill |
| Stick click — run | A — next skill |
| X — leave the possession | B — previous skill |
| Y — disabled | Stick L/R — turn (by angle or smooth) |

Left-handed? Choose **PRIMARY HAND** in OPTIONS (at the table the two hands swap roles; possession keeps the same layout).

## In-game wall panels

- **OPTIONS** — PC mirror on/off, snap turn (15/30/45/90°) or smooth turn, laser colour, invert map rotation, primary hand, passthrough colour (Virtual Desktop green screen), table size, off-hand orb show / hide.
- **CONTROLS** — the controller guide above, in English or French. The chosen language is remembered.

## PC mirror window (streamers)

In **OPTIONS** (top entry) switch **PC MIRROR** ON: a normal 16:9 Windows window shows one complete eye of the headset view, undistorted.
In OBS / Streamlabs: *Sources → Window Capture → "DK2 VR - Miroir PC"*. Closing the window with ✕ switches the option off. The black game window on the PC is normal and can stay in the background; the game no longer pauses when another window has focus (`RunInBackground=1`).

## Keyboard shortcuts (PC keyboard)

`F2` minimap mode · `F3` game panel · `F4` VR HUD on/off · `F5` reload `DK2VR.ini` · `F7` view mode · `F8` recenter · `F9` / `F10` table size

Settings live in `DK2VR\DK2VR.ini` (commented). Restart the game after editing.

## Troubleshooting

- **"Needs Dungeon Keeper 2 version 1.70"** — your `DKII-DX.exe` is another version; nothing was changed.
- **Nothing in the headset** — start the VR runtime first, then `DKII_VR.cmd`. `DK2VR\DIAGNOSTIC_VR.cmd` checks the runtime and writes `DK2VR-host.log`.
- **Game folder looks odd after a crash** — run `DK2VR\RESTAURER_JEU_ORIGINAL.cmd`.
- **Antivirus warning** — the mod works like a DirectDraw wrapper (`ddraw.dll` placed next to the game for the VR session only). Allow the `DK2VR` folder.
- **Asking for help** — close the game and run `DK2VR\COLLECTER_RAPPORT.cmd`: it builds a ZIP with logs and settings (no screenshots; user name, computer name and profile path removed; no game file). Attach it to a [GitHub issue](https://github.com/lovemakerz/Dungeon-Keeper-2-VR-MOD/issues) if you want.

## Performance & privacy

The public build contains **no measurement probe, timing, screenshot capture or diagnostic file** — only ordinary operation logs. On the first launch the launcher script downloads one small loading animation (a GIF from tenor.com) into `DK2VR\Textures`; without it, the loading screen shows the gauge only. Apart from that single download, DK2VR's own code makes no network connection.

## Known limits

- Dungeon Keeper 2 **v1.70 only**. Validated on **Quest 3 + Virtual Desktop** only.
- `Y` does nothing during a possession (Escape made the game misbehave).
- Not yet tested by many people — please report problems with a report ZIP.

## Credits & licences

Made by **LoVeMaKeRz** — <https://github.com/lovemakerz/Dungeon-Keeper-2-VR-MOD>

Dungeon Keeper 2 belongs to its owners; this is an unofficial fan project, not affiliated with or endorsed by them. Third-party components and their licences are in `DK2VR\Licences` (DxWrapper, OpenXR SDK); `openxr_loader.dll` comes from the OpenXR SDK.
