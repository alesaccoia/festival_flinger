#############################<--OGI-->######################################
##                                                                        ##
##             Center for Spoken Language Understanding                   ##
##        Oregon Graduate Institute of Science & Technology               ##
##                         Portland, OR USA                               ##
##                        Copyright (c) 2000                              ##
##                                                                        ##
##      This module is not part of the CSTR/University of Edinburgh       ##
##               release of the Festival TTS system.                      ##
##                                                                        ##
##  In addition to any conditions disclaimers below, please see the file  ##
##  "license_ogi_tts.txt" distributed with this software for information  ##
##  on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES.  ##
##                                                                        ##
#############################<--OGI-->######################################
TOP=../../..
DIRNAME=src/modules/OGIModules

LIB_BUILD_DIRS = OGIdbase OGIeffect OGIinsert OGIcommon OGIresLPC Flinger
BUILD_DIRS = $(LIB_BUILD_DIRS)
ALL_DIRS = $(BUILD_DIRS)

FILES = Makefile init_ogi_modules.cc

OBJS= init_ogi_modules.o

 
INLIB = $(TOP)/src/lib/libFestival.a
 
ALL = .buildlib
 
include $(TOP)/config/common_make_rules

