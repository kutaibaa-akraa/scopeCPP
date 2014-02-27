#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <strstrea.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <values.h>

#include "common.h"
#include "video.hpp"
#include "atimes.hpp"
#include "badexit.hpp"
#include "coord.hpp"
#include "convert.hpp"

Convert::Convert
(
	const double Z1Deg,
	const double Z2Deg,
	const double Z3Deg,
	const double LongitudeDeg,
	int Tz
):
	Coord(LongitudeDeg,Tz),

	Z1Deg(Z1Deg),
	Z2Deg(Z2Deg),
	Z3Deg(Z3Deg)
{
	SetMountErrorsDeg(Z1Deg,Z2Deg,Z3Deg);
	InitFlag=NoneInit;
	LoadOneTwo();
}

void Convert::SaveOneTwo(void)
{
	ofstream Output(OneTwoFile);
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	Output<<"One.Ra "<<setprecision(17)<<setw(17)<<One.Ra<<endl;
	Output<<"One.Dec "<<setprecision(17)<<setw(17)<<One.Dec<<endl;
	Output<<"One.Alt "<<setprecision(17)<<setw(17)<<One.Alt<<endl;
	Output<<"One.Az "<<setprecision(17)<<setw(17)<<One.Az<<endl;
	Output<<"One.SidT.Time "<<setprecision(17)<<setw(17)<<One.SidT.Time<<endl;
	Output<<"Two.Ra "<<setprecision(17)<<setw(17)<<Two.Ra<<endl;
	Output<<"Two.Dec "<<setprecision(17)<<setw(17)<<Two.Dec<<endl;
	Output<<"Two.Alt "<<setprecision(17)<<setw(17)<<Two.Alt<<endl;
	Output<<"Two.Az "<<setprecision(17)<<setw(17)<<Two.Az<<endl;
	Output<<"Two.SidT.Time "<<setprecision(17)<<setw(17)<<Two.SidT.Time<<endl;
}

void Convert::LoadOneTwo(void)
{
	char Name[NameSize];
	char Value[ValueSize];
	Position Temp;

	ifstream File(OneTwoFile);
	if(File)
	{
		while(!File.eof())
		{
			File>>Name>>Value;
			if((strncmpi(Name,"One.Ra",sizeof(Name)))==0)
				One.Ra=atof(Value);
			else if((strncmpi(Name,"One.Dec",sizeof(Name)))==0)
				One.Dec=atof(Value);
			else if((strncmpi(Name,"One.Alt",sizeof(Name)))==0)
				One.Alt=atof(Value);
			else if((strncmpi(Name,"One.Az",sizeof(Name)))==0)
				One.Az=atof(Value);
			else if((strncmpi(Name,"One.SidT.Time",sizeof(Name)))==0)
				One.SidT.Time=atof(Value);
			else if((strncmpi(Name,"Two.Ra",sizeof(Name)))==0)
				Two.Ra=atof(Value);
			else if((strncmpi(Name,"Two.Dec",sizeof(Name)))==0)
				Two.Dec=atof(Value);
			else if((strncmpi(Name,"Two.Alt",sizeof(Name)))==0)
				Two.Alt=atof(Value);
			else if((strncmpi(Name,"Two.Az",sizeof(Name)))==0)
				Two.Az=atof(Value);
			else if((strncmpi(Name,"Two.SidT.Time",sizeof(Name)))==0)
				Two.SidT.Time=atof(Value);
		}
		Temp=Current;
		Current=One;
		InitMatrix(InitOne);
		Current=Two;
		InitMatrix(InitTwo);
		Current=Temp;
	}
	else
		cout<<endl<<OneTwoFile<<" not found";
}

void Convert::SetMountErrorsDeg(const double Z1Deg,const double Z2Deg,
const double Z3Deg)
{
	Z1=Z1Deg/RadToDeg;
	Z2=Z2Deg/RadToDeg;
	Z3=Z3Deg/RadToDeg;
	if(Z1!=0 || Z2!=0)
		Z1Z2NonZeroFlag=Yes;
	else
		Z1Z2NonZeroFlag=No;
}

