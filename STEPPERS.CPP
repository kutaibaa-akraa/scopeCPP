#include <time.h>
#include <sys\timeb.h>
#include <dos.h>
#include <values.h>
#include <iostream.h>
#include <iomanip.h>
#include <time.h>
#include <conio.h>
#include <alloc.h>
#include <stdio.h>
#include <except.h>

#include "common.h"
#include "badexit.hpp"
#include "video.hpp"
#include "atimes.hpp"
#include "pport.hpp"
#include "cmosclk.hpp"
#include "handpad.hpp"
#include "steppers.hpp"

const int MaxHsIx=7;
AZByte HsOut[MaxHsIx+1];

Steppers::Steppers
(
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
	// ptr to array of size Ms,containing PWM counts
	int* PWM,
	const unsigned PPortAddr
):
	Handpad(PPortAddr),

	InvertOutput(InvertOutput),
	MaxDelay(MaxDelay),
	MinDelay(MinDelay),
	HsDelayX(HsDelayX),
	InterruptHs(InterruptHs),
	HoldReps(HoldReps),
	MsRepsTick(MsRepsTick),
	MsDelayX(MsDelayX),
	MsPause(MsPause),
	Ms(Ms)
{
	int Winding;
	int MicroStep;
	int CosWinding;
	int SinWinding;
	int IxA;
	int IxB;

	// A motor lower nibble, Z motor upper nibble
	if(InvertOutput)
	{
		MotorOff.A=0x0F;
		MotorOff.Z=0xF0;
	}
	else
		MotorOff.Z=MotorOff.A=0;

	MotorPort=PPortAddrOutByte;
	SteppersOff();

	MsPerHs=Ms/2;

	Dir.A=Dir.Z=CW;

	ActualMsRepsTick=MsRepsTick;

	HsIx.A=HsIx.Z=0;
	MsIx.A=MsIx.Z=0;
	Steps.A=Steps.Z=0;
	AccumMs.A=AccumMs.Z=0;
	IncrMs.A=IncrMs.Z=0;

	if(MinDelay>=MaxDelay)
		BadExit E("MinDelay>=MaxDelay in Steppers module");
	if(HsDelayX<1)
		BadExit E("HsDelayX must be at least 1");
	if(MsDelayX<1)
		BadExit E("MsDelayX must be at least 1");

	// A motor uses bits 0-3 (1,2,4,8)
	HsOut[0].A=1;
	HsOut[1].A=3;
	HsOut[2].A=2;
	HsOut[3].A=6;
	HsOut[4].A=4;
	HsOut[5].A=12;
	HsOut[6].A=8;
	HsOut[7].A=9;
	// Z motor uses bits 4-7 (16,32,64,128)
	for(IxA=0;IxA<=MaxHsIx;IxA++)
		HsOut[IxA].Z=HsOut[IxA].A*16;

	// set MaxHsRepsIx
	MaxHsRepsIx=MaxDelay-MinDelay;
	// build HsReps array
	/*	HsReps=(long*)malloc((MaxHsRepsIx+1)*sizeof(long));
		if(HsReps==NULL)
			BadExit E("Problem with malloc of HsReps in steppers module"); */
	try
	{
		HsReps=new long[MaxHsRepsIx+1];
	}
	catch(xalloc)
	{
		BadExit E("Bad call of HsReps=new in steppers module");
	}

	// since MoveHs() rounds up to nearest full step
	MaxHs=MAXLONG-1;
	HsReps[0]=MaxHs;
	for(HsRepsIx=1;HsRepsIx<=MaxHsRepsIx;HsRepsIx++)
		HsReps[HsRepsIx]=(long)MaxDelay/(long)(HsRepsIx+MinDelay);

	MaxPWM=PWM[0];
	HsIxToMsIxConvFactor=MaxPWM*MsPerHs;
	// set MaxMsIx;
	// # of microstep waveforms = # of microsteps (Ms) * # of windings (4);
	// # of elements in microstep array = # of microstep waveforms *
	// # of pulse width modulations per microstep voltage (MaxPWM)
	MaxMsIx=4*Ms*MaxPWM-1;

	// build microstep arrays
	try
	{
		MsPWM=new AZByte[MaxMsIx+1];
	}
	catch(xalloc)
	{
		BadExit E("Bad call of MsPWM=new in steppers module");
	}

	// zero-out arrays
	for(IxA=0;IxA<=MaxMsIx;IxA++)
		MsPWM[IxA].Z=MsPWM[IxA].A=0;

	// start cos winding at bit #1 and sin winding at bit #2,
	// cos winding will start at the highest microstep voltage and ramp
	// down to zero volts while sin winding will start at zero volts and
	// ramp up
	CosWinding=1;
	SinWinding=2;

	// start base index of microstep array at the beginning,
	// index is incremented by MaxPWM (# of pulse width modulations
	// per microstep voltage)
	IxA=0;

	// microstep array has 4 windings to build
	for(Winding=1;Winding<=4;Winding++)
	{
		// each winding has Ms # of microsteps to build
		for(MicroStep=0;MicroStep<Ms;MicroStep++)
		{
			// turn on the cos bit for PWM[MicroStep] # of times
			for(IxB=0;IxB<PWM[MicroStep];IxB++)
				MsPWM[IxA+IxB].A+=CosWinding;
			// Microstep==0 has no contribution from SinWinding
			if(MicroStep>0)
				/* the sin microstep that corresponds to the cos
				microstep of Ms is Ms-MicroStep,
				ie, if the total # of microsteps = 10
				microstep     cos     sin
				0             PWM[0]  n/a
				1             PWM[1]  PWM[9]
				2             PWM[2]  PWM[8]
				3             PWM[3]  PWM[7]
				4             PWM[4]  PWM[6]
				5             PWM[5]  PWM[5]
				6             PWM[6]  PWM[4]
				7             PWM[7]  PWM[3]
				8             PWM[8]  PWM[2]
				9             PWM[9]  PWM[1] */
				for(IxB=0;IxB<PWM[Ms-MicroStep];IxB++)
					MsPWM[IxA+IxB].A+=SinWinding;
			// increment to start of next microstep voltage
			IxA+=MaxPWM;
		}
		// goto next windings
		CosWinding*=2;
		SinWinding*=2;
		if(SinWinding>8)
			SinWinding=1;
	}
	// Z motor bits occupy upper nibble
	for(IxA=0;IxA<=MaxMsIx;IxA++)
		MsPWM[IxA].Z=MsPWM[IxA].A*16;

	if(InvertOutput)
	{
		for(IxA=0;IxA<=MaxHsIx;IxA++)
		{
			HsOut[IxA].A=MotorOff.A-HsOut[IxA].A;
			HsOut[IxA].Z=MotorOff.Z-HsOut[IxA].Z;
		}
		for(IxA=0;IxA<=MaxMsIx;IxA++)
		{
			MsPWM[IxA].A=MotorOff.A-MsPWM[IxA].A;
			MsPWM[IxA].Z=MotorOff.Z-MsPWM[IxA].Z;
		}
	}
}

