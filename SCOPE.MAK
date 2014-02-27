#
# Borland C++ IDE generated makefile
#
.AUTODEPEND


#
# Borland C++ tools
#
IMPLIB  = Implib
BCCDOS  = Bcc +BccDos.cfg 
TLINK   = TLink
TLIB    = TLib
TASM    = Tasm
#
# IDE macros
#


#
# Options
#
IDE_LFLAGSDOS =  -LD:\BC4\LIB
IDE_BFLAGS = 
LLATDOS_scopedexe =  -c -Tde -LD:\BC45\LIB;C:\BC45\LIB
RLATDOS_scopedexe = 
BLATDOS_scopedexe = 
CNIEAT_scopedexe = -ID:\BC45\INCLUDE;C:\BC45\INCLUDE -D
LNIEAT_scopedexe = -x
LEAT_scopedexe = $(LLATDOS_scopedexe)
REAT_scopedexe = $(RLATDOS_scopedexe)
BEAT_scopedexe = $(BLATDOS_scopedexe)

#
# Dependency List
#
Dep_scope = \
   scope.exe

scope : BccDos.cfg $(Dep_scope)
  echo MakeNode 

Dep_scopedexe = \
   atimes.obj\
   badexit.obj\
   cmosclk.obj\
   convert.obj\
   coord.obj\
   event.obj\
   iaca.obj\
   pport.obj\
   scope.obj\
   steppers.obj

scope.exe : $(Dep_scopedexe)
  $(TLINK)   @&&|
 /v $(IDE_LFLAGSDOS) $(LEAT_scopedexe) $(LNIEAT_scopedexe) +
D:\BC45\LIB\c0m.obj+
atimes.obj+
badexit.obj+
cmosclk.obj+
convert.obj+
coord.obj+
event.obj+
iaca.obj+
pport.obj+
scope.obj+
steppers.obj
$<,$*
D:\BC45\LIB\graphics.lib+
D:\BC45\LIB\bidsm.lib+
D:\BC45\LIB\emu.lib+
D:\BC45\LIB\mathm.lib+
D:\BC45\LIB\cm.lib

|

atimes.obj :  atimes.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ atimes.cpp
|

badexit.obj :  badexit.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ badexit.cpp
|

cmosclk.obj :  cmosclk.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ cmosclk.cpp
|

convert.obj :  convert.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ convert.cpp
|

coord.obj :  coord.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ coord.cpp
|

event.obj :  event.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ event.cpp
|

iaca.obj :  iaca.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ iaca.cpp
|

pport.obj :  pport.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ pport.cpp
|

scope.obj :  scope.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ scope.cpp
|

steppers.obj :  steppers.cpp
  $(BCCDOS) -c @&&|
 $(CEAT_scopedexe) $(CNIEAT_scopedexe) -o$@ steppers.cpp
|

# Compiler configuration file
BccDos.cfg : 
   Copy &&|
-W-
-R
-v
-vi
-H
-H=leo.csm
-mm
-f
-po
-X-
-2
-Og
-Ot
-Om
-Op
-Oi
-Ov
-H=SCOPE.CSM
-Z
-O
-O-e
-Ol
-Ob
-O-W
| $@


