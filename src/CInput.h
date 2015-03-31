#ifndef _CInput_h_
#define _CInput_h_

/*

structure for joystick and key input

*/

#include "SDL.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUTTON_UP		0
#define BUTTON_DOWN		1
#define BUTTON_LEFT		2
#define BUTTON_RIGHT	3
#define BUTTON_0		4
#define BUTTON_1		5
#define BUTTON_2		6
#define BUTTON_3		7
#define BUTTON_START		8
/* JyCet volume add */
#define BUTTON_VOLUP		9
#define BUTTON_VOLDOWN		10

#define MAX_BUTTONS		11

/* JyCet volume add */
#define GP2X_BUTTON_VOLUP           (16)
#define GP2X_BUTTON_VOLDOWN         (17)
#define GP2X_BUTTON_START           (8)
#define GP2X_BUTTON_Y               (15)

typedef struct{
	
	int button[MAX_BUTTONS];
	int hold[MAX_BUTTONS];
	int repeat[MAX_BUTTONS];
	SDL_Joystick *joystick;
	int min_axis;
	
	
}CInput;





CInput* CInputInit(int,int);
void CInputHoldArrows(CInput *);
void CInputUnholdArrows(CInput *);
void CInputDefaultSetting(CInput *this);
void CInputUpdate(CInput *this,int);
void CInputFree(CInput *this);
void CInputHoldButtons(CInput *);
void CInputUnholdButtons(CInput *);
void CInputSetMinAxis(CInput *this,int x);



#endif
