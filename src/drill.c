/*
miss driller
2000/7/7,adas
coding is restarted from 2000/8/28

Modified 2005/12/27 by Ken Fazzone for gp2x
Modified 2011/2/6 by Yuichiro Nakada for Berry Linux
*/

#include "drill.h"
#include "Cffont.h"
#include "CTime.h"
#include "CInput.h"
#include "CWavs.h"
#include "frame_skip.h"
#include "drill_audio.h"

CWavs *wavs;
#define WAVMAX	100
/*char wavfile[256]="c:\\resource\\apps\\missdriller\\wav.txt";
char wavdir[256]="c:\\resource\\apps\\missdriller\\wav\\";
char moddir[256]="c:\\resource\\apps\\missdriller\\mod\\";*/
char wavfile[256]=DRILL_PATH"/data/wav.txt";
char wavdir[256]=DRILL_PATH"/data/wav/";
char moddir[256]=DRILL_PATH"/data/mod/";
extern char *mod_TITLE;
extern char *mod_HISCORE;
extern char *mod_GAME;
extern signed char g_volume;
unsigned char flag_show_volume;
#define VOL_SHOW_DELAY	30
#define FADE_SPEED	15
#define SCROLL_SPEED	33
#define CPU_SPEED	66
extern Uint8 show_fps;

//ススムくん
CBmps *playerbmps=NULL;
char playerbmpsfile[256]="player.txt";
#define PLAYER_NUM	256

//char KDataDrive = 'C';
//#define KStorageCardD 'D'
//#define KStorageCardE 'E'

// static variables

SDL_Surface *screen_back;
SDL_Surface *screen_tmp;
SDL_Surface *screen_black;
SDL_Surface *screen_front;
SDL_Surface *bg_tmp;

char offset_x, offset_y;

CTime gametime;

#define WALKSPEED	4
#define WALKSPEED_Y	6
#define PREFALLTIME	72

#define NO_BLOCK	100
#define AIR_BLOCK 20
#define HARD_BLOCK 5
#define CLEAR_BLOCK 4

CBmps* cbmps_pause;

#define	BLOCK_NUM 256
CBmps* cbmps_blocks;
//char blockfile[256]="c:\\resource\\apps\\missdriller\\blockbg.txt";
char blockfile[256]=DRILL_PATH"/data/blockbg.txt";

Cffont *font=NULL;
//char defaultFontFile[256]="c:\\resource\\apps\\missdriller\\engrave8.bmp";
char defaultFontFile[256]=DRILL_PATH"/data/engrave8.bmp";

#define	CHARACTER_NUM 256
CBmps* cbmps_character;
//char characterfile[256]="c:\\resource\\apps\\missdriller\\charabg.txt";
char characterfile[256]=DRILL_PATH"/data/charabg.txt";

#define	OTHER_GRAPHICS_NUM 256
CBmps* cbmps_other;
//char otherfile[256]="c:\\resource\\apps\\missdriller\\otherbg.txt";
char otherfile[256]=DRILL_PATH"/data/otherbg.txt";

#define BLOCKSTATE_NONE	0
#define BLOCKSTATE_FALLING	1
#define BLOCKSTATE_PREFALL	2
#define BLOCKSTATE_EXTINGUISHING	3
#define BLOCKSTATE_FALLFINISHED	4
#define BLOCKSTATE_PREEXTINGUISHING	5
#define BLOCKSTATE_EXTINGUISHED	6
#define BLOCKSTATE_FALLSTOP	7

typedef struct
{
	int state;
	int type;
	int left;//for falling(dy)
	int lefttime;// for prefall wait time
	int done;
	int done_sub;

	int unsetlock;

	int destroycount;//for X block

	int shape;//for soft blocks

	int extinguishingframe;

	int player_dug;

}TBlockState;

#define GAME_STAGE_WIDTH 9
#define GAME_STAGE_HEIGHT (100+10)
#define STAGE_START_Y	5
//TBlockState gamestage[GAME_STAGE_WIDTH][GAME_STAGE_HEIGHT];
TBlockState** gamestage;

#define STAGE_WIDTH 9
#define STAGE_HEIGHT 9
#define STAGE_X	0
#define STAGE_Y	0

/**
game variables
**/

int my_x;
int my_y;

int dig;
int vx;
int vy;

int dig_graphic;

#define PENALTY_FRAMES	60
int penaltyframe;
int penaltybgnum;

int movingframe=0;

int fallingframe=0;

int direction;
#define DIR_DOWN	0
#define DIR_UP	1
#define DIR_LEFT	2
#define DIR_RIGHT	3
#define DIR_NONE	4

#define SCOREMEMBER 10
typedef struct
{
	char name[SCOREMEMBER+1][4];
	int time[SCOREMEMBER+1];
	int depth[SCOREMEMBER+1];
	int score[SCOREMEMBER+1];
}THighScore;

THighScore highscoredata;
THighScore fasttimedata;

THighScore *entrydata;
char *highscoreformat="%3s\n%10d\n%10d\n%10d\n";
/*#ifdef UIQ3
char *scorefile="c:\\shared\\missdriller\\drill.scr";
char *fasttimefile="c:\\shared\\missdriller\\drilltime.scr";
#else
char *scorefile="c:\\data\\missdriller\\drill.scr";
char *fasttimefile="c:\\data\\missdriller\\drilltime.scr";
#endif*/
char *scorefile=DRILL_PATH"/data/drill.scr";
char *fasttimefile=DRILL_PATH"/data/drilltime.scr";
typedef struct
{
	int x;
	int y;
	SDL_Surface *bmp;
	int clock;

}CTileScroll;
CTileScroll tscroll;

//キーを押しつづけてる？
int repeat_x;
int repeat_y;
int climbing;
int destroyed=0;
int dig_repeat=0;

//score
int my_depth;
int my_score;
int my_air;

int got_air;

int my_dead;
int my_deadcount;

int my_clear;
int my_clearcount;

int airdowncount;
int airdownspeed;
int airplus;

int my_fps;
int my_time;

//int scorecount;
int scoreplus;
int scorerest;

int lap_showing;
int lapcount;
char lapstring[64];

int airminus;
int airminuscount;

/** game setting**/
int setting_fullscreen=0;
//char setting_bitmapdir[128]="c:\\resource\\apps\\missdriller\\";
char setting_bitmapdir[128]=DRILL_PATH"/data/bmp/";
int setting_climbwait;
int setting_airdecreasewait;
int setting_defaultFPS;
int setting_fpsincreasespeed;
int setting_airinterval;
int setting_airdownspeed;
int setting_joyconfirm;
int setting_joycancel;
int setting_joyaxismax;

int setting_joyenabled;
int setting_joysticknumber;
//char setting_playerdir[128]="c:\\resource\\apps\\missdriller\\player\\";
char setting_playerdir[128]=DRILL_PATH"/data/susumi/";

/* joysticj control */
//SDL_Joystick *joystick=NULL;
#define JOYLIMIT	1000
CInput *gameinput=NULL;

/**
name entry
**/
char nameentry_moji[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz .";
int nameentry_x;
int nameentry_vx;
int nameentry_x_wait;
int name_x;
int space_repeat;
int bs_repeat;

int entry_number;
int entry_number_time;


void draw(void);
void initialize(void);
void finalize(void);
void load_graphic(void);
void set_stage(int,int,int);
void set_stage_startingpoint(void);
void game_ready(void);
void draw_me(void);

int mainloop();
void move(void);
int keyread(void);

void erase_check_optimisation(void);
int erase_block(void);
void clear_blockflag(void);
void destroy(int ,int );
void search(int ,int ,int );

void blockprocess(void);
void prefallcheck(void );
void setprefall(int ,int ,int ,int);
void search_fall(int ,int ,int ,int *,int *);
void search_number(int ,int ,int ,int *);

void draw_screen(void);
void stage_clear(void);
void drawback(void);
void other_move(void);
void draw_air(void);
int atarihantei(void);
void self_destroy(int x,int y);

void unsetprefall(int x,int y,int type);
void leftcheck(int x,int y,int type,int *checkleft,int *);
void setleft(int x,int y,int type,int left);

void unsetprefall(int x,int y,int type);

int hanabiset(int x,int y,int n);
void drawhanabi(void);
void clearhanabi(void);

void drawTilescroll(void);
void moveTilescroll(void);
int title(void);
int highscore(void);

int THighScoreLoad(THighScore *p,char *filename);
int THighScoreSave(THighScore *p,char *filename);
void THighScoreAdd(THighScore *p,char *name,int time,int depth,int score);
void THighScoreSortByTime(THighScore *p);
void THighScoreSortByScore(THighScore *p);
void THighScoreSortByDepth(THighScore *p);
void THighScoreSwap(THighScore *p,int i,int j);

void erase_check_recursive(int x,int y,int type,int *answer,int *number);
void seterase_recursive(int x,int y,int type);
void erase_check(void);

void gameover(void);
void draw_other(void);

void gameclear(void);
void load_setting(void);

void nameentry(void);
void draw_nameentry(void);
int move_nameentry(void);
int init_nameentry(int score,int depth,int playtime);

void joy_init(void);
void joy_final(void);

void unsetprefallfinished(int x,int y,int type);

int BlitForBlock(SDL_Surface *p,SDL_Surface *dest,int num,int x,int y);
void set_shape(void);

/*int g_nMemoryFD; // file descriptor of /dev/mem
volatile unsigned long *g_pulRegs;   // 32-bit version of the MMSP2 registers
volatile unsigned short *g_pusRegs;  // 16-bit version of the MMSP2 registers

void InitCPU() {
    //#ifdef GP2X
	// get access to the physical memory
	g_nMemoryFD = open("/dev/mem", 2); // 2 = O_RDWR

	// map the MMSP2 registers so we can play with them
	g_pulRegs = (unsigned long*)mmap(0, 0x10000, 3, 1, g_nMemoryFD, 0xC0000000);
	g_pusRegs = (unsigned short *) g_pulRegs;
	//#endif
}
*/
/*#define SYS_CLK_FREQ 7372800
static int cpufreq;
unsigned MDIV,PDIV,SCALE;

void set_FCLK(unsigned MHZ)
{
        unsigned v;
        unsigned mdiv,pdiv=3,scale=0;
        MHZ*=1000000;
        mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;
        //printf ("Old value = %04X\r",MEM_REG[0x924>>1]," ");
        //printf ("APLL = %04X\r",MEM_REG[0x91A>>1]," ");
        mdiv=((mdiv-8)<<8) & 0xff00;
        pdiv=((pdiv-2)<<2) & 0xfc;
        scale&=3;
        v=mdiv | pdiv | scale;
        g_pusRegs[0x910>>1]=v;

	while(g_pusRegs[0x0902>>1] & 1);    //Wait for the frequentie to be ajused
}

unsigned get_FCLK()
{
        return g_pusRegs[0x910>>1];
}

unsigned get_freq_920_CLK()
{
        unsigned i;
        unsigned reg,mdiv,pdiv,scale;
        reg=g_pusRegs[0x912>>1];
        mdiv = ((reg & 0xff00) >> 8) + 8;
        pdiv = ((reg & 0xfc) >> 2) + 2;
        scale = reg & 3;
        MDIV=mdiv;
        PDIV=pdiv;
        SCALE=scale;
        i = (g_pusRegs[0x91c>>1] & 7)+1;
        return ((SYS_CLK_FREQ * mdiv)/(pdiv << scale))/i;
}
unsigned short get_920_Div()
{
        return (g_pusRegs[0x91c>>1] & 0x7);
}
*/

//JYCET routine copie ecran back vers front
void drawSprite(SDL_Surface* srcSurface, SDL_Surface* dstSurface, int dstX, int dstY, int width, int height)
{
	static SDL_Rect dstRect;
	dstRect.x = dstX;
	dstRect.y = dstY;
	dstRect.w = width;
	dstRect.h = height;

	SDL_BlitSurface(srcSurface, NULL , dstSurface, &dstRect);
}

//JYCET routine d'update general video & son avec synchro
extern char fps_str[4];
void rafraichir_ecran(void)
{
	if (show_fps) {
		CffontBlitxy(font,fps_str,screen_back, 0, 232);
	}

	if (flag_show_volume) {
		static char string_vol[4];
		--flag_show_volume;

		sprintf(string_vol, "%03d%c", g_volume, 37);
		CffontBlitxy(font,string_vol, screen_back, 288, 0);
	}

	//flush_uppermem_cache(screen_back->pixels, screen_back->pixels + 320*240, 0);

	drawSprite(screen_back, screen_front, offset_x, offset_y, 320, 240);

	SDL_UpdateRect(screen_front, offset_x, offset_y, 320, 240);
#ifndef __SYMBIAN32__
	UpdateMikmod();
#endif
	frame_skip(0);
}

//JYCET routine control volume general
void volume_control(void)
{
	if (gameinput->button[BUTTON_VOLUP]) {
		if (g_volume < 100) {
			g_volume += 5;
			if (g_volume > 100) g_volume=100;
			else if (g_volume == 5) {
				//if (Player_Active()==0) Player_Start(module);
			}
			SetVolume( g_volume );
		}
		flag_show_volume = VOL_SHOW_DELAY;
	} else if (gameinput->button[BUTTON_VOLDOWN]) {
		if (g_volume > 0) {
			g_volume -= 5;
			if (g_volume < 0) {
				g_volume=0;
#ifdef __MIKMOD__
				if (Player_Active()) Player_Stop();
#endif
			}
			SetVolume( g_volume );
		}
		flag_show_volume = VOL_SHOW_DELAY;
	}
}

/*
*************** JyCet Add **************
*/
char mode_pause(unsigned char pause_state)
{
	SDL_Event event;
	int EPause = 0;

	//JYCET application ALPHA sur screen_back
	SDL_SetAlpha(screen_black, SDL_SRCALPHA, 64);
	SDL_BlitSurface(screen_black, NULL, screen_back, NULL);
	//JYCET dessin sprite PAUSE
	CBmpsBlit(cbmps_pause, screen_back, 0, 85, 99);
	//JYCET capture ecran
	//flush_uppermem_cache(screen_back->pixels, screen_back->pixels + 320*240, 0);
	SDL_BlitSurface(screen_back, NULL, screen_tmp, NULL);
	while(!EPause)
	{
		SDL_BlitSurface(  screen_tmp ,NULL, screen_back ,NULL);
		rafraichir_ecran();
		CTimeWait(&gametime);

		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_JOYBUTTONDOWN:
					if ( event.jbutton.button == GP2X_BUTTON_START) {
						//JYCET retour au jeu
						return 0;
					}else if ( event.jbutton.button == GP2X_BUTTON_Y) {
						//JYCET quitte le jeu
						return 1;
					}else if ( event.jbutton.button == GP2X_BUTTON_VOLUP) {
						if (g_volume < 100){
							g_volume += 5;
							if (g_volume > 100) g_volume=100;
							else if (g_volume == 5)
							{
								//if (Player_Active()==0) Player_Start(module);
							}
							SetVolume( g_volume );
						}
						flag_show_volume = VOL_SHOW_DELAY;
					}else if ( event.jbutton.button == GP2X_BUTTON_VOLDOWN) {
						if (g_volume > 0) {
							g_volume -= 5;
							if (g_volume < 0) {
								g_volume=0;
#ifdef __MIKMOD__
								if (Player_Active()) Player_Stop();
#endif
							}
							SetVolume( g_volume );
						}
						flag_show_volume = VOL_SHOW_DELAY;
					}
//				break;
				default:
				break;
			}
		}
		CInputUpdate(gameinput,0);

		if(gameinput->button[BUTTON_0] == 1) {
			return 0;
		}

		if ( gameinput->button[BUTTON_START] == 1 || gameinput->button[BUTTON_1] == 1) {
			return 1;
		}

		if ( gameinput->button[BUTTON_UP] || gameinput->button[BUTTON_RIGHT]) {
			if (g_volume < 100){
				g_volume += 5;
				if (g_volume > 100) g_volume=100;
				else if (g_volume == 5)
				{
					//if (Player_Active()==0) Player_Start(module);
				}
				SetVolume( g_volume );
			}
			flag_show_volume = VOL_SHOW_DELAY;
		}else if ( gameinput->button[BUTTON_DOWN] || gameinput->button[BUTTON_LEFT]) {
			if (g_volume > 0){
				g_volume -= 5;
				if (g_volume < 0){
					g_volume=0;
#ifdef __MIKMOD__
					if (Player_Active()) Player_Stop();
#endif
				}
				SetVolume( g_volume );
			}
			flag_show_volume = VOL_SHOW_DELAY;
		}

	}

	return 0;
}