void Convert::ZeroArrays(void)
{
	int I,J;

	for(I=0;I<4;I++)
		for(J=0;J<4;J++)
			Q[I][J]=V[I][J]=R[I][J]=X[I][J]=Y[I][J]=0;
}

void Convert::InitMatrix(const int Init)
{
	int I,J,L,M,N;
	double D,B,F,H,A,E,W;

	if(Init==InitOne || (Init==InitTwo && InitFlag))
	{
		if(Init==InitOne)
			ZeroArrays();
		D=Current.Dec;
		// B is CCW so HA formula backwards
		B=Current.Ra-Current.SidT.Time;
		X[1][Init]=cos(D)*cos(B);
		X[2][Init]=cos(D)*sin(B);
		X[3][Init]=sin(D);
		// F is CCW
		F=OneRev-Current.Az;
		H=Current.Alt+Z3;
		SubrA(F,H,Z1,Z2);
		Y[1][Init]=Y[1][0];
		Y[2][Init]=Y[2][0];
		Y[3][Init]=Y[3][0];
		// InitMatrix(InitOne) must already be called
		if(Init==InitTwo)
		{
			X[1][3]=X[2][1]*X[3][2]-X[3][1]*X[2][2];
			X[2][3]=X[3][1]*X[1][2]-X[1][1]*X[3][2];
			X[3][3]=X[1][1]*X[2][2]-X[2][1]*X[1][2];
			A=sqrt(X[1][3]*X[1][3]+X[2][3]*X[2][3]+X[3][3]*X[3][3]);
			if(A==0)
				A=1/MAXDOUBLE;
			for(I=1; I<=3; I++)
				X[I][3]/=A;
			Y[1][3]=Y[2][1]*Y[3][2]-Y[3][1]*Y[2][2];
			Y[2][3]=Y[3][1]*Y[1][2]-Y[1][1]*Y[3][2];
			Y[3][3]=Y[1][1]*Y[2][2]-Y[2][1]*Y[1][2];
			A=sqrt(Y[1][3]*Y[1][3]+Y[2][3]*Y[2][3]+Y[3][3]*Y[3][3]);
			if(A==0)
				A=1/MAXDOUBLE;
			for(I=1; I<=3; I++)
				Y[I][3]/=A;
			// transform matrix
			for(I=1; I<=3; I++)
				for(J=1; J<=3; J++)
					V[I][J]=X[I][J];
			DeterminateSubr(W);
			E=W;
			if(E==0)
				E=1/MAXDOUBLE;
			for(M=1; M<=3; M++)
			{
				for(I=1; I<=3; I++)
					for(J=1; J<=3; J++)
						V[I][J]=X[I][J];
				for(N=1; N<=3; N++)
				{
					V[1][M]=0;
					V[2][M]=0;
					V[3][M]=0;
					V[N][M]=1;
					DeterminateSubr(W);
					Q[M][N]=W/E;
				}
			}
			for(I=1; I<=3; I++)
				for(J=1; J<=3; J++)
					R[I][J]=0;
			for(I=1; I<=3; I++)
				for(J=1; J<=3; J++)
					for(L=1; L<=3; L++)
						R[I][J]+=(Y[I][L]*Q[L][J]);
			for(M=1; M<=3; M++)
			{
				for(I=1; I<=3; I++)
					for(J=1; J<=3; J++)
						V[I][J]=R[I][J];
				DeterminateSubr(W);
				E=W;
				if(E==0)
					E=1/MAXDOUBLE;
				for(N=1; N<=3; N++)
				{
					V[1][M]=0;
					V[2][M]=0;
					V[3][M]=0;
					V[N][M]=1;
					DeterminateSubr(W);
					Q[M][N]=W/E;
				}
			}
		}
		InitFlag=Init;
		if(Init==InitOne)
			One=Current;
		else
		{
			Two=Current;
			SaveOneTwo();
			CalcPostInitVars();
		}
	}
	else
		BadExit E("Bad Init parameter passed in Convert::InitMatrix");
}

