#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "projective.h"

static xpdata_t* init(size_t* npnts, size_t* ndim) {

  xpdata_t* pnts = NULL;

  xpdata_t shape[8 * 3] =
      {
        1.,  0.,  0.,
        -0.33333333,  0.94280904,  0.,
        -0.33333333, -0.47140452,  0.81649658,
        1.,  0.,  0.,
        -0.33333333, -0.47140452, -0.81649658,
        -0.33333333,  0.94280904,  0.,
        -0.33333333, -0.47140452,  0.81649658,
        -0.33333333, -0.47140452, -0.81649658
      };

  *npnts = 8;
  *ndim = 3;

  pnts = calloc(*npnts * *ndim, sizeof(xpdata_t));
  memcpy(pnts, shape, *npnts * *ndim * sizeof(xpdata_t));

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

  double rad1 = 0.1;
  double rad2 = 0.06;

  proj1[3]  = cos(rad2) * cos(rad1);
  proj1[4]  = cos(rad2) * sin(rad1);
  proj1[5]  = sin(rad2);
  proj1[6]  = -sin(rad1);
  proj1[7]  = cos(rad1);
  proj1[8]  = 0;
  proj1[9]  = -sin(rad2) * cos(rad1);
  proj1[10] = -sin(rad2) * sin(rad1);
  proj1[11] = cos(rad2);

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

        XClearWindow(disp, win);
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
