#ifndef GUIDE_HPP
#define GUIDE_HPP

#include "steppers.hpp"
#include "pec.hpp"

#define GuideFile "GUIDE.DAT"
#define MaxGuideIx (PECSize*3)

struct GuideArray
{
	Byte PECIx;
	int TenthsArcsec;
};

struct AZGuideArray {GuideArray A,Z;};

class Guide : public PEC
{
	public:
		Flag GuideFlag;
		// guiding speed
		int GuideArcsecSec;
		// guiding speed in radians per clock tick
		double GuideRadTick;
		// accumulated guiding amounts in altitude and azimuth
		AZDouble AccumGuide;
		// indexes to GuideArray
		AZInt GuideIx;
		AZGuideArray Guides[MaxGuideIx];

		Guide
		(
			const int GuideArcsecSec,
			const int FullstepsPerPECArray,
			const int InvertOutput,
			const int MaxDelay,
			const int MinDelay,
			const int HsDelayX,
			const int InterruptHs,
			const int HoldReps,
			const int MsRepsTick,
			const int MsDelayX,
			const int MsPause,
			const int Ms,
			int* PWM,
			const unsigned PPortAddr
		);
		void InitGuide(void);
		void EndAndSaveGuide(void);
		void ProcessGuide(void);
};

#endif
