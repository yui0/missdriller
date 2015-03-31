/* drill2x mikmod support and modification made by JyCet */
#include <mikmod.h>
#include <string.h>

#define MAX_CHANNEL	32

extern char wavdir[256];
#ifndef __SYMBIAN32__
MODULE *module;
/* sound effects */
SAMPLE *sfx10,*sfx11,*sfx12,*sfx13,*sfx14,*sfx15,*sfx16,*sfx17,*sfx18;
/* voices */
int v4,v1,v2,v3;
unsigned char m_volume;
unsigned int s_volume;
#endif
signed char g_volume;
char *mod_TITLE;
char *mod_HISCORE;
char *mod_GAME;
#ifndef __SYMBIAN32__

void InitMikmod(void);
void UpdateMikmod(void);

void LoadSample(void);
void PlaySample( SAMPLE *sample );
void FreeSample(void);
void FreeMikmod(void);

#endif
void LoadModuleName(void);
void LoadModule( char *modulename );
void StartModule( void );

void FreeSample(void);
void FreeModule(void);
void FreeModuleName(void);
void SetVolume(unsigned char volume);
