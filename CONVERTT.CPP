#include <iostream.h>
#include <iomanip.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <fstream.h>
#include <string.h>

#include "common.h"
#include "badexit.hpp"
#include "video.hpp"
#include "atimes.hpp"
#include "coord.hpp"
#include "convert.hpp"
#include "convertt.hpp"

ConvertTrig::ConvertTrig
(
	const double Z1Deg,
	const double Z2Deg,
	const double Z3Deg,
	const double LongitudeDeg,
	int Tz
):
	Convert(Z1Deg,Z2Deg,Z3Deg,LongitudeDeg,Tz)
{}

void ConvertTrig::GetAltaz(void)
{
	static double HoldDec;
	double SinAlt;
	double CosAz;

	if(HoldDec!=Current.Dec)
	{
		SinDec=sin(Current.Dec);
		CosDec=cos(Current.Dec);
		HoldDec=Current.Dec;
	}
	HA=Current.SidT.Time-HAOff-Current.Ra;
	CosHA=cos(HA);
	SinHA=sin(HA);
	SinAlt=SinDec*SinLat+CosDec*CosLat*CosHA;
	Current.Alt=asin(SinAlt);
	CosAz=(SinDec-SinLat*SinAlt)/(CosLat*cos(Current.Alt));
	Current.Az=acos(CosAz);
	if(SinHA>0)
		Current.Az=OneRev-Current.Az;
	Current.Az+=AzOff;
	ValidAz(Current);
}

void ConvertTrig::GetEquat(void)
{
	double SinAlt;
	// az-offset
	double Az;

	SinAlt=sin(Current.Alt);
	Az=Current.Az-AzOff;
	if(Az<=0)
		Az+=OneRev;
	SinDec=SinAlt*SinLat+cos(Current.Alt)*CosLat*cos(Az);
	Current.Dec=asin(SinDec);
	CosHA=(SinAlt-SinLat*SinDec)/(CosLat*cos(Current.Dec));
	HA=acos(CosHA);
	if(sin(Az) > 0)
		HA=OneRev-HA;
	Current.Ra=Current.SidT.Time-HAOff-HA;
	ValidRa(Current);
}

void ConvertTrig::CalcDisplayHA(void)
{
	GetHMS(RadToHundSec*HA+0.5,HAHMS);
}

void ConvertTrig::Test(void)
{
	double AzOffDeg=5;

	clrscr();
	cout<<endl<<endl<<endl<<"Test of ConvertTrig class functions:"<<endl;

	SetCoordDeg(Current,79.175,46,76.329,74.875+AzOffDeg,0);
	SetMountErrorsDeg(0,0,0);
	InitMatrix(InitOne);
	SetCoordDeg(Current,101.275,-16.71666667,18.54,138.212+AzOffDeg,0);
	InitMatrix(InitTwo);
	SetCoordDeg(Current,152.1,11.96666667,0,0,0);
	ConvertTrig::GetAltaz();
	ShowCoordDeg(Current);
	SetCoordDeg(Current,0,0,6.791,79.905+AzOffDeg,0);
	ConvertTrig::GetEquat();
	ShowCoordDeg(Current);
	cout<<endl<<"All coordinates should agree.";
	cout<<endl<<"Latitude of "<<(Lat*RadToDeg)<<" should be 44 degrees.";
	cout<<endl<<"AzOff offset of "<<(AzOff*RadToDeg)<<" should be "<<AzOffDeg
	<<" degrees.";
	cout<<endl<<"HA offset of "<<(HAOff*RadToHr)<<" should be -4.0 hrs.";
	cout<<endl<<"(HAOff) ";
	CalcDisplayHAOff();
	Display1HMS(HAOffHMS);
	cout<<"  (HA) ";
	CalcDisplayHA();
	Display1HMS(HAHMS);

	cout<<endl<<endl<<ContMsg;
	getch();
}

