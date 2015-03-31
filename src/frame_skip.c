/*  gngeo, a neogeo emulator
 *  Copyright (C) 2001 Peponas Mathieu
 * 
 *  This program is free software; you can redistribute it and/or modify  
 *  it under the terms of the GNU General Public License as published by   
 *  the Free Software Foundation; either version 2 of the License, or    
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
 */
/* small modification by JyCet for gaming application :) */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include "SDL.h"
#include "frame_skip.h"

#ifndef uclock_t
#define uclock_t Uint32
#endif

#define TICKS_PER_SEC 1000000UL
static int CPU_FPS=60;
static uclock_t F;

#define MAX_FRAMESKIP 10


static char init_frame_skip = 1;
char skip_next_frame = 0;
static struct timeval init_tv = { 0, 0 };

uclock_t get_ticks(void)
{
    struct timeval tv;

    gettimeofday(&tv, 0);
    if (init_tv.tv_sec == 0)
	init_tv = tv;
    return (tv.tv_sec - init_tv.tv_sec) * TICKS_PER_SEC + tv.tv_usec -
	init_tv.tv_usec;


}

void reset_frame_skip(void)
{
	//static Uint8 init=0;

	init_tv.tv_usec = 0;
	init_tv.tv_sec = 0;
	skip_next_frame = 0;
	init_frame_skip = 1;
	CPU_FPS=setting_defaultFPS;
	F = (uclock_t) ((double) TICKS_PER_SEC / CPU_FPS);
	
	//fps = 0;
}



int frame_skip(int init)
{
	static uclock_t sec = 0;
	static uclock_t rfd;
	static uclock_t target;
	static int nbFrame = 0;
	//static int cpuFrame = 0;

	if (init_frame_skip) {
		init_frame_skip = 0;
		target = get_ticks();
	    
		nbFrame = 0;
		//cpuFrame = 0;
		
		sec = 0;

		return 0;
	}

	target += F;
	rfd = get_ticks();
	
	if (rfd < target){
		while (get_ticks() < target) {
			if (sleep_idle) {
				SDL_Delay(10);		
			}
		} 
	}

 
	if (show_fps){
		nbFrame++;
		//cpuFrame += (1-( (target-rfd)/F) )*100;
		if (get_ticks() - sec >= TICKS_PER_SEC) {

			sprintf(fps_str, "%2d", nbFrame-1);
			//sprintf(cpu_time, "%d", (cpuFrame/nbFrame) );

			nbFrame = 0;
			//cpuFrame = 0;
			
			sec = get_ticks();
		}
	}
		
    return 0;
}
