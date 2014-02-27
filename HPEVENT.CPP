#include <string.h>
#include <values.h>
#include <math.h>
#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <strstrea.h>
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"
#include "badexit.hpp"
#include "pport.hpp"
#include "handpad.hpp"
#include "video.hpp"
#include "atimes.hpp"
#include "coord.hpp"
#include "convert.hpp"
#include "convertt.hpp"
#include "cmosclk.hpp"
#include "steppers.hpp"
#include "iaca.hpp"
#include "pec.hpp"
#include "refract.hpp"
#include "guide.hpp"
#include "hpevent.hpp"

HandpadEvent::HandpadEvent
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
	const double Z1Deg,
	const double Z2Deg,
	const double Z3Deg,
	const double LongitudeDeg,
	int Tz
):
	Guide
	(
		GuideArcsecSec,
		FullstepsPerPECArray,
		InvertOutput,
		MaxDelay,
		MinDelay,
		HsDelayX,
		InterruptHs,
		HoldReps,
		MsRepsTick,
		MsDelayX,
		MsPause,
		Ms,
		PWM,
		PPortAddr
	),
	ConvertTrig(Z1Deg,Z2Deg,Z3Deg,LongitudeDeg,Tz),

	ConvertTrigFlag(ConvertTrigFlag),
	IACA_Flag(IACA_Flag),
	MsArcsecSec(MsArcsecSec),
	RefractFlag(RefractFlag)

{
	FullStepSizeArcsec.A=AltFullStepSizeArcsec;
	FullStepSizeArcsec.Z=AzFullStepSizeArcsec;

	// init Flags
	TrackFlag=No;
	MoveToCurrentRaDecFlag=No;
	DriftEquatFlag=No;
	DriftAltazFlag=No;
	PECFlag=No;
	GuideFlag=No;
	HandpadFlag=No;
	BacklashDir.A=BacklashDir.Z=CW;
	CheckBacklashOscillation=Yes;
	if(AltBacklashArcmin>0 || AzBacklashArcmin>0)
		BacklashFlag=Yes;
	else
		BacklashFlag=No;

	// init vars
	if(FullStepSizeArcsec.A<0)
		BadExit E("FullStepSizeArcsec.A must be positive value");
	if(FullStepSizeArcsec.Z<0)
		BadExit E("FullStepSizeArcsec.Z must be positive value");
	MsRad.A=FullStepSizeArcsec.A/(Ms*RadToArcsec);
	MsRad.Z=FullStepSizeArcsec.Z/(Ms*RadToArcsec);
	HsRad.A=MsRad.A*MsPerHs;
	HsRad.Z=MsRad.Z*MsPerHs;

	InitMsTickVars(MsArcsecSec);
	MaxMsTick=MsRepsTick*MsPerHs;
	MaxTick.A=MaxMsTick*MsRad.A;
	MaxTick.Z=MaxMsTick*MsRad.Z;

	BacklashMs.A=AltBacklashArcmin/RadToArcmin/MsRad.A;
	// see MoveBacklash() for explanation as to why MsRepsTick is added in
	BacklashMs.A+=MsRepsTick;
	BacklashMs.Z=AzBacklashArcmin/RadToArcmin/MsRad.Z;
	BacklashMs.Z+=MsRepsTick;
	// vars set in MoveBacklash() to exact amount of backlash mvmt
	Backlash.A=Backlash.Z=0;

	Drift.Alt=Drift.Az=0;

	SetAccumMsToCurrentAltaz();

	In.RaHMS.Sign=In.DecDMS.Sign=Plus;
	In.RaHMS.Hr=In.RaHMS.Min=In.RaHMS.Sec=In.RaHMS.HundSec=0;
	In.DecDMS.Deg=In.DecDMS.Min=In.DecDMS.Sec=0;

	if(InitFlag==AllInit)
		GetEquat();
}

void HandpadEvent::InitMsTickVars(const int MsArcsecSec)
{
	MsTick.A=MsArcsecSec*Ms/(FullStepSizeArcsec.A*ClockTicksSec);
	MsTick.Z=MsArcsecSec*Ms/(FullStepSizeArcsec.Z*ClockTicksSec);
}

