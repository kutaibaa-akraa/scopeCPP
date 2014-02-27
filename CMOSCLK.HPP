#ifndef CMOSCLK_HPP
#define CMOSCLK_HPP

#include <dos.h>

// this class updates DOS time and date to the CMOS date and time:
// provides a way to reset the clock when interrupts disabled for long periods
// of time;

#define CMOS_Ctrl_Port 0x70
#define CMOS_Rtn_Port  0x71

#define CCP             CMOS_Ctrl_Port
#define CMOS_RTN        inport(CMOS_Rtn_Port)
#define CMOS_DECODE_BCD (int)((BCD & 0xF0)*10/0x10 + (BCD & 0x0F))

#define CMOS_Reg_BCD_Sec   0
#define CMOS_Reg_BCD_Min   2
#define CMOS_Reg_BCD_Hr    4
#define CMOS_Reg_BCD_Day   7
#define CMOS_Reg_BCD_Mon   8
#define CMOS_Reg_BCD_Yr    9
#define CMOS_Reg_Status_A 10

#define Update_In_Progress 128

class CMOSCLK
{
	public:
		CMOSCLK(void);

	private:
		struct time t;
		struct date d;

		int DOS_Yr;
		int DOS_Mon;
		int DOS_Day;
		int DOS_Hr;
		int DOS_Min;
		int DOS_Sec;

		int CMOS_Status_A;
		int BCD;
		int Hold_BCD;
		int CMOS_Sec;
		int CMOS_Min;
		int CMOS_Hr;
		int CMOS_Day;
		int CMOS_Mon;
		int CMOS_Yr;

		int UpdateInProgress(void);
		void DisplayCMOSTimeDate(void);

	public:
		void SetDOSToCMOS(void);
		void Test(void);
};

#endif
