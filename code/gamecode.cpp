/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */
#include "game_platform.h"
#include "game_math.h"
#include "game_entity.h"
#include "game_physics.h"
#include "game_render.h"
#include "gamecode.h"

internal
entity* CreateEntity(game_state* GameState,entity_flag Flag){
    entity* Result = 0;
    b32 set = false;
    for(int i=0;
        i < ArrayCount(GameState->Entity);
        i++){
        if(GameState->Entity[i].Flag == 0 && set == false) {
            GameState->Entity[i].Flag = Flag;
            Result = &GameState->Entity[i];
            set = true;
        }        
    }
    return Result;
}

internal
void InitializeWall(entity* Wall,r32 X,r32 Y,r32 Width,r32 Height){
    if(Wall->Flag == WallFlag){
        Wall->Dim.x = Width;
        Wall->Dim.y = Height;
        Wall->Pos.x = X;
        Wall->Pos.y = Y;
        Wall->dPos = {};
        Wall->ddPos.x = 0.0f; 
        Wall->ddPos.y = 0.0f; 
        
    }
}

internal
void RenderEntity(entity *Entity,rectangle2i BufferRect,game_offscreen_buffer* Buffer){
    rectangle2 EntityRect = RectCenterHalfDim(Entity->Pos,Entity->Dim);
    if(!(Entity->Dim.x == Entity->Dim.y)){
        DrawRectangle(BufferRect,EntityRect.Min,EntityRect.Max,Buffer,Entity->Color.r,Entity->Color.g,Entity->Color.b);    
    }else{
        DrawFilledCircle(Entity->Pos.x,Entity->Pos.y,Entity->Dim.x,Buffer,Entity->Color.r,Entity->Color.g,Entity->Color.b);    
    }
}

internal
v2 MoveEntity(entity *Entity,game_input* Input){
    v2 Result = {};
    Entity->dPos.x *= Entity->ddPos.x;
    Result.x = Entity->Pos.x+Input->dtForFrame*Entity->dPos.x;
    Entity->dPos.y *= Entity->ddPos.y;
    Result.y = Entity->Pos.y+Input->dtForFrame*Entity->dPos.y;
    Entity->dPos = {};
    return Result;
}