int HandpadEvent::ClockTick(void)
{
	return Coord::NewTime();
}

void HandpadEvent::PauseUntilClockTick(void)
{
	// set SidT.Time based on last clock tick
	Coord::NewTime();
	// wait for next tick
	while(!Coord::NewTime())
		;
}

void HandpadEvent::SetAccumMsToCurrentAltaz(void)
{
	AZInt Ix;
	// use Hold to calculate Alt and Az for AccumAltMs and AccumAzMs:
	// this preserves Current.Alt and Current.Az
	AZDouble Hold;
	Hold.A=Current.Alt;
	Hold.Z=Current.Az;

	if(RefractFlag)
	{
		CalcRefractSkyToScope(Current.Alt);
		Hold.A-=Refract;
	}
	if(BacklashFlag)
	{
		if(BacklashDir.A==CCW)
			Hold.A-=Backlash.A;
		if(BacklashDir.Z==CCW)
			Hold.Z-=Backlash.Z;
	}
	if(PECFlag)
	{
		GetPECIx();
		Hold.A-=PECs[PECIx.A].A/RadToTenthsArcsec;
		Hold.Z-=PECs[PECIx.Z].Z/RadToTenthsArcsec;
		// preserve PEC array indexes
		Ix=PECIx;
	}
	if(DriftAltazFlag)
	{
		Hold.A-=AccumDrift.A;
		Hold.Z-=AccumDrift.Z;
	}
	if(GuideFlag)
	{
		Hold.A-=AccumGuide.A;
		Hold.Z-=AccumGuide.Z;
	}

	AccumMs.A=(long)(0.5+Hold.A/MsRad.A);
	// set to nearest full step
	AccumMs.A-=AccumMs.A%Ms;

	AccumMs.Z=(long)(0.5+Hold.Z/MsRad.Z);
	// set to nearest full step
	AccumMs.Z-=AccumMs.Z%Ms;

	// reset PEC array offsets
	if(PECFlag)
	{
		GetPECIx();

		PECIxOffset.A+=(PECIx.A-Ix.A);
		while(PECIxOffset.A<0)
			PECIxOffset.A+=PECSize;
		while(PECIxOffset.A>=PECSize)
			PECIxOffset.A-=PECSize;

		PECIxOffset.Z+=(PECIx.Z-Ix.Z);
		while(PECIxOffset.Z<0)
			PECIxOffset.Z+=PECSize;
		while(PECIxOffset.Z>=PECSize)
			PECIxOffset.Z-=PECSize;
	}
}

// Alt, Az 'de-coupled' from AccumMs
void HandpadEvent::SetCurrentAltazToAccumMs(void)
{
	// set to AccumMs
	Current.Alt=AccumMs.A*MsRad.A;
	Current.Az=AccumMs.Z*MsRad.Z;
	if(RefractFlag)
	{
		CalcRefractScopeToSky(Current.Alt);
		Current.Alt+=Refract;
	}
	if(BacklashFlag)
	{
		if(BacklashDir.A==CCW)
			Current.Alt+=Backlash.A;
		if(BacklashDir.Z==CCW)
			Current.Az+=Backlash.Z;
	}
	if(PECFlag)
	{
		GetPECIx();
		/* positive PEC indicates excess CW movement; if scope is moving
		clockwise, Current.Alt must be increased for less clockwise motion */
		Current.Alt+=PECs[PECIx.A].A/RadToTenthsArcsec;
		Current.Az+=PECs[PECIx.Z].Z/RadToTenthsArcsec;
	}
	if(DriftAltazFlag)
	{
		AccumDrift.A+=DriftTick.Alt;
		Current.Alt+=AccumDrift.A;
		AccumDrift.Z+=DriftTick.Az;
		Current.Az+=AccumDrift.Z;
	}
	if(GuideFlag)
	{
		Current.Alt+=AccumGuide.A;
		Current.Az+=AccumGuide.Z;
		ProcessGuide();
	}
	ValidAz(Current);
}

