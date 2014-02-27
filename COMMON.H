#ifndef COMMON_H
#define COMMON_H

typedef unsigned char Byte;
typedef unsigned char Flag;
typedef signed char SignedByte;

const Byte False=0;
const Byte True=1;
const Byte None=0;
const Byte High=1;
const Byte Low=0;
const Byte ByteLogicalHigh=255;
const Byte ByteLogicalLow=0;
const Byte PortOn=255;
const Byte PortOff=0;
const Byte Yes=1;
const Byte No=0;
const Byte On=1;
const Byte Off=0;
const Byte Plus=1;
const Byte Minus=0;
const Byte Positive=1;
const Byte Negative=0;
const Byte CW=2;
const Byte CCW=1;
const Byte NoRotation=0;
const int NameSize=40;
const int ValueSize=40;

#define ContMsg "Press any key to continue..."

#define HorizBorder       196
#define VertBorder        179
#define TopLeftCorner     218
#define TopRightCorner    191
#define BottomLeftCorner  192
#define BottomRightCorner 217

#define DblHorizBorder       205
#define DblVertBorder        186
#define DblTopLeftCorner     201
#define DblTopRightCorner    187
#define DblBottomLeftCorner  200
#define DblBottomRightCorner 188

// extended keyboard codes...

#define ExtendedKeyboardStroke 0

#define F1  59
#define F2  60
#define F3  61
#define F4  62
#define F5  63
#define F6  64
#define F7  65
#define F8  66
#define F9  67
#define F10 68
#define F11 133
#define F12 134

#define Shift_F1  84
#define Shift_F2  85
#define Shift_F3  86
#define Shift_F4  87
#define Shift_F5  88
#define Shift_F6  89
#define Shift_F7  90
#define Shift_F8  91
#define Shift_F9  92
#define Shift_F10 93

#define Ctrl_F1  94
#define Ctrl_F2  95
#define Ctrl_F3  96
#define Ctrl_F4  97
#define Ctrl_F5  98
#define Ctrl_F6  99
#define Ctrl_F7  100
#define Ctrl_F8  101
#define Ctrl_F9  102
#define Ctrl_F10 103

#define Alt_F1  104
#define Alt_F2  105
#define Alt_F3  106
#define Alt_F4  107
#define Alt_F5  108
#define Alt_F6  109
#define Alt_F7  110
#define Alt_F8  111
#define Alt_F9  112
#define Alt_F10 113

#endif