int main(int argc,char **argv)
{
	int done=0;
	load_setting();

	initialize();
	load_graphic();
	LoadModuleName();
#ifndef __SYMBIAN32__
	InitMikmod();
#endif

	THighScoreLoad(&highscoredata,scorefile);
	THighScoreLoad(&fasttimedata,fasttimefile);
	SetVolume( g_volume );

	do {
		if (!title()) break;
		game_ready();
		if (!mainloop()) done = 1;
	} while (!done);

	THighScoreSave(&highscoredata,scorefile);
	THighScoreSave(&fasttimedata,fasttimefile);
	finalize();
#ifndef __SYMBIAN32__
	FreeMikmod();
#endif

	return 0;
}

void load_setting(void)
{
	FILE *fp;
	char buf[4096];
	char *ignore="#\r\n";
	//char inifile[256]="c:\\resource\\apps\\missdriller\\drill.ini";
	char inifile[256]=DRILL_PATH"/data/drill.ini";
	char *p,*q;

	fp=fopen(inifile, "rb");
	/*if (NULL==fp) {
		//inifile[0] = KStorageCardD;
		fp=fopen(inifile, "rb");
		if (NULL == fp) {
			//inifile[0] = KStorageCardE;
			fp=fopen(inifile, "rb");*/
			if (NULL == fp) {
				fprintf(stderr, "ERROR:cant open %s\n", inifile);
				return;
			}
		/*}
	}*/
	//KDataDrive = inifile[0];

	setting_fullscreen=0;
//	strcpy(setting_bitmapdir,"c:\\resource\\apps\\missdriller\\bmp\\");
	strcpy(setting_bitmapdir, DRILL_PATH"/data/bmp/");
	setting_defaultFPS=60;
	setting_airdecreasewait=50;
	setting_climbwait=9;
	setting_fpsincreasespeed=2;
	setting_airinterval=10;
	setting_airdownspeed=3;
	setting_joyconfirm=0;
	setting_joycancel=1;
	setting_joyaxismax=10000;
	setting_joyenabled=1;
	setting_joysticknumber=0;

	//JYCET default settings
	offset_x = 0;
	offset_y = 0;
	g_volume = 75;
	show_fps = 0;
	do {
		if (NULL==(fgets(buf,4096,fp))) break;
		if (NULL!=strchr(ignore,*buf)) continue;

		p=strtok(buf,"\t\r\t\n #");
		q=strtok(NULL,"\t\r\t\n #");

		if(!strcmp(p,"FULLSCREEN")){
			if(!strcmp(q,"YES"))setting_fullscreen=1;else setting_fullscreen=0;
		}
		else if(!strcmp(p,"DEFAULT_FPS")){
			if(q)setting_defaultFPS=atoi(q);
		}
		else if(!strcmp(p,"FPS_INCREASE_SPEED")){
			if(q)setting_fpsincreasespeed=atoi(q);
		}
		else if(!strcmp(p,"AIR_DECREASE_WAIT")){
			if(q)setting_airdecreasewait=atoi(q);
		}
		else if(!strcmp(p,"AIR_DOWN_SPEED")){
			if(q)setting_airdownspeed=atoi(q);
		}
		else if(!strcmp(p,"CLIMB_WAIT")){
			if(q)setting_climbwait=atoi(q);
		}
		else if(!strcmp(p,"AIR_INTERVAL")){
			if(q)setting_airinterval=atoi(q);
		}
		else if(!strcmp(p,"JOY_CONFIRM")){
			if(q)setting_joyconfirm=atoi(q);
		}
		else if(!strcmp(p,"JOY_CANCEL")){
			if(q)setting_joycancel=atoi(q);
		}
		else if(!strcmp(p,"JOY_CANCEL")){
			if(q)setting_joyaxismax=atoi(q);
		}
		else if(!strcmp(p,"JOY_ENABLED")){
			if(!strcmp(q,"YES"))setting_joyenabled=1;else setting_joyenabled=0;
		}

		else if(!strcmp(p,"JOYSTICK_NUMBER")){
			if(q)setting_joysticknumber=atoi(q);
		}
		else if(!strcmp(p,"BITMAPSDIR")){
			if(q==NULL){
				strcpy(setting_bitmapdir,"./bmp/");
			}else{
				strcpy(setting_bitmapdir,q);
			}
		}
		else if(!strcmp(p,"PLAYERDIR")){
			if(q==NULL){
//				strcpy(setting_playerdir,"./player/");
				strcpy(setting_playerdir,"./susumi/");
			}else{
				strcpy(setting_playerdir,q);
			}
		}
		//JYCET ligne d'options rajoutées dans DRILL.INI
		else if(!strcmp(p,"GENERAL_VOLUME")){
			if(q)g_volume=atoi(q);
		}
		else if(!strcmp(p,"SHOW_FPS")){
			if(!strcmp(q,"YES")) show_fps=1;else show_fps=0;
		}

	} while(1);
	fclose(fp);
	setting_bitmapdir[0] = inifile[0];

}

void gameover(void)
{
	my_time=1000000;//for no clear time
	//printf("%d %d %d\n",my_score,my_depth*100+my_y/24-STAGE_START_Y+1,my_time);
	if (init_nameentry(my_score,my_depth*100+my_y/24-STAGE_START_Y+1,my_time) < 0) ;
	else nameentry();
}

void gameclear(void)
{
	//printf("%d %d %d\n",my_score,my_depth*100+my_y/24-STAGE_START_Y+1,my_time);
	if (init_nameentry(my_score,my_depth*100+my_y/24-STAGE_START_Y+1,my_time) < 0) ;
	else nameentry();
}

/*死の判定、非0で死*/
int atarihantei(void)
{
	int mapx, mapy;
	TBlockState *p;
	int deltax,deltay;
	deltax=my_x%24;
	deltay=my_y%24;
	mapx=my_x/24;
	mapy=my_y/24;
	if(my_x%24>=12)mapx++;
	if(my_y%24>=12)mapy++;
	p=&gamestage[mapx][mapy];
	if(p->type==AIR_BLOCK && p->left <12){
		p->type=NO_BLOCK;
		p->state=BLOCKSTATE_NONE;
		p->player_dug=1;
		airplus+=22;
		got_air++;
		scorerest+=got_air*10;
		scoreplus+=got_air/2+1;
		CWavsPlay(wavs,11);
	}else
	if(p->type!=NO_BLOCK && p->state!=BLOCKSTATE_EXTINGUISHING && p->state!=BLOCKSTATE_EXTINGUISHED){
		if(p->left<12){
			my_dead=1;my_deadcount=0;vx=0;
			CWavsPlay(wavs, 14);
			return 1;
		}
	}else
	if(mapx>0 && my_x%24==12 && gamestage[mapx-1][mapy].type!=NO_BLOCK &&
		gamestage[mapx-1][mapy].type!=AIR_BLOCK &&
		gamestage[mapx-1][mapy].state!=BLOCKSTATE_EXTINGUISHING &&
		gamestage[mapx-1][mapy].state!=BLOCKSTATE_EXTINGUISHED){
		if(p->left<12){
			CWavsPlay(wavs,14);
			my_dead=1;my_deadcount=0;vx=0;
			return 1;
		}
	}
	if(my_air==0){	CWavsPlay(wavs,14);my_dead=1;my_deadcount=0;vx=0;return 2;}
	//??
	if(deltay==0 && deltax<=10 && mapx<STAGE_WIDTH-1 &&
		gamestage[mapx+1][mapy].type!=NO_BLOCK &&
		gamestage[mapx+1][mapy].type!=AIR_BLOCK &&
		gamestage[mapx+1][mapy].state!=BLOCKSTATE_EXTINGUISHING &&
		gamestage[mapx+1][mapy].state!=BLOCKSTATE_EXTINGUISHED

		){

		my_x=mapx*24;
		if(deltax>6){
			CWavsPlay(wavs,16);
			penaltyframe=PENALTY_FRAMES;
			if(direction==DIR_LEFT)
				penaltybgnum=43;//left escape
				else
				penaltybgnum=34;
		}

	}
	if(deltay==0 && deltax>=14 && mapx>0 && gamestage[mapx-1][mapy].type!=NO_BLOCK &&
		gamestage[mapx-1][mapy].type!=AIR_BLOCK &&
		gamestage[mapx-1][mapy].state!=BLOCKSTATE_EXTINGUISHING &&
		gamestage[mapx-1][mapy].state!=BLOCKSTATE_EXTINGUISHED) {
			my_x=(mapx)*24;
			if(deltax<18){
				CWavsPlay(wavs,16);
				penaltyframe=PENALTY_FRAMES;
				if(direction==DIR_LEFT)
					penaltybgnum=44;//left escape
				else
					penaltybgnum=33;
			}
	}

	return 0;
}

