#include "CInput.h"
#include "CWavs.h"
CInput* CInputInit(int joystick_number,int enable_joystick){
	
	
	
	CInput *p;
	
	p=(CInput *)malloc(sizeof(CInput));
	if(!p){
		fprintf(stderr,"error:CInputInit");
		return NULL;
	}
	memset(p,0,sizeof(CInput));
	
	
	if(enable_joystick){
		SDL_Init(SDL_INIT_JOYSTICK);
		p->joystick = SDL_JoystickOpen(joystick_number);
		if ( p->joystick == NULL ) {
			printf("Couldn't open joystick \n");
		}
	}
	return p;
	
}

void CInputFree(CInput *this){
	if(this->joystick)SDL_JoystickClose(this->joystick);
	free(this);
}

extern int General_Volume;

void CInputUpdate(CInput *this,int disabled){
	
	int i;
	Uint8 *keys;
	
	for(i=0;i<MAX_BUTTONS;++i)this->button[i]=0;
	if(disabled)return;
	if(this->joystick){
		if(SDL_JoystickGetButton(this->joystick, 13) == SDL_PRESSED)this->button[BUTTON_0]=1;
		if(SDL_JoystickGetButton(this->joystick, 12) == SDL_PRESSED)this->button[BUTTON_1]=1;
		if(SDL_JoystickGetButton(this->joystick, 14) == SDL_PRESSED)this->button[BUTTON_2]=1;
		if(SDL_JoystickGetButton(this->joystick, 15) == SDL_PRESSED)this->button[BUTTON_3]=1;
		if(SDL_JoystickGetButton(this->joystick, 0) == SDL_PRESSED)this->button[BUTTON_UP]=1;
		if(SDL_JoystickGetButton(this->joystick, 4) == SDL_PRESSED)this->button[BUTTON_DOWN]=1;
		if(SDL_JoystickGetButton(this->joystick, 6) == SDL_PRESSED)this->button[BUTTON_RIGHT]=1;
		if(SDL_JoystickGetButton(this->joystick, 2) == SDL_PRESSED)this->button[BUTTON_LEFT]=1;
		if(SDL_JoystickGetButton(this->joystick, 8) == SDL_PRESSED)this->button[BUTTON_START]=1;
		
		if(SDL_JoystickGetButton(this->joystick, 16) == SDL_PRESSED) {
		  if(General_Volume<=110)General_Volume+=10;
		  Mix_VolumeMusic(General_Volume);
		}
		if(SDL_JoystickGetButton(this->joystick, 17) == SDL_PRESSED) {
		  if(General_Volume>=10)General_Volume-=10;
		  Mix_VolumeMusic(General_Volume);
		}
	}
	keys = SDL_GetKeyState(NULL);
	
	if ( keys[SDLK_UP] == SDL_PRESSED){this->button[BUTTON_UP]=1;}
	if ( keys[SDLK_DOWN] == SDL_PRESSED ) {this->button[BUTTON_DOWN]=1;}
	if ( keys[SDLK_RIGHT] == SDL_PRESSED ) {this->button[BUTTON_RIGHT]=1;}
	if ( keys[SDLK_LEFT] == SDL_PRESSED ) {this->button[BUTTON_LEFT]=1;}
	
	if ( keys[' '] == SDL_PRESSED || keys[SDLK_F5] == SDL_PRESSED || keys['*'] == SDL_PRESSED || keys['.'] == SDL_PRESSED )this->button[BUTTON_0]=1;
	if ( keys[SDLK_BACKSPACE] == SDL_PRESSED ||  keys['0'] == SDL_PRESSED)this->button[BUTTON_1]=1;
	if ( keys['\r'] == SDL_PRESSED || keys['#'] == SDL_PRESSED)this->button[BUTTON_2]=1;
	if ( keys[SDLK_ESCAPE] == SDL_PRESSED )this->button[BUTTON_START]=1;
	for(i=0;i<MAX_BUTTONS;++i){
		if(!this->button[i]){
			this->repeat[i]=0;
		}else{
			
			if(this->hold[i] && this->repeat[i]){
				this->button[i]=0;
			}
			this->repeat[i]=1;
		}
		
	}
	
}


void CInputDefaultSetting(CInput *this){
	
	int i;
	
	for(i=0;i<MAX_BUTTONS;++i){
		if(i<4)this->hold[i]=0;else this->hold[i]=1;
	}
	this->min_axis=10000;
}

void CInputSetMinAxis(CInput *this,int x){
	this->min_axis=x;
}
void CInputHoldArrows(CInput *this){
	this->hold[BUTTON_UP]=1;
	this->hold[BUTTON_DOWN]=1;
	this->hold[BUTTON_LEFT]=1;
	this->hold[BUTTON_RIGHT]=1;
}
void CInputUnholdArrows(CInput *this){
	
	this->hold[BUTTON_UP]=0;
	this->hold[BUTTON_DOWN]=0;
	this->hold[BUTTON_LEFT]=0;
	this->hold[BUTTON_RIGHT]=0;
}
void CInputHoldButtons(CInput *this){
	this->hold[BUTTON_0]=1;
	this->hold[BUTTON_1]=1;
	this->hold[BUTTON_2]=1;
	this->hold[BUTTON_3]=1;
	this->hold[BUTTON_START]=1;
/* JyCet volume add */
	this->hold[BUTTON_VOLUP]=1;
	this->hold[BUTTON_VOLDOWN]=1;
}
void CInputUnholdButtons(CInput *this){
	
	this->hold[BUTTON_0]=0;
	this->hold[BUTTON_1]=0;
	this->hold[BUTTON_2]=0;
	this->hold[BUTTON_3]=0;
	this->hold[BUTTON_START]=0;
/* JyCet volume add */
	this->hold[BUTTON_VOLUP]=0;
	this->hold[BUTTON_VOLDOWN]=0;
}
