#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>

namespace dk2vr {
constexpr uint32_t Magic = 0x32564b44;
// Version 2 (DK2VR 0.2.0): Demeo-style table pose, table width and hand poses.
// The game DLL and the OpenXR host must always be shipped together.
constexpr uint32_t ProtocolVersion = 18; // 0.9.0: tilting table (Request::tableTilt, leverFlags); 0.9.47.34: virtual keyboard (keyboardPose/Flags/Pressed/Hover)
constexpr unsigned SlotCount = 3;
struct Vec3 { float x, y, z; };
struct Quat { float x, y, z, w; };
struct Pose { Quat orientation; Vec3 position; };
struct Fov { float left, right, up, down; };
struct Eye { Pose pose; Fov fov; };
// handFlags bits
constexpr uint32_t HandLeftValid = 1u << 0;
constexpr uint32_t HandRightValid = 1u << 1;
constexpr uint32_t HandLeftGrip = 1u << 2;
constexpr uint32_t HandRightGrip = 1u << 3;
constexpr uint32_t HandLeftTrigger = 1u << 4;
constexpr uint32_t HandRightTrigger = 1u << 5;
// 0.3.2: the hand holds the table by one of its metal handles (the game ignores that grip).
constexpr uint32_t HandLeftTable = 1u << 6;
constexpr uint32_t HandRightTable = 1u << 7;
// 0.4.0: Request::floorY is valid (height of the real floor in LOCAL space, from the STAGE space).
constexpr uint32_t RequestFloorKnown = 1u << 8;
// 0.5.16: the game screen panel is shown in the headset (F3); otherwise the game screen is not copied in game.
constexpr uint32_t RequestPanelShown = 1u << 9;
// 0.6.0: the HUD modules are shown in the headset (the game screen must be copied).
constexpr uint32_t RequestHudShown = 1u << 10;
// 0.9.1: keyboard keys held on the controller (left X = SPACE, left Y = ESCAPE; right A / B for a left-handed player at the table). Layout unchanged.
constexpr uint32_t HandKeySpace = 1u << 11;
constexpr uint32_t HandKeyEscape = 1u << 12;
// Request::pointerFlags (0.6.0)
constexpr uint32_t PointerActive = 1u << 0;
constexpr uint32_t PointerLeft = 1u << 1;
constexpr uint32_t PointerRight = 1u << 2;
constexpr uint32_t PointerComfort = 1u << 20; // VR-room setting, never a game click
constexpr uint32_t PossessionAttack = 1u << 0;
constexpr uint32_t PossessionAbilityA = 1u << 1;
constexpr uint32_t PossessionAbilityB = 1u << 2;
constexpr uint32_t PossessionWheel = 1u << 3;
constexpr uint32_t PossessionRun = 1u << 4;
constexpr uint32_t PossessionLeave = 1u << 5;   // 0.9.47.39: left X = leave the possession (A = next skill = AbilityA, B = previous skill = AbilityB)
constexpr uint32_t ComfortSnapTurn = 1u << 0;
constexpr uint32_t ComfortVignette = 1u << 1;
constexpr uint32_t ComfortVignetteShift = 1;
constexpr uint32_t ComfortVignetteMask = 3u << ComfortVignetteShift;   // unused since 0.8.85 (vignette removed)
// 0.8.85: laser colour index (0..6) in bits 4..6, inverted map rotation in bit 8 of Request::comfortFlags
constexpr uint32_t ComfortLaserShift = 4;
constexpr uint32_t ComfortLaserMask = 7u << ComfortLaserShift;
constexpr uint32_t ComfortInvertTurn = 1u << 8;
constexpr uint32_t ComfortPanelOpen = 1u << 9;   // 0.8.86: options panel opened (OPTIONS button)
constexpr uint32_t ComfortPassthrough = 1u << 10; // 0.8.94.4: Virtual Desktop passthrough: room replaced by a flat colour (0.8.94.5: chosen with the R/G/B sliders, default 0,255,0)
constexpr uint32_t ComfortHideOrb = 1u << 11;   // 0.9.47.41: LEFT HAND ORB option = HIDE (the off-hand glow is hidden above the table, the old behaviour); default SHOW = always visible
constexpr uint32_t ComfortWallRight = 1u << 12;   // 0.9.47.42: options panel on the RIGHT wall (same side as the minimap pedestal); default = left wall
constexpr uint32_t ComfortControlsOpen = 1u << 13;     // 0.9.47.49: CONTROLS panel (controller guide) opened on the wall opposite to the options
constexpr uint32_t ComfortControlsFrench = 1u << 14;   // 0.9.47.49: CONTROLS panel language: 0 = English (default), 1 = French
constexpr uint32_t ComfortMirrorOn = 1u << 15;       // 0.9.47.61: PC mirror window (both eyes, 16:9) is ON (host-side window; the panel shows ON / OFF)
// Slot::cameraMode bit (0.3.0): image drawn by the decoupled VR thread at the headset rate.
constexpr uint32_t CameraModeDecoupled = 1u << 31;
// 0.8.88: Slot::rejectedDraws bit 31 = the image is the menu room (empty room + game screen on the front wall, drawn by the VR thread)
constexpr uint32_t SlotMenuFlag = 1u << 31;
// 0.8.92: Slot::rejectedDraws bit 30 = the game's own options / pause menu is open (the wall OPTIONS button is hidden and inactive)
constexpr uint32_t SlotGameMenuFlag = 1u << 30;
// 0.8.94.23: Slot::rejectedDraws bit 29 = a game panel (HUD module) lies IN FRONT of the wall options on the laser: the nearest panel wins
constexpr uint32_t SlotHudFrontFlag = 1u << 29;
// 0.9.47.42: Slot::rejectedDraws bit 28 = the minimap pedestal stands on the RIGHT of the table (the host puts the options panel on the same side)
constexpr uint32_t SlotPedestalRightFlag = 1u << 28;
// Fixed-width POD only: shared between a 32-bit game and a 64-bit OpenXR host.
#pragma pack(push, 8)
struct Request {
    uint32_t width, height;
    uint32_t adapterLow;
    int32_t adapterHigh;
    Eye eyes[2];
    Pose anchor;              // head yaw anchor captured at recenter (legacy table, possession, panel)
    uint32_t valid, recenterSerial;
    float unitsPerMeter, possessionUnitsPerMeter;
    float tablePitchDegrees;
    uint32_t mode; // 0 auto, 1 tabletop, 2 possession, 3 flat screen
    // ---- version 2 ----
    uint32_t tableAnchor;     // 1: dungeon placed on a fixed table (Demeo style), 0: legacy head-anchored view
    float tableWidthMeters;   // physical width of the visible dungeon rectangle on the table
    uint32_t handFlags;       // HandLeftValid ...
    float floorY;             // 0.4.0 (was reserved): floor height in LOCAL space when handFlags & RequestFloorKnown
    Pose table;               // table centre (floor of the dungeon) in LOCAL space, yaw only
    Pose hands[2];            // controller grip poses in LOCAL space (0 left, 1 right)
    // ---- version 3 (0.6.0) ----
    float pointerX, pointerY; // pointed position on the game screen, fraction of the back buffer (0..1)
    uint32_t pointerFlags;    // PointerActive, PointerLeft, PointerRight
    float rayLength;          // laser length in metres (0: no laser), drawn by the game DLL
    Vec3 rayOrigin, rayDirection; // laser in LOCAL space (aim pose of the pointing hand)
    float stickX, stickY;     // 0.6.2: thumbstick of the other hand (map scrolling), -1..1, dead zone applied
    float turnStick, zoomStick; // 0.8.19: thumbstick of the pointing hand: left/right = turn the dungeon, up/down = zoom (-1..1, dead zone applied)
    // ---- version 5 (0.8.31): only active while controlling a creature ----
    float possessionMoveX, possessionMoveY, possessionTurn;
    uint32_t possessionFlags, possessionSkill, comfortFlags; // possessionSkill carries the snap-angle readout in v6
    Pose wheelPose; // the skill wheel is disabled; 0.8.94.5: position.xyz = PASSTHROUGH VD colour (red, green, blue, 0..255), see passthroughRgb() in Comfort.hpp
    uint32_t comfortHover; // 0 none, else the id of the options-panel control under the laser (Comfort.hpp), 0xffffffff = panel between controls
    // ---- version 17 (0.9.0): tilting table ----
    float tableTilt;      // radians (0 = level, up to TiltMaxDeg): Request::table already CONTAINS the tilt (TableTilt.hpp); this value rebuilds the level pose
    uint32_t leverFlags;  // LeverHoverLeft ... (TableTilt.hpp)
    // ---- version 18 (0.9.47.34): VR virtual keyboard (Keyboard.hpp) ----
    Pose keyboardPose;         // panel pose in LOCAL space, fixed while the keyboard is open
    uint32_t keyboardFlags;    // KeyboardOpen, KeyboardShift
    uint32_t keyboardPressed;  // 0 none, else key index + 1 held down by the trigger (at least KbMinHoldMs)
    uint32_t keyboardHover;    // 0 none, else key index + 1 under the laser
    uint32_t keyboardReserved;
};
struct Slot {
    volatile LONG state; // 0 free, 1 producer, 2 ready, 3 consumer
    uint32_t generation;
    uint64_t frame;
    uint64_t handles[3]; // left, right, desktop (legacy shared D3D handles)
    uint32_t width, height, desktopWidth, desktopHeight;
    Eye eyes[2];
    Pose anchor;
    uint32_t worldDraws, uiDraws, rejectedDraws, cameraMode;
    float possessionTurn; // 0.8.67: commanded body turn (rad, Request::possessionTurn units) already contained in this image (possession prediction)
};
struct Shared {
    uint32_t magic, version, size, hostPid;
    volatile LONG hostAlive;
    volatile LONG gameAlive;
    volatile LONG requestSerial;
    float possessionGain; // 0.8.67 (was reserved): measured game turn per commanded turn (0 = unknown), written by the game DLL
    Request request; // protected by named mutex
    Slot slots[SlotCount];
};
#pragma pack(pop)
static_assert(sizeof(Pose) == 28 && sizeof(Eye) == 44, "Protocol layout");
static_assert(offsetof(Slot, handles) == 16, "Cross-architecture alignment");
static_assert(sizeof(Slot) == 192, "Cross-architecture slot size");
static_assert(offsetof(Request, tableAnchor) == 156, "Version 1 prefix preserved");
static_assert(sizeof(Request) == 420, "Cross-architecture request size"); // 0.9.0: +tableTilt +leverFlags; 0.9.47.34: +keyboard (28+16 bytes)
static_assert(offsetof(Shared, slots) == 456, "Cross-architecture slot offset");
static_assert(sizeof(Shared) == 1032, "Cross-architecture mapping size");
}
