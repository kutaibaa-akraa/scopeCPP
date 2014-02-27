#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <strstrea.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#include "badexit.hpp"
#include "common.h"
#include "video.hpp"
#include "atimes.hpp"
#include "coord.hpp"

Coord::Coord(const double LongitudeDeg,int Tz):

	ATimes(LongitudeDeg,Tz)
{
	LoadCurrentAltaz();
	Current.Ra=0;
	Current.Dec=0;
	Current.SidT=SidT;
}

Coord::~Coord(void)
{
	SaveCurrentAltaz();
}

void Coord::SaveCurrentAltaz(void)
{
	ofstream Output(CurrentFile);
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	Output<<"Current.Alt "<<setprecision(17)<<setw(17)<<Current.Alt<<endl;
	Output<<"Current.Az "<<setprecision(17)<<setw(17)<<Current.Az<<endl;
}

void Coord::LoadCurrentAltaz(void)
{
	char Name[NameSize];
	char Value[ValueSize];

	ifstream File(CurrentFile);
	if(File)
	{
		while(!File.eof())
		{
			File>>Name>>Value;
			if((strncmpi(Name,"Current.Alt",sizeof(Name)))==0)
				Current.Alt=atof(Value);
			else if((strncmpi(Name,"Current.Az",sizeof(Name)))==0)
				Current.Az=atof(Value);
		}
	}
	else
	{
		cout<<endl<<CurrentFile<<" not found: setting Alt=0 and Az=0";
		Current.Alt=Current.Az=0;
	}
}

int Coord::NewTime(void)
{
	if(ATimes::NewTime())
	{
		Current.SidT.Time=SidT.Time;
		return Yes;
	}
	return No;
}


void Coord::ValidRa(Position& Pos)
{
	while(Pos.Ra > OneRev)
		Pos.Ra-=OneRev;
	while(Pos.Ra < 0)
		Pos.Ra+=OneRev;
}

void Coord::ValidDec(Position& Pos)
{
	if(Pos.Dec > OneRev/4)
		Pos.Dec=OneRev;
	else if(Pos.Dec < -OneRev/4)
		Pos.Dec=-OneRev;
}

void Coord::ValidAz(Position& Pos)
{
	while(Pos.Az > OneRev)
		Pos.Az-=OneRev;
	while(Pos.Az < 0)
		Pos.Az+=OneRev;
}

void Coord::SetCoordDeg(Position& Pos,double RaDeg,double DecDeg,
double AltDeg,double AzDeg,double SidTDeg)
{
	Pos.Ra=RaDeg/RadToDeg;
	Pos.Dec=DecDeg/RadToDeg;
	Pos.Alt=AltDeg/RadToDeg;
	Pos.Az=AzDeg/RadToDeg;
	Pos.SidT.Time=SidTDeg/RadToDeg;
}

void Coord::ShowCoordDeg(Position& Pos)
{
	cout<<endl<<"coordinates in degrees are:"<<endl;
	cout<<"Ra: " <<(Pos.Ra*RadToDeg);
	cout<<"  Dec: " <<(Pos.Dec*RadToDeg);
	cout<<"  Alt: " <<(Pos.Alt*RadToDeg);
	cout<<"  Az: " <<(Pos.Az*RadToDeg);
	cout<<"  SidT: " <<(Pos.SidT.Time*RadToDeg);
}

void Coord::ShowCoord(Position& Pos)
{
	CalcDisplayRa(Pos);
	cout<<"Ra ";
	Display4HMS(Pos.RaHMS);
	CalcDisplayDec(Pos);
	cout<<"  Dec ";
	DisplayDMS(Pos.DecDMS);
	cout<<"  Alt ";
	Coord::DisplayAltDeg(Pos);
	cout<<"  Az ";
	Coord::DisplayAzDeg(Pos);
	CalcDisplaySidT(Pos.SidT);
	cout<<"  SidT ";
	Display4HMS(Pos.SidT.TimeHMS);
}

void Coord::CalcDisplayRa(Position& Pos)
{
	GetHMS(RadToHundSec*Pos.Ra+0.5,Pos.RaHMS);
}

void Coord::CalcDisplayDec(Position& Pos)
{
	GetDMS(RadToArcsec*Pos.Dec+0.5,Pos.DecDMS);
}

void Coord::DisplayAltDeg(Position& Pos)
{
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	cout<<setprecision(3)<<setw(6)<<Pos.Alt*RadToDeg;
}

void Coord::DisplayAzDeg(Position& Pos)
{
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	cout<<setprecision(3)<<setw(7)<<Pos.Az*RadToDeg;
}

void Coord::DisplayAltArcsec(Position& Pos)
{
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	cout<<setprecision(3)<<setw(6)<<Pos.Alt*RadToArcsec;
}

void Coord::DisplayAzArcsec(Position& Pos)
{
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	cout<<setprecision(3)<<setw(7)<<Pos.Az*RadToArcsec;
}

