#include "CTime.h"

int CTimeWait(CTime *p){
	
	Uint32 lefttime;
	
	p->nowtime=SDL_GetTicks();
	p->under+=p->interval;
	lefttime=p->lasttime+(p->under>>16)-p->nowtime;
	
	p->isDelay=0;
	p->framecount++;

	p->lasttime+=(p->under>>16);
	p->clock+=(p->under>>16);
	p->fpsclock+=(p->under>>16);
	if(p->fpsclock>1000){
		p->fps=p->framecount;
		p->framecount=0;
		p->fpsclock-=1000;
	}
	p->under &= 0x0ffff;
	
	return(0);
}

int CTimeReset(CTime *p , int fpsmax){
	
	p->interval=( 65536.0*1000.0/fpsmax );
	p->nowtime=SDL_GetTicks();
	p->lasttime=p->nowtime;
	p->under=0;
	p->clock=0;
	p->isDelay=0;
	
	p->fps=0;
	p->fpsclock=0;
	p->framecount=0;
	
	
	return(0);
}

/*int CTimeChangeFPS(CTime *p,int fpsmax){
	p->interval=( 65536.0*1000.0/fpsmax );
}*/
