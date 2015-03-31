CC		= gcc
CFLAGS		= -Os
#CFLAGS		= -O2 -g
#CFLAGS		= -O2 -g -D_MINGW32_
#LDFLAGS		= -lSDL_mixer -lmikmod
LDFLAGS		= -lSDL_mixer

DEST		= .

LD		= gcc

SDL_LDFLAGS	= $(shell sdl-config --libs)
SDL_CFLAGS	= $(shell sdl-config --cflags)

MAKEFILE	= Makefile

OBJS		= drill.o drill_audio.o frame_skip.o CBmps.o Cffont.o CTime.o CWavs.o CInput.o

PRINT		= pr

PROGRAM		= drill

SHELL		= /bin/sh

%.o:		%.c
		$(CC) $(SDL_CFLAGS) $(CFLAGS) -c $<
all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		@echo "Linking $(PROGRAM) ..."
		$(LD) -o $(PROGRAM) $(OBJS)  $(LDFLAGS) $(SDL_LDFLAGS)

