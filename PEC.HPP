#ifndef PEC_HPP
#define PEC_HPP

#include "common.h"
#include "steppers.hpp"

#define PECFile "PEC.DAT"
#define PECSize 200

/* arrays hold values that show scope movement errors in tenths of arcseconds
with PEC rotation divided into PECSize units: positive values indicate
excess CW movement;
for double or quad worms, set FullstepsPerPECArray to cover 2x or 4x worm
rotation */

class PEC : public Steppers
{
	private:
		int Ix;
		int TenthsArcsec;
		char Index[ValueSize];
		char Offset[ValueSize];

	public:
		// microsteps per one complete PEC revolution
		int MsPerPECArray;
		// microsteps per PEC index
		int MsPerPECIx;
		// offset between AccumMs%FullstepsPerPECArray and PEC array [0]
		AZInt PECIxOffset;
		// indexes to PEC array
		AZInt PECIx;
		// arrays of PEC
		AZInt PECs[PECSize];

		PEC
		(
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
		void LoadPEC(void);
		void SavePEC(void);
		// this function synchs PEC file to current rotor angle
		void SynchPEC(void);
		// this function sets the PEC array indexes based on rotor angle
		void GetPECIx(void);
};

#endif