void other_move(void)
{
	if(my_y>=(STAGE_START_Y-1)*24)airdowncount++;
	if(airdowncount>airdownspeed){
		airdowncount=0;
		my_air--;
	}
	if(airplus>0){
		airplus--;
		my_air++;
	}
	if(my_air<=0)my_air=0;
	if(my_air>=100)my_air=100;

	if(scoreplus>0){
		if(scorerest>scoreplus){

			my_score+=scoreplus;
			scorerest-=scoreplus;
		}else{
			my_score+=scorerest;
			scorerest=0;
			scoreplus=0;
		}
	}
}

int my_pause = 0;

//mod by Ken, possibly complete
void move(void)
{
	int mapx,mapy;
	int digx,digy;
	TBlockState *p,*q;

	//erase_block();
	erase_check_optimisation();
	erase_check();

	if (dig_graphic) {
		dig_graphic++;
		if(dig_graphic>=9)dig_graphic=0;
	}

	if (climbing) {
		my_y+=vy;
		if(my_y<0)my_y=0;
		if(my_y%24==0){
//			my_x+=vx*7;//for walkspeed=4
//			my_x+=vx*5;//for walkspeed=6
			if(vx>0)my_x+=14;else my_x-=14;
			climbing=0;
			repeat_x=0;
		}else vy=-WALKSPEED_Y;
		return;
	}else{
		vx=0;
		vy=0;
		dig=0;
		if(!my_dead && !my_clear && penaltyframe==0){

			CInputUpdate(gameinput,0);
		}else{
			CInputUpdate(gameinput,1);

		}
//			keys = SDL_GetKeyState(NULL);
/*
			if(joystick && SDL_JoystickGetButton(joystick, setting_joyconfirm) == SDL_PRESSED)joy_space=1;
			if(joystick && (int)SDL_JoystickGetAxis(joystick, 1) < -setting_joyaxismax)joy_up=1;
			if(joystick && (int)SDL_JoystickGetAxis(joystick, 1) > setting_joyaxismax)joy_down=1;
			if(joystick && (int)SDL_JoystickGetAxis(joystick, 0) > setting_joyaxismax)joy_right=1;
			if(joystick && (int)SDL_JoystickGetAxis(joystick, 0) < -setting_joyaxismax)joy_left=1;
		}
		*/

		if ( gameinput->button[BUTTON_UP]){direction=DIR_UP;}
		if ( gameinput->button[BUTTON_DOWN]) {direction=DIR_DOWN;}
		if ( gameinput->button[BUTTON_RIGHT]) {vx= WALKSPEED;direction=DIR_RIGHT;}
		if ( gameinput->button[BUTTON_LEFT]) {vx=-WALKSPEED;direction=DIR_LEFT;}

		if ( (gameinput->button[BUTTON_0] || gameinput->button[BUTTON_2]) && direction!=DIR_NONE && my_y%24==0){
			if(dig_repeat)dig=0;else{
				dig=1;
				dig_graphic=1;
				dig_repeat=1;
			}
		}else{
			dig_repeat=0;
		}
	}

	volume_control();

	if(vx==0)repeat_x=0;

	if(penaltyframe>0){
		penaltyframe--;
	}


	mapx=my_x/24;
	mapy=my_y/24;
	if(my_x%24>=12)mapx++;
	if(my_y%24>=12)mapy++;
	p=&gamestage[mapx][mapy];
	q=&gamestage[mapx][mapy+1];

	if(my_y%24==0){

		if(q->type==NO_BLOCK||q->type==AIR_BLOCK||q->state==BLOCKSTATE_EXTINGUISHING){
			vx=0;
			vy=WALKSPEED_Y;
			dig=0;
			my_x=mapx*24;
			penaltyframe=0;
		}
	}else{
		vx=0;
		vy=WALKSPEED_Y;
		dig=0;
		penaltyframe=0;
	}


	if(my_x%24 >=6 &&my_x%24 < 18){
		if(direction==DIR_RIGHT||direction==DIR_LEFT)dig=0;
	}

	if(vx!=0 && my_x%24==0){

		mapx=my_x/24;
		mapy=my_y/24;

		if(vx>0)mapx++;
		if(vx<0)mapx--;

		if(mapx<0)vx=0; else
		if(mapx>=STAGE_WIDTH)vx=0; else
		if(gamestage[mapx][mapy].type==AIR_BLOCK){
			;
		}else if(
			gamestage[mapx][mapy].state==BLOCKSTATE_EXTINGUISHING||
			gamestage[mapx][mapy].state==BLOCKSTATE_EXTINGUISHED){
			;
		}else if(gamestage[mapx][mapy].type!=NO_BLOCK){

			if(gamestage[my_x/24][mapy-1].type==NO_BLOCK && gamestage[my_x/24][mapy-1].state!=BLOCKSTATE_EXTINGUISHING
			 && (gamestage[mapx][mapy-1].type==NO_BLOCK||gamestage[mapx][mapy-1].type==AIR_BLOCK)){
				repeat_x+=vx;
				if(repeat_x>WALKSPEED*setting_climbwait||repeat_x<-WALKSPEED*setting_climbwait){
					climbing=1;

					vy=-WALKSPEED_Y;
				}else vx=0;
			}else vx=0;

		}

	}


	if(dig){

		digx=my_x/24;
		if(my_x%24>=12)digx++;
		digy=my_y/24;
		if(direction==DIR_DOWN){
			digy++;
		}else
		if(direction==DIR_UP){
			digy--;
		}else
		if(direction==DIR_RIGHT){
			digx++;
		}else
		if(direction==DIR_LEFT){
			digx--;
		}
		destroyed=0;
		if(digx<0 || digx>=STAGE_WIDTH || digy<0 || digy>=GAME_STAGE_HEIGHT);else{
			if(gamestage[digx][digy].type!=NO_BLOCK &&
			gamestage[digx][digy].state!=BLOCKSTATE_FALLING &&
			gamestage[digx][digy].state!=BLOCKSTATE_EXTINGUISHING &&
			gamestage[digx][digy].state!=BLOCKSTATE_EXTINGUISHED

			){
				if(gamestage[digx][digy].type==CLEAR_BLOCK){
					stage_clear();
					CWavsPlay(wavs,17);
				}else{
					self_destroy(digx,digy);
					destroyed=1;
					repeat_x=0;
				}
			}
		}

	}

	if(my_y<0)vy=1;

	if(vx!=0)movingframe++;else movingframe=0;
	if(!climbing)my_x+=vx;
	if(vy>0)fallingframe++;else fallingframe=0;
	my_y+=vy;

	if(my_x<0)my_x=0;
	if(my_y<0)my_y=0;

	prefallcheck();
	blockprocess();
}

int keyread(void)
{
	SDL_Event event;

	/* Check for events */
	while (SDL_PollEvent (&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:

			break;
		case SDL_KEYUP:
			break;
		case SDL_JOYBUTTONDOWN:
			/* if press Start button, terminate program */
			if ( event.jbutton.button == 9 && event.jbutton.button == 10 && event.jbutton.button == 11){
				finalize();
			}
			if ( event.jbutton.button == BUTTON_START) {
				//JYCET chgt pause
				if (my_dead == 0){
					my_pause ++;
					my_pause &= 1;
				}
			}
			break;
		case SDL_JOYBUTTONUP:
			break;
		case SDL_QUIT:
			break;
		default:
			break;
		}
	}

	return 0;
}

void drawback(void)
{
	/*int gy,y;
	for(gy=-(my_y>>1)%24,y=0;y<11;++y,gy+=24){
		CBmpsBlit(cbmps_character,screen_back,102,0,gy);
	}*/
	drawSprite(bg_tmp, screen_back, 0 , -(my_y>>1)%24 ,216, 264);
}

void key_start()
{
    Uint8 *keys;
    keys = SDL_GetKeyState(NULL);

    if(gameinput->button[BUTTON_START] == 0 && keys[BUTTON_START] == SDL_PRESSED) gameinput->button[BUTTON_START] = 1;
    else gameinput->button[BUTTON_START] = 0;
}

int mainloop()
{
	int y;

	char buf[64];
	signed short f = 255;

	//CTimeChangeFPS(&gametime,my_fps);

	set_shape();

	drawback();
	draw();
	draw_me();
	draw_other();
	drawhanabi();
	draw_screen();
	draw_air();
	y=my_depth*100+my_y/24-STAGE_START_Y+1;

	sprintf(buf,"%4d m.",y<0?0:y);
	CffontBlitxy(font,buf,screen_back,250,50);

	sprintf(buf,"%6d0",my_score);
	CffontBlitxy(font,buf,screen_back,240,110);

	sprintf(buf,"%3d%%",my_air);
	CffontBlitxy(font,buf,screen_back,250,164);
	//JYCET capture ecran
	SDL_BlitSurface( screen_back ,NULL, screen_tmp ,NULL);
	//JYCET fade
	while(f>0) {
		SDL_BlitSurface(screen_tmp,NULL,screen_back,NULL);
		SDL_SetAlpha(screen_black,SDL_SRCALPHA,f);
		SDL_BlitSurface(screen_black,NULL,screen_back,NULL);
		f -= FADE_SPEED;
		if (f < FADE_SPEED) f=0;
		rafraichir_ecran();
	}

	reset_frame_skip();
	StartModule();

	do{
		//keyread();
		//JYCET pause changement
		if(my_pause) {
			if ( mode_pause(my_pause) ){
				my_dead = 1;
				my_deadcount = 250;
			}
			my_pause = 0;
		}else{
			if(my_y==(STAGE_START_Y-1)*24) CTimeReset(&gametime,setting_defaultFPS);

			//calcul block
			set_shape();

			if(keyread())return 0;

			if(lap_showing){
				lapcount++;
				if(lapcount>300)lap_showing=0;
			}

			if(airminus){
				airminuscount++;
				if(airminuscount>80)airminus=0;
			}

			if(my_dead){
				//JyCet evite d'attendre pour GAMEOVER
				CInputUpdate(gameinput,0);
				if ( gameinput->button[BUTTON_START] == SDL_PRESSED ) my_deadcount=250;
				else my_deadcount++;

				if(my_deadcount>250){
					gameover();
					FreeModule();
					//SetVolume( g_volume );
					/*drawback();
					draw();
					draw_me();
					draw_other();
					drawhanabi();
					draw_screen();
					draw_air();

					y=my_depth*100+my_y/24-STAGE_START_Y+1;

					sprintf(buf,"%4d m.",y<0?0:y);
					CffontBlitxy(font,buf,screen_back,250,50);

					sprintf(buf,"%6d0",my_score);
					CffontBlitxy(font,buf,screen_back,240,110);

					sprintf(buf,"%3d%%",my_air);
					CffontBlitxy(font,buf,screen_back,250,164);*/
					//JYCET capture ecran
					SDL_BlitSurface( screen_back ,NULL, screen_tmp ,NULL);
					f=0;
					while(f<256) {
						SDL_BlitSurface(screen_tmp,NULL,screen_back,NULL);
						SDL_SetAlpha(screen_black,SDL_SRCALPHA,f);
						SDL_BlitSurface(screen_black,NULL,screen_back,NULL);
						f += FADE_SPEED;
						rafraichir_ecran();
					}
					break;
				}
			}

			if(my_clear){
				my_clearcount++;
				if(my_clearcount>300){
					gameclear();
					break;
				}
			}

			//deplacement susumi
			move();
			//calcul air et score
			other_move();


			if ( gameinput->button[BUTTON_START] == 1 || gameinput->button[BUTTON_1] == 1) {
						//JYCET chgt pause
						if (my_dead == 0){
							my_pause ++;
							my_pause &= 1;
						}
					}

			//test collision susumi
			if(!my_dead && !my_clear)atarihantei();

			drawback();
			draw();
			draw_me();
			draw_other();
			drawhanabi();
			draw_screen();
			draw_air();

			y=my_depth*100+my_y/24-STAGE_START_Y+1;

			sprintf(buf,"%4d m.",y<0?0:y);
			CffontBlitxy(font,buf,screen_back,250,50);

			sprintf(buf,"%6d0",my_score);
			CffontBlitxy(font,buf,screen_back,240,110);

			sprintf(buf,"%3d%%",my_air);
			CffontBlitxy(font,buf,screen_back,250,164);

			rafraichir_ecran();
			CTimeWait(&gametime);
		}
	}while(1);

	return (1);
}

