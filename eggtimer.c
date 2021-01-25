#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>

static char* cmd_done = "aplay ./beep.wav"; /* put whatever command you wanna run when the time ticks down here */
static unsigned char dispdigits[] =    {0b11110111, 
					0b10010010, 
					0b11011101, 
					0b11011011, 
					0b10111010, 
					0b11101011, 
					0b11101111, 
					0b11010010, 
					0b11111111, 
					0b11111010};

typedef struct
{
	int cs;
	int dcs;
	int s;
	int ds;
	int m;
	int dm;
	unsigned char csdisp;
	unsigned char dcsdisp;
	unsigned char sdisp;
	unsigned char dsdisp;
	unsigned char mdisp;
	unsigned char dmdisp;
} eggtimer;

int
setbits(eggtimer* e)
{
	e->csdisp = dispdigits[e->cs];
	e->dcsdisp = dispdigits[e->dcs];
	e->sdisp = dispdigits[e->s];
	e->dsdisp = dispdigits[e->ds];
	e->mdisp = dispdigits[e->m];
	e->dmdisp = dispdigits[e->dm];
	return 0;
}

int
tick(eggtimer* e)
{
	e->cs -=1;
	e->csdisp = dispdigits[e->cs];
	if (e->cs < 0)
	{
		e->dcs -=1;
		e->cs = 9;
		e->dcsdisp = dispdigits[e->dcs];
		e->csdisp = dispdigits[e->cs];
	}
	if (e->dcs < 0)
	{
		e->s -=1;
		e->dcs = 9;
		e->sdisp = dispdigits[e->s];
		e->dcsdisp = dispdigits[e->dcs];
	}
	if (e->s < 0)
	{
		e->ds -=1;
		e->s = 9;
		e->dsdisp = dispdigits[e->ds];
		e->sdisp = dispdigits[e->s];
	}
	if (e->ds < 0)
	{
		e->m -=1;
		e->ds = 5;
		e->mdisp = dispdigits[e->m];
		e->dsdisp = dispdigits[e->ds];
	}
	if (e->m < 0)
	{
		e->dm -=1;
		e->m = 9;
		e->dmdisp = dispdigits[e->dm];
		e->mdisp = dispdigits[e->m];
	}
	if (e->dm < 0)
	{
		e->cs = 0;
		e->dcs = 0;
		e->s = 0;
		e->ds = 0;
		e->m = 0;
		e->dm = 0;
		setbits(e);
		popen(cmd_done, "r");
		return 1;
	}
	else return 0;
}

unsigned char
drawdigit(Display* dpy, Drawable w, int s, unsigned char digit, int x)
{
	if(digit & 128)
	{
		if(!(digit & 64)) XClearArea(dpy, w, x + 10, 10, 30, 10, False);
		if(!(digit & 32)) XClearArea(dpy, w, x + 10, 10, 10, 30, False);
		if(!(digit & 16)) XClearArea(dpy, w, x + 30, 10, 10, 30, False);
		if(!(digit & 8)) XClearArea(dpy, w, x + 10, 30, 30, 10, False);
		if(!(digit & 4)) XClearArea(dpy, w, x + 10, 30, 10, 40, False);
		if(!(digit & 2)) XClearArea(dpy, w, x + 30, 30, 10, 40, False);
		if(!(digit & 1)) XClearArea(dpy, w, x + 10, 60, 30, 10, False);
		if(digit & 64) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 10, 10, 30, 10);
		if(digit & 32) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 10, 10, 10, 30);
		if(digit & 16) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 30, 10, 10, 30);
		if(digit & 8) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 10, 30, 30, 10);
		if(digit & 4) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 10, 30, 10, 40);
		if(digit & 2) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 30, 30, 10, 40);
		if(digit & 1) XFillRectangle(dpy, w, DefaultGC(dpy, s), x + 10, 60, 30, 10);
		digit &= 127;
	}
	return digit;
}

