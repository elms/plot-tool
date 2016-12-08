#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "projective.h"

int main(int argc, char** argv) {

  Display* disp = XOpenDisplay(NULL);
  if (NULL == disp) {

    exit(0);
  }
  int screen = DefaultScreen(disp);

  int width=600;
  int height = 600;

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
  xpdata_t diffs[4][6] =
    {
      {1.0/3.0, 1.0/3.0, 0, 0, 0, 0},
      {1.0/3.0, 1.0/3.0, 1.0/3.0, 0, M_PI/3, M_PI/3},
      {1.0/3.0, 1.0/3.0, 0.5, 0.866/3.0, -M_PI/3, -M_PI/3},
      {1.0/3.0, 1.0/3.0, 2.0/3.0, 0, 0, 0}
    };
  x = diffs[0][0];
  y = diffs[0][1];

  int ssc = 500;

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
        bcnt++;
	{
	  int xo = rand()%ssc;
	  int yo = rand()%ssc;
	  int ss = rand()%ssc;

	  unsigned long colors[3] = { 0xaa3300, 0x339900, 0xaa5511 };
          XSetForeground(disp, gc, colors[rand()%3]);

	  double rot = (rand()%360) * 2 * M_PI/360;

	  for (int ii=0; ii<10000; ii++) {
	    rind = rand()%4;

	    xpdata_t* aff = diffs[rind];
	    xpdata_t tmp = aff[0]*cos(aff[4]) * x + aff[1]*-sin(aff[5])*y + aff[2];
	    y            = aff[0]*sin(aff[4]) * x + aff[1]* cos(aff[5])*y + aff[3];
	    x = tmp;

	    double rot2 = 0;
	    for (int ii=0; ii<6; ii++) {
	      xpdata_t x2 = cos(rot2) * (x-0.5) - sin(rot2) * y;
	      xpdata_t y2 = sin(rot2) * (x-0.5) + cos(rot2) * y + sqrt(0.75);
	      rot2 += M_PI/3.0;
	      XDrawPoint(disp, win, gc,
			 round(ss*(cos(rot) * x2 - sin(rot) * y2)) + xo,
			 round(ss*(sin(rot) * x2 + cos(rot) * y2)) + yo);
	    }
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
