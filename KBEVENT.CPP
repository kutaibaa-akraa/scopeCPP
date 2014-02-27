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
#include "kbevent.hpp"

const int MaxCoord=24;

const Border MsgBorder={12,9,68,17," Message "};
const Border MainBorder={1,1,80,24,
" Altaz Scope Control Program last compiled Oct 25, 1995 by Mel Bartels "};
const Border MenuBorder={2,3,33,18," Menu "};
const Border StatusBorder={35,3,79,18," Status "};
const Border InitBorder={2,20,79,24," Initialized Positions "};
const Border CoordBorder={4,5,76,18," Coordinates "};

const int MaxMenu=14;
const char MenuTitle[MaxMenu][35]=
{
	"'h' - Handpad switch",
	"'t' - Tracking on/off",
	"'p' - PEC synch/off",
	"'s' - Change microstep speed",
	"'g' - Change guide speed",
	"'a' - Enter altaz coord",
	"'e' - Enter equat coord",
	"'o' - Enter offset to input",
	"'f' - Select coord file",
	"'i' - Enter init",
	"'d' - Enter drift",
	"'r' - Reset to input",
	"'m' - Move to input",
	"'q' - Quit"
};

const int MaxStatusLines=14;
const char StatusTitle[MaxStatusLines][45]=
{
	"Local Time:             Date:",
	"Sidereal Time:",
	"Handpad Switch:",
	"Track Status:           PEC Status:",
	"Halfstepping w/interrupts disabled:",
	"Speed \"/sec Microstep:       Guide:",
	"Field Rotate:",
	"Refraction:",
	"Current  Alt:            Az:",
	"Current   Ra:           Dec:",
	"Input     Ra:           Dec:",
	"Drift /hr Ra:           Dec:",
	"Drift\"/min Alt:          Az:",
	"File:              Object:"
};

#define XS StatusBorder.Left
#define YS StatusBorder.Top
const XY StatusXY[]=
{
	{XS+14,YS+1},{XS+32,YS+1},
	{XS+17,YS+2},
	{XS+18,YS+3},
	{XS+16,YS+4},{XS+38,YS+4},
	{XS+38,YS+5},
	{XS+24,YS+6},{XS+37,YS+6},
	{XS+16,YS+7},
	{XS+16,YS+8},
	{XS+17,YS+9},{XS+32,YS+9},
	{XS+16,YS+10},{XS+31,YS+10},
	{XS+16,YS+11},{XS+31,YS+11},
	{XS+16,YS+12},{XS+31,YS+12},
	{XS+18,YS+13},{XS+32,YS+13},
	{XS+7,YS+14},{XS+28,YS+14}
};

KeyboardEvent::KeyboardEvent
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
	const double Z1Deg,
	const double Z2Deg,
	const double Z3Deg,
	const double LongitudeDeg,
	int Tz
):
	HandpadEvent
	(
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
	),
	TextAttr(TextAttr)
{
	cout<<endl<<"Warning: to prevent the chance of overheating motors & electronics,";
	cout<<endl<<"         turn off the motor power supply when program not running";
	cout<<endl<<"         and stop tracking before hot-keying to another program.";
	cout<<endl;
	cout<<endl<<"Please turn on motor power supply.";
	cout<<endl<<endl;
	ContMsgRoutine();

	QuitFlag=No;

	// setup screen
	textattr(TextAttr);
	clrscr();
	_wscroll=No;
	_setcursortype(_NOCURSOR);
	DrawBorder(MainBorder);
	DrawBorder(MenuBorder);
	DrawBorder(StatusBorder);
	DrawBorder(InitBorder);
	WriteMenu();
	WriteStatusTitles();
	if(InitFlag==AllInit)
	{
		WriteInit(InitOne);
		WriteInit(InitTwo);
	}
	WriteHandpadStatus();
	WriteTrackStatus();
	WritePECStatus();
	WriteMsArcsecSec();
	WriteGuideArcsecSec();
}

KeyboardEvent::~KeyboardEvent(void)
{
	_setcursortype(_NORMALCURSOR);
	gotoxy(1,25);
	cout<<endl<<"ActualMsRepsTick="<<ActualMsRepsTick;
	cout<<endl<<"Please turn off motor power supply."<<endl;
	ContMsgRoutine();
}

