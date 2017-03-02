#if !defined(GAMECODE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */

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
    
} game_state;

#define GAMECODE_H
#endif
