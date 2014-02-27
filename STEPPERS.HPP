#ifndef STEPPERS_HPP
#define STEPPERS_HPP

#include <values.h>
#include "common.h"
#include "scope.hpp"

/*
each of the 4 windings of a motor is controlled directly through the parallel
port by Pulse Width Modulation, thereby giving individual current control over
each winding;

move in overvoltage, halfstep mode for fastest speed, highest torque motion:
	homebuilt stepper motor drive circuit with diode + zener suppression +
	4X overdrive voltage with ramping and interrupts disabled = 10,000 hs/sec
	with minimal torque loading;

move in microstep mode for smooth, fine motion;

microstepping algorithm:
	for first full step (angle of 0 deg to 90 deg):
	start winding A with max voltage and winding B with 0 voltage,
	gradually ramp winding B voltage up to max voltage and ramp down winding
	A voltage to 0: winding A voltage follows cos of angle, winding B	voltage
	follows sin, for next full step winding B follows cos of angle and winding
	C follows sin,
	can skip over microsteps up to halfstep for faster speed,
	experiment shows that theoretical cos/sin voltage values need to be modified
	slightly to take into account timing idiosyncrasies of the PC and the
	finite on/off timing of the opto-isolators and drive transistors;

microstep accuracy:
	from stepper motor theory and manuals:
	absolute accuracy is 5' or ~1/22 full step (1 full step=1.8 degrees),
	shaft deflection directly proportional to torque loading when positioned
	inbetween windings;
	conclusion: if torque loading <10%, 10 microsteps/fullstep OK,
	at minimum, stepper should be accurate at quarter steps;

Microstep Angle(deg)  Cos(Angle)
0			  0			 1.000
1			  9 			 0.988
2			 18			 0.951
3			 27			 0.891
4			 36			 0.809
5			 45			 0.707
6			 54			 0.588
7			 63			 0.454
8			 72			 0.309
9			 81			 0.156
10			 90			 0.000

Halfstepping:
Halfstep	Winding:       	Bit-Sum
			A   B   C   D
1			1   0   0   0   	1
2			1   1   0   0   	3
3			0   1   0   0   	2
4			0   1   1   0   	6
5			0   0   1   0   	4
6			0   0   1   1  	12
7			0   0   0   1   	8
8			1   0   0   1   	9

Module algorithms:
	1. use microsteps for tracking & guiding, and halfsteps for slewing,
	2. use PWM (Pulse Width Modulation) to control motor winding voltages,
	3. keep CPU usage (recalculations, writing to screen, etc) to mimimum
	while microstepping to keep motor rotation smooth, minimum PC requirements
	probably are 386 or above, with co-processor for conversion routines,
	4. use pre-calculated Byte arrays for outportb() values for faster speed,
	5. speed of microstepping set by:
		a. (double) IncrMs which says how many microsteps per PWM
		will occur; when IncrMs > 1 (max is 5, equivalent to halfstepping),
		some microsteps are skipped,
		b. # of PWM per bios clock tick (bios clock tick occurs 18.2x a second)
		is approximated by MsRepsTick, no matter the value of MsRepsTick,
		microstepping PWM will conclude with when bios clock tick occurs
		checked for at the end of each PWM),
	6. tracking timing (re-)synchronized by bios clock tick,
	7. start half stepping with a holding time to precisely align rotor,
	8. when finished with halfstepping or microstepping, end on fullstep
	alignment with braking time to dampen shaft oscillation,
	9. speed of halfstepping determined by a delay counting function,
	10.linearily ramp motor speed during halfstepping to protect worm and
	gear speed reducer and to give fastest slewing speed possible,
	11.for long, open loop slews, set steps to move to MAXLONG-1 and bring
	motor to ramped stop by setting HsMoveOKFlag,
	12.for fastest halfstepping, disable interrupts for smoothest timing
	pulses: reset pc time based on CMOS clock when finished,
	13.module keeps track of accumulated steps made,
	14.controls 2 motors called 'A' and 'Z',
	15.slewing moves both motors initially then finishes with the remaining
	motor's leftover steps,
	16.for faster PCs, run through halfstep delay loop HsDelayX times,
	17.for faster PCs, repeat each microstepping outportb() MsDelayX times
	and run through a pause loop MsPause times,
	18.use parallel port to interface to motors since the parallel port has 8
	bits of output;
*/