// called by MoveToCurrentRaDec() or HandpadEvent::ProcessEvents()
void HandpadEvent::MoveHs(void)
{
	CheckBacklashOscillation=No;
	MoveBacklash();
	CheckBacklashOscillation=Yes;
	Steppers::MoveHs();
	SetCurrentAltazToAccumMs();
}

void HandpadEvent::MoveMs(void)
{
	MoveBacklash();
	Steppers::MoveMs();
	SetCurrentAltazToAccumMs();
}

void HandpadEvent::AlignMs(void)
{
	Steppers::AlignMs();
	SetCurrentAltazToAccumMs();
	GetEquat();
}

void HandpadEvent::AltazMoveMs(void)
{
	AZLong HoldSteps=Steps;

	InitHandPad=HandPad;
	HandPadOKFlag=Yes;
	while(HandPadOKFlag)
	{
		Steps=HoldSteps;
		MoveMs();
		SetHandPadOKFlag();
	}
	AlignMs();
}

int HandpadEvent::MoveBacklash(void)
{
	AZLong HoldSteps;
	const int LastDirSize=6;
	static AZFlag LastDir[LastDirSize];
	int Ix;
	static int HoldIx=0;
	Flag CancelBacklash=No;

	/* if backlash 'on', motor(s) can oscillate to and fro when microstepping
	tracking and steps = 0 or 1 because Dir can flip/flop;
	ignore oscillation check if handpad initiated move or halfstep slew */
	if(CheckBacklashOscillation && MoveToCurrentRaDecFlag)
		// prevent backlash action if to and fro motor oscillations
		for(Ix=0;Ix<LastDirSize;Ix++)
			if(LastDir[Ix].A!=Dir.A || LastDir[Ix].Z!=Dir.Z)
			{
				CancelBacklash=Yes;
				break;
			}
	HoldIx++;
	if(HoldIx==LastDirSize)
		HoldIx=0;
	LastDir[HoldIx]=Dir;

	if(BacklashFlag && !CancelBacklash)
	{
		HoldSteps=Steps;
		Steps.A=Steps.Z=0;
		/* don't allow backlash if steps < 2 because Dir can slowly
		flip/flop every couple of seconds causing slow oscillation */
		if(HoldSteps.A>1)
		{
			if(Dir.A==CW && BacklashDir.A==CCW)
			{
				BacklashDir.A=CW;
					Steps.A=BacklashMs.A;
			}
			if(Dir.A==CCW && BacklashDir.A==CW)
			{
				BacklashDir.A=CCW;
				Steps.A=BacklashMs.A;
			}
		}
		if(HoldSteps.Z>1)
		{
			if(Dir.Z==CCW && BacklashDir.Z==CW)
			{
				BacklashDir.Z=CCW;
				Steps.Z=BacklashMs.Z;
			}
			if(Dir.Z==CW && BacklashDir.Z==CCW)
			{
				BacklashDir.Z=CW;
				Steps.Z=BacklashMs.Z;
			}
		}
		if(Steps.A || Steps.Z)
		{
			/* MsRepsTick added into BacklashMs so that Backlash movement
			concludes when Steps <= MsRepsTick; Steppers::MoveMs() proportions
			speed too slowly when Steps<MsRepsTick */
			while(Steps.A>MsRepsTick || Steps.Z>MsRepsTick)
				// not MoveMs() 'cause MoveMs() calls MoveBacklash() causing
				// infinite loop
				Steppers::MoveMs();
			Backlash.A=(BacklashMs.A-Steps.A)*MsRad.A;
			Backlash.Z=(BacklashMs.Z-Steps.Z)*MsRad.Z;
			Steps=HoldSteps;
			return True;
		}
		else
		{
			Steps=HoldSteps;
			return False;
		}
	}
	else
		return False;
}