void draw_other(void){
	int gy;
	static int frame=0;
	frame++;
	if(my_dead==1){
		if(my_deadcount>20)	CBmpsBlit(cbmps_character,screen_back,70,31,70);//coords?
	}
	if(my_clear==1){
		if(my_clearcount>20)	CBmpsBlit(cbmps_character,screen_back,72,31,70);//coords?
	}
	if(lap_showing){
		if(lapcount%40<25)CffontBlitxy(font,lapstring,screen_back,0,0);
	}
	if(my_y>24*90){
		//draw
		gy=102*24-my_y;
		CBmpsBlit(cbmps_character,screen_back,120+my_depth,33,gy+20+24); //should '20' be '10'?
	}
	if(my_air<25){
		if (my_air==20) 	CWavsPlay(wavs, 15);
		if ((frame%12/6)==0)
			CBmpsBlit(cbmps_character,screen_back,111,33,25); //changed
	}
	if(airminus){
		CBmpsBlit(cbmps_character,screen_back,110,58,160); //changed
	}
}

void draw_air(void)
{
	SDL_Rect dest={243,160,50,16};
	SDL_Rect src={0,0,50,16};

	if(my_air<=0)return;

	src.w=my_air/2;
	dest.w=my_air/2;

	SDL_BlitSurface(cbmps_character->bmp[103],&src,screen_back,&dest);
}

//mod by Ken
void draw_me(void)
{
//	CBmpsBlit(cbmps_character,screen_back,0,my_x*48,3*48);
	int i = 0;
	int j;

	switch(direction){
		case DIR_RIGHT:i=30;break;
		case DIR_LEFT:i=40;break;
		case DIR_DOWN:i=10;break;
		case DIR_UP:i=20;break;
	}
	if(dig_graphic==0 && movingframe>0){
		j=movingframe/2;
		j%=4;
		i+=j;
		i+=5;
	}

	i+=dig_graphic/3;//frameeach
	if(penaltyframe>0)i=penaltybgnum;

	if(fallingframe>20){

		i=110+(fallingframe%12)/6;
	}


	if(my_dead==1){
		i=100;
		if(my_deadcount>30)i=101;
	}
	CBmpsBlit(playerbmps,screen_back,i,my_x-6,4*24-6);
}

void draw_screen(void)
{
	CBmpsBlit(cbmps_character,screen_back,100,216,0);
}

void stage_clear(void){

	int colors[]={4,3,4,4,2,4,3,3,4,4,1};
//	int colors[]={1,1,1,1,1,4,3,2,3,4,1};
//	int percent[]={50,52,54,56,58,60,63,64,66,68,10};
	int percent[]={50,55,60,65,30,65,70,75,80,80,10};
	int blockstyle[]={1,3,3,4,1,4,4,3,4,4,1};

	int i,x,y;

	for(i=0,y=my_y/24-4;i<9;i++,y++)//should'-4' be '-2'?
	for(x=0;x<9;x++){
		if(gamestage[x][y].type!=NO_BLOCK)hanabiset(x,i-3,15);
	}

	my_depth++;
	if(my_depth>=10){my_clear=1;my_clearcount=0;}
	set_stage(colors[my_depth],percent[my_depth],blockstyle[my_depth]);
	climbing=0;

	my_y=-2*24;
	airdownspeed-=setting_airdownspeed;
	airdowncount=0;
	scorerest+=my_depth*100;
	scoreplus+=my_depth*5;
	my_fps+=setting_fpsincreasespeed;

	my_time+=gametime.clock/10;

	fallingframe=20;

	sprintf(lapstring,"%2d00m Passing %02d'%02d'%02d",
		my_depth,
		gametime.clock/6000/10,
		(gametime.clock/100/10)%60,
		(gametime.clock/10)%100
		);
	lap_showing=1;
	lapcount=0;
	CTimeReset(&gametime,setting_defaultFPS);
	//CTimeChangeFPS(&gametime,my_fps);
}

//mod by Ken
void game_ready(void)
{
	my_x=4*24;
//	my_y=(STAGE_START_Y-1)*48;
//	my_y=0;
	my_y=-2*24;

	set_stage(4,50,1);
//	set_stage(3,95);

	set_stage_startingpoint();
	direction=DIR_DOWN;

	climbing=0;
	airdownspeed=setting_airdecreasewait;
	airdowncount=0;
	got_air=0;
	my_score=0;
	scoreplus=0;
	scorerest=0;
	my_air=100;
	my_depth=0;
//	my_depth=9;
	my_dead=0;
	my_deadcount=0;
	clearhanabi();
	my_clear=0;
	my_clearcount=0;
	my_fps=setting_defaultFPS;
	my_time=0;

	lap_showing=0;

	dig_graphic=0;

	penaltybgnum=0;
	penaltyframe=0;
	fallingframe=60;
	//CTimeChangeFPS(&gametime,my_fps);
	CTimeReset(&gametime,my_fps);

	LoadModule( mod_GAME );
	reset_frame_skip();

	//JYCET correction activation du mode pause durant l'ecran titre ?
	my_pause = 0;
}

void initialize(void)
{
	int i;
	int w =0;
	int h = 0;
		w=320;
		h=240;
		offset_x = 0;
		offset_y = 0;
	srand(time(NULL));

	/*allocating mem for GAME_STAGE_WIDTH by GAME_STAGE_HEIGHT TBlockState matrix*/
	gamestage = (TBlockState**)malloc(GAME_STAGE_WIDTH * sizeof(TBlockState*));
	for (i = 0; i < GAME_STAGE_WIDTH; ++i) {
		gamestage[i] = (TBlockState*)malloc(GAME_STAGE_HEIGHT * sizeof(TBlockState));
	}

	if (setting_fullscreen) {
		//screen_back=CScreenInitDefaultHW();
		screen_front=CScreenInitDefaultHW(w, h); //////////////////////// JYCET
		//screen_front=CScreenInitDefault(w, h);
	} else {
		//screen_back=CScreenInitDefault();
		screen_front=CScreenInitDefault(w, h); //////////////////////// JYCET
	}

	screen_back = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000); //////////////////////// JYCET
	screen_tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000); //////////////////////// JYCET
	screen_black = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 16, 0xf800, 0x07e0, 0x001f, 0x0000); //////////////////////// JYCET
	SDL_FillRect(screen_black,0,SDL_MapRGB(screen_black->format,0,0,0));
	//pour preparation bg complet mainback
	bg_tmp = SDL_CreateRGBSurface(SDL_HWSURFACE, 216, 11*24, 16, 0xf800, 0x07e0, 0x001f, 0x0000); //////////////////////// JYCET

	cbmps_blocks=CBmpsInit(BLOCK_NUM);
	cbmps_pause=CBmpsInit(1);
	cbmps_character=CBmpsInit(CHARACTER_NUM);
	cbmps_other=CBmpsInit(OTHER_GRAPHICS_NUM);
	playerbmps=CBmpsInit(PLAYER_NUM);
	//defaultFontFile[0] = KDataDrive;

	font=CffontInitDefault8(defaultFontFile);
	CAudioInitDefault();
	wavs=CWavsInit(WAVMAX);
	joy_init();
}

void finalize(void)
{
	int i;
	CBmpsFree(cbmps_pause);
	CBmpsFree(cbmps_blocks);
	CBmpsFree(cbmps_character);
	CBmpsFree(cbmps_other);
	CBmpsFree(playerbmps);
	playerbmps=NULL;
	CffontFree(font);

	if(screen_front) SDL_FreeSurface(screen_front);
	if(screen_tmp) SDL_FreeSurface(screen_tmp);
	if(screen_back) SDL_FreeSurface(screen_back);
	if(screen_black) SDL_FreeSurface(screen_black);
	if(bg_tmp) SDL_FreeSurface(bg_tmp);

	//FreeModule();
	FreeModuleName();
	CWavsFree(wavs);
	CAudioClose();

	joy_final();
	/*free mem for GAME_STAGE_WIDTH by GAME_STAGE_HEIGHT TBlockState matrix*/
	for (i = 0; i < GAME_STAGE_WIDTH; ++i) {
		free(gamestage[i]);
	}
	free(gamestage);
	SDL_Quit();
}

void load_graphic(void){  //and sound
	int y;

	//char filename[128] = "c:\\resource\\apps\\missdriller\\bmp\\pause.bmp";
	char filename[128] = DRILL_PATH"/data/bmp/pause.bmp";
	//filename[0] = KDataDrive;
	CBmpsLoad(cbmps_pause,filename);
	/*blockfile[0] = KDataDrive;
	characterfile[0] = KDataDrive;
	otherfile[0] = KDataDrive;
	wavdir[0] = KDataDrive;
	wavfile[0] = KDataDrive;
	moddir[0] = KDataDrive;*/

	CBmpsLoadFromFileWithDir(cbmps_blocks,blockfile,setting_bitmapdir);
	CBmpsLoadFromFileWithDir(cbmps_character,characterfile,setting_bitmapdir);
	CBmpsLoadFromFileWithDir(cbmps_other,otherfile,setting_bitmapdir);
	//setting_playerdir[0] = KDataDrive;
	sprintf(filename, "%s%s", setting_playerdir, playerbmpsfile);

	CBmpsLoadFromFileWithDir(playerbmps,filename,setting_playerdir);

	if(setting_fullscreen){
        CBmpsConvert(cbmps_pause);
		CBmpsConvert(cbmps_blocks);
		CBmpsConvert(cbmps_character);
		CBmpsConvert(cbmps_other);
		//JYCET conversion surface susumi
		CBmpsConvert(playerbmps);
	}
	tscroll.bmp=cbmps_character->bmp[52];
	tscroll.x=0;
	tscroll.y=0;
	tscroll.clock=0;
	//wavdir[0] = KDataDrive;
	CWavsLoadFromFileWithDir(wavs,wavfile,wavdir);
	//prepare surface bg
	for (y=0;y<11;++y)
		CBmpsBlit(cbmps_character,bg_tmp,102,0,y*24);

}

