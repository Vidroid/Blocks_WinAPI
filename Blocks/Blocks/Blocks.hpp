#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "Drawing.h"

#ifndef byte
typedef unsigned char byte;
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#ifndef null
#define null 0
#endif

#define IntPoint(x,y) D2D1::Point2F((FLOAT)(x), (FLOAT)(y))
#define IntRect(left,top,right,bottom) D2D1::RectF((FLOAT)(left), (FLOAT)(top), (FLOAT)(right), (FLOAT)(bottom))
#define BlockRect(block) IntRect(block->x, block->y, block->x + block->width, block->y + block->height + blockHeaderHeight)
#define HeaderRect(block) IntRect(block->x, block->y, block->x + block->width, block->y + blockHeaderHeight)
#define BodyRect(block) IntRect(block->x, block->y + blockHeaderHeight, block->x + block->width, block->y + block->height + blockHeaderHeight)
#define CloseRect(block) IntRect(block->x + block->width - closeButtonSize, block->y, block->x + block->width, block->y + blockHeaderHeight)
#define IntEllipse(center,x,y) D2D1::Ellipse(center, (FLOAT)(x), (FLOAT)(y))

typedef enum _DrawType {
	DT_GDI,
	DT_DIB,
	DT_DX
} DrawType;

typedef struct _Connector Connector;

typedef struct _InPort {
	Connector *connector;
	wchar_t *name;
} InPort;

typedef struct _OutPort {
	Connector *connector;
	wchar_t *name;
} OutPort;

typedef struct _Ports {
	int inPortsCount;
	int outPortscount;
	InPort *in;
	OutPort *out;
} Ports;

typedef struct _Connector {
	wchar_t *name;
	InPort *in;
	OutPort *out;
} Connector;

typedef struct _Block {
	int x, y, width, height;
	wchar_t *headerString;
	bool isSelected, isDragging;
	Ports *ports;
} Block;

typedef struct _Space {
	int blockCount;
	Block *blocks;
} Space;

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