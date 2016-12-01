#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>

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
  }

  return 0;
}

static int perspective(XPoint* pnts,
                       xpdata_t* vals, const size_t nDim, const size_t nCnt) {
  xpdata_t* tmp = malloc(sizeof(xpdata_t) * TargetDim);

  for (size_t ii=0; ii<nCnt; ii++) {
    tmp[0] = 10*(vals[ii*nDim + 0])/(vals[ii*nDim + 2]+2);
    tmp[1] = 10*(vals[ii*nDim + 1])/(vals[ii*nDim + 2]+2);

    pnts[ii].x = round(tmp[0]);
    pnts[ii].y = round(tmp[1]);

    printf("pnt: %f %f %d %d\n", tmp[0], tmp[1], pnts[ii].x, pnts[ii].y);
  }

  free(tmp);
  return 0;
}


int main(int argc, char** argv) {

  Display* disp = XOpenDisplay(NULL);
  if (NULL == disp) {

    exit(0);
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
  static size_t bcnt = 0;

  xpdata_t x = 1;
  xpdata_t y = 1;
  int rind = 0;
  xpdata_t diffs[3][2] = { {0, 1}, {0.5, 0}, {1, 1} };
  xpdata_t sc = 0.5;
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

        for (int ii=0; ii<1000; ii++) {
          rind = rand()%3;

          x = sc * (x + diffs[rind][0]);
          y = sc * (y + diffs[rind][1]);

          //printf("xy: %f %f\n", x, y);

          XDrawPoint(disp, win, gc, round(ssc*x), round(ssc*y));
        }

        printf("button: %zu\n", bcnt);

        if (ev.type == KeyPress) {
          printf("keypress: %#x\n", ev.xkey.keycode);
          if (0x14 == ev.xkey.keycode) {
            cont = 0;
          }
        }
        break;
    }
  }

  XDestroyWindow(disp, win);
  XCloseDisplay(disp);
}
