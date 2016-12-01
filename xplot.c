#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "projective.h"

static xpdata_t* init(size_t* npnts, size_t* ndim) {

  xpdata_t* pnts = NULL;

  xpdata_t cube[24 * 3] =
      {
        +1, +1, +1,
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

  *npnts = 24;
  *ndim = 3;

  pnts = calloc(*npnts * *ndim, sizeof(xpdata_t));
  memcpy(pnts, cube, *npnts * *ndim * sizeof(xpdata_t));

  return pnts;
}

int main(int argc, char** argv) {

  Display* disp = XOpenDisplay(NULL);
  if (NULL == disp) {
    fprintf(stderr, "unable to open display\n");
    exit(1);
  }

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

  size_t npnts = 0;
  size_t ndim = 0;

  xpdata_t* pnts = NULL;

  pnts = init(&npnts, &ndim);

  XPoint* xpnts = calloc(npnts, sizeof(pnts));

  // offset vector + projecting matrix
  xpdata_t* proj1 = calloc(ndim * (1 + ndim), sizeof(xpdata_t));

  proj1[0] = 0;
  proj1[1] = 0;
  proj1[2] = 0;

  proj1[3]  = cos(0.05) * cos(0.1);
  proj1[4]  = cos(0.05) * sin(0.1);
  proj1[5]  = sin(0.05);
  proj1[6]  = -sin(0.1);
  proj1[7]  = cos(0.1);
  proj1[8]  = 0;
  proj1[9]  = -sin(0.05) * cos(0.1);
  proj1[10] = -sin(0.05) * sin(0.1);
  proj1[11] = cos(0.05);

  xpdata_t offset = 30;
  size_t xo = 200;
  size_t yo = 200;

  perspective(xpnts, pnts, ndim, npnts, offset, xo, yo);
  //projectToX(pnts, pnts, ndim, npnts, NULL);

  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
            unsigned long color = 0xff;
          XSetForeground(disp, gc, color);
          XDrawLine(disp, win, gc, 0, 0, 100, 200);

          XDrawLines(disp, win, gc, xpnts, npnts, CoordModeOrigin);

          break;
        }
      case ButtonPress:
      case KeyPress:

        projectToX(pnts, pnts, ndim, npnts, proj1);
        perspective(xpnts, pnts, ndim, npnts, offset, xo, yo);
        XDrawLines(disp, win, gc, xpnts, npnts, CoordModeOrigin);

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
