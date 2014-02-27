#include <dos.h>
#include <conio.h>
#include <iostream.h>
#include <iomanip.h>

#include "badexit.hpp"
#include "common.h"
#include "cmosclk.hpp"

CMOSCLK::CMOSCLK(void)
{
	SetDOSToCMOS();
	cout<<"Updating DOS Time & Date to CMOS Time & Date of ";
	DisplayCMOSTimeDate();
}

int CMOSCLK::UpdateInProgress(void)
{
	outportb(CCP,CMOS_Reg_Status_A);
	return CMOS_RTN & Update_In_Progress;
}

void CMOSCLK::DisplayCMOSTimeDate(void)
{
	cout<<setfill('0');
	cout<<setw(2)<<CMOS_Hr<<":"<<setw(2)<<CMOS_Min<<":"<<setw(2)<<CMOS_Sec;
	cout<<"   ";
	cout<<setw(4)<<CMOS_Yr<<"-"<<setw(2)<<CMOS_Mon<<"-"<<setw(2)<<CMOS_Day;
}

void CMOSCLK::SetDOSToCMOS(void)
{
	disable();
	// wait until CMOS clock is ready to increment to next second
	while(!UpdateInProgress())
	;
	// wait until CMOS clock is finished updating
	while(UpdateInProgress())
	;
	// get CMOS values
	outportb(CCP,CMOS_Reg_BCD_Sec);
	BCD=CMOS_RTN;
	CMOS_Sec=CMOS_DECODE_BCD;
	outportb(CCP,CMOS_Reg_BCD_Min);
	BCD=CMOS_RTN;
	CMOS_Min=CMOS_DECODE_BCD;
	outportb(CCP,CMOS_Reg_BCD_Hr);
	BCD=CMOS_RTN;
	CMOS_Hr=CMOS_DECODE_BCD;
	outportb(CCP,CMOS_Reg_BCD_Day);
	BCD=CMOS_RTN;
	CMOS_Day=CMOS_DECODE_BCD;
	outportb(CCP,CMOS_Reg_BCD_Mon);
	BCD=CMOS_RTN;
	CMOS_Mon=CMOS_DECODE_BCD;
	outportb(CCP,CMOS_Reg_BCD_Yr);
	BCD=CMOS_RTN;
	CMOS_Yr=1900+CMOS_DECODE_BCD;
	// set DOS to CMOS
	d.da_year=CMOS_Yr;
	d.da_mon=CMOS_Mon;
	d.da_day=CMOS_Day;
	setdate(&d);
	t.ti_hour=CMOS_Hr;
	t.ti_min=CMOS_Min;
	t.ti_sec=CMOS_Sec;
	t.ti_hund=0;
	settime(&t);
	enable();
}

void CMOSCLK::Test(void)
{
	cout<<endl<<endl<<endl;
	cout<<"Test of CMOSCLK module: display of CMOS time & date"<<endl;
	cout<<"press any key to quit...";
	while(!kbhit())
	{
		SetDOSToCMOS();
		cout<<endl;
		DisplayCMOSTimeDate();
	}
	getch();
	cout<<endl<<endl<<ContMsg;
	getch();
}

