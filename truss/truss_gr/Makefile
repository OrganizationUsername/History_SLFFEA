# SLFFEA Makefile
# Version:  1.5
# Copyright (C) 1999, 2000  San Le (slffea@yahoo.com)
# http://slffea.com/
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


all: tspost 

LIB1 = -I/usr/local/include -O2 -funroll-loops -ansi -pedantic \
	 -ffast-math -I/usr/X11R6/include -DSHM 

LIB2 = -L/usr/local/lib -lglut -lGLU -lGL -lm -L/usr/X11/lib \
	-L/usr/X11R6/lib -lX11 -lXext -lXmu -lXt -lXi -lSM -lICE

OBJECTS_TS = ../truss/tsmemory.o ../truss/tsreader.o 

OBJECTS_TSH = ../truss/tsconst.h ../truss/tsstruct.h 

COMMON = ../../common

COMMON_GR = ../../common_gr

OBJECTS_GR = $(COMMON_GR)/pwinston.o $(COMMON_GR)/axes.o \
	$(COMMON_GR)/mshcommon.o $(COMMON_GR)/crtlcommon.o \
	$(COMMON_GR)/printext.o $(COMMON_GR)/screenshot.o \
	$(COMMON_GR)/filecheck.o $(COMMON_GR)/pointlocate.o \
	$(COMMON_GR)/axeslabel.o $(COMMON_GR)/quat.o \
	$(COMMON_GR)/crossplane.o

OBJECTS_GRH = $(COMMON_GR)/color_gr.h $(COMMON_GR)/control.h 

OBJECTS2 = $(COMMON)/crossx.o $(COMMON)/matx.o

COMMON_GR = ../../common_gr

PROGS = tspost 

CC = /usr/bin/gcc

tspost: tspost.o \
		tsmemgr.o \
		tsreadgr.o \
		tsparam.o \
		tsregetparam.o \
		tsset.o \
		bc_vec.o \
		tsmshdsp.o \
		tsmesh.o \
		tsmshkey.o \
		tsmenu.o \
		tsgetnew.o \
		tscrtldsp.o \
		tscrtlmse.o \
		$(OBJECTS_TSH) \
		$(OBJECTS_GRH) \
		tsgui.h \
		tsstrcgr.h 
	$(CC) -o tspost $(LIB1) tspost.o \
		tsmemgr.o \
		tsreadgr.o \
		tsparam.o \
		tsregetparam.o \
		tsset.o \
		bc_vec.o \
		tsmshdsp.o \
		tsmesh.o \
		tsmshkey.o \
		tsmenu.o \
		tsgetnew.o \
		tscrtldsp.o \
		tscrtlmse.o \
		$(OBJECTS_TS) $(OBJECTS_GR) $(OBJECTS2) $(LIB2) 

tscrtlmse.o: tscrtlmse.c 
	$(CC) -c tscrtlmse.c 

tscrtldsp.o: tscrtldsp.c 
	$(CC) -c tscrtldsp.c 

tsgetnew.o: tsgetnew.c 
	$(CC) -c tsgetnew.c 

tsmenu.o: tsmenu.c 
	$(CC) -c tsmenu.c 

tsmshkey.o: tsmshkey.c 
	$(CC) -c tsmshkey.c 

tsmesh.o: tsmesh.c 
	$(CC) -c tsmesh.c 

tsmshdsp.o: tsmshdsp.c 
	$(CC) -c tsmshdsp.c 

bc_vec.o: $(COMMON_GR)/bc_vec.c
	$(CC) -c $(COMMON_GR)/bc_vec.c -DTRUSS1

tsset.o: tsset.c 
	$(CC) -c tsset.c 

tsregetparam.o: tsregetparam.c 
	$(CC) -c tsregetparam.c 

tsparam.o: tsparam.c 
	$(CC) -c tsparam.c 

tsreadgr.o: tsreadgr.c 
	$(CC) -c tsreadgr.c 

tsmemgr.o: tsmemgr.c 
	$(CC) -c tsmemgr.c 

tspost.o: tspost.c 
	$(CC) -c tspost.c

clean:
	-rm $(PROGS)
	-rm *.o 