void KeyboardEvent::MoveHs(void)
{
	gotoxy(StatusXY[HsStatus].X,StatusXY[HsStatus].Y);
	cout<<"On";
	HandpadEvent::MoveHs();
	// don't call GetEquat() at this point in case MoveHs() being called from
	// MoveToCurrentRaDec(): this will mess up Current.Ra and Current.Dec
	if(MoveToCurrentRaDecFlag && !HandPadOKFlag)
		// GetEquat() called by PressKeyToContMsg()
		PressKeyToContMsg("Slew interrupted by pressing handpad button");
	gotoxy(StatusXY[HsStatus].X,StatusXY[HsStatus].Y);
	cout<<"  ";
}

void KeyboardEvent::InitMatrix(const int Init)
{
	HandpadEvent::InitMatrix(Init);
	WriteInit(InitOne);
	if(InitFlag==AllInit)
		WriteInit(InitTwo);
}

int KeyboardEvent::IACA_Event(void)
{
	if(HandpadEvent::IACA_Event())
	{
		DisplayIn();
		gotoxy(StatusXY[ObjectNameStatus].X,StatusXY[ObjectNameStatus].Y);
		cout<<"from IACA";
		return True;
	}
	else
		return False;
}

void KeyboardEvent::DrawBorder(Border P)
{
	// write top
	for(Column=P.Left+1; Column<P.Right; Column++)
		{
		gotoxy(Column,P.Top);
		cout<<(char)HorizBorder;
	}
	// write bottom
	for(Column=P.Left+1; Column<P.Right; Column++)
	{
		gotoxy(Column,P.Bottom);
		cout<<(char)HorizBorder;
	}
	// write left
	Column=P.Left;
	for(Row=P.Top+1; Row<P.Bottom; Row++)
	{
		gotoxy(Column,Row);
		cout<<(char)VertBorder;
	}
	// write right
	Column=P.Right;
	for(Row=P.Top+1; Row<P.Bottom; Row++)
	{
		gotoxy(Column,Row);
		cout<<(char)VertBorder;
	}
	// write corners
	gotoxy(P.Left,P.Top);
	cout<<(char)TopLeftCorner;
	gotoxy(P.Right,P.Top);
	cout<<(char)TopRightCorner;
	gotoxy(P.Left,P.Bottom);
	cout<<(char)BottomLeftCorner;
	gotoxy(P.Right,P.Bottom);
	cout<<(char)BottomRightCorner;
	// write title
	gotoxy((P.Left+P.Right+1)/2-strlen(P.Title)/2,P.Top);
	cout<<P.Title;
}

void KeyboardEvent::WriteMenu(void)
{
	int Left=MenuBorder.Left+2;
	int Top=MenuBorder.Top+1;

	for(int Ix=0; Ix<MaxMenu; Ix++)
	{
		gotoxy(Left,Top++);
		cout<<MenuTitle[Ix];
	}
}

void KeyboardEvent::WriteStatusTitles(void)
{
	int Left,Top;

	Left=StatusBorder.Left+2;
	Top=StatusBorder.Top+1;
	for(int Ix=0; Ix<MaxStatusLines; Ix++)
	{
		gotoxy(Left,Top++);
		cout<<StatusTitle[Ix];
	}
}

void KeyboardEvent::WriteInit(const int Init)
{
	int X=InitBorder.Left;
	int Y=InitBorder.Top;

	if(Init==InitOne)
	{
		gotoxy(X+1,Y+1);
		cout<<"1:";
		gotoxy(X+4,Y+1);
		ShowCoord(One);
	}
	else
	{
		gotoxy(X+1,Y+2);
		cout<<"2:";
		gotoxy(X+4,Y+2);
		ShowCoord(Two);

		gotoxy(X+1,Y+3);
		cout.setf(ios::fixed,ios::floatfield);
		cout<<"Scope Lat: "<<setprecision(3)<<setw(6)<<Lat*RadToDeg
		<<"   Long: "<<setprecision(3)<<setw(6)<<Longitude*RadToDeg
		<<"   Az Off: "<<setprecision(3)<<setw(6)<<AzOff*RadToDeg
		<<"   HA Off: ";
		CalcDisplayHAOff();
		Display1HMS(HAOffHMS);
	}
}