void set_stage_startingpoint(void){

	int i;
	int num_xy;
	TBlockState *p;

	const struct{
		int x;
		int y;
		int n;
	}xy[]={
		{0,0,HARD_BLOCK},{1,0,HARD_BLOCK},{2,0,HARD_BLOCK},{3,0,HARD_BLOCK},{4,0,0},{5,0,HARD_BLOCK},{6,0,HARD_BLOCK},{7,0,HARD_BLOCK},{8,0,HARD_BLOCK},
		{0,1,HARD_BLOCK},{1,1,HARD_BLOCK},{2,1,HARD_BLOCK},{3,1,HARD_BLOCK},{4,1,1},{5,1,HARD_BLOCK},{6,1,HARD_BLOCK},{7,1,HARD_BLOCK},{8,1,HARD_BLOCK},
		{0,2,HARD_BLOCK},{1,2,HARD_BLOCK},{2,2,2},{3,2,2},{4,2,2},{5,2,2},{6,2,2},{7,2,HARD_BLOCK},{8,2,HARD_BLOCK},
		{0,3,HARD_BLOCK},{1,3,HARD_BLOCK},{2,3,3},{3,3,3},{4,3,3},{5,3,3},{6,3,3},{7,3,HARD_BLOCK},{8,3,HARD_BLOCK},
		{0,4,0},{1,4,0},{2,4,0},{3,4,0},{4,4,0},{5,4,0},{6,4,0},{7,4,0},{8,4,0},
	};
	num_xy=sizeof(xy)/sizeof(xy[0]);

	for(i=0;i<num_xy;++i){
		p=&(gamestage[xy[i].x][xy[i].y+STAGE_START_Y]);
		p->type=xy[i].n;
		p->state=BLOCKSTATE_NONE;
		p->destroycount=0;
	}

}

void set_stage(int number,int percentage,int blockstyle){

	int x,y;
	TBlockState *p;
	int range;

	/*ÊíÌubNð~«lßé*/
	for(y=0;y<GAME_STAGE_HEIGHT;++y){
		for(x=0;x<GAME_STAGE_WIDTH;++x){
			p=&(gamestage[x][y]);

			p->type=((double)rand()/RAND_MAX)*number;
			if(p->type>=4)p->type=0;
			p->destroycount=0;
			p->state=BLOCKSTATE_NONE;
			p->left=0;
			p->lefttime=0;
			p->shape=0;
		}
	}
	/*d¢ubNðÎçT­*/
	for(y=10;y<GAME_STAGE_HEIGHT-6;++y){

		if(((double)rand()/RAND_MAX)*100 > percentage)continue;
		range=((double)rand()/RAND_MAX)*4;

		x=((double)rand()/RAND_MAX)*GAME_STAGE_WIDTH;

		while(range>0){
			range--;
			if(x+range>=GAME_STAGE_WIDTH)continue;
			p=&(gamestage[x+range][y]);
			p->type=HARD_BLOCK;
			p->state=BLOCKSTATE_NONE;
			p->left=0;
			p->destroycount=0;
			p->player_dug=0;
		}
	}

	/*airÌÝuyÑüèÉd¢ubNðzu*/
	for(y=11;y<GAME_STAGE_HEIGHT-10;y+=(setting_airinterval+my_depth)){
		x=((double)rand()/RAND_MAX)*(GAME_STAGE_WIDTH-2)+1;
		p=&(gamestage[x][y]);
		p->type=AIR_BLOCK;
		p->state=BLOCKSTATE_NONE;
		p->destroycount=0;
		//YŠ`FbNÍµÈ­ÄàÇ¢æ€ÉAirðzu

		if(blockstyle==4)gamestage[x-1][y].type=HARD_BLOCK;
		if(blockstyle==4)gamestage[x+1][y].type=HARD_BLOCK;
		if(blockstyle==4||blockstyle==1||blockstyle==3)gamestage[x][y-1].type=HARD_BLOCK;
		if(blockstyle==4||blockstyle==2||blockstyle==3)gamestage[x][y+1].type=HARD_BLOCK;



	}

	/*NAubNðzu*/
	for(y=95+STAGE_START_Y;y<100+STAGE_START_Y;++y){
		for(x=0;x<GAME_STAGE_WIDTH;++x){
			p=&(gamestage[x][y]);

			p->type=CLEAR_BLOCK;
			p->state=BLOCKSTATE_NONE;
			p->left=0;
		}
	}
	/*X^[g]ðÝu*/

	for(y=0;y<STAGE_START_Y;++y){
		for(x=0;x<GAME_STAGE_WIDTH;++x){
			p=&(gamestage[x][y]);

			p->type=NO_BLOCK;
			p->state=BLOCKSTATE_NONE;
			p->left=0;
		}
	}

}


//modified by Ken
void draw(void){


	int x,y,gy;

	int delta_y;

	static int pre_calcul_x[STAGE_WIDTH]={0,24,48,72,96,120,144,168,192};

	static int vibration[]={
		-1,-2,-3,-4,-3,-2,-1,0,
		1,2,3,4,3,2,1,0,
		-1,-2,-3,-4,-3,-2,-1,0,
		1,2,3,4,3,2,1,0,
		-1,-2,-3,-4,-3,-2,-1,0,
		1,2,3,4,3,2,1,0,
		-1,-2,-3,-4,-3,-2,-1,0,
		1,2,3,4,3,2,1,0,
		-1,-2,-3,-4,-3,-2,-1,0,
		1,2,3,4,3,2,1,0,
	};





	TBlockState *p;

	int alpha=0;
	delta_y=my_y%24;

	for(gy=-delta_y,y=my_y/24-4;y<=my_y/24+6;++y,gy+=24){
		if(y<0||y>=GAME_STAGE_HEIGHT)continue;
		for(x=0;x<STAGE_WIDTH;x++){
		alpha=0;
			p=&(gamestage[x][y]);
			if(p->type==NO_BLOCK)continue;

			if(p->type==HARD_BLOCK){
				alpha=p->destroycount;
//				CBmpsBlit(cbmps_blocks,screen_back,p->type+alpha,x*48+p->lefttime%6-3,gy-p->left);
			}else if(p->type==AIR_BLOCK){
//				CBmpsBlit(cbmps_blocks,screen_back,p->type+alpha,x*48+p->lefttime%6-3,gy-p->left);


			}else{
				switch(p->state){
					case BLOCKSTATE_FALLING:
						BlitForBlock(cbmps_blocks->bmp[p->type],screen_back,p->shape,pre_calcul_x[x],gy-p->left);
					break;

					case BLOCKSTATE_PREFALL:
						if(p->lefttime<66)
						BlitForBlock(cbmps_blocks->bmp[p->type],screen_back,p->shape,pre_calcul_x[x]+vibration[p->lefttime],gy);
					else
						BlitForBlock(cbmps_blocks->bmp[p->type],screen_back,p->shape,pre_calcul_x[x],gy);
					break;

					case BLOCKSTATE_EXTINGUISHING:
						CBmpsBlit(cbmps_character,screen_back,109-p->extinguishingframe/4,pre_calcul_x[x],gy);
					break;

					case BLOCKSTATE_EXTINGUISHED:
						CBmpsBlit(cbmps_character,screen_back,109,pre_calcul_x[x],gy);
					break;

					default:
						BlitForBlock(cbmps_blocks->bmp[p->type],screen_back,p->shape,pre_calcul_x[x],gy);
					break;
				}
				continue;
			}

			switch(p->state){
				case BLOCKSTATE_FALLING:
					CBmpsBlit(cbmps_blocks,screen_back,p->type+alpha,pre_calcul_x[x],gy-p->left);
				break;

				case BLOCKSTATE_PREFALL:
					if(p->lefttime<66)
						CBmpsBlit(cbmps_blocks,screen_back,p->type+alpha,pre_calcul_x[x]+vibration[p->lefttime],gy);
					else
						CBmpsBlit(cbmps_blocks,screen_back,p->type+alpha,pre_calcul_x[x],gy);
				break;

				case BLOCKSTATE_EXTINGUISHING:
					CBmpsBlit(cbmps_character,screen_back,109-p->extinguishingframe/4,pre_calcul_x[x],gy);
				break;

				case BLOCKSTATE_EXTINGUISHED:
					CBmpsBlit(cbmps_character,screen_back,109,pre_calcul_x[x],gy);
				break;
				default:
					CBmpsBlit(cbmps_blocks,screen_back,p->type+alpha,pre_calcul_x[x],gy);
				break;
			}

		}
	}
}


//unset lock
void clear_blockflag(void){
	int x,y;
	TBlockState *p;
	for(y=0;y<GAME_STAGE_HEIGHT;++y)
	for(x=0;x<GAME_STAGE_WIDTH;++x){
		p=&(gamestage[x][y]);
		p->done=0;
		p->unsetlock=0;

	}
}

void clear_blockflag_sub(void){
	int x,y;
	TBlockState *p;
	for(y=0;y<GAME_STAGE_HEIGHT;++y)
	for(x=0;x<GAME_STAGE_WIDTH;++x){
		p=&(gamestage[x][y]);
		p->done_sub=0;
		//JYCET ajout de clear_blockflag car il est appelé toujours avec ce void
		p->done=0;
		p->unsetlock=0;

	}
}

void search(int x,int y,int type){
	TBlockState *p;

	p=&(gamestage[x][y]);
	if(p->done)return;
	if(p->type!=type)return;
	if(p->state==BLOCKSTATE_FALLING)return;
	p->done=1;
	p->state = BLOCKSTATE_EXTINGUISHING;
	p->player_dug=1;
//	p->extinguishingframe = 15;
	p->extinguishingframe = 8;
	if(x>0)search(x-1,y,type);
	if(y>0)search(x,y-1,type);
	if(x<GAME_STAGE_WIDTH-1)search(x+1,y,type);
	if(y<GAME_STAGE_HEIGHT-1)search(x,y+1,type);
}

void self_destroy(int x,int y){

	TBlockState *p;
	clear_blockflag();
	p=&(gamestage[x][y]);
	if(p->type==HARD_BLOCK){
		CWavsPlay(wavs,18);
		p->destroycount++;
		if(p->destroycount>=5){
			p->state=BLOCKSTATE_EXTINGUISHING;
			p->extinguishingframe=8;
			my_air-=20;
			airminus=1;airminuscount=0;
			p->player_dug=1;
			scoreplus+=1;
			scorerest+=1;
		}
		return;
	}
	destroy(x,y);
	scoreplus++;
	scorerest++;
}

void destroy(int x,int y){

	TBlockState *p;
	clear_blockflag();
	p=&(gamestage[x][y]);
	if(p->type==AIR_BLOCK)return;
	if(p->state==BLOCKSTATE_EXTINGUISHING) return;
	search(x,y,p->type);
	CWavsPlay(wavs,12);
}

