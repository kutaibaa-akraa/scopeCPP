#include <dos.h>
#include <conio.h>
#include <iostream.h>

#include "common.h"
#include "badexit.hpp"
#include "video.hpp"
#include "atimes.hpp"
#include "pport.hpp"
#include "handpad.hpp"

Handpad::Handpad(const unsigned PPortAddr): 
	PPort(PPortAddr)
{
	ReadHandPad();
}

void Handpad::ReadHandPad(void)
{
	HandPad=InNibble();
}

void Handpad::SetHandPadOKFlag(void)
{
	// don't let flag reset itself back to 1 from 0 when in middle of MoveHs()
	if(HandPadOKFlag)
	{
		ReadHandPad();
		if(HandPad==InitHandPad)
			HandPadOKFlag=Yes;
		else
			HandPadOKFlag=No;
	}
}

void Handpad::Test(void)
{
	int Input;
	int Pad;

	clrscr();
	cout<<endl<<endl<<endl<<"Starting Handpad module test:"<<endl;
	cout<<"'r' to read Pad, 'q' to quit."<<endl;
	Input=' ';
	while(Input!='q')
	{
		Input=getch();
		if(Input=='r' || Input=='R')
		{
			Pad=BiDirInNibble()+InNibble();
			cout<<" bit 1:"<<(Pad&1);
			cout<<" bit 2:"<<(Pad&2);
			cout<<" bit 4:"<<(Pad&4);
			cout<<" bit 8:"<<(Pad&8);
			cout<<" bit 16:"<<(Pad&16);
			cout<<" bit 32:"<<(Pad&32);
			cout<<" bit 64:"<<(Pad&64);
			cout<<" bit 128:"<<(Pad&128)<<endl;
		}
	}
	cout<<endl<<endl<<ContMsg;
	getch();
}

