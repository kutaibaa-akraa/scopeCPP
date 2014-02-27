#include <time.h>
#include <sys\timeb.h>
#include <dos.h>
#include <math.h>
#include <iostream.h>
#include <iomanip.h>
#include <conio.h>

#include "common.h"
#include "video.hpp"
#include "atimes.hpp"

ATimes::ATimes(const double LongitudeDeg,int Tz):
	Longitude(LongitudeDeg/RadToDeg),
	Tz(Tz)
{
	SidT.TimeHMS.Sign=Plus;
	// must be called for CalcSidTStart() and CalcJDStart()
	NewTime();
	CalcSidTStart();
	CalcJDStart();
	// correct SidT for SidTStart
	CalcSidT();
	CalcJD();
}

// this function returns the elapsed seconds since Jan 1,1970
double ATimes::SecSince1970(void)
{
	struct timeb t;
	ftime(&t);
	// time functions return based on Eastern Time Zone
	return ((double)t.time)+t.millitm/1000.+(Tz-ETZ)*3600.;
}

void ATimes::CalcSidTStart(void)
{
	SidTStart=fmod((SecSince1970()-ProgramElapsedTimeSec)*SidRate/RadToSec
	+Sid1970-Longitude,OneRev);
}

void ATimes::CalcJDStart(void)
{
	JDStart=(SecSince1970()-ProgramElapsedTimeSec)/DaysToSec+JD1970;
}

void ATimes::CalcSidT(void)
{
	SidT.Time=fmod(SidTStart+ProgramElapsedTimeSec*SidRate/RadToSec,OneRev);
}

void ATimes::CalcJD(void)
{
	JD=JDStart+ProgramElapsedTimeSec/DaysToSec;
}

int ATimes::NewTime(void)
{
	long ClockTicks;
	// last value of ticks since program start
	static long LastClockTicks;
	static int Days;
	static Flag PreviousTimeFlag=Positive;

	ClockTicks=clock();
	if(ClockTicks==LastClockTicks)
		return No;
	else
	{
		LastClockTicks=ClockTicks;

		ProgramElapsedTimeSec=ClockTicks/CLK_TCK;
		// adjust for rollover past midnight
		if(ProgramElapsedTimeSec<0 && PreviousTimeFlag==Positive)
		{
			PreviousTimeFlag=Negative;
			Days++;
		}
		if(ProgramElapsedTimeSec>=0 && PreviousTimeFlag==Negative)
			PreviousTimeFlag=Positive;
		if(Days)
			ProgramElapsedTimeSec+=Days*DaysToSec;

		CalcSidT();
		return Yes;
	}
}

void ATimes::CalcRadFromHMS(double& R,HMS H)
{
	R=(double)H.Hr/RadToHr+(double)H.Min/RadToMin+(double)H.Sec/RadToSec+
	(double)H.HundSec/RadToHundSec;
	if(H.Sign==Minus)
		R=-R;
}

void ATimes::CalcRadFromDMS(double& R,DMS D)
{
	R=(double)D.Deg/RadToDeg+(double)D.Min/RadToArcmin+(double)D.Sec/RadToArcsec;
	if(D.Sign==Minus)
		R=-R;
}

void ATimes::GetDMS(long TotalSec,DMS& D)
{
	if(TotalSec<0)
	{
		TotalSec=-TotalSec;
		D.Sign=Minus;
	}
	else
		D.Sign=Plus;
	D.Deg=(int)(TotalSec/3600);
	D.Min=(int)((TotalSec-(long)(D.Deg)*3600)/60);
	D.Sec=(int)(TotalSec-D.Deg*3600-D.Min*60);
}

void ATimes::DisplayDMS(DMS D)
{
	if(D.Sign)
		cout<<"+";
	else
		cout<<"-";
	cout<<setfill(' ')<<setw(2)<<D.Deg<<":"<<setfill('0')<<setw(2)<<D.Min
	<<":"<<setw(2)<<D.Sec;
}

void ATimes::GetHMS(long TotalHundSec,HMS& H)
{
	long HrHundSec;
	long MinHundSec;

	if(TotalHundSec<0)
	{
		TotalHundSec=-TotalHundSec;
		H.Sign=Minus;
	}
	else
		H.Sign=Plus;
	H.Hr=(int)(TotalHundSec/360000L);
	HrHundSec=(long)H.Hr*360000L;
	H.Min=(int)((TotalHundSec-HrHundSec)/6000);
	MinHundSec=(long)H.Min*6000;
	H.Sec=(int)((TotalHundSec-HrHundSec-MinHundSec)/100);
	H.HundSec=(int)(TotalHundSec-HrHundSec-MinHundSec-H.Sec*100);
}

void ATimes::Display1HMS(HMS H)
{
	if(H.Sign)
		cout<<"+";
	else
		cout<<"-";
	cout<<setfill(' ')<<setw(2)<<H.Hr<<":"<<setfill('0')<<setw(2)<<H.Min
	<<":"<<setw(2)<<H.Sec<<"."<<setw(2)<<H.HundSec;
}

void ATimes::Display2HMS(HMS H)
{
	cout<<setfill(' ')<<setw(2)<<H.Hr<<":"<<setfill('0')<<setw(2)<<H.Min
	<<":"<<setw(2)<<H.Sec<<"."<<setw(2)<<H.HundSec;
}

void ATimes::Display3HMS(HMS H)
{
	cout<<setfill(' ')<<setw(2)<<H.Hr<<":"<<setfill('0')<<setw(2)<<H.Min
	<<":"<<setw(2)<<H.Sec;
}