void Convert::SubrA(double F,double H,double Z1,double Z2)
{
	double CosF,CosH,SinF,SinH;

	CosF=cos(F);
	CosH=cos(H);
	SinF=sin(F);
	SinH=sin(H);
	if(Z1Z2NonZeroFlag)
	{
		Y[1][0]=CosF*CosH-SinF*Z2+SinF*CosH*Z1;
		Y[2][0]=SinF*CosH+CosF*Z2-CosF*SinH*Z1;
	}
	else
	{
		Y[1][0]=CosF*CosH;
		Y[2][0]=SinF*CosH;
	}
	Y[3][0]=SinH;
}

void Convert::SubrB(double F,double H,double Z1,double Z2)
{
	double CosF,CosH,SinF,SinH;

	CosF=cos(F);
	CosH=cos(H);
	SinF=sin(F);
	SinH=sin(H);
	if(Z1Z2NonZeroFlag)
	{
		Y[1][1]=CosF*CosH+SinF*Z2-SinF*CosH*Z1;
		Y[2][1]=SinF*CosH-CosF*Z2+CosF*SinH*Z1;
	}
	else
	{
		Y[1][1]=CosF*CosH;
		Y[2][1]=SinF*CosH;
	}
	Y[3][1]=SinH;
}

void Convert::DeterminateSubr(double& W)
{
	W=V[1][1]*V[2][2]*V[3][3]+V[1][2]*V[2][3]*V[3][1]+V[1][3]*V[3][2]*V[2][1]-
	V[1][3]*V[2][2]*V[3][1]-V[1][1]*V[3][2]*V[2][3]-V[1][2]*V[2][1]*V[3][3];
}

void Convert::AngleSubr(double& F,double& H)
{
	double C;

	C=sqrt(Y[1][1]*Y[1][1]+Y[2][1]*Y[2][1]);
	if(C==0 && Y[3][1]>0)
		H=OneRev/4;
	if(C==0 && Y[3][1]<0)
		H=-OneRev/4;
	if(C!=0)
		H=atan(Y[3][1]/C);
	if(C==0)
	{
		// F should be indeterminate: program listing is F=1000/RadToDeg;
		F=0;
	}
	if(C!=0 && Y[1][1]==0 && Y[2][1]>0)
		F=OneRev/4;
	if(C!=0 && Y[1][1]==0 && Y[2][1]<0)
		F=OneRev*(3/4);
	if(Y[1][1]>0)
		F=atan(Y[2][1]/Y[1][1]);
	if(Y[1][1]<0)
		F=atan(Y[2][1]/Y[1][1])+OneRev/2;
	while(F>OneRev)
		F-=OneRev;
	while(F<0)
		F+=OneRev;
}

void Convert::CalcPostInitVars(void)
{
	Position Temp=Current;

	// find Lat by setting for 90 deg declination(NCP)
	Current.Dec=OneRev/4;
	Current.Ra=0;
	Convert::GetAltaz();
	Lat=Current.Alt;
	SinLat=sin(Lat);
	CosLat=cos(Lat);
	SinLatDividedByCosLat=SinLat/CosLat;

	// set azimuth offset
	AzOff=Current.Az;

	// find longitude
	Current.Alt=OneRev/4;
	Current.Az=0;
	Convert::GetEquat();
	// ATimes::Longitude+Current.SidT.Time=Greenwich Sidereal Time;
	// difference between GST and Current.Ra (==zenith) will be scope longitude
	Longitude=ATimes::Longitude+Current.SidT.Time-Current.Ra;
	while(Longitude<0)
		Longitude+=OneRev;
	while(Longitude>OneRev)
		Longitude-=OneRev;

	// find hour angle offset=LST(Current.SidT.Time)-scope's meridian,
	// HA=LST-HAOff-Ra, or, HA=LST-HAOff-Ra, by setting for zenith;
	// +offset=scope tilted to West, -offset=scope tilted to East;
	// HAOff varies from -offset to +offset(should be a small amount)
	HAOff=Current.SidT.Time-Current.Ra;
	while(HAOff > M_PI)
		HAOff-=OneRev;
	while(HAOff < -M_PI)
		HAOff+=OneRev;

	// restore current coordinates
	Current=Temp;
}

