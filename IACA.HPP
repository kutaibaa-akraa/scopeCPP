#ifndef IACA_HPP
#define IACA_HPP

#include <dos.h>
#include "common.h"

/*
the Intra-Application Communication Area is a 16 byte area starting at
0040:00F0 that DOS sets aside to allow programs to communicate with each other;

this area is used to exchange coordinates between the scope control program
and graphically oriented CD-ROM planetarium programs such as Guide;

the planetarium program's coordinates serve as input to the scope,
the scope program tells the planetarium program where the scope is pointing;

coordinates are assumed to be precessed to the current date;

each program, at startup, sets all coordinates initially to zero: this way,
when non-zero coordinates are found, it can be assumed that the other program
put them there;

ie,
long iaca_ra = *(long far *)(0x4f0);
long iaca_dec = *(long far *)(0x4f4);
long scope_ra = *(long far *)(0x4f8);
long scope_dec = *(long far *)(0x4fc);
double scope_ra_in_decimal_hours = (double)scope_ra/1.e+7;
double scope_dec_in_decimal_degrees = (double)scope_dec / 1.e+7;
*/

#define PUT_SCOPE if(IACA_Flag) PutScope(Current.Ra,Current.Dec)

class IACA
{
	public:
		IACA(void);
		void PutScope(double RaRad,double DecRad);
		int New_IACA(void);
		void Get_IACA(double &RaRad,double &DecRad);
		void Test(void);
};

#endif