int erase_block(void){
	TBlockState *p;
	int x,y;
	int res=0;
	for(y=0;y<GAME_STAGE_HEIGHT;++y)
	for(x=0;x<GAME_STAGE_WIDTH;++x){

		p=&(gamestage[x][y]);
		if(p->state == BLOCKSTATE_EXTINGUISHED){
			res=1;
			p->state=BLOCKSTATE_NONE;
			p->type=NO_BLOCK;
		}
	}
	return res;
}
//check and return whether these blocks can fall
//checksheat is 1 when calling search
void search_number(int x,int y,int type,int *number){

	TBlockState *p;

	p=&gamestage[x][y];
	if(p->done)return;
	if(p->type != type)return;
	p->done=1;
	if(p->state==BLOCKSTATE_FALLING)return;
	(*number)++;

	if(x>0)search_number(x-1,y,type,number);
	if(x<GAME_STAGE_WIDTH-1)search_number(x+1,y,type,number);
	if(y>0)search_number(x,y-1,type,number);
	if(y<GAME_STAGE_HEIGHT-1)search_number(x,y+1,type,number);
}
void search_fall(int x,int y,int type,int *checksheat,int *checkleft){

	TBlockState *p,*q;
	//int flag=0;
	p=&gamestage[x][y];

	if(p->done)return;
	if(p->type != type)return;
	p->done=1;
	if(p->state==BLOCKSTATE_FALLING)return;
	q=&gamestage[x][y+1];
	if(q->type != NO_BLOCK && q->type != type){
		if(q->state!=BLOCKSTATE_PREFALL && q->state!=BLOCKSTATE_FALLING)*checksheat&=0;
	}else{

			;
	}



	if(p->state==BLOCKSTATE_NONE)p->left=PREFALLTIME;
	if(q->state==BLOCKSTATE_PREFALL)p->left=q->left;
	if(p->left>*checkleft)*checkleft=p->left;

	if(x>0)search_fall(x-1,y,type,checksheat,checkleft);
	if(x<GAME_STAGE_WIDTH-1)search_fall(x+1,y,type,checksheat,checkleft);
	if(y>0)search_fall(x,y-1,type,checksheat,checkleft);
	if(y<GAME_STAGE_HEIGHT-1)search_fall(x,y+1,type,checksheat,checkleft);


}
void setprefall(int x,int y,int type,int left){
	TBlockState *p,*q;

	p=&gamestage[x][y];
	q=&gamestage[x][y+1];

	if(p->done_sub)return;
	if(p->type != type)return;
	p->done_sub=1;

	if(p->state==BLOCKSTATE_FALLING)return;

/*
	p->state=BLOCKSTATE_FALLING;
	p->left=48;
*/


	p->lefttime=left;
	/*
	p->lefttime=PREFALLTIME;
	if(p->state==BLOCKSTATE_FALLFINISHED)p->lefttime=1;
	if(q->state==BLOCKSTATE_PREFALL) p->lefttime=q->lefttime;
	*/
	p->state=BLOCKSTATE_PREFALL;

	if(x>0)setprefall(x-1,y,type,left);
	if(x<GAME_STAGE_WIDTH-1)setprefall(x+1,y,type,left);
	if(y>0)setprefall(x,y-1,type,left);
	if(y<GAME_STAGE_HEIGHT-1)setprefall(x,y+1,type,left);

}
void unsetprefall(int x,int y,int type){
	TBlockState *p,*q;

	p=&gamestage[x][y];
	q=&gamestage[x][y+1];

	if(p->done)return;
	if(p->type != type)return;
	p->done=1;
	if(p->state==BLOCKSTATE_FALLING)return;
	if(p->state==BLOCKSTATE_EXTINGUISHED||p->state==BLOCKSTATE_EXTINGUISHING)return;
	p->state=BLOCKSTATE_NONE;
	p->lefttime=PREFALLTIME;
	p->unsetlock=1;
	if(p->type==AIR_BLOCK)return;

	if(x>0)unsetprefall(x-1,y,type);
	if(x<GAME_STAGE_WIDTH-1)unsetprefall(x+1,y,type);
	if(y>0)unsetprefall(x,y-1,type);
	if(y<GAME_STAGE_HEIGHT-1)unsetprefall(x,y+1,type);

}
void unsetprefallfinished(int x,int y,int type){
	TBlockState *p,*q;

	p=&gamestage[x][y];
	q=&gamestage[x][y+1];

	if(p->done)return;
	if(p->type != type)return;
	p->done=1;
	if(p->state==BLOCKSTATE_FALLING)return;
	if(p->state==BLOCKSTATE_EXTINGUISHED||p->state==BLOCKSTATE_EXTINGUISHING)return;
	p->state=BLOCKSTATE_FALLFINISHED;
	p->lefttime=PREFALLTIME;
	p->unsetlock=1;
	if(p->type==AIR_BLOCK)return;

	if(x>0)unsetprefallfinished(x-1,y,type);
	if(x<GAME_STAGE_WIDTH-1)unsetprefallfinished(x+1,y,type);
	if(y>0)unsetprefallfinished(x,y-1,type);
	if(y<GAME_STAGE_HEIGHT-1)unsetprefallfinished(x,y+1,type);

}
void erase_check_recursive(int x,int y,int type,int *answer,int *number){

	TBlockState *p;

	p=&gamestage[x][y];
	if(p->done)return;
	if(p->type != type)return;
	p->done=1;
	if(p->state==BLOCKSTATE_FALLING)return;
	(*number)++;
//	if(p->state==BLOCKSTATE_EXTINGUISHED||p->state==BLOCKSTATE_EXTINGUISHING)(*answer)=1;
	if(p->state==BLOCKSTATE_FALLFINISHED)(*answer)=1;

	if(x>0)erase_check_recursive(x-1,y,type,answer,number);
	if(x<GAME_STAGE_WIDTH-1)erase_check_recursive(x+1,y,type,answer,number);
	if(y>0)erase_check_recursive(x,y-1,type,answer,number);
	if(y<GAME_STAGE_HEIGHT-1)erase_check_recursive(x,y+1,type,answer,number);



}

void seterase_recursive(int x,int y,int type){
	TBlockState *p,*q;

	p=&gamestage[x][y];
	q=&gamestage[x][y+1];

	if(p->done_sub)return;
	if(p->type != type)return;
	p->done_sub=1;
	if(p->state==BLOCKSTATE_FALLING)return;
	if(p->state==BLOCKSTATE_EXTINGUISHING||p->state==BLOCKSTATE_EXTINGUISHED){
		if(p->extinguishingframe<8)return;
	}
	p->state=BLOCKSTATE_EXTINGUISHING;
	p->extinguishingframe=20;
	p->left=0;
	p->lefttime=0;
	p->player_dug=0;
	if(x>0)seterase_recursive(x-1,y,type);
	if(x<GAME_STAGE_WIDTH-1)seterase_recursive(x+1,y,type);
	if(y>0)seterase_recursive(x,y-1,type);
	if(y<GAME_STAGE_HEIGHT-1)seterase_recursive(x,y+1,type);

}

void erase_check_optimisation(void){
	// erase_block(); + clear_blockflag_sub();
	TBlockState *p;
	int x,y;
	int res=0;
	for(y=0;y<GAME_STAGE_HEIGHT;++y)
	for(x=0;x<GAME_STAGE_WIDTH;++x){
		p=&(gamestage[x][y]);
		//erase_block();
		if(p->state == BLOCKSTATE_EXTINGUISHED){
			res=1;
			p->state=BLOCKSTATE_NONE;
			p->type=NO_BLOCK;
		}
		// clear_blockflag_sub();
		p->done_sub=0;
		//JYCET ajout de clear_blockflag car il est appelé toujours avec ce void
		p->done=0;
		p->unsetlock=0;

	}
//	return res;
}

void erase_check(void){

	int x,y,answer,number;
	TBlockState *p;

	//clear_blockflag_sub(); supprimé par erase_check_optimisation
	//clear_blockflag(); intégré dans clear_blockflag_sub

	for(y=GAME_STAGE_HEIGHT-5;y>=1;y--)
	for(x=0;x<GAME_STAGE_WIDTH;x++){
		p=&gamestage[x][y];
		if(p->done)continue;
		if(p->type == NO_BLOCK||p->type==AIR_BLOCK)continue;
		if(p->state == BLOCKSTATE_FALLING /*|| p->state == BLOCKSTATE_PREFALL*/)continue;

		answer=0;number=0;
		erase_check_recursive(x,y,p->type,&answer,&number);
		if(answer == 1 && number>3){
			seterase_recursive(x,y,p->type);
			CWavsPlay(wavs, 13);
		}

	}

}

void prefallcheck(void ){

	int x,y,check,lefttime;
	TBlockState *p,*q;

	clear_blockflag_sub();
	//clear_blockflag();

	for(y=GAME_STAGE_HEIGHT-5;y>=1;y--)
	for(x=0;x<GAME_STAGE_WIDTH;x++){
		p=&gamestage[x][y];
		if(p->type == NO_BLOCK)continue;
		if(p->state == BLOCKSTATE_FALLING
		||p->state==BLOCKSTATE_EXTINGUISHING
		||p->state==BLOCKSTATE_EXTINGUISHED)continue;

		//if(p->state == BLOCKSTATE_FALLFINISHED)left=1;else left=PREFALLTIME;
		q=&gamestage[x][y+1];
		if(p->unsetlock)continue;
		if(q->type==NO_BLOCK){


			if(p->state==BLOCKSTATE_NONE){
				p->lefttime=0;
				if(q->player_dug)p->lefttime=PREFALLTIME;
				p->state=BLOCKSTATE_PREFALL;
			}else if(p->state==BLOCKSTATE_FALLFINISHED){
				p->lefttime=0;
				p->state=BLOCKSTATE_PREFALL;
			}else {
				p->state=BLOCKSTATE_PREFALL;

			}
		}/*else if(q->type==p->type){

			p->state=q->state;
			p->state=q->state;


		}*/
		else if(q->state==BLOCKSTATE_PREFALL){
			//if(p->state==BLOCKSTATE_NONE){
				p->state=BLOCKSTATE_PREFALL;
				p->lefttime=q->lefttime;
			//}
		}else
		if(q->state==BLOCKSTATE_EXTINGUISHING
		||q->state==BLOCKSTATE_FALLFINISHED||
		q->state==BLOCKSTATE_EXTINGUISHED){
			if(p->state!=BLOCKSTATE_FALLFINISHED){

				p->lefttime=PREFALLTIME;
				p->state=BLOCKSTATE_NONE;
				unsetprefall(x,y,p->type);

			}else{
				unsetprefallfinished(x,y,p->type);
//				p->state=BLOCKSTATE_PREFALL;
			}
		}
		else {
				p->lefttime=PREFALLTIME;
				p->state=BLOCKSTATE_NONE;
			unsetprefall(x,y,p->type);
		}



	}


	//clear_blockflag();
	clear_blockflag_sub();

	for(y=GAME_STAGE_HEIGHT-5;y>=1;y--)
	for(x=0;x<GAME_STAGE_WIDTH;x++){
		p=&gamestage[x][y];
		if(p->done)continue;
		if(p->type==NO_BLOCK)continue;
		if(p->state!=BLOCKSTATE_PREFALL)continue;
		lefttime=p->lefttime;
		check=1;
		leftcheck(x,y,p->type,&lefttime,&check);
		setleft(x,y,p->type,lefttime);
	}



}

void leftcheck(int x,int y,int type,int *checkleft,int *check){

	TBlockState *p,*q;
	//int flag=0;
	int lefttime;
	p=&gamestage[x][y];
	q=&gamestage[x][y+1];


	if(p->done)return;
	if(p->type != type)return;
	p->done=1;
//	if(p->state!=BLOCKSTATE_PREFALL)return;
	if(p->state==BLOCKSTATE_FALLING||p->state==BLOCKSTATE_EXTINGUISHING)return;

	if(p->lefttime>(*checkleft)){
		*checkleft=p->lefttime;
//		*check=0;
	}
	if(q->type!=p->type){

		if(q->type != NO_BLOCK //&&
//			q->state!=BLOCKSTATE_EXTINGUISHING &&
//			q->state!=BLOCKSTATE_EXTINGUISHED
			){

			if(q->state==BLOCKSTATE_PREFALL && p->state==BLOCKSTATE_PREFALL){
				if(q->lefttime>(*checkleft)) *checkleft=q->lefttime;

			}else{
				/* if(q->state==BLOCKSTATE_NONE||q->state==BLOCKSTATE_FALLFINISHED
				||q->state==BLOCKSTATE_FALLING){
				*/
				lefttime=0;
				if(q->player_dug)lefttime=PREFALLTIME;
				if(lefttime>(*checkleft)) *checkleft=lefttime;
				*check=0;

			}

		}

	}

	if(p->type==AIR_BLOCK)return;

	if(x>0)leftcheck(x-1,y,type,checkleft,check);
	if(x<GAME_STAGE_WIDTH-1)leftcheck(x+1,y,type,checkleft,check);
	if(y>0)leftcheck(x,y-1,type,checkleft,check);
	if(y<GAME_STAGE_HEIGHT-1)leftcheck(x,y+1,type,checkleft,check);


}

