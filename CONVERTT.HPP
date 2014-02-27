#ifndef CONVERTT_H
#define CONVERTT_H

/* ConvertTrig class functions set mount alignment errors to 0;
accuracy between the two conversion methods is a fraction of an arc sec;
hysteresis while repeatedly alternating calls of Getaltaz and GetEquat is
nonexistent with the trig conversion routines (which is not the case with
the matrix conversion routines) */

class ConvertTrig : public Convert
{
	public:
		ConvertTrig
		(
			const double Z1Deg,
			const double Z2Deg,
			const double Z3Deg,
			const double LongitudeDeg,
			int Tz
		);
	private:
		double SinHA;
		double CosHA;
		double SinDec;
		double CosDec;
	public:
		HMS HAHMS;
		void GetAltaz(void);
		void GetEquat(void);
		void CalcDisplayHA(void);
		void Test(void);
};

#endif