void HandpadEvent::MoveToCurrentRaDec(void)
{
	AZDouble Hold;

	if(InitFlag==AllInit)
		if(TrackFlag)
		{
			MoveToCurrentRaDecFlag=Yes;
			Hold.A=Current.Alt;
			Hold.Z=Current.Az;
			if(ConvertTrigFlag)
				ConvertTrig::GetAltaz();
			else
				Convert::GetAltaz();
			// altitude...
			Delta.A=Current.Alt-Hold.A;
			if(Delta.A < 0)
			{
				Delta.A=-Delta.A;
				Dir.A=CCW;
			}
			else
				Dir.A=CW;
			// azimuth...
			// 4 possibilities of calculating az steps to move:
			// 1. Hold=350, New=200, Az=-150, Az=210, yes, Az=-150
			// 2. Hold=200, New=250, Az= 150,         no,  Az= 150
			// 3. Hold=350, New=100, Az=-250, Az=110, no,  Az= 110
			// 4. Hold=100, New=350, Az= 250,         yes, Az=-110
			// az increases CW
			Delta.Z=Current.Az-Hold.Z;
			if(Delta.Z < 0)
				Delta.Z+=OneRev;
			// if > 1/2 rev,go other way
			if(Delta.Z > M_PI)
				// get distance to go (- or +)
				Delta.Z-=OneRev;
			if(Delta.Z < 0)
			{
				Delta.Z=-Delta.Z;
				Dir.Z=CCW;
			}
			else
				Dir.Z=CW;
			Current.Alt=Hold.A;
			Current.Az=Hold.Z;
			// if distance too large for microstep, do halfstepping slew
			if(Delta.A>MaxTick.A || Delta.Z>MaxTick.Z)
			{
				Steps.A=(long)(0.5+(Delta.A/HsRad.A));
				Steps.Z=(long)(0.5+(Delta.Z/HsRad.Z));
				MoveHs();
			}
			// else move in microsteps
			else
			{
				Steps.A=(long)(0.5+Delta.A/MsRad.A);
				Steps.Z=(long)(0.5+Delta.Z/MsRad.Z);
				MoveMs();
			}
			MoveToCurrentRaDecFlag=No;
		}
		else
		{
			PauseUntilClockTick();
			GetEquat();
		}
	else
		PauseUntilClockTick();
}

void HandpadEvent::GetEquat(void)
{
	if(InitFlag==AllInit)
	{
		if(ConvertTrigFlag)
			ConvertTrig::GetEquat();
		else
			Convert::GetEquat();
		PUT_SCOPE;
	}
}

void HandpadEvent::InitMatrix(const int Init)
{
	Position Temp;

	if(InitFlag==AllInit)
	{
		if(Init==InitTwo)
		{
			Temp=Current;
			Current=One;
			Convert::InitMatrix(InitOne);
			Current=Temp;
			Convert::InitMatrix(InitTwo);
		}
		else
		{
			Convert::InitMatrix(InitOne);
			Temp=Current;
			Current=Two;
			Convert::InitMatrix(InitTwo);
			Current=Temp;
		}
	}
	else
		Convert::InitMatrix(Init);

	PUT_SCOPE;
}

void HandpadEvent::MenuResetToIn(void)
{
	Current.Ra=In.Ra;
	Current.Dec=In.Dec;

	if(ConvertTrigFlag)
		ConvertTrig::GetAltaz();
	else
		Convert::GetAltaz();

	SetAccumMsToCurrentAltaz();
}

void HandpadEvent::AddDriftCalculatedFromGuide(void)
{
	double TimeMin=(EndDriftT-StartDriftT)*RadToMin;

	// add in guide on top of already existing drift: allows refinement of
   // drift by doing guide more than once
	Drift.Alt+=AccumGuide.A/TimeMin;
	Drift.Az+=AccumGuide.Z/TimeMin;

	DriftTick.Alt=Drift.Alt/ClockTicksMin;
	DriftTick.Az=Drift.Az/ClockTicksMin;

	// add in results of guide so that guide star does not suddenly move
	// back to position at guide init
	AccumDrift.A+=AccumGuide.A;
	AccumDrift.Z+=AccumGuide.Z;

	if(Drift.Alt!=0 || Drift.Az!=0)
		DriftAltazFlag=Yes;
	else
		DriftAltazFlag=No;

	DisplayAltazDrift();
}

void HandpadEvent::DisplayAltazDrift(void)
{}