void KeyboardEvent::WriteMsgWindow(void)
{
	gettext(MsgBorder.Left,MsgBorder.Top,MsgBorder.Right,MsgBorder.Bottom,Buf);
	window(MsgBorder.Left,MsgBorder.Top,MsgBorder.Right,MsgBorder.Bottom);
	clrscr();
	window(1,1,80,25);
	DrawBorder(MsgBorder);
}

void KeyboardEvent::RemoveMsgWindow(void)
{
	puttext(MsgBorder.Left,MsgBorder.Top,MsgBorder.Right,MsgBorder.Bottom,Buf);
}

void KeyboardEvent::ContMsgRoutine(void)
{
	cout<<ContMsg;
	while(!kbhit())
		;
	getch();
}

void KeyboardEvent::PressKeyToContMsg(const char* Msg)
{
	WriteMsgWindow();
	gotoxy(MsgBorder.Left+2,MsgBorder.Top+2);
	cout<<Msg;
	sound(500);
	delay(50);
	nosound();
	gotoxy(MsgBorder.Left+2,MsgBorder.Top+4);
	ContMsgRoutine();
	RemoveMsgWindow();
	PauseUntilClockTick();
	GetEquat();
}

void KeyboardEvent::GetNum(int& Num)
{
	// includes terminating char
	const int MaxChars=4;
	char NumStr[MaxChars+2];
	int X,Y;
	int Ix;

	NumStr[0]=MaxChars;
	X=wherex();
	Y=wherey();
	cout<<"[";
	for(Ix=0;Ix<MaxChars-1;Ix++)
		cout<<" ";
	cout<<"]";
	gotoxy(X+1,Y);
	Num=atoi(cgets(NumStr));
}

void KeyboardEvent::GetNum(double& Num)
{
	// includes terminating char
	const int MaxChars=7;
	char NumStr[MaxChars+2];
	int X,Y;
	int Ix;

	NumStr[0]=MaxChars;
	X=wherex();
	Y=wherey();
	cout<<"[";
	for(Ix=0;Ix<MaxChars-1;Ix++)
		cout<<" ";
	cout<<"]";
	gotoxy(X+1,Y);
	Num=atof(cgets(NumStr));
}

void KeyboardEvent::WriteHandpadStatus(void)
{
	gotoxy(StatusXY[HandpadStatus].X,StatusXY[HandpadStatus].Y);
	switch(HandpadFlag)
	{
		case Off:
			cout<<"Off     ";
			break;
		case InitOn:
			cout<<"Init On ";
			break;
		case GuideOn:
			cout<<"Guide On";
	}
}

void KeyboardEvent::WriteTrackStatus(void)
{
	gotoxy(StatusXY[TrackStatus].X,StatusXY[TrackStatus].Y);
	if(TrackFlag)
		cout<<"On ";
	else
		cout<<"Off";
}

void KeyboardEvent::WritePECStatus(void)
{
	gotoxy(StatusXY[PECStatus].X,StatusXY[PECStatus].Y);
	if(PECFlag)
		cout<<"On ";
	else
		cout<<"Off";
}

void KeyboardEvent::WriteMsArcsecSec(void)
{
	gotoxy(StatusXY[MsArcsecSecStatus].X,StatusXY[MsArcsecSecStatus].Y);
	cout<<setfill(' ')<<setw(4)<<MsArcsecSec;
}

void KeyboardEvent::WriteGuideArcsecSec(void)
{
	gotoxy(StatusXY[GuideArcsecSecStatus].X,StatusXY[GuideArcsecSecStatus].Y);
	cout<<setfill(' ')<<setw(4)<<GuideArcsecSec;
}

void KeyboardEvent::MenuMsArcsecSec(void)
{
	int X,Y;

	WriteMsgWindow();

	X=MsgBorder.Left+5;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Please enter new MsArcsecSec";
	gotoxy(X+=29,Y);
	GetNum(MsArcsecSec);
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();
	InitMsTickVars(MsArcsecSec);
	WriteMsArcsecSec();
}

