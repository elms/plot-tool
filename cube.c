#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define TargetDim 2


typedef float xpdata_t;

static int applyAffine(xpdata_t* res,
                       xpdata_t* pnt, const size_t nDim, xpdata_t* tAffine) {
  xpdata_t* tmp = malloc(sizeof(xpdata_t) * nDim);

  for (size_t ii=0; ii < nDim; ii++) {
    tmp[ii] = pnt[ii] + tAffine[ii];
  }

  for (size_t ii=0; ii < nDim; ii++) {
    res[ii] = 0;
    for (size_t jj=0; jj < nDim; jj++) {
      // ii+1 to step over the offset in the affine transform
      res[ii] += tmp[jj] * tAffine[jj + (ii+1) * nDim];
    }
  }

  free(tmp);
  return 0;
}

static int projectToX(xpdata_t* pnts,
                      xpdata_t* vals, const size_t nDim, const size_t nCnt,
                      xpdata_t* tAffine) {

  for (size_t ii=0; ii<nCnt; ii++) {
    applyAffine(pnts + ii*nDim, vals + ii*nDim, nDim, tAffine);
    printf("%f %f %f\t %f\n", pnts[ii*nDim + 0], pnts[ii*nDim + 1], pnts[ii*nDim + 2],
           sqrt(pnts[ii*nDim + 0]*pnts[ii*nDim + 0] + pnts[ii*nDim + 1]*pnts[ii*nDim + 1] +  pnts[ii*nDim + 2]*pnts[ii*nDim + 2]));
  }

  return 0;
}

static int perspective(XPoint* pnts,
                       xpdata_t* vals, const size_t nDim, const size_t nCnt) {
  xpdata_t* tmp = malloc(sizeof(xpdata_t) * TargetDim);
  xpdata_t offset = 30;

  for (size_t ii=0; ii<nCnt; ii++) {
    tmp[0] = 100*offset*(vals[ii*nDim + 0])/(vals[ii*nDim + 2] + offset);
    tmp[1] = 100*offset*(vals[ii*nDim + 1])/(vals[ii*nDim + 2] + offset);

    pnts[ii].x = round(tmp[0]);
    pnts[ii].y = round(tmp[1]);

    printf("pnt: %f %f %d %d\n", tmp[0], tmp[1], pnts[ii].x, pnts[ii].y);
  }

  free(tmp);
  return 0;
}


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

  perspective(xpnts, cube, ndim, npnts);
  for (size_t ii=0; ii<npnts; ii++) {
    xpnts[ii].x += 200;
    xpnts[ii].y += 200;
  }

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

        perspective(xpnts, pnts, ndim, npnts);
        for (size_t ii=0; ii<npnts; ii++) {
          xpnts[ii].x = 200 + xpnts[ii].x;
          xpnts[ii].y = 200 + xpnts[ii].y;
        }

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
