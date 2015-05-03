TOP=../../../../..
DIRNAME=src/modules/OGIModules/Flinger/MIDI

H = AllEvent.h ChanPres.h Event.h EvntUtil.h KeyPres.h MChanPrf.h \
MCopy.h MCue.h MEOT.h MEvent.h MInstNam.h MKey.h MLyric.h MMarker.h \
MPortNum.h MSMPTE.h MSeqNam.h MSeqNum.h MSeqSpec.h MTempo.h MText.h \
MTime.h MUnknown.h NormEvnt.h Note.h NoteOff.h NoteOn.h Param.h \
PitchWhl.h Program.h SMFHead.h SMFTrack.h SMFUtils.h SysEx.h \
EvntTree.h Song.h rb.h list.h tcl.h

SRCSCXX = ChanPres.cc Event.cc EvntUtil.cc KeyPres.cc MChanPrf.cc \
MCopy.cc MCue.cc MEOT.cc MEvent.cc MInstNam.cc MKey.cc MLyric.cc \
MMarker.cc MPortNum.cc MSMPTE.cc MSeqNam.cc MSeqNum.cc MSeqSpec.cc \
MTempo.cc MText.cc MTime.cc MUnknown.cc NormEvnt.cc Note.cc NoteOff.cc \
NoteOn.cc Param.cc PitchWhl.cc Program.cc SysEx.cc \
SMFHead.cc SMFTrack.cc SMFUtils.cc SysEx.cc EvntTree.cc Song.cc 


SRCSC = rb.c list.c

SRCS = $(SRCSCXX) $(SRCSC)

OBJS = $(SRCSCXX:.cc=.o)  $(SRCSC:.c=.o) 


OBJS = $(SRCSC:.c=.o) $(SRCSCXX:.cc=.o)
 
FILES = Makefile $(SRCS) $(H)
 
LOCAL_INCLUDES = -I../../../include 


 
INLIB = $(TOP)/src/lib/libFestival.a
 
ALL = .buildlib
 
include $(TOP)/config/common_make_rules



