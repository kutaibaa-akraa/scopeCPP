#include <dos.h>
#include <conio.h>
#include <iostream.h>

#include "common.h"
#include "pport.hpp"

PPort::PPort(const unsigned PPortAddr)
{
	PPortAddrOutByte=PPortAddr;
	PPortAddrInNibble=PPortAddr+1;
	PPortAddrInOutNibble=PPortAddr+2;
	OutByteValue=Off;
	OutByte();
	BiDirOutNibbleValue=Off;
	BiDirOutNibble();
}

PPort::~PPort(void)
{
	OutByteValue=Off;
	OutByte();
	BiDirOutNibbleValue=Off;
	BiDirOutNibble();
}

void PPort::OutByte(void)
{
	outportb(PPortAddrOutByte,OutByteValue);
}

void PPort::BiDirOutNibble(void)
{
	outportb(PPortAddrInOutNibble,BiDirOutNibbleValue);
}

Byte PPort::InNibble(void)
{
	return(inportb(PPortAddrInNibble)&240)^128;
}

Byte PPort::BiDirInNibble(void)
{
	return(inportb(PPortAddrInOutNibble)&15)^11;
}

void PPort::Test(void)
{
	int Input;

	clrscr();
	cout<<endl<<endl<<endl<<"Starting PPort module test:"<<endl;
	cout<<"'h' for logical high, 'l' for logical low, 'q' to quit."<<endl;
	Input=' ';
	while(Input!='q')
	{
		Input=getch();
		if(Input=='h' || Input=='H')
		{
			cout<<"outputting logical high..."<<endl;
			outportb(PPortAddrOutByte,PortOn);
		}
		else
			if(Input=='l' || Input=='L')
			{
				cout<<"outputting logical low..."<<endl;
				outportb(PPortAddrOutByte,PortOff);
			}
	}
	outportb(PPortAddrOutByte,PortOff);
	cout<<endl<<endl<<ContMsg;
	getch();
}