int HandpadEvent::IACA_Event(void)
{
	if(New_IACA())
	{
		Get_IACA(In.Ra,In.Dec);
		GetHMS(In.Ra*RadToHundSec+.5,In.RaHMS);
		GetDMS(In.Dec*RadToArcsec+.5,In.DecDMS);
		return True;
	}
	else
		return False;
}

void HandpadEvent::ProcessEvents(void)
{
	static int InitTimer=0;
	// 1/2 sec delay before init'ing again allowed
	const int StartTimer=9;	
	AZLong StepsToMove;


	// to prevent too rapid init'ing
	if(InitTimer)
		InitTimer--;
	// check for handpad event;
	// code from Handpad::ReadHandPad() and PPort::InNibble()
	HandPad=(inportb(PPortAddrInNibble)&240)^128;

	// no buttons pushed...
	if(!HandPad || HandPad==MsKey)
		if(IACA_Flag && IACA_Event())
			;
		else
			MoveToCurrentRaDec();
	else
	{
		MsSpeed=HandPad&MsKey;
		Buttons=HandPad-MsSpeed;
		if(Buttons==UpKey || Buttons==DownKey || Buttons==CCWKey ||
		Buttons==CWKey)
		{
			if(GuideFlag)
			{
				switch(Buttons)
				{
					case UpKey:
						AccumGuide.A-=GuideRadTick;
						break;
					case DownKey:
						AccumGuide.A+=GuideRadTick;
						break;
					case CWKey:
						AccumGuide.Z-=GuideRadTick;
						break;
					case CCWKey:
						AccumGuide.Z+=GuideRadTick;
				}
				if(IACA_Flag && IACA_Event())
					;
				else
					MoveToCurrentRaDec();
			}
			else
			{
				if(MsSpeed)
				{
					StepsToMove.A=MsTick.A;
					StepsToMove.Z=MsTick.Z;
				}
				else
					StepsToMove.A=StepsToMove.Z=MaxHs;

				Steps.A=Steps.Z=0;
				switch(Buttons)
				{
					case UpKey:
						Steps.A=StepsToMove.A;
						Dir.A=CW;
						break;
					case DownKey:
						Steps.A=StepsToMove.A;
						Dir.A=CCW;
						break;
					case CCWKey:
						Steps.Z=StepsToMove.Z;
						Dir.Z=CCW;
						break;
					case CWKey:
						Steps.Z=StepsToMove.Z;
						Dir.Z=CW;
				}
				if(MsSpeed)
					// keeps control until Ms mvmt ends, then calls AlignMs()
					// which calls GetEquat()
					AltazMoveMs();
				else
				{
					MoveHs();
					GetEquat();
				}
			}
		}
		else
			switch(HandpadFlag)
			{
				case InitOn:
					if(Buttons==LeftKey && !InitTimer)
					{
						InitTimer=StartTimer;
						Current.Ra=In.Ra;
						Current.Dec=In.Dec;
						InitMatrix(InitOne);
					}
					else
						if(Buttons==RightKey && InitFlag && !InitTimer)
						{
							InitTimer=StartTimer;
							Current.Ra=In.Ra;
                     Current.Dec=In.Dec;
							InitMatrix(InitTwo);
						}
						else
							if(IACA_Flag && IACA_Event())
								;
							else
								MoveToCurrentRaDec();
					break;
				case GuideOn:
					if(!GuideFlag && Buttons==LeftKey && InitFlag==AllInit
					&& TrackFlag && PECFlag)
					{
						StartDriftT=Current.SidT.Time;
						InitGuide();
					}
					else
						if(GuideFlag && Buttons==RightKey)
						{
							EndDriftT=Current.SidT.Time;
							// if cross 24 hr==0hr boundary
							while(EndDriftT<StartDriftT)
								EndDriftT+=OneRev;
							AddDriftCalculatedFromGuide();
							EndAndSaveGuide();
						}
						else
							if(IACA_Flag && IACA_Event())
								;
							else
								MoveToCurrentRaDec();
					break;
				default:
					if(IACA_Flag && IACA_Event())
						;
					else
						MoveToCurrentRaDec();
			}
	}
}

