#ifndef HANDPAD_HPP
#define HANDPAD_HPP

#include "common.h"

/*
HANDPAD LAYOUT

handpad uses 4 data lines, each line tied to a bit from the parallel port,
attach bit 16 to pin 13 of the parallel port,
attach bit 32 to pin 12 of the parallel port,
attach bit 64 to pin 10 of the parallel port,
attach bit 128 to pin 11 of the parallel port,

the handpad also needs +5V DC

use a 9-pin serial connector cable to attach the handpad to the pc

when a button(s) is pressed, +5V DC is applied to the appropriate bit(s),
otherwise the bit(s) is left pulled to ground

all bits are pulled to ground with 220 ohm resistors

use diodes positioned just after the switches and buttons to ensure that 
only the bits desired are raised to +5V DC

upper part of handpad is a 3 way switch, and is wired in the following
manner:
switch position #1, for initiliazing position #1, activates bits 16, 64
switch position #2, neutral - does nothing, activates nothing
switch position #3, for initiliazing position #2, activates bits 32, 64
switch also used for other functions via keyboard direction;

lower part of handpad consists of 4 momentary on buttons in the following
pattern with a 2 way switch in the middle:
	*
*  S  *
	*

upper button=Up, activates bit 16
lower button=Down, activates bit 32
left button=CCW, activates bit 64
right button=CW, activates bits 16, 32

switch position #1, slow microstepping, activates bit 128
switch position #2, fast halfstepping, activates nothing
*/

class Handpad : public PPort
{
	public:
		Handpad(const unsigned PPortAddr);

		// return value from handpad
		int HandPad;
		// value of HandPad at start of move
		int InitHandPad;
		// if handPad changes during move, HandPadOKFlag set to No
		int HandPadOKFlag;

		void ReadHandPad(void);
		void SetHandPadOKFlag(void);
		void Test(void);
};

#endif
