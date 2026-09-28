#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>

namespace dk2vr {
constexpr uint32_t Magic = 0x32564b44;
// Version 2 (DK2VR 0.2.0): Demeo-style table pose, table width and hand poses.
// The game DLL and the OpenXR host must always be shipped together.
constexpr uint32_t ProtocolVersion = 3; // 0.6.0: pointer fields
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
// Request::pointerFlags (0.6.0)
constexpr uint32_t PointerActive = 1u << 0;
constexpr uint32_t PointerLeft = 1u << 1;
constexpr uint32_t PointerRight = 1u << 2;
// Slot::cameraMode bit (0.3.0): image drawn by the decoupled VR thread at the headset rate.
constexpr uint32_t CameraModeDecoupled = 1u << 31;
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
};
struct Shared {
    uint32_t magic, version, size, hostPid;
    volatile LONG hostAlive;
    volatile LONG gameAlive;
    volatile LONG requestSerial;
    uint32_t reserved;
    Request request; // protected by named mutex
    Slot slots[SlotCount];
};
#pragma pack(pop)
static_assert(sizeof(Pose) == 28 && sizeof(Eye) == 44, "Protocol layout");
static_assert(offsetof(Slot, handles) == 16, "Cross-architecture alignment");
static_assert(sizeof(Slot) == 192, "Cross-architecture slot size");
static_assert(offsetof(Request, tableAnchor) == 156, "Version 1 prefix preserved");
static_assert(sizeof(Request) == 304, "Cross-architecture request size");
static_assert(offsetof(Shared, slots) == 336, "Cross-architecture slot offset");
static_assert(sizeof(Shared) == 912, "Cross-architecture mapping size");
}