Steppers::~Steppers(void)
{
	delete(HsReps);
	delete(MsPWM);
}

void Steppers::SteppersOff(void)
{
	outportb(MotorPort,MotorOff.A+MotorOff.Z);
}

void Steppers::Hold(void)
{
	AZByte Out;
	int RepCount,HsDelayXCount,DelayCount;

	if(HoldMotor.A)
		Out.A=HsOut[HsIx.A].A;
	else
		Out.A=MotorOff.A;
	if(HoldMotor.Z)
		Out.Z=HsOut[HsIx.Z].Z;
	else
		Out.Z=MotorOff.Z;
	outportb(MotorPort,Out.A+Out.Z);
	for(RepCount=0;RepCount<HoldReps;RepCount++)
		for(HsDelayXCount=0;HsDelayXCount<HsDelayX;HsDelayXCount++)
			for(DelayCount=0;DelayCount<HsRepsIx+MinDelay;DelayCount++)
				;
}

int Steppers::ClockTick(void)
{
	static long HoldClock;

	long Clock=clock();
	if(Clock==HoldClock)
		return No;
	else
	{
		HoldClock=Clock;
		return Yes;
	}
}

void Steppers::PauseUntilClockTick(void)
{
	ClockTick();
	while(!ClockTick())
	;
}

void Steppers::DoOneHs(void)
{
	AZByte Out;
	int HsDelayXCount,DelayCount;

	if(Steps.A)
	{
		if(Dir.A==CW)
		{
			HsIx.A++;
			if(HsIx.A>MaxHsIx)
				HsIx.A=0;
			AccumMs.A+=MsPerHs;
		}
		else
		{
			HsIx.A--;
			if(HsIx.A<0)
				HsIx.A=MaxHsIx;
			AccumMs.A-=MsPerHs;
		}
		Steps.A--;
		Out.A=HsOut[HsIx.A].A;
	}
	else
		Out.A=MotorOff.A;
	MsIx.A=HsIx.A*HsIxToMsIxConvFactor;

	if(Steps.Z)
	{
		if(Dir.Z==CW)
		{
			HsIx.Z++;
			if(HsIx.Z>MaxHsIx)
				HsIx.Z=0;
			AccumMs.Z+=MsPerHs;
		}
		else
		{
			HsIx.Z--;
			if(HsIx.Z<0)
				HsIx.Z=MaxHsIx;
			AccumMs.Z-=MsPerHs;
		}
		Steps.Z--;
		Out.Z=HsOut[HsIx.Z].Z;
	}
	else
		Out.Z=MotorOff.Z;
	MsIx.Z=HsIx.Z*HsIxToMsIxConvFactor;

	outportb(MotorPort,Out.A+Out.Z);
	// repetitions of delay counts
	for(HsDelayXCount=0;HsDelayXCount<HsDelayX;HsDelayXCount++)
		for(DelayCount=0;DelayCount<HsRepsIx+MinDelay;DelayCount++)
			;
	SetHandPadOKFlag();
}