void ATimes::Display4HMS(HMS H)
{
	cout<<setfill(' ')<<setw(2)<<H.Hr<<":"<<setfill('0')<<setw(2)<<H.Min
	<<":"<<setw(2)<<H.Sec<<".";cout<<setw(1)<<H.HundSec/10;
}

void ATimes::Display5HMS(HMS H)
{
	if(H.Sign)
		cout<<"+";
	else
		cout<<"-";
	cout<<setfill(' ')<<setw(2)<<H.Hr<<":"<<setfill('0')<<setw(2)<<H.Min
	<<":"<<setw(2)<<H.Sec;
}

void ATimes::CalcDisplaySidT(TimeStruct& S)
{
	GetHMS(RadToHundSec*S.Time+0.5,S.TimeHMS);
}

void ATimes::DisplayCTime(void)
{
	time_t t;

	time(&t);
	cout<<ctime(&t);
}

void ATimes::DisplayDate(void)
{
	struct date D;

	getdate(&D);
	cout<<setfill(' ')<<setw(2)<<(int)D.da_mon<<"/"<<setfill('0')<<setw(2)
	<<(int)D.da_day<<"/"<<setw(4)<<D.da_year;
}

void ATimes::DisplayDayMon(void)
{
	struct date D;

	getdate(&D);
	cout<<setfill(' ')<<setw(2)<<(int)D.da_mon<<"/"<<setfill('0')<<setw(2)
	<<(int)D.da_day;
}

void ATimes::DisplayTime(void)
{
	struct time T;

	gettime(&T);
	cout<<setfill(' ')<<setw(2)<<(int)T.ti_hour<<":"<<setfill('0')
	<<(int)T.ti_min<<":"<<(int)T.ti_sec;
}

void ATimes::Test(double LongitudeDeg,int Tz,int DST)
{
	int X,Y;
	struct time t;
	struct date d;
	long Yr;
	int M;
	int D;
	int h;
	int m;
	double s;
	double ExtSidHr;
	double ExtJD;
	int precise;


	clrscr();
	cout<<endl<<endl<<endl<<"Test of ATimes module:"<<endl;

	ATimes::NewTime();
	CalcJD();
	getdate(&d);
	Yr=(long)d.da_year;
	M=(int)d.da_mon;
	D=(int)d.da_day;
	gettime(&t);
	h=(int)t.ti_hour;
	m=(int)t.ti_min;
	s=((double)t.ti_sec)+(t.ti_hund/100.);
	ExtSidHr=::CalcSidHr(Yr,M,D,Tz,DST,h,m,s,LongitudeDeg);
	ExtJD=::CalcJD(Yr,M,D,Tz,DST,h,m,s);

	precise=cout.precision();
	cout.precision(13);
	cout<<endl<<"Elapsed time method:JD "<<JD<<"   SidHr "
	<<(RadToHr*SidT.Time)<<endl;
	cout<<"Standard method:    JD "<<ExtJD<< "   SidHr "<<ExtSidHr<<endl;
	cout.precision(precise);
	cout<<endl<<"Differences:JD="<<3600.*24*(JD-ExtJD)<<" sec   SidHr="
	<<3600.*(RadToHr*SidT.Time-ExtSidHr)<<" sec"<<endl;

	cout<<endl<<"SidT=";
	ATimes::NewTime();
	CalcDisplaySidT(SidT);
	Display2HMS(SidT.TimeHMS);
	cout<<"   LocalTime=";
	DisplayCTime();

	cout<<endl<<ContMsg<<endl;
	getch();
	cout<<ContMsg<<endl;
	X=wherex();
	Y=wherey();
	while(!kbhit())
	{
		while(!NewTime())
		;
		gotoxy(X,Y);
		CalcDisplaySidT(SidT);
		Display2HMS(SidT.TimeHMS);
	}
	getch();

	cout<<endl<<ContMsg;
	getch();
}

double CalcJD(long Y,int M,int D,int Tz,int DST,int h,int m,double s)
{
	long A;
	double B;

	// return 1st formula if Gregorian calendar, otherwise 2nd formula for
	// Julian calendar: Sky and Telescope,August,1991,pg 183
	if((Y>1582)||(Y==1582 && M>10)||(Y==1582 && M==10 && D>15))
		A=367*Y-7*(Y+(M+9)/12)/4-3*((Y+(M-9)/7)/100+1)/4+275*M/9+D+1721029L;
	else
		A=367*Y-7*(Y+5001+(M-9)/7)/4+275*M/9+D+1729777L;
	// subtract 12hrs since JD starts at 12noon UT
	B=(Tz-DST-12+h+m/60.0+s/3600)/24;
	return A+B;
}

double CalcSidHr(long Y,int M,int D,int Tz,int DST,int h,int m,double s,
double LongitudeDeg)
{
	double JD;
	// JD at 0hrs UT (will be in form of a whole number+0.5)
	double JD0HrUT;
	// fractional part of JD beyond JD0HrUT
	double FracDay;
	// intermediate calculated result
	double T;
	// sidereal time at 0hrs UT
	double Sid0HrUT;
	double SidT;

	JD=CalcJD(Y,M,D,Tz,DST,h,m,s);

	FracDay=fmod(JD,1);
	// FracDay meas. from 0hr UT or <JD>.5
	if(FracDay > 0.5)
		FracDay-=0.5;
	else
		FracDay+=0.5;
	JD0HrUT=JD-FracDay;

	// Astronomical Formulae for Calculators,by Jean Meeus,pg 39
	T=(JD0HrUT-2415020.0)/36525.0;
	Sid0HrUT=6.6460656+2400.051262*T+0.00002581*T*T;
	SidT=FracDay*SidRate*24.0+Sid0HrUT-LongitudeDeg/15.0;
	return fmod(SidT,24);
}