void KeyboardEvent::MenuGuideArcsecSec(void)
{
	int X,Y;

	WriteMsgWindow();

	X=MsgBorder.Left+3;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Please enter new GuideArcsecSec";
	gotoxy(X+=33,Y);
	GetNum(GuideArcsecSec);
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();
	WriteGuideArcsecSec();
}

void KeyboardEvent::InputAltaz(Position& P,int& X,int& Y)
{
	gotoxy(X,Y++);
	cout<<"Alt ";
	GetNum(P.Alt);
	gotoxy(X,Y++);
	cout<<"Az  ";
	GetNum(P.Az);
	P.Alt/=RadToDeg;
	P.Az/=RadToDeg;
}

void KeyboardEvent::InputEquat(Position& P,int& X,int& Y)
{
	gotoxy(X,Y++);
	cout<<"RaHr   ";
	GetNum(P.RaHMS.Hr);
	gotoxy(X,Y++);
	cout<<"RaMin  ";
	GetNum(P.RaHMS.Min);
	gotoxy(X,Y++);
	cout<<"RaSec  ";
	GetNum(P.RaHMS.Sec);
	gotoxy(X,Y++);
	cout<<"DecDeg ";
	GetNum(P.DecDMS.Deg);
	gotoxy(X,Y++);
	cout<<"DecMin ";
	GetNum(P.DecDMS.Min);
	gotoxy(X,Y++);
	cout<<"DecSec ";
	GetNum(P.DecDMS.Sec);

	P.RaHMS.Sign=Plus;
	if(P.RaHMS.Hr<0)
	{
		P.RaHMS.Sign=Minus;
		P.RaHMS.Hr=-P.RaHMS.Hr;
	}
	if(P.RaHMS.Min<0)
	{
		P.RaHMS.Sign=Minus;
		P.RaHMS.Min=-P.RaHMS.Min;
	}
	if(P.RaHMS.Sec<0)
	{
		P.RaHMS.Sign=Minus;
		P.RaHMS.Sec=-P.RaHMS.Sec;
	}
	P.RaHMS.HundSec=0;

	P.DecDMS.Sign=Plus;
	if(P.DecDMS.Deg<0)
	{
		P.DecDMS.Sign=Minus;
		P.DecDMS.Deg=-P.DecDMS.Deg;
	}
	if(P.DecDMS.Min<0)
	{
		P.DecDMS.Sign=Minus;
		P.DecDMS.Min=-P.DecDMS.Min;
	}
	if(P.DecDMS.Sec<0)
	{
		P.DecDMS.Sign=Minus;
		P.DecDMS.Sec=-P.DecDMS.Sec;
	}

	CalcRadFromHMS(P.Ra,P.RaHMS);
	CalcRadFromDMS(P.Dec,P.DecDMS);
}

void KeyboardEvent::MenuAltaz(void)
{
	int X,Y;

	WriteMsgWindow();

	X=MsgBorder.Left+5;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Please enter";
	gotoxy(X+=13,Y);
	InputAltaz(Current,X,Y);
	_setcursortype(_NOCURSOR);
	SetAccumMsToCurrentAltaz();
	RemoveMsgWindow();
}

void KeyboardEvent::MenuEquat(void)
{
	int X,Y;

	WriteMsgWindow();

	X=MsgBorder.Left+5;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Please enter";
	gotoxy(X+=13,Y);
	InputEquat(In,X,Y);
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();
	DisplayIn();
}

void KeyboardEvent::MenuOffset(void)
{
	int X,Y;
	Position Offset;

	WriteMsgWindow();

	X=MsgBorder.Left+3;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Enter offset from input";
	gotoxy(X+=25,Y);
	InputEquat(Offset,X,Y);
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();
	In.Ra+=Offset.Ra;
	In.Dec+=Offset.Dec;
	// limit In.Ra and In.Dec to valid values
	ValidRa(In);
	ValidDec(In);
	GetHMS(In.Ra*RadToHundSec+.5,In.RaHMS);
	GetDMS(In.Dec*RadToArcsec+.5,In.DecDMS);
	DisplayIn();
}

