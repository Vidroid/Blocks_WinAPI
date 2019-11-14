#include "Config.h"
#include "BlockStructs.hpp"

typedef struct _InPort {
	Connector *connector;
	wchar_t *name;
} InPort;

typedef struct _OutPort {
	Connector *connector;
	wchar_t *name;
} OutPort;

typedef struct _Ports {
	int inPortsCount, outPortsCount;
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
	Ports ports;
} Block;

typedef struct _Space {
	int blockCount;
	Block *blocks;
} Space;
