#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <iomanip.h>

#include "common.h"
#include "video.hpp"

// routines courtesy Dale Eason
Video::Video(void)
{
	union REGS Regs;

	cout<<endl<<"video mode: ";
	Regs.h.ah=15;
	int86(0x10,&Regs,&Regs);
	if(Regs.h.al==7)
	{
		Segment=0xB000;
		cout<<"monochrome";
	}
	else
	{
		Segment=0xB800;
		cout<<"graphics adaptor";
	}
	cout<<endl;
	Offset=Regs.h.bh*(unsigned)0x1000;
	ScreenPtr=(ScreenArea far*)(((long)Segment<<16) |(long)Offset);
}

void Video::WriteScreen(void)
{
	char* StrPtr=Buffer;

	while(*StrPtr)
		Screen[Row][Column++].Ch=*StrPtr++;
}


void Video::Test(void)
{
	int X,Y;
	int i;
	time_t t;
	time_t start;

	cout<<endl<<endl<<"Test of video module:"<<endl;
	cout<<"segment "<<Segment<<"   offset "<<Offset<<endl<<endl;

	directvideo=1;
	X=wherex();
	Y=wherey();
	cout<<"To display ";

	// my turn
	start=time(NULL);
	for(i=1;i<=30000;i++)
	{
		// my direct video
		sprintf(Buffer,"%6d",i);
		Column=X+11;
		Row=Y-1;
		WriteScreen();
	}
	t=time(NULL);
	gotoxy(X+20,Y);
	cout<<" times";
	gotoxy(X,Y+1);
	cout<<"Took my code "<<(t-start)<<" seconds";

	// Borland's turn
	start=time(NULL);
	for(i=1;i<=30000;i++)
	{
		gotoxy(X+12,Y);
		cout<<setw(6)<<i;
	}
	t=time(NULL);
	gotoxy(X,Y+2);
	cout<<"Took Borland with direct video "<<(t-start)<<" seconds";

	// direct video off */
	directvideo=0;
	start=time(NULL);
	for(i=1;i<=30000;i++)
	{
		gotoxy(X+12,Y);
		cout<<setw(6)<<i;
	}
	t=time(NULL);
	gotoxy(X,Y+3);
	cout<<"Borland w/o direct video "<<(t-start)<<" seconds";

	cout<<endl<<ContMsg;
	getch();
}