void Convert::GetAltaz(void)
{
	int I,J;
	double D,B,F,H;
	double CosD;

	D=Current.Dec;
	CosD=cos(D);
	// B is CCW so HA formula backwards
	B=Current.Ra-Current.SidT.Time;
	X[1][1]=CosD*cos(B);
	X[2][1]=CosD*sin(B);
	X[3][1]=sin(D);
	Y[1][1]=0;
	Y[2][1]=0;
	Y[3][1]=0;
	for(I=1; I<=3; I++)
		for(J=1; J<=3; J++)
			Y[I][1]+=(R[I][J]*X[J][1]);
	AngleSubr(F,H);
	SubrB(F,H,Z1,Z2);
	AngleSubr(F,H);
	H-=Z3;
	Current.Alt=H;
	// convert back to CW
	Current.Az=OneRev-F;
}

void Convert::GetEquat(void)
{
	int I,J;
	double F,H;

	// convert from CW to CCW az
	F=OneRev-Current.Az;
	H=Current.Alt+Z3;
	SubrA(F,H,Z1,Z2);
	X[1][1]=Y[1][0];
	X[2][1]=Y[2][0];
	X[3][1]=Y[3][0];
	Y[1][1]=0;
	Y[2][1]=0;
	Y[3][1]=0;
	for(I=1; I<=3; I++)
		for(J=1; J<=3; J++)
			Y[I][1]+=(Q[I][J]*X[J][1]);
	AngleSubr(F,H);
	F+=Current.SidT.Time;
	Current.Ra=F;
	ValidRa(Current);
	Current.Dec=H;
}

void Convert::CalcDisplayHAOff(void)
{
	GetHMS(RadToHundSec*HAOff+0.5,HAOffHMS);
}

void Convert::CalcFieldR(void)
{
	double A;
	double SinHA;

	HA=Current.SidT.Time-HAOff-Current.Ra;
	SinHA=sin(HA);
	A=SinLatDividedByCosLat*cos(Current.Dec)-sin(Current.Dec)*cos(HA);
	if(A<0)
		FieldR=atan(SinHA/A)+M_PI/2;
	else
		if(A==0)
			if(SinHA<0)
				FieldR=-M_PI/2;
			else
				if(SinHA==0)
					FieldR=0;
				else
					FieldR=M_PI/2;
		else
			FieldR=atan(SinHA/A);
	if(FieldR<0)
		FieldR+=OneRev;
}

void Convert::DisplayFieldR(void)
{
	cout.setf(ios::showpoint);
	cout.setf(ios::fixed,ios::floatfield);
	cout<<setprecision(3)<<setw(7)<<FieldR*RadToDeg;
}

void Convert::Test(void)
{
	clrscr();
	cout<<endl<<endl<<endl<<"Test of Convert class functions:"<<endl;
	InitFlag=0;
	SetMountErrorsDeg(-.04,.4,-1.63);
	SetCoordDeg(Current,79.172,45.998,39.9,360-39.9,39.2*SidRate/4);
	Convert::InitMatrix(InitOne);
	SetCoordDeg(Current,37.96,89.264,36.2,360-94.6,40.3*SidRate/4);
	Convert::InitMatrix(InitTwo);
	SetCoordDeg(Current,326.05,9.88,0,0,47*SidRate/4);
	Convert::GetAltaz();
	ShowCoordDeg(Current);
	cout<<endl<<"Should be Alt: 42.16   Az: "<<(360-202.54);
	SetCoordDeg(Current,71.53,17.07,0,0,62*SidRate/4);
	Convert::GetAltaz();
	ShowCoordDeg(Current);
	cout<<endl<<"Should be Alt: 40.31   Az: "<<(360-359.98);
	SetCoordDeg(Current,0,0,35.5,360-24.1,71.9*SidRate/4);
	Convert::GetEquat();
	ShowCoordDeg(Current);
	cout<<endl<<"Should be Ra: 87.99   Dec: 32.51";
	cout<<endl<<endl<<ContMsg;
	getch();
}

