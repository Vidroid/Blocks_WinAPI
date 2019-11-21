#ifndef __Blocks_hpp__
#define __Blocks_hpp__

#include "Config.h"
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "Drawing.h"
#ifndef USE_CLASSES
#include "BlockStructs.cpp"
#else
#include "BlockClasses.cpp"
#endif

using namespace Blocks;

#define IntPoint(x,y) D2D1::Point2F((FLOAT)(x), (FLOAT)(y))
#define IntRect(left,top,right,bottom) D2D1::RectF((FLOAT)(left), (FLOAT)(top), (FLOAT)(right), (FLOAT)(bottom))
#define BlockRect(block) IntRect(block->x, block->y, block->x + block->width, block->y + block->height + blockHeaderHeight)
#define HeaderRect(block) IntRect(block->x, block->y, block->x + block->width, block->y + blockHeaderHeight)
#define BodyRect(block) IntRect(block->x, block->y + blockHeaderHeight, block->x + block->width, block->y + block->height + blockHeaderHeight)
#define CloseRect(block) IntRect(block->x + block->width - closeButtonSize, block->y, block->x + block->width, block->y + blockHeaderHeight)
#define IntEllipse(center,x,y) D2D1::Ellipse(center, (FLOAT)(x), (FLOAT)(y))

typedef void (*InitProc)(void);
typedef void (*DrawProc)(Space *, void *);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void GDIInit(void);
void DIBInit(void);
void DXInit(void);
void GDIDraw(Space *, void *);
void DIBDraw(Space *, void *);
void DXDraw(Space *, void *);
void GDIDrawBlock(Block *);
void DIBDrawBlock(Block *);
void DXDrawBlock(Block *);

#endif // __Blocks_h__
