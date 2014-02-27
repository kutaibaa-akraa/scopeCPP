#ifndef CONVERT_HPP
#define CONVERT_HPP

#include "common.h"

/*
Conversion routines are based on Sky and Tel, Feb, '89, pg 194-196,
Convert functions input and output horizon increasing CW, then convert to CCW
internally for calculating as per S&T program.
Set coordinates including SidT before making function calls.
Avoid repeatedly alternating Convert::GetEquat() and Convert::GetAltaz() calls,
which can lead to hysteresis or creep of the coordinates of approx 0.1 arc sec.
*/

#define OneTwoFile "ONETWO.DAT"

class Convert : public Coord
{
	public:
		Convert
		(
			const double Z1Deg,
			const double Z2Deg,
			const double Z3Deg,
			const double LongitudeDeg,
			int Tz
		);
	private:
		// storage arrays for matrix multiplication
		double Q[4][4];
		double V[4][4];
		double R[4][4];
		double X[4][4];
		double Y[4][4];
		// fabrication errors:
		/* offset of elevation to perpendicular of horizon, ie, one side of
		rocker box higher than the other */
		double Z1Deg;
		// in radians
		double Z1;
		/* optical axis pointing error in same plane, ie, tube horiz.: optical
		axis error left to right (horiz) */
		double Z2Deg;
		// in radians
		double Z2;
		// correction to zero setting of elevation, ie, vertical offset error
		double Z3Deg;
		// in radians
		double Z3;
		Flag Z1Z2NonZeroFlag;

		// these functions are sundry calculating subroutines
		void SubrA(double F, double H, double Z1, double Z2);
		void SubrB(double F, double H, double Z1, double Z2);
		void DeterminateSubr(double& W);
		void AngleSubr(double& F, double& H);
	public:
		const enum
		{
			NoneInit=0,
			InitOne=1,
			InitTwo=2,
			AllInit=InitTwo
		};
		// 0=not init; 1=1 star init, 2=2 stars init
		Flag InitFlag;
		// coordinates from initialize position #1
		Position One;
		// coordinates from initialize position #2
		Position Two;
		// following values in radians
		double Lat;
		double Longitude;
		double SinLat;
		double CosLat;
		// this value, also in radians, used by field rotation angle calculation
		double SinLatDividedByCosLat;
		// hour angle
		double HA;
		// field rotation angle
		double FieldR;
		// azimuth offset of init'ed position from true azimuth
		double AzOff;
		// Hour Angle Offset = Local Sidereal Time - scope's meridian, or,
		// HAOff = LST - HA - Ra,or,HA = LST - HAOff - Ra;
		// (+)offset = scope tilted to West, (-)offset = scope tilted to East;
		// HAOff varies from -offset to +offset (should only be a few deg.)
		double HAOff;
		HMS HAOffHMS;

		void SaveOneTwo(void);
		void LoadOneTwo(void);
		void SetMountErrorsDeg(const double Z1Deg, const double Z2Deg,
		const double Z3Deg);
		void ZeroArrays(void);
		virtual void InitMatrix(const int Init);
		// call after init 2, calculates apparent scope latitude, longitude and
		// offset azimuth
		void CalcPostInitVars(void);
		virtual void GetAltaz(void);
		virtual void GetEquat(void);
		void CalcDisplayHAOff(void);
		void CalcFieldR(void);
		void DisplayFieldR(void);
		void Test(void);
};

#endif