TOP=../../../..
DIRNAME=src/modules/OGIModules/OGIeffect
H = effect.h OGIeffect.h effect_io.h delay.h reverb.h
CPPSRCS = effect.cc delay.cc effect_io.cc reverb.cc

SRCS = $(CPPSRCS)

OBJS = $(SRCS:.cc=.o)
 
FILES = Makefile $(SRCS) $(H)
 
LOCAL_INCLUDES = -I../../../src/include -I../OGIcommon
 
INLIB = $(TOP)/src/lib/libFestival.a
 
ALL = .buildlib
 
include $(TOP)/config/common_make_rules