// ability to page down through lists of unlimited size courtesy Dale Eason
int KeyboardEvent::FillObjects(ifstream& File, Object O[])
{
		int Ix=0;
		int Count=0;

		while(!File.eof() && Ix<MaxCoord)
		{
			File>>O[Ix].Name>>O[Ix].RaHMS.Hr>>O[Ix].RaHMS.Min>>O[Ix].RaHMS.Sec
			>>O[Ix].DecDMS.Deg>>O[Ix].DecDMS.Min>>O[Ix].DecDMS.Sec;
			O[Ix].RaHMS.Sign=Plus;
			O[Ix].RaHMS.HundSec=0;
			O[Ix].DecDMS.Sign=Plus;
			if(O[Ix].DecDMS.Deg<0)
			{
				O[Ix].DecDMS.Sign=Minus;
				O[Ix].DecDMS.Deg=-O[Ix].DecDMS.Deg;
			}
			if(O[Ix].DecDMS.Min<0)
			{
				O[Ix].DecDMS.Sign=Minus;
				O[Ix].DecDMS.Min=-O[Ix].DecDMS.Min;
			}
			if(O[Ix].DecDMS.Sec<0)
			{
				O[Ix].DecDMS.Sign=Minus;
				O[Ix].DecDMS.Sec=-O[Ix].DecDMS.Sec;
			}
			Count=Ix++;
		}
		for(;Ix<MaxCoord;Ix++)
		{
			strcpy(O[Ix].Name," ");
			O[Ix].RaHMS.Hr=O[Ix].RaHMS.Min=O[Ix].RaHMS.Sec=O[Ix].RaHMS.HundSec=0;
			O[Ix].DecDMS.Deg=O[Ix].DecDMS.Min=O[Ix].DecDMS.Sec=0;
			O[Ix].RaHMS.Sign=O[Ix].DecDMS.Sign=Plus;
		}
		return(Count);
}

void KeyboardEvent::MenuCoordFile(void)
{
	Object O[MaxCoord];
   static char Filename[NameSize]="bstars.dat";
	int X,Y;
	int Left,Top;
	int Input;
	int Ix=0;
	int ValidInputFlag=No;

	WriteMsgWindow();

	X=MsgBorder.Left+5;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Please enter file name ["<<Filename<<"]: ";
	gets(Buffer);
	if (Buffer[0])
		sscanf(Buffer, "%s", Filename);
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();

	ifstream File(Filename);
	if(File)
	{
		// save screen & draw border
		gettext(CoordBorder.Left,CoordBorder.Top,CoordBorder.Right,
		CoordBorder.Bottom,Buf);

		// while there are objects in the input file
		while(FillObjects(File,O))
		{
			char Menu='a';
			window(CoordBorder.Left,CoordBorder.Top,CoordBorder.Right,
			CoordBorder.Bottom);
			clrscr();
			window(1,1,80,25);
			DrawBorder(CoordBorder);
			// write objects and instructions
			Left=CoordBorder.Left+3;
			Top=CoordBorder.Top+2;
			gotoxy(Left,Top);
			for (Ix=1;Ix<=MaxCoord;Ix++)
			{
				gotoxy(Left,Top);
				cout<<Menu<<". "<<O[Ix-1].Name;
				Menu++;

				if(Ix%8)
					Top++;
				else
				{
					Top -=7;
					Left+=23;
				}
			}
			Left=CoordBorder.Left+13;
			Top+=9;
			gotoxy(Left,Top);
			cout<<">>> select a-"<<(char)('a'+MaxCoord-1)
			<<" or any other key to escape <<<";
			if (!File.eof())
			{
				gotoxy(Left+13,Top+1);
				cout <<"<Space Bar> for more";
			}
			Input=getch();
			// exit loop if valid input lower case
			if(Input>='a' && Input<'a'+MaxCoord)
			{
				ValidInputFlag=Yes;
				Ix=Input-(int)'a';
				break;
			}
			// exit loop if valid input upper case
			else
				if(Input>='A' && Input<'A'+MaxCoord)
				{
					ValidInputFlag=Yes;
					Ix=Input-(int)'A';
					break;
				}
				else
					// read in more if space bar was pressed
					if(!File.eof() && Input==' ')
						continue;
					else
						break;
		}
		// restore screen
		puttext(CoordBorder.Left,CoordBorder.Top,CoordBorder.Right,
		CoordBorder.Bottom,Buf);
		// if valid input, process input
		if(ValidInputFlag)
		{
			In.RaHMS=O[Ix].RaHMS;
			In.DecDMS=O[Ix].DecDMS;
			CalcRadFromHMS(In.Ra,In.RaHMS);
			CalcRadFromDMS(In.Dec,In.DecDMS);
			DisplayIn();
			gotoxy(StatusXY[FilenameStatus].X,StatusXY[FilenameStatus].Y);
			cout<<Filename;
			gotoxy(StatusXY[ObjectNameStatus].X,StatusXY[ObjectNameStatus].Y);
			cout<<O[Ix].Name;
		}
	}
	else
		PressKeyToContMsg("Can't find File");
}

