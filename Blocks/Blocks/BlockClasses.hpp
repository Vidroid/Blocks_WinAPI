#ifndef __BlockClasses_hpp__
#define __BlockClasses_hpp__

namespace Block
{

	class Space;
	class Block;
	class Ports;
	class Port;
	class InPort;
	class OutPort;
	class Connector;

	class Space
	{
	public:
		Space();
		~Space();
		int blockCount;
		Block *blocks;
	};

	class Block
	{
	public:
		Block();
		~Block();
		int x, y, width, height;
		wchar_t *headerString;
		bool isSelected, isDragging;
		struct {
			int inPortsCount, outPortsCount;
			InPort *in;
			OutPort *out;
		} ports;
	};

	class Port
	{
	public:
		Port();
		~Port();
		Connector *connector;
		wchar_t *name;
	};

	class InPort : public Port
	{
	public:
		InPort();
		~InPort();
	};

	class OutPort : public Port
	{
	public:
		OutPort();
		~OutPort();
	};

	class Connector
	{
	public:
		Connector();
		~Connector();
		wchar_t *name;
		InPort *in;
		OutPort *out;
	};

}

#endif // __BlockClasses_hpp__
