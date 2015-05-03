TOP=../../../..
DIRNAME=src/modules/OGIModules/OGIresLPC

H = resLPC.h resLPC_concat.h resLPC_io.h resLPC_dump.h resLPC_vc.h \
resLPC_mod.h resLPC_wave.h noLPC_wave.h gmm.h matrix.h numeric.h stat.h \
resLPC_analysis.h resLPC_resyn.h resLPC_pmark.h pmark_analysis.h resLPC_vqual.h

SRCS = resLPC.cc resLPC_concat.cc resLPC_io.cc resLPC_dump.cc resLPC_vc.cc \
resLPC_mod.cc resLPC_scm.cc resLPC_wave.cc noLPC_wave.cc gmm.cc matrix.cc numeric.cc stat.cc  \
resLPC_analysis.cc resLPC_resyn.cc resLPC_pmark.cc pmark_analysis.cc resLPC_vqual.cc

OBJS = $(SRCS:.cc=.o)
 
FILES = Makefile $(SRCS) $(H)
 
LOCAL_INCLUDES = -I../../../src/include -I../OGIdbase -I ../OGIcommon

LOCAL_DEFINES = -DTMATRIX_BOUNDS_CHECKING=0



 
INLIB = $(TOP)/src/lib/libFestival.a
 
ALL = .buildlib
 
include $(TOP)/config/common_make_rules

