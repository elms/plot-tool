#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "projective.h"

#define N_ITS 100000

int main(int argc, char** argv) {

  Display* disp = XOpenDisplay(NULL);
  if (NULL == disp) {

    exit(0);
  }
  int screen = DefaultScreen(disp);

  int width=1200;
  int height = 1200;

  unsigned long black = BlackPixel(disp, screen);
  unsigned long white = WhitePixel(disp, screen);

  Window win = XCreateSimpleWindow(disp, RootWindow(disp, screen),
				   0, 0, width, height, 0, white, black);


  XSelectInput(disp, win, ExposureMask | KeyPressMask | ButtonPressMask);
  XMapWindow(disp, win);

  GC gc = XCreateGC(disp, win, 0, NULL);

  XClearWindow(disp, win);
  XMapRaised(disp, win);

  XEvent ev;
  int cont = 1;
  static size_t bcnt = 0;

  xpdata_t x = 0;
  xpdata_t y = 0;
  int rind = 0;
  xpdata_t diffs[5][6] =
    {
#if 0
      {+0.2, -0.4, -0.4, -0.2, 0.8, 0.4},
      {-0.4, -0.2, -0.2, +0.4, 0.4, 0.2},
      {-0.4, -0.2, -0.2, +0.4, 0.4, 1.2},
#if CONWAY_LINES
      {+0.4, -0.2, +0.2, +0.4, 0.4, 0.2},
#else
      {-0.4, +0.2, -0.2, -0.4, 0.4, 1.2},
#endif
      {-0.4, +0.2, -0.2, -0.4, 0.4, 1.2},
#elif 1
      // Conway aorta https://arxiv.org/pdf/1001.2203.pdf
      {-0.4, -0.2, -0.2, +0.4, -0.4, -0.2},
      {+0.4, -0.2, +0.2, +0.4, +0.0, +0.0},
      {-0.4, +0.2, -0.2, -0.4, -0.2, +0.4},

      {+0.4, -0.2, +0.2, +0.4, +0.0, +0.0},
      {-0.4, +0.2, -0.2, -0.4, -0.2, +0.4},
#else
      // Sierpinkski examples
      {+0.5, -0.0, -0.0, +0.5, 0.0, 0.0},
      {+0.5, -0.0, +0.0, +0.5, 0.0, 0.5},
      {+0.5, -0.0, +0.0, +0.5, 0.5, 0.0},
      {+0.5, -0.0, +0.0, +0.5, 0.0, 0.5},
      {+0.5, +0.0, -0.0, +0.5, 0.0, 0.5},
#endif
    };
  
  x = 0;
  y = 0;

  int ssc = 50;
  int soff = 500;

  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
          printf("expose\n");
          unsigned long color = 0xff;
          XSetForeground(disp, gc, color);

          break;
        }
      case ButtonPress:
      case KeyPress:

#if 0
	{
	xpdata_t x1;
	xpdata_t y1;
	xpdata_t x2;
	xpdata_t y2;
	int xo = width/2;//rand()%soff;
	int yo = height/2;//rand()%soff;
	int ss = 20;//rand()%ssc;
	
	for (size_t ii=0; ii< 5; ii++) {
	  xpdata_t* aff = diffs[ii];
	  for (size_t jj=0; jj<4; jj++) {
	    x = (jj == 1) * 1;
	    y = (jj == 2) * 2;
	    printf("x: %f %f\n", x, y);
	    xpdata_t x2 = aff[0] * x + aff[1] * y + aff[4];
	    xpdata_t y2 = aff[2] * x + aff[3] * y + aff[5];
	    printf("xp: %f %f\n", x2, y2);

	    if (jj>0) {
	      XDrawLine(disp, win, gc,
			ss * x1 + xo, ss * y1 + yo,
			ss * x2 + xo, ss * y2 + yo);
	    }
	    x1 = x2;
	    y1 = y2;
	  }
	}
	}
#endif

	//x = rand();
	//y = rand();
        bcnt++;
	{
	  int xo = width/2;//rand()%soff;
	  int yo = height/2;//rand()%soff;
	  int ss = 1200;//rand()%ssc;
	  //XClearWindow(disp, win);


	  unsigned long colors[3] = { 0xaa0000, 0x00aaaa, 0x0000aa };
          XSetForeground(disp, gc, colors[rand()%3]);

	  double rot = M_PI;//(rand()%360) * 2 * M_PI/360;

	  for (int ii=0; ii<N_ITS; ii++) {
	    rind = rand() % 5;

	    xpdata_t* aff = diffs[rind];
	    xpdata_t tmp = aff[0] * x + aff[1] * y + aff[4];
	    y            = aff[2] * x + aff[3] * y + aff[5];
	    x = tmp;

#if 1
	    XDrawPoint(disp, win, gc,
		       round(ss*(cos(rot) * x - sin(rot) * y)) + xo,
		       round(ss*(sin(rot) * x + cos(rot) * y)) + yo);
#endif

	  }
	}

        printf("button: %zu\n", bcnt);

        if (ev.type == KeyPress) {
          KeySym ksym = XLookupKeysym(&ev.xkey, 0);
          if (XK_q == ksym || XK_Q == ksym) {
            cont = 0;
          }
        }
        break;
    }
  }

  XDestroyWindow(disp, win);
  XCloseDisplay(disp);
}
