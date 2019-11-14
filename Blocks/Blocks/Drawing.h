#ifndef __Drawing_h__
#define __Drawing_h__

#include "Config.h"

#define pixelBits 24

typedef enum _DrawType DrawType;
typedef struct _PixelStruct PixelStruct;

typedef enum _DrawType {
	DT_GDI,
	DT_DIB,
	DT_DX
} DrawType;

typedef struct _PixelStruct {
	byte blue, green, red;
} PixelStruct;

#endif // __Drawing_h__
