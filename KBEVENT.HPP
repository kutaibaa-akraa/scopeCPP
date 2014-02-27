#ifndef KBEVENT_HPP
#define KBEVENT_HPP

#include "common.h"

struct Border
{
	int Left;
	int Top;
	int Right;
	int Bottom;
	char* Title;
};

struct XY {int X,Y;};

struct Object
{
	char Name[20];
	HMS RaHMS;
	DMS DecDMS;
};

class KeyboardEvent : public HandpadEvent
{
	public:
		KeyboardEvent
		(
			const int TextAttr,
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
		~KeyboardEvent(void);

		const enum
		{
			LocalTStatus,
			DateStatus,
			SidTStatus,
			HandpadStatus,
			TrackStatus,
			PECStatus,
			HsStatus,
			MsArcsecSecStatus,
			GuideArcsecSecStatus,
			FieldRStatus,
			RefractStatus,
			CurrentAltStatus,
			CurrentAzStatus,
			CurrentRaStatus,
			CurrentDecStatus,
			InputRaStatus,
			InputDecStatus,
			DriftRaStatus,
			DriftDecStatus,
			DriftAltStatus,
			DriftAzStatus,
			FilenameStatus,
			ObjectNameStatus,
			MaxStatus
		};
		Flag QuitFlag;
		// screen foreground and background colors
		int TextAttr;
		// to save screen window
		char Buf[80*25*2];

		void MoveHs(void);
		void InitMatrix(const int Init);
		int IACA_Event(void);

		// draws border around inside edge of window
		void DrawBorder(Border P);
		void WriteMenu(void);
		void WriteStatusTitles(void);
		void WriteInit(const int Init);
		void WriteMsgWindow(void);
		void RemoveMsgWindow(void);
		// writes ContMsg and waits for keyboard tap
		void ContMsgRoutine(void);
		// after keyboard key is pressed, waits until next clock tick
		void PressKeyToContMsg(const char* Msg);
		void GetNum(int& Num);
		void GetNum(double& Num);
		void WriteHandpadStatus(void);
		void WriteTrackStatus(void);
		void WritePECStatus(void);
		void WriteMsArcsecSec(void);
		void WriteGuideArcsecSec(void);
		void MenuMsArcsecSec(void);
		void MenuGuideArcsecSec(void);
		void InputAltaz(Position& P,int& X,int& Y);
		void InputEquat(Position& P,int& X,int& Y);
		void MenuAltaz(void);
		void MenuEquat(void);
		void MenuOffset(void);
		// used by MenuCoordFile() to fill window until out of data from file
		int FillObjects(ifstream& File, Object O[]);
		void MenuCoordFile(void);
		// use to enter init's with different SidT's
		void MenuInit(void);
		void MenuDrift(void);
		void DisplayAltazDrift(void);
		void DisplayIn(void);
		// Keyboard events checked every bios clock tick
		void ProcessEvents(void);
};

#endif

