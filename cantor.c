#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "projective.h"

int main(int argc, char** argv) {

  Display* disp = XOpenDisplay(NULL);
  int screen = DefaultScreen(disp);

  int width=800;
  int height = 400;

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

  xpdata_t x = 1;
  xpdata_t y = 0.5;
  int rind = 0;
  xpdata_t diffs[2][2] = { {0, 1}, {3, 1} };
  xpdata_t sc = 1.0/3.0;
  int ssc = 500;

  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
          unsigned long color = 0xff;
          XSetForeground(disp, gc, color);

          break;
        }
      case ButtonPress:
      case KeyPress:
        bcnt++;

        for (int ii=0; ii<1000; ii++) {
          rind = rand()%2;

          x = sc * (x + diffs[rind][0]);

          XDrawPoint(disp, win, gc, round(ssc*x), round(ssc*y));
        }

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
