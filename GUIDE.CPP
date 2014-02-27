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
#include "video.hpp"
#include "atimes.hpp"
#include "pport.hpp"
#include "handpad.hpp"
#include "coord.hpp"
#include "cmosclk.hpp"
#include "steppers.hpp"
#include "pec.hpp"
#include "guide.hpp"

Guide::Guide
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
):
	PEC
	(
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
	GuideArcsecSec(GuideArcsecSec)
{
	InitGuide();
}

void Guide::InitGuide(void)
{
	GuideFlag=Yes;
	GuideRadTick=GuideArcsecSec/RadToArcsec/ClockTicksSec;
	AccumGuide.A=AccumGuide.Z=0;
	GuideIx.A=GuideIx.Z=0;
}

void Guide::EndAndSaveGuide(void)
{
	int Ix;

	GuideFlag=Off;
	AccumGuide.A=AccumGuide.Z=0;
	ofstream Output(GuideFile);
	for(Ix=0;Ix<GuideIx.A;Ix++)
		Output<<(int)Guides[Ix].A.PECIx<<" "<<Guides[Ix].A.TenthsArcsec<<endl;
	Output<<endl;
	for(Ix=0;Ix<GuideIx.Z;Ix++)
		Output<<(int)Guides[Ix].Z.PECIx<<" "<<Guides[Ix].Z.TenthsArcsec<<endl;
}

void Guide::ProcessGuide(void)
{
	static AZInt Sound;
	static AZInt HoldPECIx;

	// only write to file if PEC index changes
	if(PECIx.A!=HoldPECIx.A)
	{
		if(!PECIx.A)
		{
			sound(1000);
			Sound.A=!Sound.A;
		}
		else
			if(Sound.A)
			{
				Sound.A=!Sound.A;
				nosound();
			}
		Guides[GuideIx.A].A.PECIx=PECIx.A;
		Guides[GuideIx.A].A.TenthsArcsec=AccumGuide.A*RadToTenthsArcsec;
		HoldPECIx.A=PECIx.A;
		GuideIx.A++;
	}
	if(PECIx.Z!=HoldPECIx.Z)
	{
		if(!PECIx.Z)
		{
			sound(500);
			Sound.Z=!Sound.Z;
		}
		else
			if(Sound.Z)
			{
				Sound.Z=!Sound.Z;
				nosound();
			}
		Guides[GuideIx.Z].Z.PECIx=PECIx.Z;
		Guides[GuideIx.Z].Z.TenthsArcsec=AccumGuide.Z*RadToTenthsArcsec;
		HoldPECIx.Z=PECIx.Z;
		GuideIx.Z++;
	}
	if(GuideIx.A>=MaxGuideIx || GuideIx.Z>=MaxGuideIx)
		EndAndSaveGuide();
}

