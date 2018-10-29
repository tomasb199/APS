CC		= g++
CFLAGS	= -O3 -Wall -Wno-deprecated -IGL
UNAME := $(shell uname -s)

ALL =   Mandelbrot

all:  $(ALL)

%: %.cpp
	$(CC) -o $@ $(CFLAGS) $< $(LFLAGS)

# Windows
LFLAGS	= -lm -lglew32 -lFreeGLUT -lOpenGL32 -lglu32 -lpthread
clean:
	-del *.o
