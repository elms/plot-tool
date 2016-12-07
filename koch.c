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
      {0, 0, 1, 0, 0, 1},
      {0.3333, 0, 0.5, -0.866, 0.866, 0.5},
      {0.5, 0.3333*0.866, 0.5, 0.866, -0.866, 0.5},
      {0.6666, 0, 1, 0 , 0, 1} };
  x = diffs[0][0];
  y = diffs[0][1];
  xpdata_t sc = 0.3333;
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
	  int xo = 0;//rand()%ssc;
	  int yo = 100;//rand()%ssc;
	  int ss = ssc;//rand()%ssc;

	  unsigned long colors[3] = { 0xaa3300, 0x339900, 0xaa5511 };
          //XSetForeground(disp, gc, colors[rand()%3]);

	  double rot = 0;//(rand()%360) * 2 * M_PI/360;

	  for (int ii=0; ii<1000; ii++) {
	    rind = rand()%4;

	    xpdata_t tmp = sc*(diffs[rind][2]*x + diffs[rind][3]*y) + diffs[rind][0];
	    y            = sc*(diffs[rind][4]*x + diffs[rind][5]*y) + diffs[rind][1];
	    x = tmp;

	    XDrawPoint(disp, win, gc,
		       round(ss*(cos(rot) * x - sin(rot) * y)) + xo,
		       round(ss*(sin(rot) * x + cos(rot) * y)) + yo);
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
