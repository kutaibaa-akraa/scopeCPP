#ifndef HPEVENT_HPP
#define HPEVENT_HPP

#include "common.h"
#include "guide.hpp"

class HandpadEvent : public Guide, public Refract, public IACA,
public ConvertTrig
{
	public:
		HandpadEvent
		(
			const Flag ConvertTrigFlag,
			const Flag IACA_Flag,
			const int MsArcsecSec,
			const double AltFullStepSizeArcsec,
			const double AzFullStepSizeArcsec,
			const Flag RefractFlag,
			const double AltBacklashArcmin,
			const double AzBacklashArcmin,
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
			const unsigned PPortAddr,
			const double Z1,
			const double Z2,
			const double Z3,
			const double LongitudeDeg,
			int Tz
		);
		const enum
		{
			UpKey=16,
			DownKey=32,
			CCWKey=64,
			CWKey=16+32,
			LeftKey=16+64,
			RightKey=32+64,
			MsKey=128
		};
		const enum {Off,InitOn,GuideOn,MaxHandpadFlag};
		// set by keyboard: can assume the above states (not MaxHandpadFlag)
		Flag HandpadFlag;
		// Pad's speed switch setting: if true, speed==microstepping
		int MsSpeed;
		// Pad's buttons
		int Buttons;
		// if true, then use ConvertTrig routines instead of Convert routines
		Flag ConvertTrigFlag;
		// if true, coordinates passed to/from planetarium program via IACA
		Flag IACA_Flag;
		// if true, then refraction included in calcs;
		// if false, refraction display only
		Flag RefractFlag;
		// tracking to current Ra, Dec stopped when flag == false;
		// altaz mvmt initiated by handpad allowed regardless of flag
		Flag TrackFlag;
		// non-zero backlash in either Alt or Az
		Flag BacklashFlag;
		// check for motor direction oscillation caused by backlash updating
		Flag CheckBacklashOscillation;
		// use PEC
		Flag PECFlag;
		// move initiated by MoveToCurrentRaDec()
		Flag MoveToCurrentRaDecFlag;
		// if true, add in drift
		Flag DriftEquatFlag;
		Flag DriftAltazFlag;
		// in radians
		AZDouble Backlash;
		// direction that backlash has been taken up, or reduced to zero, in
		AZFlag BacklashDir;
		// steps to move to take up backlash
		AZLong BacklashMs;
		int MsArcsecSec;
		// microstep distance per clock tick
		AZDouble MsTick;
		// max # of microsteps per clock tick
		double MaxMsTick;
		// max alt, az distances in radians per clock tick
		AZDouble MaxTick;
		// fullstep distances in arc sec
		AZDouble FullStepSizeArcsec;
		// alt,az microstep sizes in radians
		AZDouble MsRad;
		// alt, az halfstep sizes in radians
		AZDouble HsRad;
		// used by SetAltazDeltas()
		AZDouble Delta;
		double StartDriftT,EndDriftT;
		AZDouble AccumDrift;
		// equat drift per hour, altaz drift per min
		Position Drift;
		Position DriftTick;
		Position In;

		// change MsTick.A and MsTick.Z based on MsArcsecSec
		void InitMsTickVars(const int MsArcsecSec);
		// overrides Steppers::ClockTick()
		int ClockTick(void);
		// this function waits until next clock tick
		void PauseUntilClockTick(void);
		// this function sets AccumMs.A and AccumMs.Z based on Current.Alt and
		// Current.Az
		void SetAccumMsToCurrentAltaz(void);
		// this function sets Current.Alt and Current.Az based on AccumAltMs and
		// AccumAzMs
		void SetCurrentAltazToAccumMs(void);
		// this function overrides Steppers::MoveHs()
		void MoveHs(void);
		// this function overrides Steppers::MoveMs()
		void MoveMs(void);
		// this function overrides Steppers::AlignMs()
		void AlignMs(void);
		// moves in (altaz) Ms until handpad buttons change then calls AlignMs()
		void AltazMoveMs(void);
		// moves to takeup backlash if necessary; returns true if mvmt occured
		int MoveBacklash(void);
		/* scope is brought to position calculated at start of clock tick by
		microstepping until next clock tick (scope is always 1 clock tick
		behind during tracking or microstepping) if possible, or by
		halfstepping if move too large;
		if slew does not bring scope close enough to initiate microstepping,
		then next pass through function will also slew, eventually moving scope
		close enough to desired	position to track by microstepping */
		void MoveToCurrentRaDec(void);
		// overrides Convert::GetEquat()
		void GetEquat(void);
		// overrides Convert::InitMatrix()
		void InitMatrix(const int Init);
		void MenuResetToIn(void);
		void AddDriftCalculatedFromGuide(void);
		virtual void DisplayAltazDrift(void);
		virtual int IACA_Event(void);
		virtual void ProcessEvents(void);
};

#endif