void setleft(int x,int y,int type,int left){
	TBlockState *p,*q;

	p=&gamestage[x][y];
	q=&gamestage[x][y-1];

	if(p->done_sub)return;
	if(p->type != type)return;
	p->done_sub=1;

//	if(p->state==BLOCKSTATE_FALLING||p->state==BLOCKSTATE_EXTINGUISHING)return;
	if(p->state!=BLOCKSTATE_PREFALL)return;

	p->lefttime=left;
/*	if(
		q->state!=BLOCKSTATE_EXTINGUISHING &&
		q->type!=NO_BLOCK &&
		q->state==BLOCKSTATE_PREFALL)q->lefttime=left;
*/
	if(p->type==AIR_BLOCK)return;

	if(x>0)setleft(x-1,y,type,left);
	if(x<GAME_STAGE_WIDTH-1)setleft(x+1,y,type,left);
	if(y>0)setleft(x,y-1,type,left);
	if(y<GAME_STAGE_HEIGHT-1)setleft(x,y+1,type,left);

}

//block processing
void blockprocess(void){

	int x,y;
	TBlockState *p,*q;
	clear_blockflag();
	for(y=GAME_STAGE_HEIGHT-1;y>=0;y--)
	for(x=0;x<GAME_STAGE_WIDTH;x++){
		p=&gamestage[x][y];

		if(p->type==NO_BLOCK)continue;

		switch(p->state){


			case BLOCKSTATE_PREFALL:{

				p->lefttime--;
				if(p->lefttime<=0){
					p->lefttime=0;
					q=&gamestage[x][y+1];
//					if(q->state==BLOCKSTATE_EXTINGUISHING)hanabiset(x,y+1,q->extinguishingframe);

					*q=*p;

					p->type=NO_BLOCK;
					p->state=BLOCKSTATE_NONE;

					q->state=BLOCKSTATE_FALLING;
					q->left=24-WALKSPEED_Y;

				}
			}break;


			case BLOCKSTATE_FALLING:{

				p->left-=WALKSPEED_Y;
				if(p->left<WALKSPEED_Y){//finshing.

					p->left=0;
					p->lefttime=1;
					p->state=BLOCKSTATE_FALLFINISHED;
				}


			}break;

			case BLOCKSTATE_EXTINGUISHING:{

				p->extinguishingframe--;
				if(p->extinguishingframe==0)p->state=BLOCKSTATE_EXTINGUISHED;

			}break;

			case BLOCKSTATE_FALLFINISHED:{
			}break;
		}
	}
}
#define HANABIMAX 100
typedef struct{

	int x;
	int y;
	int time;
	int avail;
}Thanabi;
Thanabi hanabi[HANABIMAX];
void clearhanabi(void){
	int i;
	Thanabi init={0,0,0,0};
	for(i=0;i<HANABIMAX;++i){

		hanabi[i]=init;

	}
}
void drawhanabi(void){
	Thanabi* p;
	int gy;
	int i;

	for(i=0;i<HANABIMAX;++i){
		p=&hanabi[i];
		if(!p->avail)continue;

		p->time--;
		if(p->time<=0){

			p->avail=0;
			continue;
		}
		gy=(p->y+4)*24-my_y;
		if(gy>-24 && gy<320){
			//draw
			CBmpsBlit(cbmps_character,screen_back,109-p->time/3,p->x*24,gy);

		}else{
			p->avail=0;
		}
	}
}

int hanabiset(int x,int y,int n){

	int i=0;

	do{
		if(hanabi[i].avail!=0)i++;else{

			hanabi[i].x=x;
			hanabi[i].y=y;
			hanabi[i].time=n;
			hanabi[i].avail=1;

			return 0;
		}
	}while(i<HANABIMAX);

	return 1;
}

void moveTilescroll(void){

	CTileScroll *p;
	//int interval= SCROLL_SPEED ;//milisec
	p=&tscroll;

	//if(p->clock+interval<gametime.clock){
		p->x--;
		if(p->x<=-(p->bmp->w))p->x=0;
		p->y--;
		if(p->y<=-(p->bmp->h))p->y=0;

		//p->clock=gametime.clock;
	//}else{
		//if(p->clock>gametime.clock+interval*5)p->clock=gametime.clock;
	//}

}
void drawTilescroll(void){

	int x,y;

	for(y=tscroll.y;y<screen_back->h;y+=tscroll.bmp->h){
		for(x=tscroll.x;x<screen_back->w;x+=tscroll.bmp->w){

			CBmpsBlit(cbmps_character,screen_back,52,x,y);
		}
	}

}

int title(void){

	//printf("starting title loop\n");
	//Uint8 *keys;
	int y=0;
	signed short f = 255;

	int i;
	static int dx[]={
		0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,
		0,1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1
	};
	int nx=0,maxx;
	int wait=0;

	//char buffer[255];

	maxx=sizeof(dx)/sizeof(int);
	LoadModule( mod_TITLE );
	reset_frame_skip();

	//JYCET creation premier tampon
	drawTilescroll();
	CBmpsBlit(cbmps_character,screen_back,50,0,0);
	for(i=0;i<3;++i) CBmpsBlit(cbmps_character,screen_back,60+i,90,182+i*18);
	SDL_BlitSurface( screen_back ,NULL, screen_tmp ,NULL);
	//JYCET fade in
	while(f>0){

		SDL_BlitSurface(screen_tmp,NULL,screen_back,NULL);
		SDL_SetAlpha(screen_black,SDL_SRCALPHA,f);
		SDL_BlitSurface(screen_black,NULL,screen_back,NULL);
		f -= FADE_SPEED;
		if (f < FADE_SPEED) f=0;
		rafraichir_ecran();
	}

	reset_frame_skip();
	StartModule();

	CTimeReset(&gametime,setting_defaultFPS);
	//CTimeChangeFPS(&gametime,setting_defaultFPS);
	do{
		if(keyread()) return 0;
		moveTilescroll();
		CInputUpdate(gameinput,0);

		if ( gameinput->button[BUTTON_START] == 1 || gameinput->button[BUTTON_1] == 1) {
#ifdef __MIKMOD__
			if (Player_Active())
#endif
				FreeModule();
			//JYCET capture ecran
			SDL_BlitSurface( screen_back ,NULL, screen_tmp ,NULL);
			//JYCET fade out
			f=0;
			while(f<256) {
				SDL_BlitSurface(screen_tmp,NULL,screen_back,NULL);
				SDL_SetAlpha(screen_black,SDL_SRCALPHA,f);
				SDL_BlitSurface(screen_black,NULL,screen_back,NULL);
				f += FADE_SPEED;
				rafraichir_ecran();
			}
			return 0;
		}

		if ( gameinput->button[BUTTON_0] == SDL_PRESSED || gameinput->button[BUTTON_2] == SDL_PRESSED ){
		    CWavsPlay(wavs, 10);
			switch(y){
				case 0:{
#ifdef __MIKMOD__
					if (Player_Active())
						FreeModule();
#endif
					//JYCET capture ecran
					SDL_BlitSurface( screen_back ,NULL, screen_tmp ,NULL);
					//JYCET fade out
					f=0;
					while(f<256) {
						SDL_BlitSurface(screen_tmp,NULL,screen_back,NULL);
						SDL_SetAlpha(screen_black,SDL_SRCALPHA,f);
						SDL_BlitSurface(screen_black,NULL,screen_back,NULL);
						f += FADE_SPEED;
						rafraichir_ecran();
					}
					SDL_Delay(500);
					return 2;
				}break;

				case 1:{
					SDL_Delay(500);
					if(!highscore())return 0;
					LoadModule( mod_TITLE );
					StartModule();
					reset_frame_skip();
					SDL_Delay(500);
					y=0;
				}break;

				case 2:{
#ifdef __MIKMOD__
					if (Player_Active())
#endif
						FreeModule();
					//JYCET capture ecran
					SDL_BlitSurface( screen_back ,NULL, screen_tmp ,NULL);
					//JYCET fade out
					f=0;
					while(f<256) {
						SDL_BlitSurface(screen_tmp,NULL,screen_back,NULL);
						SDL_SetAlpha(screen_black,SDL_SRCALPHA,f);
						SDL_BlitSurface(screen_black,NULL,screen_back,NULL);
						f += FADE_SPEED;
						rafraichir_ecran();
					}
					return 0;
				}break;

			}
		}else volume_control();

		if (!wait&&( gameinput->button[BUTTON_UP] || gameinput->button[BUTTON_RIGHT]) ){
			if(y>0){
				y--;
				CWavsPlay(wavs, 10);
			}
			wait=15;
		}
		if (!wait&&( gameinput->button[BUTTON_DOWN] || gameinput->button[BUTTON_LEFT]) ){
			if(y<2){
				y++;
				CWavsPlay(wavs, 10);
			}
			wait=15;
		}
		wait--;if(wait<=0)wait=0;


		//if(!gametime.isDelay){

			drawTilescroll();
			//CBmpsBlit(cbmps_character,screen_back,50,0,0);
			SDL_BlitSurface(cbmps_character->bmp[50],NULL,screen_back,NULL);
			for(i=0;i<3;++i){
				if(y==i)
					CBmpsBlit(cbmps_character,screen_back,60+i,90+dx[nx++],182+i*18);
				else
					CBmpsBlit(cbmps_character,screen_back,60+i,90,182+i*18);
			}
			nx=nx%maxx;

			rafraichir_ecran();
		//}
		//CTimeWait(&gametime);
	}while(1);

	return 0;
}



int highscoretemp_score;
int highscoretemp_depth;
int highscoretemp_playtime;
int highscoretemp_entrynumber_score;
int highscoretemp_entrynumber_time;
char highscoretemp_name[4];

int init_nameentry(int score,int depth,int playtime){


	int i;

	nameentry_x=0;
	nameentry_x_wait=0;
	space_repeat=0;
	bs_repeat=0;
	nameentry_vx=0;
	name_x=0;


	highscoretemp_score=score;
	highscoretemp_depth=depth;
	highscoretemp_playtime=playtime;

	THighScoreAdd(&highscoredata,"___",playtime,depth,score);
	THighScoreSortByTime(&highscoredata);
	THighScoreSortByDepth(&highscoredata);
	THighScoreSortByScore(&highscoredata);

	for(i=0;i<SCOREMEMBER+1;++i){
		if(!strcmp(highscoredata.name[i],"___"))break;
	}
	highscoretemp_entrynumber_score=i;



	THighScoreAdd(&fasttimedata,"___",playtime,depth,score);
	THighScoreSortByDepth(&fasttimedata);
	THighScoreSortByScore(&fasttimedata);
	THighScoreSortByTime(&fasttimedata);

	for(i=0;i<SCOREMEMBER+1;++i){
		if(!strcmp(fasttimedata.name[i],"___"))break;
	}
	highscoretemp_entrynumber_time=i;



	if(highscoretemp_entrynumber_score==SCOREMEMBER&&
	highscoretemp_entrynumber_time==SCOREMEMBER)return -1;//highsore is not marked!

	strcpy(highscoretemp_name,"___");
	//CWavsPlayMusicStyle(wavs,2);
	return i;

}

