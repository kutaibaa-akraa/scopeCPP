#ifndef ATIMES_HPP
#define ATIMES_HPP

#include <math.h>

/* all internal values kept in radians:
1 revolution=2 Pi Radians=360 Degrees=24 Hours=1440 Minutes */

#define OneRev            (2*M_PI)
#define RadToDeg          (360./OneRev)
#define RadToArcmin       (60.*RadToDeg)
#define RadToArcsec       (60.*RadToArcmin)
#define RadToTenthsArcsec (10.*RadToArcsec)
#define RadToHr           (24./OneRev)
#define RadToMin          (60.*RadToHr)
#define RadToSec          (60.*RadToMin)
#define RadToHundSec      (100.*RadToSec)
#define DaysToHr	        24.
#define DaysToMin         (60.*DaysToHr)
#define DaysToSec         (60.*DaysToMin)
#define ArcsecPerRev      1296000.

#define SidRate           1.002737909

// ~18.2065 clock ticks per second
#define ClockTicksDay     1573040.
#define ClockTicksSec     (ClockTicksDay/DaysToSec)
#define ClockTicksMin     (ClockTicksDay/DaysToMin)
#define ClockTicksHr	     (ClockTicksDay/DaysToHr)
#define ClockTickIncr     (OneRev/ClockTicksDay)
#define ClockTickSidIncr  (ClockTickIncr*SidRate)

// Julian Date at 1/1/70 00:00:00 UT
#define JD1970    2440587.5
// Sidereal Time at 1/1/70 00:00:00 UT
#define SidHr1970 6.6819616467
#define Sid1970   (SidHr1970/RadToHr)

#define ETZ 5

struct DMS
{
	int Sign;
	int Deg;
	int Min;
	int Sec;
};

struct HMS
{
	int Sign;
	int Hr;
	int Min;
	int Sec;
	int HundSec;
};

struct TimeStruct
{
	double Time;
	HMS TimeHMS;
};

class ATimes : public Video
{
	public:
		ATimes(const double LongitudeDeg,int Tz);
	private:
		int DST;
		double SidTStart;
		double JDStart;

		// this function returns the seconds elapsed since 0 UT,1/1/70
		double SecSince1970(void);
		// must call NewTime() first to set ProgramElapsedTimeSec
		void CalcSidTStart(void);
		// must call NewTime() first to set ProgramElapsedTimeSec
		void CalcJDStart(void);
	public:
		// inputed Longitude (stored in radians)
		double Longitude;
		// inputed timezone, ignoring daylight savings
		int Tz;
		// program elapsed time in seconds
		double ProgramElapsedTimeSec;
		// storage variable for Julian Date
		double JD;
		TimeStruct SidT;

		void CalcJD();
		// this function calculates the sidereal time; called inside NewTime()
		void CalcSidT(void);
		/* this function returns 0 until the system time changes, then returns 1
		and calculates the new elapsed program time and sidereal time:
		system time changes with each bios clock tick */
		virtual int NewTime(void);
		void CalcRadFromHMS(double& R,HMS H);
		void CalcRadFromDMS(double& R,DMS D);
		void GetDMS(long AbsTotalSec,DMS& D);
		virtual void DisplayDMS(DMS D);
		void GetHMS(long AbsTotalHundSec,HMS& H);
		// w/ Sign, w/ HundSec
		virtual void Display1HMS(HMS H);
		// w/o Sign, w/ HundSec
		virtual void Display2HMS(HMS H);
		// w/o Sign, w/o HundSec
		virtual void Display3HMS(HMS H);
		// w/o Sign, w/ tenths of second
		virtual void Display4HMS(HMS H);
		// w/ Sign, w/o HundSec
		virtual void Display5HMS(HMS H);
		void CalcDisplaySidT(TimeStruct& S);
		virtual void DisplayCTime(void);
		virtual void DisplayDate(void);
		virtual void DisplayDayMon(void);
		virtual void DisplayTime(void);
		void Test(double LongitudeDeg,int Tz,int DST);
};

// this stand-alone function calculates the Julian Date
double CalcJD(long Y,int M,int D,int Tz,int DST,int h,int m,double s);

// this stand-alone function calculates the local sidereal time in hours
double CalcSidHr(long Y,int M,int D,int Tz,int DST,int h,int m,double s,
double LongitudeDeg);

#endif
