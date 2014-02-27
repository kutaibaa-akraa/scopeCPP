#include <dos.h>
#include <iostream.h>
#include <conio.h>

#include "common.h"
#include "video.hpp"
#include "atimes.hpp"
#include "iaca.hpp"

long far* const IACA_Ra_Decimal_Hrs_Ptr=(long far*)0x4f0;
long far* const IACA_Dec_Decimal_Degrees_Ptr=(long far*)0x4f4;
long far* const Scope_Ra_Decimal_Hrs_Ptr=(long far*)0x4f8;
long far* const Scope_Dec_Decimal_Degrees_Ptr=(long far*)0x4fc;

const Conv_Factor = 1.e+7;

IACA::IACA(void)
{
	*IACA_Ra_Decimal_Hrs_Ptr=0;
	*IACA_Dec_Decimal_Degrees_Ptr=0;
	*Scope_Ra_Decimal_Hrs_Ptr=0;
	*Scope_Dec_Decimal_Degrees_Ptr=0;
}


void IACA::PutScope(double RaRad,double DecRad)
{
	*Scope_Ra_Decimal_Hrs_Ptr=(long)(RaRad*RadToHr*Conv_Factor);
	*Scope_Dec_Decimal_Degrees_Ptr=(long)(DecRad*RadToDeg*Conv_Factor);
}

int IACA::New_IACA(void)
{
	static long HoldRa,HoldDec;

	if(HoldRa!=*IACA_Ra_Decimal_Hrs_Ptr)
	{
		HoldRa=*IACA_Ra_Decimal_Hrs_Ptr;
		return True;
	}
	else
		if(HoldDec!=*IACA_Dec_Decimal_Degrees_Ptr)
		{
			HoldDec=*IACA_Dec_Decimal_Degrees_Ptr;
			return True;
		}
		else
			return False;
}

void IACA::Get_IACA(double &RaRad,double &DecRad)
{
	RaRad=(double)*IACA_Ra_Decimal_Hrs_Ptr/(RadToHr*Conv_Factor);
	DecRad=(double)*IACA_Dec_Decimal_Degrees_Ptr/(RadToDeg*Conv_Factor);
}

void IACA::Test(void)
{
	double Ra,Dec;
	char Response='y';

	cout<<endl<<endl<<endl<<"Starting IACA module test:";
	cout<<endl<<"(Ra in decimal hours and Dec in decimal degrees)";
	do
	{
		cout<<endl<<"Please enter Scope Ra Dec ";
		cin>>Ra>>Dec;
		PutScope(Ra/RadToHr,Dec/RadToDeg);
		cout<<"Scope Ra = "<<(double)*Scope_Ra_Decimal_Hrs_Ptr/Conv_Factor;
		cout<<" Dec = "<<(double)*Scope_Dec_Decimal_Degrees_Ptr/Conv_Factor;

		Get_IACA(Ra,Dec);
		cout<<endl<<"IACA Ra = "<<Ra*RadToHr<<" Dec = "<<Dec*RadToDeg;
		cout<<endl<<"Inject new IACA Ra Dec? ";
		cin>>Response;
		if(Response=='y'||Response=='Y')
		{
			cout<<"Enter new IACA Ra Dec ";
			cin>>Ra>>Dec;
			*IACA_Ra_Decimal_Hrs_Ptr=(long)(Ra*Conv_Factor);
			*IACA_Dec_Decimal_Degrees_Ptr=(long)(Dec*Conv_Factor);
		Get_IACA(Ra,Dec);
		cout<<"IACA Ra = "<<Ra*RadToHr<<" Dec = "<<Dec*RadToDeg;
		}
		cout<<endl<<"Another go (y/n)? ";
		cin>>Response;
	}while(Response=='y'||Response=='Y');
	cout<<ContMsg;
	getch();
}

