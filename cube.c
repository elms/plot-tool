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
  size_t npnts = 24;
  size_t ndim = 3;

  xpdata_t cube[24 * 3] =
      { +1, +1, +1,
        +1, +1, -1,
        +1, +1, +1,
        +1, -1, +1,
        +1, +1, +1,
        -1, +1, +1,

        -1, +1, -1,
        -1, +1, +1,
        -1, +1, -1,
        -1, -1, -1,
        -1, +1, -1,
        +1, +1, -1,

        +1, -1, -1,
        +1, -1, +1,
        +1, -1, -1,
        +1, +1, -1,
        +1, -1, -1,
        -1, -1, -1,

        -1, -1, +1,
        -1, -1, -1,
        -1, -1, +1,
        -1, +1, +1,
        -1, -1, +1,
        +1, -1, +1

        };
  XPoint* xpnts = calloc(npnts, sizeof(XPoint));
  xpdata_t* pnts = calloc(npnts*ndim, sizeof(xpdata_t));

  static size_t bcnt = 0;

  // offset vector + projecting matrix
  xpdata_t* proj1 = calloc(ndim * (1 + ndim), sizeof(xpdata_t));

  proj1[0] = 0;
  proj1[1] = 0;
  proj1[2] = 0;

  proj1[3] = 1;
  proj1[4] = 1;
  proj1[5] = 0;

  proj1[6] = -1;
  proj1[7] = 1;
  proj1[8] = 0;

  proj1[9] = 0;
  proj1[10] = 0;
  proj1[11] = 1;

  xpdata_t offset = 30;
  size_t xo = 200;
  size_t yo = 200;

  perspective(xpnts, cube, ndim, npnts, offset, xo, yo);

  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
          printf("expose\n");
          unsigned long color = 0xff;
          XSetForeground(disp, gc, color);

          XDrawLines(disp, win, gc, xpnts, npnts, CoordModeOrigin);

          break;
        }
      case ButtonPress:
      case KeyPress:
#if 1
        proj1[3]  = cos(0.05*bcnt) * cos(0.1*bcnt);
        proj1[4]  = cos(0.05*bcnt) * sin(0.1*bcnt);
        proj1[5]  = sin(0.05*bcnt);
        proj1[6]  = -sin(0.1*bcnt);
        proj1[7]  = cos(0.1*bcnt);
        proj1[8]  = 0;
        proj1[9]  = -sin(0.05*bcnt) * cos(0.1*bcnt);
        proj1[10] = -sin(0.05*bcnt) * sin(0.1*bcnt);
        proj1[11] = cos(0.05*bcnt);
#else
        proj1[3]  = cos(0.1*bcnt);
        proj1[4]  = 0;
        proj1[5]  = sin(0.1*bcnt);
        proj1[6]  = 0;
        proj1[7]  = 1;
        proj1[8]  = 0;
        proj1[9]  = -sin(0.1*bcnt);
        proj1[10] = 0;
        proj1[11] = cos(0.1*bcnt);
#endif
        projectToX(pnts, cube, ndim, npnts, proj1);

        perspective(xpnts, pnts, ndim, npnts, offset, xo, yo);

        bcnt++;

        XClearWindow(disp, win);
        XDrawLines(disp, win, gc, xpnts, npnts, CoordModeOrigin);

        printf("button: %zu\n", bcnt);

        if (ev.type = KeyPress) {
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
