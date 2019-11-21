#include "Config.h"
#include <Windows.h>
#include "BlockClasses.hpp"

namespace Blocks
{

	typedef enum _BlockClickResult
	{

	} BlockClickResult;

	class Space
	{
	public:
		int blockCount;
		Block *blocks;
	};

	class Block
	{
	public:
		Block()
		{
			this->width = defaultBlockWidth;
			this->height = defaultBlockHeight;
			this->headerString = defaultBlockHeaderString;
		}
		Block(int x, int y, int width, int height, wchar_t *headerString)
		{

			this->x = x;
			this->y = y;
			this->width = width;
			this->height = height;
			this->headerString = headerString;
			this->isSelected = true;
			this->isDragging = false;
			this->ports.inPortsCount = 0;
			this->ports.outPortsCount = 0;
			this->ports.in = null;
			this->ports.out = null;

		}
		~Block()
		{

			if (ports.in) {
				for (int i = 0; i < this->ports.inPortsCount; i++) {
					//delete &this->ports.in[i];
				}
				free(ports.in);
			}
			if (ports.out) {
				for (int i = 0; i < this->ports.outPortsCount; i++) {
					//delete &this->ports.out[i];
				}
				free(ports.out);
			}

		}
		int x, y, width, height;
		wchar_t *headerString;
		bool isSelected, isDragging;
		struct
		{
			int inPortsCount, outPortsCount;
			InPort *in;
			OutPort *out;
		} ports;
		BlockClickResult Click(int x, int y);
	};

	class Port
	{
	public:
		wchar_t *name;
	};

	class InPort : public Port
	{
	public:
		InPort();
		~InPort()
		{
			
			if (connector) {
				free(connector);
			}
			if (name) {
				free(name);
			}

		}
		Connector *connector;
	};

	class OutPort : public Port
	{
	public:
		OutPort();
		~OutPort()
		{
			
			if (connector) {
				free(connector);
			}
			if (name) {
				free(name);
			}

		}
		Connector *connector;
	};

	class Connector
	{
	public:
		wchar_t *name;
		InPort *in;
		OutPort *out;
	};

}
