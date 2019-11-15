#ifndef __BlockClasses_hpp__
#define __BlockClasses_hpp__

namespace Blocks
{

	typedef class _Space Space;
	typedef class _Block Block;
	typedef class _Port Port;
	typedef class _InPort InPort;
	typedef class _OutPort OutPort;
	typedef class _Connector Connector;

	typedef class _Space
	{
	public:
		int blockCount;
		Block *blocks;
	} Space;

	typedef class _Block
	{
	public:
		int x, y, width, height;
		wchar_t *headerString;
		bool isSelected, isDragging;
		struct {
			int inPortsCount, outPortsCount;
			InPort *in;
			OutPort *out;
		} ports;
	} Block;

	typedef class _Port
	{
	public:
		Connector *connector;
		wchar_t *name;
	} Port;

	typedef class _InPort : public Port
	{
	} InPort;

	typedef class _OutPort : public Port
	{
	} OutPort;

	typedef class _Connector
	{
	public:
		wchar_t *name;
		InPort *in;
		OutPort *out;
	} Connector;

}

#endif // __BlockClasses_hpp__
