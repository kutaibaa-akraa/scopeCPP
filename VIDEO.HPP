#ifndef VIDEO_HPP
#define VIDEO_HPP

// courtesy Dale Eason
/* The PC's screen in text mode consists of 25 rows and 80 columns.
The first row and column is addressed as location (0,0), and the last
row and column as location (24,79).  Each screen location actually
consists of two components: a character and an attribute.  To support
this arragement, a structure called 'Texel' is used to represent
the character and the attribute written to a screen location. */

// as in 'text' element, analagous to 'pixel' = picture element
struct Texel
{
	char Ch;
	unsigned char Attr;
};

/* Using this structure we can define a two-dimensional array to
correspond to the memory arrangement of the screen: */

typedef struct Texel ScreenArea[25][80];

/* To write a character and its attribute to a screen location, a screen
pointer is used: ScreenArea far *ScreenPtr; */

/* This screen pointer can be set to one of two values: B000:000h for a
monochrome display adapter and B800:000h for a graphics adapter.
To determine which adapter is installed, use a call to the
ROM-BIOS(interrupt 0x10). */

/* Once ScreenPtr has been assigned a screen address, a character and an
attribute can be written to the screen.  But first, to simplify the notation,
we've defined the macro constant: */

#define Screen (*ScreenPtr)

/* Putting all this together, we can write the character A with normal video
to row 6, column 11 using:

	  Screen[5][10].Ch = 'A';
	  Screen[5][10].Attr = 7;

We can also use this notation to read a character and its attributes at
a specific screen location:

	 Ch = Screen[5][10].Ch;
	 Attr = Screen[5][10].Attr; */

class Video
{
	public:
		Video(void);

		char Buffer[80];
		int Row,Column;
		unsigned Segment,Offset;
		ScreenArea far *ScreenPtr;

		// write a character string using Buffer to the screen at Row and Column
		void WriteScreen(void);
		void Test(void);
};

#endif