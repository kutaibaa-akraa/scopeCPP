#ifndef PPORT_HPP
#define PPORT_HPP

#include "common.h"

/*
8 bits out:
parallel port 1 portid lpt1=0x378, lpt2=0x278, monochrome video card=0x3BC,
25-pin connector pins 2 (least significant) through 9 (most significant),
logical low in produces logical low out;

4 bits in:
portid 0x379 (or try 0x279 or 0x3BD):

	16	32	64	128	bit
	H	H	H	H		in
	13	12	10	11		25-pin connector pin #
	H	H	H	L		result, eg, inverted for bit 128;

	module logic rectifies the inverted bit;

4 bits in or out:
portid 0x37A (or try 0x27A or 0x3BE):

	1	2	4	8		bit
	H	H	H	H		in or out
	1	14	16	17		25-pin connector pin #
	L	L	H	L		result, eg, inverted for all bits except for bit 4;

	module logic rectifies the inverted bits;

remaining pin connections:
25-pin connector pins 18 through 25 are grounds;
*/

class PPort
{
	public:
		PPort(const unsigned PPortAddr);
		~PPort(void);

		unsigned PPortAddrOutByte;
		unsigned PPortAddrInNibble;
		unsigned PPortAddrInOutNibble;
		Byte OutByteValue;
		Byte BiDirOutNibbleValue;

		void OutByte(void);
		void BiDirOutNibble(void);
		Byte InNibble(void);
		Byte BiDirInNibble(void);
		void Test(void);
};

#endif