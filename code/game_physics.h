#if !defined(GAME_PHYSICS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: All Rights Reserved. $
   ======================================================================== */
#define TINYC2_IMPL
#include <string.h> // for memcpy
#include "tinyc2.h"

struct minkowskirect{

    v2 Center;
    v2 Dim;
};


internal
minkowskirect MinkowskiSum(entity* A,entity* B){
    minkowskirect Result = {};
    Result.Center = (A->Pos - B->Pos);
    Result.Dim = (A->Dim + B->Dim);
    return Result;
}


internal
b32 CheckCollision(entity* Test,entity* A){
    b32 Result = false;
#if 1
    rectangle2 TRect = RectCenterHalfDim(Test->Pos,Test->Dim);
    rectangle2 ARect = RectCenterHalfDim(A->Pos,A->Dim);
    if(RectanglesIntersect(TRect,ARect)){
    minkowskirect Rect = MinkowskiSum(A,Test);

 if(AbsoluteValue(Rect.Center.x) < Rect.Dim.x && AbsoluteValue(Rect.Center.y) < Rect.Dim.y){
       /* collision! */
     float wy =Rect.Dim.x * Rect.Center.y;
    float hx = Rect.Dim.y * Rect.Center.x;

    #define CollideTop 1
    #define CollideLeft 2
    #define CollideRight 3
    #define CollideBottom 4
    
    if (wy > hx)
        if (wy > -hx)
            Result = CollideTop; /* collision at the top */
        else
            Result = CollideLeft; /* on the left */
    else
        if (wy > -hx)
            Result = CollideRight; /* on the right */
        else
            Result = CollideBottom; /* at the bottom */

    return Result;
    }
}
#else
    c2AABB A = { {A->Pos.x - A->Dim.x,A->Pos.y - A->Dim.y},{A->Pos.x + A->Dim.x,A->Pos.y + A->Dim.y} };
    c2AABB T = { {T->Pos.x - T->Dim.x,T->Pos.y - T->Dim.y},{T->Pos.x + T->Dim.x,T->Pos.y + T->Dim.y} };
    c2Manifold Manifold = {}; 
    c2AABBtoAABBManifold(A,T,&Manifold);
    if(Manifold.count){
    c2v CollisionPt = Manifold.contact_points[1];
        
    }
#endif
    return Result; 
}

#define GAME_PHYSICS_H
#endif
