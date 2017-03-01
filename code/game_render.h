#if !defined(GAME_RENDER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */


internal void DrawPixel(game_offscreen_buffer* Buffer,real32 X,real32 Y,real32 R,real32 G,real32 B,real32 A=1.0f){

    if(X<0)
    {
        X = 0;
    }

    if(Y<0)
    {
        Y = 0;
    }
    if(X>Buffer->Width)
    {
        X = (real32)Buffer->Width;
    }

    if(Y>Buffer->Height)
    {
        Y = (real32)Buffer->Height;
    }

// BIT PATTERN: 0x AA RR GG BB    
    uint32 Color =(uint32)((RoundReal32ToUInt32(R * 255.0f)<<16)|
                           (RoundReal32ToUInt32(G * 255.0f)<<8)|
                           (RoundReal32ToUInt32(B * 255.0f)<<0)|
                           (RoundReal32ToUInt32(A * 255.0f)<<24));

    uint8 *Row = (uint8 *) Buffer->Memory +
        (uint32)X* 4 +
        (uint32)Y * Buffer->Pitch;
    uint32 *Pixel = (uint32 *)Row;
    *Pixel = Color;
}

internal void DrawBresenhamLine(game_offscreen_buffer *Buffer,real32 x,real32 y,real32 x2, real32 y2,real32 R,real32 G,real32 B,real32 A =1.0f ) {
    real32 w = x2 - x ;
    real32 h = y2 - y ;         
    real32 dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0 ;
    if (w<0) dx1 = -1 ; else if (w>0) dx1 = 1 ;
    if (h<0) dy1 = -1 ; else if (h>0) dy1 = 1 ;
    if (w<0) dx2 = -1 ; else if (w>0) dx2 = 1 ;
    real32 longest = AbsoluteValue(w) ;
    real32 shortest = AbsoluteValue(h) ;
    if (!(longest>shortest)) {
        longest = AbsoluteValue(h) ;
        shortest = AbsoluteValue(w) ;
        if (h<0) dy2 = -1 ; else if (h>0) dy2 = 1 ;
        dx2 = 0 ;            
    }
    real32 numerator = longest / 2 ;
    for (real32 i=0;i<=longest;i++) {
        DrawPixel(Buffer,x,y,R,G,B,A);
        numerator += shortest ;
        if (!(numerator<longest)) {
            numerator -= longest ;
            x += dx1 ;
            y += dy1 ;
        } else {
            x += dx2 ;
            y += dy2 ;
        }
    }
}

internal void DrawRectangle(rectangle2i ClipRect,v2 vMin,v2 vMax,game_offscreen_buffer* Buffer, real32 R,real32 G,real32 B,real32 A =1.0f )
{

#define BITMAP_BYTES_PER_PIXEL 4
    rectangle2i FillRect;
    FillRect.MinX = RoundReal32ToInt32(vMin.x);
    FillRect.MinY = RoundReal32ToInt32(vMin.y);
    FillRect.MaxX = RoundReal32ToInt32(vMax.x);
    FillRect.MaxY = RoundReal32ToInt32(vMax.y);

    FillRect = Intersect(ClipRect, FillRect);

    uint32 Color =(uint32)((RoundReal32ToUInt32(R * 255.0f)|
                            (RoundReal32ToUInt32(G * 255.0f)<<8)|
                            (RoundReal32ToUInt32(B * 255.0f)<<16)|
                            (RoundReal32ToUInt32(A * 255.0f))<<24));

    uint8 *Row = (uint8 *) Buffer->Memory +
        (uint32)FillRect.MinX* BITMAP_BYTES_PER_PIXEL +
        (uint32)FillRect.MinY * Buffer->Pitch;    
    for (uint32 Y = (uint32)FillRect.MinY;
         Y <= (uint32)FillRect.MaxY;
         ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (uint32 X = (uint32)FillRect.MinX;
             X <= (uint32)FillRect.MaxX;
             ++X)
        {
            *Pixel++ = Color;
        }
        Row +=  Buffer->Pitch;
    }

}
#if 0
void DrawFilledCircle(real32 x0, real32 y0, real32 radius,game_offscreen_buffer* Buffer, real32 R,real32 G,real32 B,real32 A =1.0f)
{
real32 x = radius;
    real32 y = 0;
    real32 err = 0;
    
    while (x >= y)
    {
        DrawBresenhamLine(Buffer,x0+x,y0+y,x0+y,y0+x,R,G,B,A); 
        DrawBresenhamLine(Buffer,x0-y,y0+x,x0-x,y0+y,R,G,B,A); 
        DrawBresenhamLine(Buffer,x0-x,y0-y,x0-y,y0-x,R,G,B,A); 
        DrawBresenhamLine(Buffer,x0+y,y0-x,x0+x,y0-y,R,G,B,A); 
        
        y += 1;
        err += 1 + 2*y;
        if (2*(err-x) + 1 > 0)
        {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}
#endif
void DrawFilledCircle(real32 x0, real32 y0, real32 Radius,game_offscreen_buffer* Buffer, real32 R,real32 G,real32 B,real32 A =1.0f)
{
    int radius = (int) Radius;
    //Found from http://stackoverflow.com/a/1237519
    for(int y=-radius; y<=radius; y++){
        for(int x=-radius; x<=radius; x++){
            if(x*x+y*y <= radius*radius) // Consider equation of circle : x^2 + y^2 = r^2
                DrawPixel(Buffer,x0+x,y0+y,R,G,B,A);
        }
    }
}


#define GAME_RENDER_H
#endif
