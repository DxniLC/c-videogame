#pragma once

#include <cstdint>

enum EntityType : uint32_t
{
    NONE = 0,
    PLAYER = 1 << 1,
    BOX = 1 << 2,
    WORLD = 1 << 3,
    CAMERA = 1 << 4,
    PLATE = 1 << 5,
    PLATFORM = 1 << 6,
    CONSUMABLE = 1 << 7,
    CHECKPOINT = 1 << 8,
    FALL_VOID = 1 << 9,
    SPIKES = 1 << 10,
    PROJECTILE = 1 << 11,
    CLIMBING_WALL = 1 << 12,
    LOAD_LEVEL = 1 << 13,
    LOAD_CINEMATIC = 1 << 14,
    NPC = 1 << 15,
    CHEST = 1 << 16,
    SHOW_TUTORIAL_BOARD = 1 << 17,
    HIDE_TUTORIAL_BOARD = 1 << 18,
    NPCFLOOR = 1 << 19,
    DIVINE_OBJECT = 1 << 20,
    SNOW_BALL = 1 << 21,

    // ALL bits to 1;
    END_ENTITY_TYPE
};