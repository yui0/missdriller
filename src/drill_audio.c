/* drill2x mikmod support and modification made by JyCet */
#include "drill_audio.h"
#include "CWavs.h"

extern int General_Volume;
void SetVolume( unsigned char volume)
{
//#ifdef __SYMBIAN32__
#ifndef __MIKMOD__
	General_Volume = volume;
	Mix_VolumeMusic(General_Volume);
#else
	m_volume = (volume<<7)/100;
	s_volume = (volume<<8)/100;
	md_musicvolume = m_volume;
#endif
}

#ifndef __SYMBIAN32__

Mix_Music *music;
inline void InitMikmod()
{
#ifdef __MIKMOD__
	/* register all the drivers */
//	printf("register all the drivers\n");
	drv_oss.CommandLine("buffer=12,count=2");
	MikMod_RegisterDriver(&drv_oss);
	printf("register MOD loader\n");
	MikMod_RegisterLoader(&load_mod);
	MikMod_RegisterLoader(&load_s3m);
	// initialize the library
	//printf("initialize the library\n");
	md_mixfreq = 22100;
	md_mode = DMODE_16BITS | DMODE_STEREO | DMODE_SOFT_MUSIC | DMODE_SOFT_SNDFX;
	//MikMod_Init("");
	if (MikMod_Init("")) {
		fprintf(stderr, "Could not initialize sound, reason: %s\n",
			MikMod_strerror(MikMod_errno));
		return;
	}
	MikMod_SetNumVoices(-1, 4);
	MikMod_EnableOutput();
#endif
}

inline void FreeMikmod()
{
#ifdef __MIKMOD__
	MikMod_Exit();
	return;
#endif
}

inline void UpdateMikmod()
{
#ifdef __MIKMOD__
	if (Player_Active()) MikMod_Update();
	return;
#endif
	/*
	* 演奏開始。2 番目の引数には演奏回数を指定する。
	* -1 なら無限に繰り返す。
	*/
	//Mix_PlayMusic(music, -1);
}

#ifdef __MIKMOD__
void LoadSample()
{
	FILE *fp;
	char g_string[255];
	char args[255];
	int argd;
	char ligne[256];

	//if ((fp = fopen( "./data/wav/wav.txt" , "r")) != NULL) {
	if ((fp = fopen("/opt/game/missdriller/data/wav.txt", "r")) != NULL) {
		while (fgets(ligne, sizeof(ligne), fp) != NULL) {

			sscanf(ligne, "%d %s", &argd,(char*)&args);

			if ( argd == 10 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx10 = Sample_Load( g_string );
			} else if ( argd == 11 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx11 = Sample_Load( g_string );
			} else if ( argd == 12 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx12 = Sample_Load( g_string );
			} else if ( argd == 13 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx13 = Sample_Load( g_string );
			} else if ( argd == 14 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx14 = Sample_Load( g_string );
			} else if ( argd == 15 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx15 = Sample_Load( g_string );
			} else if ( argd == 16 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx16 = Sample_Load( g_string );
			} else if ( argd == 17 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx17 = Sample_Load( g_string );
			} else if ( argd == 18 ) {
				sprintf((char*)g_string, "%s%s" , wavdir , args);
				sfx18 = Sample_Load( g_string );
			}
		}
		fclose(fp);
	}

	return;
}

void FreeSample()
{
	Sample_Free(sfx10);
	Sample_Free(sfx11);
	Sample_Free(sfx12);
	Sample_Free(sfx13);
	Sample_Free(sfx14);
	Sample_Free(sfx15);
	Sample_Free(sfx16);
	Sample_Free(sfx17);
	Sample_Free(sfx18);
}

unsigned char voice_num = 0;
void PlaySample(SAMPLE *sample)
{
	switch (voice_num) {
	case 0:
		v1 = Sample_Play( sample, 0, 0);
		Voice_SetVolume(v1 , s_volume);
		break;
	case 1:
		v2 = Sample_Play( sample, 0, 0);
		Voice_SetVolume(v2 , s_volume);
		break;
	case 2:
		v3 = Sample_Play( sample, 0, 0);
		Voice_SetVolume(v3 , s_volume);
		break;
	case 3:
		v4 = Sample_Play( sample, 0, 0);
		Voice_SetVolume(v4 , s_volume);
		break;
	}
	++voice_num;
	voice_num&=3;
	return;
}
#endif
#endif
extern char moddir[256];

void FreeModuleName()
{
	free(mod_TITLE);
	free(mod_HISCORE);
	free(mod_GAME);
}

void LoadModuleName()
{
	FILE *fp;
	char arg1[255];
	char arg2[255];
	char ligne[256];
	char g_string[255];
	sprintf((char*)g_string, "%smod.txt", moddir);
	//printf("[%s]\n", g_string);

	if ((fp = fopen(g_string, "r")) != NULL) {
		while (fgets(ligne, sizeof(ligne), fp) != NULL) {
			sscanf(ligne, "%s %s", (char*)&arg1, (char*)&arg2);
			//printf("[%s]\n", ligne);

			if (strcmp(arg1,"title")==0) {
				mod_TITLE = (char*) calloc( strlen(arg2) + 1 , sizeof(char));
				strcpy( mod_TITLE , arg2 );
				//printf("[%s]\n", arg2);
			} else if (strcmp(arg1,"hiscore")==0) {
				mod_HISCORE = (char*) calloc( strlen(arg2) + 1 , sizeof(char));
				strcpy( mod_HISCORE , arg2 );
				//printf("[%s]\n", arg2);
			} else if (strcmp(arg1,"game")==0) {
				mod_GAME = (char*) calloc( strlen(arg2) + 1 , sizeof(char));
				strcpy( mod_GAME , arg2 );
				//printf("[%s]\n", arg2);
			}
		}
		fclose(fp);
	}

	return;
}

void LoadModule(char *modulename)
{
	char g_string[255];
	sprintf((char*)g_string,"%s%s" ,moddir, modulename);
#ifdef __SYMBIAN32__
	CWavsLoadMusic(g_string);
#else
#ifdef __MIKMOD__
	//check module played or not
	if (Player_Active()) FreeModule();

	module = Player_Load(g_string, MAX_CHANNEL, 0);
#endif
	//printf("Loading the module [%s]\n", g_string);
	music = Mix_LoadMUS(g_string);
	if (!music) printf("impossible to read [%s]: %s\n", g_string, Mix_GetError());
#endif
}

inline void StartModule()
{
#ifdef __SYMBIAN32__
	CWavsPlayMusic();
#else
#ifdef __MIKMOD__
	if (module) {
		//loop
		module->wrap = 1;
		// Start playing the module
		printf("Start playing the module\n");
		Player_Start(module);
	} else printf("impossible to read module\n");
#endif
	// 演奏開始。2 番目の引数には演奏回数を指定する。
	// -1 なら無限に繰り返す。
	Mix_PlayMusic(music, -1);
#endif
}

void FreeModule(void)
{
#ifdef __SYMBIAN32__
	CWavsFreeMusic();
#else
#ifdef __MIKMOD__
	Player_Stop();
	Player_Free(module);
#endif
	/* 演奏停止 */
	Mix_HaltMusic();
	/* 読み込んだ曲を解放 */
	Mix_FreeMusic(music);
#endif
}
