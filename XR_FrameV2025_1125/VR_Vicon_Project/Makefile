#!/bin/smake -f
# makefile for interactive CG programing workshop
# 2008-2020 T.OHSHIMA

# 使い方：
# makeコマンドから参照されます．
# make TARGET=プログラム名（拡張子省略）
# 例）make TARGET=sample1

CC	= g++
OPTION =
#OPTION	= -O2 -Wall

# use WIN for Cygwin
#OS	= LINUX
#OS	= WIN
OS = MAC

ifeq ($(OS), LINUX) 
#LFLAG	= -L/usr/X11R6/lib -lm -lX11 -lGL -lGLU -lglut -lXext -lXmu -lXi
LFLAG	= -L/usr/X11R6/lib -lm -lGL -lGLU -lglut -lXext -lXmu -lXi
endif
ifeq ($(OS), WIN)
LFLAG	= -lm -lglut32 -lglu32 -lopengl32
endif
ifeq ($(OS), MAC)
LFLAG = -framework GLUT -framework OpenGL -mmacosx-version-min=10.8
endif
SRCS = GLMetaseq.c calc.cpp cylindrical.cpp ezTrack.cpp ezUtil.cpp draw.cpp keyboard.cpp mouse.cpp light.cpp main.cpp projview.cpp sim.cpp stereo.cpp mymodel.cpp
default: $(TARGET)

.cpp.o:
	$(CC) -c $<

.c.o:
	$(CC) -c $<

$(TARGET):
	$(CC) -o $@ $(SRCS) $(OPTION) $(LFLAG)

ifeq ($(OS),LINUX)
clean:
	@rm -f *.o *.out
endif
ifeq ($(OS), WIN)
clean:
	@rm -f *.o *.exe
endif
ifeq ($(OS), MAC)
clean:
	@rm -f *.o *.out
endif

cleanall: clean
	@rm -f *~ *.*~ *.*.*~