extern "C"
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{

#if 1
    #define OffsetX 0
    #define OffsetY 100
    rectangle2 BufferRect = {{OffsetX,OffsetY},{Buffer->Width-1,Buffer->Height-1}};
    rectangle2i _BufferRect = {OffsetX,OffsetY,(int32)Buffer->Width-1,(int32)Buffer->Height-1};
#else
    rectangle2 BufferRect = {{0,0},{800,600}};
    rectangle2i _BufferRect = {0,0,800,600};
#endif
    DrawRectangle(_BufferRect,BufferRect.Min,BufferRect.Max,Buffer,0,0,0); // Clear Screen to black

    //NOTE: Gamecode comes here
        
    game_state *GameState = (game_state *) Memory->PermanentStorage;
    
    if(!Memory->IsInitialized)
    {
        Memory->IsInitialized = true;
        // NOTE: Player 
        GameState->Player= CreateEntity(GameState,PlayerFlag);        
        GameState->Player->Dim.x = 10.0f;
        GameState->Player->Dim.y = 20.0f;
        GameState->Player->Pos.x = BufferRect.Min.x + GameState->Player->Dim.x;
        GameState->Player->Pos.y = BufferRect.Min.y + GameState->Player->Dim.y;
        GameState->Player->Color.r= 0.1f;
        GameState->Player->Color.g= 0.1f;
        GameState->Player->Color.b= 0.5f;
        GameState->Player->dPos = {};
        GameState->Player->ddPos.x = 410.0f; 
        GameState->Player->ddPos.y = 410.0f; 

        //NOTE: Enemy
        
        GameState->Enemy = CreateEntity(GameState,EnemyFlag);    
        GameState->Enemy->Dim.x = 10.0f;
        GameState->Enemy->Dim.y = 20.0f;
        GameState->Enemy->Pos.x = BufferRect.Max.x - GameState->Player->Pos.x -GameState->Enemy->Dim.x;
        GameState->Enemy->Pos.y = BufferRect.Max.y - GameState->Player->Pos.y -GameState->Enemy->Dim.y;
        GameState->Enemy->Color.r= 0.5f;
        GameState->Enemy->Color.g= 0.1f;
        GameState->Enemy->Color.b= 0.1f;
        GameState->Enemy->dPos = {};
        GameState->Enemy->ddPos.x = 410.0f; 
        GameState->Enemy->ddPos.y = 410.0f; 

        // NOTE: Ball
        
        GameState->Ball = CreateEntity(GameState,BallFlag);
        GameState->Ball->Pos.x = 500.0f;
        GameState->Ball->Pos.y = 500.0f;
        GameState->Ball->Dim.x = 10.0f;
        GameState->Ball->Dim.y = 10.0f;
        GameState->Ball->Color.r= 0.1f;
        GameState->Ball->Color.g= 0.1f;
        GameState->Ball->Color.b= 0.5f;
        GameState->Ball->dPos = {};
        GameState->Ball->ddPos.x = 110.0f; 
        GameState->Ball->ddPos.y = 120.0f;

        // NOTE: Walls
        
        GameState->WallTop = CreateEntity(GameState,WallFlag);
        InitializeWall(GameState->WallTop,BufferRect.Max.x/2,BufferRect.Min.y-1,BufferRect.Max.x/2,1);
        GameState->WallBottom = CreateEntity(GameState,WallFlag);
        InitializeWall(GameState->WallBottom,BufferRect.Max.x/2,BufferRect.Max.y + 1,BufferRect.Max.x/2,1);
        GameState->WallLeft = CreateEntity(GameState,WallFlag);
        InitializeWall(GameState->WallLeft,BufferRect.Min.x-1,BufferRect.Max.y/2,1,BufferRect.Max.y/2);
        GameState->WallRight = CreateEntity(GameState,WallFlag);
        InitializeWall(GameState->WallRight,BufferRect.Max.x + 1 ,BufferRect.Max.y/2,1,BufferRect.Max.x/2);
    }

    entity* Player = GameState->Player;
    entity* Enemy = GameState->Enemy;
    entity* Ball = GameState->Ball;

    entity* WallTop = GameState->WallTop;
    entity* WallLeft = GameState->WallLeft;
    entity* WallRight = GameState->WallRight;
    entity* WallBottom = GameState->WallBottom;

    for(int Controllerindex =0;
        Controllerindex < 1;
        Controllerindex++)
    {
        
        // NOTE: Movement Code
           {
                //NOTE: Moving the ball...
               if(AbsoluteValue(Ball->ddPos.x) < 100.0f) Ball->ddPos.x = SignOf(Ball->ddPos.x)*110.f;
               if(AbsoluteValue(Ball->ddPos.y) < 100.0f) Ball->ddPos.y = SignOf(Ball->ddPos.x)*110.f;
                Ball->dPos.x = 1.0f;
                Ball->dPos.y = 1.0f;
            }

        game_controller_input *Controller = GetController(Input,Controllerindex);
        if(Controller->IsAnalog)
        {
            // NOTE: Handle Controller input
            
        }
        else{

            if(Controller->MoveUp.EndedDown)
            {
                Player->dPos.y = -1.0f;
                Enemy->dPos.y = 1.0f;
            }
            if(Controller->MoveDown.EndedDown)
            {
                Player->dPos.y = 1.0f;
                Enemy->dPos.y = -1.0f;
            }
            if(Controller->MoveLeft.EndedDown)
            {
                Player->dPos.x = -1.0f;
                Enemy->dPos.x = 1.0f;
            }
            if(Controller->MoveRight.EndedDown)
            {
                Player->dPos.x = 1.0f;
                Enemy->dPos.x = -1.0f;
            }

            {
                // NOTE:No X movement in pong unless not classic pong.... 
                Player->ddPos.x = 0;
                Enemy->ddPos.x = 0;
            }

            
            // NOTE: Physics
            // TODO: Collision detection

            
            v2 OldPos = {};
            OldPos = Player->Pos;
            Player->Pos = MoveEntity(Player,Input);
            if(CheckCollision(Player,Ball)|CheckCollision(Player,Enemy)||CheckCollision(Player,WallRight)|CheckCollision(Player,WallLeft)|CheckCollision(Player,WallTop)|CheckCollision(Player,WallBottom))
            {
                Player->Pos = OldPos;
                
            }
            OldPos = Enemy->Pos;
            Enemy->Pos = MoveEntity(Enemy,Input);
            if(CheckCollision(Enemy,Ball)|CheckCollision(Enemy,Player)|CheckCollision(Enemy,WallRight)|CheckCollision(Enemy,WallLeft)|CheckCollision(Enemy,WallTop)|CheckCollision(Enemy,WallBottom))
            {
                Enemy->Pos = OldPos;
            }
            Ball->Pos = MoveEntity(Ball,Input);
            b32 BallCollided = CheckCollision(Player,Ball)|CheckCollision(Enemy,Ball)|CheckCollision(WallRight,Ball)|CheckCollision(WallLeft,Ball)|CheckCollision(WallTop,Ball)|CheckCollision(WallBottom,Ball);
            if(BallCollided) {
                switch (BallCollided){
                    case CollideTop:{
                    Ball->ddPos.y = -Ball->ddPos.y;
                    Ball->ddPos.x *= 0.999; 
                    }break;
                    case CollideBottom:{
                    Ball->ddPos.y = -Ball->ddPos.y; 
                    Ball->ddPos.x *= 0.999; 
                    }break;
                    case CollideLeft:{
                    Ball->ddPos.y *= 0.999; 
                    Ball->ddPos.x = -Ball->ddPos.x; 
                    }break;
                    case CollideRight:{
                    Ball->ddPos.y *= 0.999; 
                    Ball->ddPos.x = -Ball->ddPos.x; 
                    }break;
                    default: break;
                }
                
                Ball->Pos = MoveEntity(Ball,Input);
            }
        }
    }

    // NOTE: Renderer        
    {
        RenderEntity(Player,_BufferRect,Buffer);
        RenderEntity(Enemy,_BufferRect,Buffer);
        RenderEntity(Ball,_BufferRect,Buffer);
    }
    
}


extern "C"
GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *GameState = (game_state *) Memory->PermanentStorage;
}

