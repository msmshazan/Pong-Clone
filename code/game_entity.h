#if !defined(GAME_ENTITY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */

enum entity_flag{
    None = 0,
    PlayerFlag = (1 << 0),
    EnemyFlag = (1 << 1),
    BallFlag = (1 << 2),
    WallFlag = (1 << 3)
};

struct entity{
    entity_flag Flag = None;
    v2 Pos;    // Position
    v2 dPos;   // Velocity
    v2 ddPos;  // Acceleration
    v2 Dim;
    v4 Color;
    real64 Health;
};

#define GAME_ENTITY_H
#endif