int move_nameentry(void){

	Uint8 *keys;
//	int joy_up=0,joy_down=0,joy_left=0,joy_right=0,joy_space=0,joy_cancel=0;


	if(nameentry_x_wait==0)keys = SDL_GetKeyState(NULL);
	if(nameentry_x_wait==0){


		CInputUpdate(gameinput,0);
		/*
		if(joystick && SDL_JoystickGetButton(joystick, setting_joyconfirm) == SDL_PRESSED)joy_space=1;
		if(joystick && SDL_JoystickGetButton(joystick, setting_joycancel) == SDL_PRESSED)joy_cancel=1;
		if(joystick && (int)SDL_JoystickGetAxis(joystick, 0) > setting_joyaxismax)joy_right=1;
		if(joystick && (int)SDL_JoystickGetAxis(joystick, 0) < -setting_joyaxismax)joy_left=1;
		*/


		if ( (gameinput->button[BUTTON_RIGHT] || gameinput->button[BUTTON_UP]) && nameentry_x<sizeof(nameentry_moji)-2 ) {nameentry_x_wait=4;nameentry_vx=1;}
		if ( (gameinput->button[BUTTON_LEFT] || gameinput->button[BUTTON_DOWN]) && nameentry_x>0 ) {nameentry_x_wait=4;nameentry_vx=-1;}

		if ( gameinput->button[BUTTON_2] || gameinput->button[BUTTON_START]){
			if(!strcmp(highscoretemp_name,"___"))strcpy(highscoretemp_name,"S.H");
			strcpy(highscoredata.name[highscoretemp_entrynumber_score],highscoretemp_name);
			strcpy(fasttimedata.name[highscoretemp_entrynumber_time],highscoretemp_name);

			return 1;
		}
		if (gameinput->button[BUTTON_0]){

			if(space_repeat);else{


				if(name_x<3){

					highscoretemp_name[name_x]=nameentry_moji[nameentry_x];
					name_x++;
				};


				space_repeat=1;
			}

		}else{
			space_repeat=0;
		}
		if ( gameinput->button[BUTTON_1] ){

			if(bs_repeat);else{

				if(name_x>0)name_x--;
				highscoretemp_name[name_x]=' ';


				bs_repeat=1;
			}

		}else{
			bs_repeat=0;
		}

		volume_control();
	}

	if(nameentry_x_wait>0){
		nameentry_x_wait--;
		if(nameentry_x_wait==0){
			nameentry_x+=nameentry_vx;
			nameentry_vx=0;
		}
	}
	return 0;
}

void draw_nameentry(void){


	CffontBlitxy(font,nameentry_moji,screen_back,155-nameentry_x*8,152);  //all  done
	CBmpsBlit(cbmps_character,screen_back,53,0,0);
}

void nameentry(void){

	char buffer[64];
	Uint8 *keys;
	char *scoreboard="NAME DEPTH   SCORE  TIME";


	CTimeReset(&gametime,setting_defaultFPS);
	//CTimeChangeFPS(&gametime,setting_defaultFPS);

	LoadModule( mod_HISCORE );
	reset_frame_skip();
	StartModule();

	do{
		if(keyread()){
			strcpy(highscoredata.name[highscoretemp_entrynumber_score],"S.H");
			strcpy(fasttimedata.name[highscoretemp_entrynumber_time],"S.H");
			return;
			}
		moveTilescroll();

		keys = SDL_GetKeyState(NULL);
		if(move_nameentry())break;
		//if(!gametime.isDelay){

			drawTilescroll();

			if(highscoretemp_depth!=1000){
				sprintf(buffer," %3s %4d  %6d0  --'--'--",
					highscoretemp_name,
					highscoretemp_depth,
					highscoretemp_score
					);
			}else{
				sprintf(buffer," %3s %4d  %6d0  %02d'%02d'%02d",
					highscoretemp_name,
					highscoretemp_depth,
					highscoretemp_score,
					highscoretemp_playtime/6000,
					(highscoretemp_playtime/100)%60,
					highscoretemp_playtime%100

				);
			}
			CffontBlitxy(font,buffer,screen_back,35,12+75); //done
			CffontBlitxy(font,scoreboard,screen_back,35,75); //likewise

			draw_nameentry();

			rafraichir_ecran();
			//SDL_UpdateRect(screen_back,0,0,0,0);
		//}
		CTimeWait(&gametime);
	}while(1);
}

int highscore(void){
	int i;
	char buf[SCOREMEMBER][64];
	char buf_fast[SCOREMEMBER][64];
	char *scoreboard="NAME DEPTH   SCORE";
	char *scoreboard_fast="NAME   SCORE   TIME";
	int mode=0;//0--highscore 1--fasttime

	int scroll_x = 0;

//	int joy_space=0;
	for(i=0;i<SCOREMEMBER;++i){
		sprintf(buf[i]," %3s %4d  %6d0",
			highscoredata.name[i],
			highscoredata.depth[i],
			highscoredata.score[i]
		);

	}
	for(i=0;i<SCOREMEMBER;++i){
		sprintf(buf_fast[i]," %3s %6d0 %02d'%02d'%02d",
			fasttimedata.name[i],
			fasttimedata.score[i],
			fasttimedata.time[i]/6000,
			(fasttimedata.time[i]/100)%60,
			fasttimedata.time[i]%100

			);


	}
	CTimeReset(&gametime,setting_defaultFPS);
	//CTimeChangeFPS(&gametime,setting_defaultFPS);

	LoadModule( mod_HISCORE );
	reset_frame_skip();
	StartModule();

	do{
//		joy_space=0;
		if(keyread())return 0;
		moveTilescroll();

//		keys = SDL_GetKeyState(NULL);
		CInputUpdate(gameinput,0);
//		if(joystick && SDL_JoystickGetButton(joystick, setting_joyconfirm) == SDL_PRESSED)joy_space=1;
		if ( gameinput->button[BUTTON_0] ){SDL_Delay(100);return 1;}//exit highscore
		if( gameinput->button[BUTTON_RIGHT] && mode<1){
			mode++;
		}
		if( gameinput->button[BUTTON_LEFT] && mode>0){
			mode--;
		}

		volume_control();


		//if(!gametime.isDelay){

			drawTilescroll();

			if(mode==0){
				if (scroll_x<0) scroll_x += 10;
			}else{
				if (scroll_x>-320) scroll_x -= 10;
			}


			//if(mode==0){
				CBmpsBlit(cbmps_character,screen_back,51,scroll_x,0);
				CffontBlitxy(font,scoreboard,screen_back,scroll_x+65,60);//done
			//}else{
				CBmpsBlit(cbmps_character,screen_back,54,scroll_x+320,0);
				CffontBlitxy(font,scoreboard_fast,screen_back,scroll_x+320+65,60);//done
			//}

			for(i=0;i<SCOREMEMBER;++i){

				//if(mode==0){
					CffontBlitxy(font,buf[i],screen_back,scroll_x+65,i*12+75);//done, 12 should be 12.5
				//}else{
					CffontBlitxy(font,buf_fast[i],screen_back,scroll_x+320+65,i*12+75);//likewise

				//}
			}

			rafraichir_ecran();
		//}
		CTimeWait(&gametime);
	}while(1);

	return 0;
}
void THighScoreSwap(THighScore *p,int i,int j){
	char name[4];
	int time,score,depth;
	strcpy(name,p->name[j]);
	time=p->time[j];
	score=p->score[j];
	depth=p->depth[j];

	strcpy(p->name[j],p->name[i]);
	p->time[j]=p->time[i];
	p->score[j]=p->score[i];
	p->depth[j]=p->depth[i];

	strcpy(p->name[i],name);
	p->time[i]=time;
	p->score[i]=score;
	p->depth[i]=depth;
}

void THighScoreSortByTime(THighScore *p){
	int i,j;

	for(j=0;j<SCOREMEMBER;++j)
	for(i=j+1;i<SCOREMEMBER+1;++i){
		if(p->time[i]<p->time[j])THighScoreSwap(p,i,j);
	}
}
void THighScoreSortByDepth(THighScore *p){
	int i,j;

	for(j=0;j<SCOREMEMBER;++j)
	for(i=j+1;i<SCOREMEMBER+1;++i){
		if(p->depth[i]>p->depth[j])THighScoreSwap(p,i,j);
	}
}
void THighScoreSortByScore(THighScore *p){
	int i,j;

	for(j=0;j<SCOREMEMBER;++j)
	for(i=j+1;i<SCOREMEMBER+1;++i){
		if(p->score[i]>p->score[j])THighScoreSwap(p,i,j);
	}
	return;
}

void THighScoreAdd(THighScore *p,char *name,int time,int depth,int score){

	strcpy(p->name[SCOREMEMBER],name);
	p->time[SCOREMEMBER]=time;
	p->depth[SCOREMEMBER]=depth;
	p->score[SCOREMEMBER]=score;
}

int THighScoreSave(THighScore *p,char *filename){

	FILE *fp;
	int i;

	fp=fopen(filename,"w");
	if(fp==NULL){
		//fprintf(stderr,"cant open %s",filename);
		return -1;
	}
	for(i=0;i<SCOREMEMBER;++i){

		fprintf(fp,highscoreformat,
			(p->name[i]),
			(p->time[i]),
			(p->depth[i]),
			(p->score[i])
			);

	}
	fclose(fp);
//	sync();
	return 0;
}
int THighScoreLoad(THighScore *p,char *filename){


	int i;
	FILE *fp;
	THighScore init={

		{"ZEN","K.K","IKU","FKD","IWA","M.N","TT ","KOB","SAD","ADA","DUM"},
		{100000,100000,100000,100000,100000,100000,100000,100000,100000,100000,100000},
		{100,90,80,70,60,50,40,30,20,10,0},
		{100,90,80,70,60,50,40,30,20,10,0}
	};

	fp=fopen(filename,"rb");
	if(fp==NULL){
	//	printf("create new save file\n");
		*p=init;
		return THighScoreSave(&init,filename);
	}
	for(i=0;i<SCOREMEMBER;++i){

		fscanf(fp,highscoreformat,
			&(p->name[i]),
			&(p->time[i]),
			&(p->depth[i]),
			&(p->score[i])
			);

	}
	fclose(fp);
	return 0;

}

void joy_init(void){
	gameinput=CInputInit(setting_joysticknumber,setting_joyenabled);
	CInputDefaultSetting(gameinput);
	CInputHoldButtons(gameinput);
	CInputUnholdArrows(gameinput);
	CInputSetMinAxis(gameinput,setting_joyaxismax);
}
void joy_final(void){
	CInputFree(gameinput);
	gameinput=NULL;
}

int BlitForBlock(SDL_Surface *p,SDL_Surface *dest,int num,int x,int y){

	static SDL_Rect dr;
	static SDL_Rect rects[]={
		{0,0,24,24},
		{24,0,24,24},
		{0,72,24,24},
		{24,72,24,24},

		{72,0,24,24},
		{48,0,24,24},
		{72,72,24,24},
		{48,72,24,24},

		{0,24,24,24},
		{24,24,24,24},
		{0,48,24,24},
		{24,48,24,24},

		{72,24,24,24},
		{48,24,24,24},
		{72,48,24,24},
		{48,48,24,24},
	};

	if(p==NULL)return(-1);

	dr.w=rects[num].w;
	dr.h=rects[num].h;
	dr.x=x;
	dr.y=y;

	SDL_BlitSurface(p,&rects[num],dest,&dr);

	return 0;

}
void set_shape(void){

	int x,y;
	TBlockState *p;

	for(y=0;y<GAME_STAGE_HEIGHT;++y){
		for(x=0;x<GAME_STAGE_WIDTH;++x){
			p=&(gamestage[x][y]);

			if(p->type==NO_BLOCK||p->type==HARD_BLOCK||p->type==AIR_BLOCK){++p;
				continue;}

			if(p->state==BLOCKSTATE_FALLING
				||p->state==BLOCKSTATE_PREFALL
				||p->state==BLOCKSTATE_EXTINGUISHING
			) {++p;
				continue;}
			p->shape=0;

			if(x>0  &&gamestage[x-1][y].type==p->type) /*p->shape+=4;*/{
				++p->shape;
				++p->shape;
				++p->shape;
				++p->shape;}
			if(x<GAME_STAGE_WIDTH-1  && gamestage[x+1][y].type==p->type) /*p->shape+=1;*/ ++p->shape;
			if(y>0  && gamestage[x][y-1].type==p->type) /*p->shape+=2;*/ {
				++p->shape;
				++p->shape;}
			if(y<GAME_STAGE_HEIGHT-1  && gamestage[x][y+1].type==p->type) /*p->shape+=8;*/{
				++p->shape;
				++p->shape;
				++p->shape;
				++p->shape;
				++p->shape;
				++p->shape;
				++p->shape;
				++p->shape;}
		}
	}
}