void KeyboardEvent::MenuInit(void)
{
	int InitToDo=InitOne;
	int X,Y;
	Position Temp;

	WriteMsgWindow();

	_setcursortype(_NORMALCURSOR);
	X=MsgBorder.Left+2;
	Y=MsgBorder.Top+1;
	if(InitFlag)
	{
		gotoxy(X,Y++);
		cout<<"Init to do: ";
		GetNum(InitToDo);
	}
	if(InitToDo==InitOne || InitToDo==InitTwo)
	{
		Temp=Current;
		gotoxy(X,Y);
		cout<<"Please enter";
		gotoxy(X+=13,Y);
		InputEquat(Current,X,Y);
		gotoxy(X+=17,Y-=6);
		InputAltaz(Current,X,Y);
		Y++;
		gotoxy(X,Y++);
		cout<<"SidHr  ";
		GetNum(Current.SidT.TimeHMS.Hr);
		gotoxy(X,Y++);
		cout<<"SidMin ";
		GetNum(Current.SidT.TimeHMS.Min);
		gotoxy(X,Y++);
		cout<<"SidSec ";
		GetNum(Current.SidT.TimeHMS.Sec);
		Current.SidT.TimeHMS.HundSec=0;
		Current.SidT.TimeHMS.Sign=Plus;
		CalcRadFromHMS(Current.SidT.Time,Current.SidT.TimeHMS);

		InitMatrix(InitToDo);

		// restore Alt,Az
		Current.Alt=Temp.Alt;
		Current.Az=Temp.Az;
	}
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();
}

void KeyboardEvent::MenuDrift(void)
{
	int X,Y;

	WriteMsgWindow();

	X=MsgBorder.Left+3;
	Y=MsgBorder.Top+2;
	_setcursortype(_NORMALCURSOR);
	gotoxy(X,Y);
	cout<<"Please enter ";
	gotoxy(X,Y+1);
	cout<<"   RA, Dec drift /hr";
	gotoxy(X,Y+2);
	cout<<"   Alt, Az drift \"/min";
	gotoxy(X+=24,Y);
	InputEquat(Drift,X,Y);
	gotoxy(X+=14,Y-=6);
	InputAltaz(Drift,X,Y);
	//Drift entered in degrees converted to radians by InputAltaz
	Drift.Alt/=3600.;
	Drift.Az/=3600.;
	_setcursortype(_NOCURSOR);
	RemoveMsgWindow();
	gotoxy(StatusXY[DriftRaStatus].X,StatusXY[DriftRaStatus].Y);
	Display5HMS(Drift.RaHMS);
	gotoxy(StatusXY[DriftDecStatus].X,StatusXY[DriftDecStatus].Y);
	DisplayDMS(Drift.DecDMS);
	DisplayAltazDrift();

	Drift.RaHMS.HundSec=0;
	Drift.RaHMS.Sign=Plus;
	CalcRadFromHMS(Drift.Ra,Drift.RaHMS);
	// per clock tick over one hour
	DriftTick.Ra=Drift.Ra/ClockTicksHr;

	CalcRadFromDMS(Drift.Dec,Drift.DecDMS);
	// per clock tick over one hour
	DriftTick.Dec=Drift.Dec/ClockTicksHr;

	if(Drift.Ra!=0 || Drift.Dec!=0)
		DriftEquatFlag=Yes;
	else
		DriftEquatFlag=No;

	// "/min
	DriftTick.Alt=Drift.Alt/ClockTicksMin;
	DriftTick.Az=Drift.Az/ClockTicksMin;
	AccumDrift.A=AccumDrift.Z=0;
	if(Drift.Alt!=0 || Drift.Az!=0)
		DriftAltazFlag=Yes;
	else
		DriftAltazFlag=No;
}

