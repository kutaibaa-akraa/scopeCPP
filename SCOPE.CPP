// project compiled with Borland C++ 4.5x compiler;
// comments on class algorithms in h files, specific coding comments in cpp
// files

// use medium memory model (large program, small data)

// argc == # of command line arguments (including program name)
// char* argv[] == array of ptrs to char with argv[0]=program name
// void main(int argc,char* argv[])

#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream.h>
#include <values.h>
#include <fstream.h>
#include <strstrea.h>
#include <string.h>
#include <stdio.h>

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
#include "kbevent.hpp"
#include "scope.hpp"

void main(void)
{
	// configuration vars

	int TextAttr;

	Flag ConvertTrigFlag;
	Flag IACA_Flag;
	int MsArcsecSec;
	double AltFullStepSizeArcsec;
	double AzFullStepSizeArcsec;
	Flag RefractFlag;
	double AltBacklashArcmin;
	double AzBacklashArcmin;

	int GuideArcsecSec;

	int FullstepsPerPECArray;

	int InvertOutput;
	int MaxDelay;
	int MinDelay;
	int HsDelayX;
	int InterruptHs;
	int HoldReps;
	int MsRepsTick;
	int MsDelayX;
	int MsPause;
	int Ms;
	int PWM[MaxMs];

	unsigned PPortAddr;

	double Z1Deg;
	double Z2Deg;
	double Z3Deg;

	double LongitudeDeg;
	int Tz;

	int TestModule;
	int TestDS;
	int TestMs;
	double TestIncrMs;
	long TestHs;

	// other vars

	char Name[NameSize];
	char Value[ValueSize];


	// get configuration values from CfgFile

	ifstream File(CfgFile);
	if(!File)
		BadExit E(strcat("Could not open ",CfgFile));
	while(!File.eof())
	{
		File>>Name>>Value;

		if((strncmpi(Name,"TextAttr",sizeof(Name)))==0)
			TextAttr=atoi(Value);

		else if((strncmpi(Name,"ConvertTrigFlag",sizeof(Name)))==0)
			ConvertTrigFlag=atoi(Value);
		else if((strncmpi(Name,"IACA_Flag",sizeof(Name)))==0)
			IACA_Flag=atoi(Value);
		else if((strncmpi(Name,"MsArcsecSec",sizeof(Name)))==0)
			MsArcsecSec=atoi(Value);
		else if((strncmpi(Name,"AltFullStepSizeArcsec",sizeof(Name)))==0)
			AltFullStepSizeArcsec=atof(Value);
		else if((strncmpi(Name,"AzFullStepSizeArcsec",sizeof(Name)))==0)
			AzFullStepSizeArcsec=atof(Value);
		else if((strncmpi(Name,"RefractFlag",sizeof(Name)))==0)
			RefractFlag=atoi(Value);
		else if((strncmpi(Name,"AltBacklashArcmin",sizeof(Name)))==0)
			AltBacklashArcmin=atof(Value);
		else if((strncmpi(Name,"AzBacklashArcmin",sizeof(Name)))==0)
			AzBacklashArcmin=atof(Value);

		else if((strncmpi(Name,"FullstepsPerPECArray",sizeof(Name)))==0)
			FullstepsPerPECArray=atoi(Value);

		else if((strncmpi(Name,"GuideArcsecSec",sizeof(Name)))==0)
			GuideArcsecSec=atoi(Value);

		else if((strncmpi(Name,"InvertOutput",sizeof(Name)))==0)
			InvertOutput=atoi(Value);
		else if((strncmpi(Name,"MaxDelay",sizeof(Name)))==0)
			MaxDelay=atoi(Value);
		else if((strncmpi(Name,"MinDelay",sizeof(Name)))==0)
			MinDelay=atoi(Value);
		else if((strncmpi(Name,"HsDelayX",sizeof(Name)))==0)
			HsDelayX=atoi(Value);
		else if((strncmpi(Name,"InterruptHs",sizeof(Name)))==0)
			InterruptHs=atoi(Value);
		else if((strncmpi(Name,"HoldReps",sizeof(Name)))==0)
			HoldReps=atoi(Value);
		else if((strncmpi(Name,"MsRepsTick",sizeof(Name)))==0)
			MsRepsTick=atoi(Value);
		else if((strncmpi(Name,"MsDelayX",sizeof(Name)))==0)
			MsDelayX=atoi(Value);
		else if((strncmpi(Name,"MsPause",sizeof(Name)))==0)
			MsPause=atoi(Value);
		else if((strncmpi(Name,"Ms",sizeof(Name)))==0)
			Ms=atoi(Value);
		else if((strncmpi(Name,"PWM[0]",sizeof(Name)))==0)
			PWM[0]=atoi(Value);
		else if((strncmpi(Name,"PWM[1]",sizeof(Name)))==0)
			PWM[1]=atoi(Value);
		else if((strncmpi(Name,"PWM[2]",sizeof(Name)))==0)
			PWM[2]=atoi(Value);
		else if((strncmpi(Name,"PWM[3]",sizeof(Name)))==0)
			PWM[3]=atoi(Value);
		else if((strncmpi(Name,"PWM[4]",sizeof(Name)))==0)
			PWM[4]=atoi(Value);
		else if((strncmpi(Name,"PWM[5]",sizeof(Name)))==0)
			PWM[5]=atoi(Value);
		else if((strncmpi(Name,"PWM[6]",sizeof(Name)))==0)
			PWM[6]=atoi(Value);
		else if((strncmpi(Name,"PWM[7]",sizeof(Name)))==0)
			PWM[7]=atoi(Value);
		else if((strncmpi(Name,"PWM[8]",sizeof(Name)))==0)
			PWM[8]=atoi(Value);
		else if((strncmpi(Name,"PWM[9]",sizeof(Name)))==0)
			PWM[9]=atoi(Value);

		else if((strncmpi(Name,"PPortAddr",sizeof(Name)))==0)
			PPortAddr=(unsigned)atoi(Value);

		else if((strncmpi(Name,"Z1Deg",sizeof(Name)))==0)
			Z1Deg=atof(Value);
		else if((strncmpi(Name,"Z2Deg",sizeof(Name)))==0)
			Z2Deg=atof(Value);
		else if((strncmpi(Name,"Z3Deg",sizeof(Name)))==0)
			Z3Deg=atof(Value);

		else if((strncmpi(Name,"LongitudeDeg",sizeof(Name)))==0)
			LongitudeDeg=atof(Value);
		else if((strncmpi(Name,"Tz",sizeof(Name)))==0)
			Tz=atoi(Value);

		else if((strncmpi(Name,"TestModule",sizeof(Name)))==0)
			TestModule=atoi(Value);
		else if((strncmpi(Name,"TestDS",sizeof(Name)))==0)
			TestDS=atoi(Value);
		else if((strncmpi(Name,"TestMs",sizeof(Name)))==0)
			TestMs=atoi(Value);
		else if((strncmpi(Name,"TestIncrMs",sizeof(Name)))==0)
			TestIncrMs=atof(Value);
		else if((strncmpi(Name,"TestHs",sizeof(Name)))==0)
			TestHs=atol(Value);
	}
	File.close();

	if(Ms>MaxMs)
		BadExit E(strcat("Ms value too large in ",CfgFile));

	if(TestModule==0)
	{
		KeyboardEvent K
		(
			TextAttr,
			ConvertTrigFlag,
			IACA_Flag,
			MsArcsecSec,
			AltFullStepSizeArcsec,
			AzFullStepSizeArcsec,
			RefractFlag,
			AltBacklashArcmin,
			AzBacklashArcmin,
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
			PPortAddr,
			Z1Deg,
			Z2Deg,
			Z3Deg,
			LongitudeDeg,
			Tz
		);
		while(!K.QuitFlag)
			K.ProcessEvents();
	}
	if(TestModule==1)
	{
		ATimes A(LongitudeDeg,Tz);
		A.Test(LongitudeDeg,Tz,TestDS);
	}
	if(TestModule==2)
	{
		Convert C(Z1Deg,Z2Deg,Z3Deg,LongitudeDeg,Tz);
		C.Test();
	}
	if(TestModule==3)
	{
		ConvertTrig C(Z1Deg,Z2Deg,Z3Deg,LongitudeDeg,Tz);
		C.Test();
	}
	if(TestModule==4)
	{
		Steppers S
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
		);
		S.Test(TestMs,TestIncrMs,TestHs);
	}
	if(TestModule==5)
	{
		CMOSCLK C;
		C.Test();
	}
	if(TestModule==6)
	{
		PPort P(PPortAddr);
		P.Test();
	}
	if(TestModule==7)
	{
		Handpad H(PPortAddr);
		H.Test();
	}
	if(TestModule==8)
	{
		IACA I;
		I.Test();
	}
	if(TestModule==9)
	{
		Video V;
		V.Test();
	}
}

