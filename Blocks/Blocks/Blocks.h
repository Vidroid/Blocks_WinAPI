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

typedef enum {
	DT_GDI,
	DT_DIB,
	DT_DX
} DrawType;

typedef struct {
	wchar_t *name;
} InPort;

typedef struct {
	wchar_t *name;
} OutPort;

typedef struct {
	wchar_t *name;
	InPort *in;
	OutPort *out;
} Connector;

typedef struct {
	InPort *in;
	OutPort *out;
} Ports;

typedef struct {
	int x, y, width, height;
	wchar_t *headerString;
	Ports *ports;
} Block;

typedef struct {
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