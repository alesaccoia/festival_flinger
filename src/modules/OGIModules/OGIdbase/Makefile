TOP=../../../..
DIRNAME=src/modules/OGIModules/OGIdbase


LOCAL_INCLUDES = -I../../../src/include -I../OGIcommon 

# uncomment if including optional OGIsine support
#INCLUDE_OGISINE=1
#

H = OGIdbUnits.h OGIdbase.h OGIdbase_scm.h OGIresLPC_db.h OGIunitsel_diphone.h

TSRCS = OGIdbUnits.cc OGIresLPC_db.cc 

LOCAL_DEFINES=
ifdef INCLUDE_OGISINE
	LOCAL_DEFINES=-DINCLUDE_OGISINE
	H += OGIsine_db.h
	TSRCS += OGIsine_db.cc
	LOCAL_INCLUDES += -I../OGIsine
endif

LOCAL_DEFINES += -DTMATRIX_BOUNDS_CHECKING=0

CPPSRCS = OGIdbase.cc OGIdbase_scm.cc OGIunitsel_diphone.cc $(TSRCS)
SRCS = $(CPPSRCS)

OBJS = $(SRCS:.cc=.o)
 
FILES = Makefile $(SRCS) $(H)
 
INLIB = $(TOP)/src/lib/libFestival.a
 
ALL = .buildlib

 
include $(TOP)/config/common_make_rules
