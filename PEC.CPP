#include <iostream.h>
#include <iomanip.h>
#include <fstream.h>
#include <strstrea.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <stdlib.h>

#include "common.h"
#include "video.hpp"
#include "atimes.hpp"
#include "pport.hpp"
#include "handpad.hpp"
#include "coord.hpp"
#include "cmosclk.hpp"
#include "steppers.hpp"
#include "pec.hpp"

PEC::PEC
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
):
	Steppers
	(
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
	MsPerPECArray(Ms*FullstepsPerPECArray)
{
	MsPerPECIx=MsPerPECArray/PECSize;
	for(Ix=0;Ix<PECSize;Ix++)
		PECs[Ix].A=PECs[Ix].Z=0;
	LoadPEC();
}

void PEC::LoadPEC(void)
{
	Flag ArrayFlag=0;

	ifstream File(PECFile);
	if(File)
	{
		while(!File.eof())
		{
			File>>Index>>Offset;
			Ix=atoi(Index);
			TenthsArcsec=atoi(Offset);
			// 1st time through indexes, ArrayFlag==1; 2nd time, ArrayFlag==2
			if(Ix==0)
				ArrayFlag++;
			if(ArrayFlag==1)
				PECs[Ix].A=TenthsArcsec;
			else
				PECs[Ix].Z=TenthsArcsec;
		}
	}
	else
		cout<<PECFile<<" not found"<<endl;
}

void PEC::SavePEC(void)
{
	ofstream Output(PECFile);
	for(PECIx.A=0;PECIx.A<PECSize;PECIx.A++)
		Output<<PECIx.A<<" "<<PECs[PECIx.A].A<<endl;
	Output<<endl;
	for(PECIx.Z=0;PECIx.Z<PECSize;PECIx.Z++)
		Output<<PECIx.Z<<" "<<PECs[PECIx.Z].Z<<endl;
}

void PEC::SynchPEC(void)
{
	PECIxOffset.A=(int)(AccumMs.A%MsPerPECArray)/MsPerPECIx;
	PECIxOffset.Z=(int)(AccumMs.Z%MsPerPECArray)/MsPerPECIx;
}

void PEC::GetPECIx(void)
{
	PECIx.A=(int)(AccumMs.A%MsPerPECArray)/MsPerPECIx-PECIxOffset.A;
	while(PECIx.A<0)
		PECIx.A+=PECSize;
	while(PECIx.A>=PECSize)
		PECIx.A-=PECSize;

	PECIx.Z=(int)(AccumMs.Z%MsPerPECArray)/MsPerPECIx-PECIxOffset.Z;
	while(PECIx.Z<0)
		PECIx.Z+=PECSize;
	while(PECIx.Z>=PECSize)
		PECIx.Z-=PECSize;
}