struct AZInt {int A,Z;};
struct AZLong {long A,Z;};
struct AZByte {Byte A,Z;};
struct AZFlag {Flag A,Z;};
struct AZDouble {double A,Z;};

class Steppers : public Handpad, public CMOSCLK
{
	public:
		Steppers
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
			// ptr to array of size(Ms), containing PWM counts
			int* PWM,
			const unsigned PPortAddr
		);
		~Steppers(void);

	private:
		// if 1, then output is inverted, ie, from logical high to logical low
		int InvertOutput;
		// variables that turn off all windings belonging to the stepper motor
		AZByte MotorOff;

		// variables relating to halfstepping...

		// delay loop value for slowest motor speed
		int MaxDelay;
		// delay loop value for highest motor speed
		int MinDelay;
		// delay loop multiplier: HsDelayX of 2 will double the delay time; so that
		// halfstep delay array doesn't become too large with faster PCs
		int HsDelayX;
		/* if # of Hs exceeds this value, interrupts are disabled to give
		consistent timing pulses for fastest motor speed */
		int InterruptHs;
		// # of repeats of slowest motor speed to hold motors in position
		int HoldReps;
		// used to tell Hold() which motors to energize
		AZByte HoldMotor;
		/* for speed control and linear ramping:
		HsReps is array of # of half step repetitions to do for each delay
		value: delay value is set by moving through an empty for-next loop
		HsRepsIx + MinDelay number of times;
		HsRepsIx varies from 0 to MaxDelay-MinDelay;	HsReps[0]=MAXLONG; */
		long *HsReps;
		AZInt HsIx;

		// variables relating to microstepping...

		int MsDelayX;
		int MsPause;
		int MaxMsIx;
		// microstepping arrays containing on/offs generating voltage values
		AZByte *MsPWM;
		AZInt MsIx;
		// value of passed PWM[0]
		int MaxPWM;
		// convert factor for microstep array indexes to halfstep array indexes
		int HsIxToMsIxConvFactor;

		void SteppersOff(void);
		// holds motors position by energizing windings for HoldReps reps of
		// slowest motor speed: use to align motors and brake motors
		void Hold(void);
		// returns Yes if clock advanced since last call of function
		virtual int ClockTick(void);
		// pauses until next clock tick
		virtual void PauseUntilClockTick(void);

	public:
		int Ms;
		int MsPerHs;
		unsigned MotorPort;
		// # of reps of loop that generates microstep voltage in 1 clock tick
		int MsRepsTick;
		// loop counts per clock tick
		int ActualMsRepsTick;
		int HsRepsIx;
		int MaxHsRepsIx;
		// accumulated motors' movement stored in form of microsteps
		AZLong AccumMs;
		// steps: must always be positive
		AZLong Steps;
		AZFlag Dir;
		// Ms increment per Microstep PWM;
		// determines speed of microstepping; # of microsteps per PWM
		AZDouble IncrMs;
		Flag MsAlignFlag;
		// max halfsteps that ASteps or ZSteps can be set to
		long MaxHs;

		void DoOneHs(void);
		/* keeps control until motors come to a stop after ramping down;
		moves	to nearest full step */
		virtual void MoveHs(void);
		/* speed of microstepping set by IncrMs, if IncrMs left at zero, function
		sets IncrMs to spread Steps over 1 bios clock tick */
		virtual void MoveMs(void);
		// moves steppers to nearest full step
		virtual void AlignMs(void);
		void Test(int TestMs,double TestIncrMs,long TestHs);
};

#endif
