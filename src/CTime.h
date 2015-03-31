#ifndef _CTime_h_included
#define _CTime_h_included
/************************

wait routine
you must use CTimeReset before using CTimeWait.

ex:

CTimeReset(&time);
do{
	...
	CTimeWait(&time);
}while(1);



************************/
#include "SDL.h"
#include "SDL_timer.h"
#define FPS_MAX 35

//ŽžŠÔ
typedef struct{
	
	Uint32 interval;
	Uint32 nowtime;
	Uint32 lasttime;
	Uint32 under;
	int isDelay;
	Uint32 clock;//milisecond.
	
	Uint32 fpsclock;
	Uint32 framecount;
	Uint32 fps;
	
	
}CTime;

int CTimeReset(CTime *,int);
int CTimeWait(CTime * );
//int CTimeChangeFPS(CTime *,int);

#endif