void Steppers::MoveHs(void)
{
	Flag InterruptFlag=No;
	long MvmtHs;
	long RampHs;
	long SameSpeedHs;

	InitHandPad=HandPad;
	HandPadOKFlag=Yes;
	// end on next fullstep
	if(Steps.A%2)
		Steps.A++;
	if(Steps.Z%2)
		Steps.Z++;
	while(HandPadOKFlag && (Steps.A || Steps.Z))
	{
		// do smaller # of steps 1st unless a motor has 0 steps to do:
		// flags set for Hold() depend on this mvmt algorithm
		if(Steps.A==0)
			MvmtHs=Steps.Z;
		else
			if(Steps.Z==0)
				MvmtHs=Steps.A;
			else
				if(Steps.A<Steps.Z)
					MvmtHs=Steps.A;
				else
					MvmtHs=Steps.Z;
		RampHs=MvmtHs/2;
		/* set for max delay giving slowest speed (index is decremented
		in while loop before being used for 1st time) */
		HsRepsIx=MaxHsRepsIx+1;
		/* if steps to move exceed InterruptHs, then disable interrupts for
		smoothest timing pulses to allow faster speeds */
		if(RampHs>InterruptHs)
		{
			InterruptFlag=Yes;
			disable();
		}
		// hold motor(s) position for brief time to align
		if(Steps.A)
			HoldMotor.A=On;
		else
			HoldMotor.A=Off;
		if(Steps.Z)
			HoldMotor.Z=On;
		else
			HoldMotor.Z=Off;
		Hold();
		// ramp up
		while(HandPadOKFlag && RampHs)
		{
			HsRepsIx--;
			SameSpeedHs=HsReps[HsRepsIx];
			while(HandPadOKFlag && RampHs && SameSpeedHs)
			{
				DoOneHs();
				RampHs--;
				SameSpeedHs--;
			}
		}
		// ramp down
		while(HsRepsIx<=MaxHsRepsIx)
		{
			while(SameSpeedHs<HsReps[HsRepsIx])
			{
				/* if move cancelled while moving at max speed, then immediate
				ramp down */
				if(!HandPadOKFlag && HsRepsIx==0)
					SameSpeedHs=HsReps[HsRepsIx]-1;
				DoOneHs();
				RampHs--;
				SameSpeedHs++;
			}
			SameSpeedHs=0;
			HsRepsIx++;
		}
		// hold motor(s) position for brief time to brake
		Hold();
		HoldMotor.A=HoldMotor.Z=Off;
	}
	SteppersOff();
	if(InterruptFlag)
	{
		SetDOSToCMOS();
		enable();
	}
	PauseUntilClockTick();
}

