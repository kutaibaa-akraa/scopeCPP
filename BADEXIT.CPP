#include <conio.h>
#include <process.h>
#include <iostream.h>

#include "badexit.hpp"

BadExit::BadExit(char* Msg)
{
	clrscr();
	gotoxy(1,1);
	cout<<"Error: "<<Msg<<".";
	gotoxy(1,2);
	cout<<"Press any key to continue...";
	while (!kbhit())
	;
	_exit(1);
}