void KeyboardEvent::DisplayAltazDrift(void)
{
	Column=StatusXY[DriftAltStatus].X-1;
	Row=StatusXY[DriftAltStatus].Y-1;
	sprintf(Buffer,"% 6.3f",Drift.Alt*RadToArcsec);
	WriteScreen();
	Column=StatusXY[DriftAzStatus].X-1;
	Row=StatusXY[DriftAzStatus].Y-1;
	sprintf(Buffer,"% 6.3f",Drift.Az*RadToArcsec);
	WriteScreen();
}

void KeyboardEvent::DisplayIn(void)
{
	gotoxy(StatusXY[InputRaStatus].X,StatusXY[InputRaStatus].Y);
	Display3HMS(In.RaHMS);
	gotoxy(StatusXY[InputDecStatus].X,StatusXY[InputDecStatus].Y);
	DisplayDMS(In.DecDMS);
	if(In.Ra!=Current.Ra || In.Dec!=Current.Dec)
	{
		gotoxy(StatusXY[FilenameStatus].X,StatusXY[FilenameStatus].Y);
		cout<<"            ";
		gotoxy(StatusXY[ObjectNameStatus].X,StatusXY[ObjectNameStatus].Y);
		cout<<"                ";
	}
}

void KeyboardEvent::ProcessEvents(void)
{
	struct date D;
	struct time T;
	char Sign;
	static int Tick;
	static double HoldRa,HoldDec;
	static int far *headkeys=(int far*)0x0040001aL;
	static int far *tailkeys=(int far*)0x0040001cL;
	int InputA;

	while(!QuitFlag)
	{
		// add non-zero equatorial drift
		if(InitFlag==AllInit && DriftEquatFlag)
		{
			Current.Ra+=DriftTick.Ra;
			Current.Dec+=DriftTick.Dec;
		}
		// update display statuses
		switch(Tick)
		{
			case 0:
				gettime(&T);
				sprintf(Buffer,"%2d:%02d:%02d",T.ti_hour,T.ti_min,T.ti_sec);
				Column=StatusXY[LocalTStatus].X-1;
				Row=StatusXY[LocalTStatus].Y-1;
				WriteScreen();
				break;
			case 1:
				getdate(&D);
				sprintf(Buffer,"%2d/%02d",D.da_mon,D.da_day);
				Column=StatusXY[DateStatus].X-1;
				Row=StatusXY[DateStatus].Y-1;
				WriteScreen();
				break;
			case 2:
				CalcDisplaySidT(SidT);
				break;
			case 3:
				sprintf(Buffer,"%2d:%02d:%02d",SidT.TimeHMS.Hr,SidT.TimeHMS.Min,
				SidT.TimeHMS.Sec);
				Column=StatusXY[SidTStatus].X-1;
				Row=StatusXY[SidTStatus].Y-1;
				WriteScreen();
				break;
			case 4:
				CalcRefractScopeToSky(Current.Alt);
				break;
			case 5:
				Column=StatusXY[RefractStatus].X-1;
				Row=StatusXY[RefractStatus].Y-1;
				sprintf(Buffer,"%5.3f",Refract*RadToDeg);
				WriteScreen();
				break;
			case 6:
				Column=StatusXY[CurrentAltStatus].X-1;
				Row=StatusXY[CurrentAltStatus].Y-1;
				sprintf(Buffer,"%6.3f",Current.Alt*RadToDeg);
				WriteScreen();
				break;
			case 7:
				Column=StatusXY[CurrentAzStatus].X-1;
				Row=StatusXY[CurrentAzStatus].Y-1;
				sprintf(Buffer,"%7.3f",Current.Az*RadToDeg);
				WriteScreen();
				break;
			case 8:
				if(InitFlag==AllInit)
					CalcFieldR();
				break;
			case 9:
				if(InitFlag==AllInit)
				{
					Column=StatusXY[FieldRStatus].X-1;
					Row=StatusXY[FieldRStatus].Y-1;
					sprintf(Buffer,"%7.3f",FieldR*RadToDeg);
					WriteScreen();
				}
				break;
			case 10:
				if(InitFlag==AllInit && Current.Ra!=HoldRa)
					CalcDisplayRa(Current);
				break;
			case 11:
				if(InitFlag==AllInit && Current.Ra!=HoldRa)
				{
					sprintf(Buffer,"%2d:%02d:%02d",Current.RaHMS.Hr,
					Current.RaHMS.Min,Current.RaHMS.Sec);
					Column=StatusXY[CurrentRaStatus].X-1;
					Row=StatusXY[CurrentRaStatus].Y-1;
					WriteScreen();
					HoldRa=Current.Ra;
				}
				break;
			case 12:
				if(InitFlag==AllInit && Current.Dec!=HoldDec)
					CalcDisplayDec(Current);
				break;
			case 13:
				if(InitFlag==AllInit && Current.Dec!=HoldDec)
				{
					if(Current.DecDMS.Sign==Plus)
						Sign=' ';
					else
						Sign='-';
					sprintf(Buffer,"%c%2d:%02d:%02d",Sign,Current.DecDMS.Deg,
					Current.DecDMS.Min,Current.DecDMS.Sec);
					Column=StatusXY[CurrentDecStatus].X-1;
					Row=StatusXY[CurrentDecStatus].Y-1;
					WriteScreen();
					HoldDec=Current.Dec;
				}
				break;
			case 14:
				Tick=-1;
		}
		Tick++;
		// check for keyboard input
		if(*headkeys!=*tailkeys)
		{
			InputA=getch();
			switch(InputA)
			{
				case 'h':
				case 'H':
					HandpadFlag++;
					if(HandpadFlag==MaxHandpadFlag)
						HandpadFlag=Off;
					WriteHandpadStatus();
					break;
				case 't':
				case 'T':
					TrackFlag=!TrackFlag;
					WriteTrackStatus();
					// if tracking being turned off, save current to input so
					// that upon resumption of tracking, scope can slew to last
					// tracking position
					if(!TrackFlag &&InitFlag==AllInit)
					{
						In=Current;
						DisplayIn();
					}
					break;
				case 'p':
				case 'P':
					PECFlag=!PECFlag;
					WritePECStatus();
					if(PECFlag)
						SynchPEC();
					break;
				case 's':
				case 'S':
					AlignMs();
					MenuMsArcsecSec();
					PauseUntilClockTick();
					break;
				case 'g':
				case 'G':
					AlignMs();
					MenuGuideArcsecSec();
					PauseUntilClockTick();
					break;
				case 'a':
				case 'A':
					AlignMs();
					MenuAltaz();
					PauseUntilClockTick();
					GetEquat();
					break;
				case 'e':
				case 'E':
					AlignMs();
					MenuEquat();
					PauseUntilClockTick();
					GetEquat();
					break;
				case 'o':
				case 'O':
					AlignMs();
					MenuOffset();
					PauseUntilClockTick();
					GetEquat();
					break;
				case 'f':
				case 'F':
					AlignMs();
					MenuCoordFile();
					PauseUntilClockTick();
					GetEquat();
					break;
				case 'i':
				case 'I':
					AlignMs();
					MenuInit();
					PauseUntilClockTick();
					GetEquat();
					break;
				case 'd':
				case 'D':
					AlignMs();
					MenuDrift();
					PauseUntilClockTick();
					break;
				case 'r':
				case 'R':
					MenuResetToIn();
					GetEquat();
					break;
				case 'm':
				case 'M':
					// scope will slew or track to the new Current Ra and Dec
					Current.Ra=In.Ra;
					Current.Dec=In.Dec;
					PUT_SCOPE;
					break;
				case 'q':
				case 'Q':
					QuitFlag=Yes;
			}
		}
		else
			HandpadEvent::ProcessEvents();
	}
}