void Steppers::MoveMs(void)
{
	int IxA,IxB,IxC;
	AZInt MsToDo;
	AZDouble MsRunningTotal;

	MsAlignFlag=No;
	ActualMsRepsTick=0;
	MsRunningTotal.A=MsRunningTotal.Z=0;

	/* speed of microstepping set by IncrMs (# of microsteps per PWM):
	when < 1, multiple PWMs occur for a microstep, when > 1 (max=5),
	microsteps are skipped over */
	if(IncrMs.A==0 && IncrMs.Z==0)
	{
		IncrMs.A=(double)Steps.A/(double)(MsRepsTick);
		IncrMs.Z=(double)Steps.Z/(double)(MsRepsTick);
	}
	// allow skipping of microsteps up to halfstepping
	if(IncrMs.A>MsPerHs)
		IncrMs.A=MsPerHs;
	if(IncrMs.Z>MsPerHs)
		IncrMs.Z=MsPerHs;
	do
	{
		if(Steps.A)
		{
			MsRunningTotal.A+=IncrMs.A;
			MsToDo.A=MsRunningTotal.A;
			MsRunningTotal.A-=MsToDo.A;
			Steps.A-=MsToDo.A;
			if(Dir.A==CW)
			{
				MsIx.A+=MaxPWM*MsToDo.A;
				if(MsIx.A>MaxMsIx)
					MsIx.A-=MaxMsIx+1;
				AccumMs.A+=MsToDo.A;
			}
			else
			{
				MsIx.A-=MaxPWM*MsToDo.A;
				if(MsIx.A<0)
					MsIx.A+=MaxMsIx+1;
				AccumMs.A-=MsToDo.A;
			}
		}
		if(Steps.Z)
		{
			MsRunningTotal.Z+=IncrMs.Z;
			MsToDo.Z=MsRunningTotal.Z;
			MsRunningTotal.Z-=MsToDo.Z;
			Steps.Z-=MsToDo.Z;
			if(Dir.Z==CW)
			{
				MsIx.Z+=MaxPWM*MsToDo.Z;
				if(MsIx.Z>MaxMsIx)
					MsIx.Z-=MaxMsIx+1;
				AccumMs.Z+=MsToDo.Z;
			}
			else
			{
				MsIx.Z-=MaxPWM*MsToDo.Z;
				if(MsIx.Z<0)
					MsIx.Z+=MaxMsIx+1;
				AccumMs.Z-=MsToDo.Z;
			}
		}
		// send voltages to motors
		for(IxA=0;IxA<MaxPWM;IxA++)
			// repeat outportb() MsDelayX times
			for(IxB=0;IxB<MsDelayX;IxB++)
				outportb(MotorPort,MsPWM[MsIx.A+IxA].A+MsPWM[MsIx.Z+IxA].Z);
		SteppersOff();
		// pause for MsPause counts
		for(IxC=0;IxC<MsPause;IxC++)
			SteppersOff();
		// one microstepping voltage waveform finished
		ActualMsRepsTick++;
	} while(!ClockTick());

	HsIx.A=MsIx.A/HsIxToMsIxConvFactor;
	HsIx.Z=MsIx.Z/HsIxToMsIxConvFactor;
	IncrMs.A=IncrMs.Z=0;
}

void Steppers::AlignMs(void)
{
	if(MsAlignFlag)
		PauseUntilClockTick();
	else
	{
		// find microsteps to nearest fullstep
		Steps.A=(MsIx.A/MaxPWM)%Ms;
		if(Steps.A && Dir.A==CW)
			Steps.A=Ms-Steps.A;
		Steps.Z=(MsIx.Z/MaxPWM)%Ms;
		if(Steps.Z && Dir.Z==CW)
			Steps.Z=Ms-Steps.Z;
		Steppers::MoveMs();
		MsAlignFlag=Yes;
	}
}

void Steppers::Test(int TestMs,double TestIncrMs,long TestHs)
{
	int Input=0;
	int IxA,IxB;

	clrscr();
	cout<<endl<<endl<<endl<<"Starting steppers module test:"<<endl;

	cout<<endl<<endl<<"microstepping at TestIncrMs rate test:";
	if(TestMs>MsRepsTick*MsPerHs)
		TestMs=MsRepsTick*MsPerHs;
	cout<<endl<<ContMsg<<endl;
	Steps.A=Steps.Z=1000;
	PauseUntilClockTick();
	while(!kbhit())
	{
		IncrMs.A=IncrMs.Z=TestIncrMs;
		Steppers::MoveMs();
		Steps.A+=TestMs;
		Steps.Z+=TestMs;
	}
	Steppers::AlignMs();
	getch();

	cout<<endl<<endl<<"individual microstep test:";
	Input=' ';
	while(Input!='q')
	{
		cout<<endl<<"press 'l' for left, 'r' for right, 'q' to quit"<<endl;
		Input=getch();
		if(Input=='l')
			Dir.A=Dir.Z=CW;
		else if(Input=='r')
			Dir.A=Dir.Z=CCW;
		else
			continue;
		PauseUntilClockTick();
		for(IxA=0;IxA<Ms;IxA++)
		{
			cout<<"  "<<IxA;
			Steps.A=Steps.Z=1;
			Steppers::MoveMs();
			for(IxB=0;IxB<5;IxB++)
				Steppers::MoveMs();
		}
	}

	cout<<endl<<"halfstepping test:";
	Input=' ';
	while(Input!='q')
	{
		cout<<endl<<"press 'm' to move, 'q' to quit";
		Input=getch();
		if(Input=='m')
		{
			Steps.A=Steps.Z=TestHs;
			Steppers::MoveHs();
		}
	}

	cout<<endl<<endl<<"end of stepper module tests";
	cout<<endl<<endl<<ContMsg;
	getch();
}

