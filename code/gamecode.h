#if !defined(GAMECODE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */

#include "game_memory.h"

typedef struct game_state{

    entity Entity[8];
    entity* Player;
    entity* Enemy;
    entity* Ball;
    entity* WallTop;
    entity* WallBottom;
    entity* WallLeft;
    entity* WallRight;
    u32 Scores[4];
    u32* PlayerScore;
    u32* EnemyScore;
    memory_arena Arena;
} game_state;

struct transient_state {
    bool32 IsInitialized;
    loaded_bitmap Bitmap[25];
    loaded_bitmap DrawBufferModel;
    memory_arena TranArena;
};

#define GAMECODE_H
#endif
