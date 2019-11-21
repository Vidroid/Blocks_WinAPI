#ifndef __BlockClasses_hpp__
#define __BlockClasses_hpp__

#include "Config.h"

namespace Blocks
{
	
	const int defaultBlockWidth = 120;
	const int defaultBlockHeight = 120;
	wchar_t *const defaultBlockHeaderString = L"Block!";

	typedef enum _BlockClickResult BlockClickResult;

	class Space;
	class Block;
	class Port;
	class InPort;
	class OutPort;
	class Connector;

}

#endif // __BlockClasses_hpp__
