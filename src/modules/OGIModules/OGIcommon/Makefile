TOP=../../../..

DIRNAME=src/modules/OGIModules/OGIcommon

H = OGI_Buffer.h OGI_file.h OGI_Filter.h OGI_LPC.h OGI_Macro.h OGI_Math.h OGI_Math.h OGI_SingleTrack.h OGI_TimeWarp.h OGI_Track.bak.h OGI_Track.h OGI_Vector.h OGI_Wave.h OGI_WaveChunk.h OGIcommon.h OGIduration.h OGIgain.h OGIpitch.h

CPPSRCS = OGI_Buffer.cc OGI_file.cc OGI_Filter.cc OGI_LPC.cc OGI_Math.cc OGI_SingleTrack.cc OGI_TimeWarp.cc OGI_Track_file_templates.cc OGI_Track_file.cc OGI_Track.cc OGI_Vector.cc OGI_WaveChunk.cc OGIcommon.cc OGIduration.cc OGIgain.cc OGIpitch.cc hash_s_dvec_t.cc hash_s_fvec_t.cc vector_EST_IVector.cc vector_l_t.cc vector_ptr_t.cc

CFLAGS=-D INSTANTIATE_TEMPLATES=1

CPPFLAGS=-D INSTANTIATE_TEMPLATES=1

SRCS = $(CPPSRCS)

OBJS = $(SRCS:.cc=.o)
 
FILES = Makefile $(SRCS) $(H)
 
LOCAL_INCLUDES = -I../../../src/include
 
INLIB = $(TOP)/src/lib/libFestival.a
 
ALL = .buildlib
 
include $(TOP)/config/common_make_rules