int
drawclock(Display* dpy, Drawable w, int s, eggtimer* e)
{
	e->dmdisp = drawdigit(dpy, w, s, e->dmdisp, 0);
	e->mdisp = drawdigit(dpy, w, s, e->mdisp, 40);
	e->dsdisp = drawdigit(dpy, w, s, e->dsdisp, 90);
	e->sdisp = drawdigit(dpy, w, s, e->sdisp, 130);
	e->dcsdisp = drawdigit(dpy, w, s, e->dcsdisp, 180);
	drawdigit(dpy, w, s, e->csdisp, 220);
	return 0;
}

int
takeinput(eggtimer* e, int i)
{
	e->dm = e->m;
	e->m = e->ds;
	e->ds = e->s;
	e->s = e->dcs;
	e->dcs = e->cs;
	e->cs = i;
	return 0;
}

int init(eggtimer* e)
{
        e->cs = 0;
        e->dcs = 0:
        e->s = 0;
        e->ds = 0;
        e->m = 0;
        e->dm = 0;
        setbits(e);
        return 0;
}

int
main(void)
{
	Display *dpy;
	Window w;
	XEvent e;
	int s;
	int ui;
	int running = 0;
	long int starttime;
	long int stoptime;
	struct timeval start, stop;
	eggtimer et;
	KeySym k;
	XKeyEvent *ev;

	init(e);
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL)
	{
		fprintf(stderr, "Cannot open display.\n");
		exit(1);
	}
	s = DefaultScreen(dpy);
	w = XCreateSimpleWindow(dpy, RootWindow(dpy, s), 10, 10, 250, 70, 1, BlackPixel(dpy, s), WhitePixel(dpy, s));
	XSelectInput(dpy, w, ExposureMask | KeyPressMask);
	XMapWindow(dpy, w);
	for (;;)
	{
		gettimeofday(&start, NULL);
		gettimeofday(&stop, NULL);
		starttime = start.tv_sec * 1000000 + start.tv_usec;
		stoptime = stop.tv_sec * 1000000 + start.tv_usec;
		while(running)
		{
			if(stoptime - starttime >= 10000)
			{
				running = !tick(&et);
				printf("(tick)current time: %d%d %d%d %d%d\n", et.dm, et.m, et.ds, et.s, et.dcs, et.cs);
				gettimeofday(&start, NULL);
				gettimeofday(&stop, NULL);
				starttime = start.tv_sec * 1000000 + start.tv_usec;
				stoptime = stop.tv_sec * 1000000 + stop.tv_usec;
			}
			else
			{
				gettimeofday(&stop, NULL);
				stoptime = stop.tv_sec * 1000000 + stop.tv_usec;
			}
			drawclock(dpy, w, s, &et);
		}
		XNextEvent(dpy, &e);
		if (e.type == KeyPress)
		{
			ev = &e.xkey;
			k = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
			if(k == XK_space) running = 1;
			if(k == XK_Escape) break;
			else
			{
				switch(k)
				{
					case XK_0:
					ui = 0;
					break;

					case XK_1:
					ui = 1;
					break;

					case XK_2:
					ui = 2;
					break;

					case XK_3:
					ui = 3;
					break;

					case XK_4:
					ui = 4;
					break;

					case XK_5:
					ui = 5;
					break;

					case XK_6:
					ui = 6;
					break;

					case XK_7:
					ui = 7;
					break;

					case XK_8:
					ui = 8;
					break;

					case XK_9:
					ui = 9;
					break;

					default:
					ui = -1;
					break;
				}
				if(ui != -1) takeinput(&et, ui);
				setbits(&et);
				drawclock(dpy, w, s, &et);
				printf("current time: %d%d %d%d %d%d\n", et.dm, et.m, et.ds, et.s, et.dcs, et.cs);
			}
		}
	}
	XCloseDisplay(dpy);
	return 0;
}
