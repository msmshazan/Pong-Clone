#if !defined(GAME_RENDER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Mohamed Shazan $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#include "game_memory.h"

#include "stb_rect_pack.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC 
#include "stb_truetype.h" // NOTE: Using STB_truetype :  why not?

// NOTE: (TODO):Load fonts as bitmaps because ease

#define BITMAP_BYTES_PER_PIXEL 4

struct loaded_bitmap {
    void *Memory;
    s32 Width;
    s32 Height;
    // TODO: Get rid of pitch!
    s32 Pitch;
};

internal void
ClearBitmap(loaded_bitmap *Bitmap) {
    if (Bitmap->Memory) {
    int32 TotalBitmapSize = Bitmap->Width * Bitmap->Height * BITMAP_BYTES_PER_PIXEL;
        ZeroSize(TotalBitmapSize, Bitmap->Memory);
    }
}

internal void
ResizeBitmap(loaded_bitmap *InBitmap,int Width,int Height){
    u8* Dupe = (u8 *)malloc(Width*Height*4);
    stbir_resize_uint8((u8 *)InBitmap->Memory,InBitmap->Width,InBitmap->Height,0,
                       Dupe,Width,Height,0,4);
    Copy(Width*Height*4,Dupe,InBitmap->Memory);
    InBitmap->Width = Width;
    InBitmap->Height = Height;
    free(Dupe);
    
}

internal loaded_bitmap
MakeEmptyBitmap(memory_arena *Arena, int32 Width, int32 Height, bool32 ClearToZero = true) {
    loaded_bitmap Result = {};

    Result.Width = Width;
    Result.Height = Height;
    Result.Pitch = Result.Width * BITMAP_BYTES_PER_PIXEL;
    int32 TotalBitmapSize = Width * Height * BITMAP_BYTES_PER_PIXEL;
    Result.Memory = PushSize(Arena, TotalBitmapSize, 16);
    if (ClearToZero) {
        ClearBitmap(&Result);
    }

    return Result;
}

internal loaded_bitmap LoadBitmap(const char* Filename ,memory_arena *Arena){
    int Width,Height,Bytes_per_pixel;
    loaded_bitmap Result = {};
    void *Data = stbi_load(Filename, &Width, &Height, &Bytes_per_pixel,BITMAP_BYTES_PER_PIXEL);
    if(Data){
        Result.Width = Width;
        Result.Height = Height;
        Result.Pitch = Width*BITMAP_BYTES_PER_PIXEL;
        memory_index Size = Width*Height*BITMAP_BYTES_PER_PIXEL;
        Result.Memory =PushCopy(Arena,Size,Data);
        stbi_image_free(Data);
    }
    return Result;
}

internal void
DrawBitmap(loaded_bitmap *Buffer, loaded_bitmap *Bitmap,
           real32 RealX, real32 RealY, real32 Alpha = 1.0)
{

    int MinX = RoundReal32ToInt32(RealX);
    int MinY = RoundReal32ToInt32(RealY);
    int MaxX = MinX + Bitmap->Width;
    int MaxY = MinY + Bitmap->Height;

    int32 SourceOffsetX = 0;
    if (MinX < 0) {
        SourceOffsetX = -MinX;
        MinX = 0;
    }

    int32 SourceOffsetY = 0;
    if (MinY < 0) {
        SourceOffsetY = -MinY;
        MinY = 0;
    }

    if (MaxX > Buffer->Width) {
        MaxX = Buffer->Width;
    }

    if (MaxY > Buffer->Height) {
        MaxY = Buffer->Height;
    }

    uint8 *SourceRow = (uint8 *)Bitmap->Memory + SourceOffsetY * Bitmap->Pitch + BITMAP_BYTES_PER_PIXEL * SourceOffsetX;
    uint8 *DestRow = ((uint8 *) Buffer->Memory +
                      MinX * BITMAP_BYTES_PER_PIXEL +
                      MinY * Buffer->Pitch);
    for (int32 Y = MinY; Y < MaxY; ++Y) {
        uint32 *Dest = (uint32 *) DestRow;
        uint32 *Source = (uint32 *) SourceRow;
        for (int32 X = MinX; X < MaxX; ++X) {
         v4 Result = { (real32)((*Source >> 0) & 0xFF),
                         (real32)((*Source >> 8) & 0xFF),
                         (real32)((*Source >> 16) & 0xFF),
                         (real32)((*Source >> 24) & 0xFF) };
            
            *Dest = (((uint32)(Result.a + 0.5f) << 24) |
                     ((uint32)(Result.r + 0.5f) << 16) |
                     ((uint32)(Result.g + 0.5f) << 8) |
                     ((uint32)(Result.b + 0.5f) << 0));

            *Source = (uint32)(RoundReal32ToUInt32(Alpha*255.0f)<<24);
            ++Dest;
            ++Source;
        }
        DestRow += Buffer->Pitch;
        SourceRow += Bitmap->Pitch;
    }
}

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
    uint32* Pixel = (uint32 *)Row;
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
