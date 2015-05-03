TOP=../../../..
DIRNAME=src/modules/OGIModules/Flinger
BUILD_DIRS = MIDI

SRCSCXX = flinger.cc fl_io.cc  fl_midi_in.cc fl_midi_parse.cc fl_midi_print.cc fl_lyricize.cc fl_token.cc fl_duration.cc fl_pitch_and_gain.cc fl_JustTuning.cc GenList.cc

H = fl_io.h  fl_midi_in.h  fl_midi_parse.h fl_midi_print.h fl_midi_ext.h f fl_token.h l_lyricize.h fl_duration.h fl_pitch.h GenList.h

SRCS = $(SRCSCXX)

OBJS = $(SRCSCXX:.cc=.o) 

FILES=Makefile $(SRCS) $(H)

LOCAL_INCLUDES = -I../include -I../OGIcommon -IMIDI

INLIB = $(TOP)/src/lib/libFestival.a

ALL = .buildlib $(BUILD_DIRS)
 
include $(TOP)/config/common_make_rules


