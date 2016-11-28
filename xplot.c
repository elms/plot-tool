#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>

#define TargetDim 2


typedef float xpdata_t;

static int applyAffine(xpdata_t* res,
                       xpdata_t* pnt, const size_t nDim, xpdata_t* tAffine) {
  xpdata_t* tmp = malloc(sizeof(xpdata_t) * nDim);
  size_t ii, jj;

  for (ii=0; ii < nDim; ii++) {
    tmp[ii] = pnt[ii] + tAffine[ii];
  }

  for (ii=0; ii < TargetDim; ii++) {
    res[ii] = 0;
    for (jj=0; jj < nDim; jj++) {
      // ii+1 to step over the offset in the affine transform
      res[ii] += tmp[jj] * tAffine[jj + (ii+1) * nDim];
    }
  }

  free(tmp);
  return 0;
}

static int projectToX(XPoint* pnts,
                      xpdata_t* vals, const size_t nDim, const size_t nCnt,
                      xpdata_t* tAffine) {
  xpdata_t* tmp = malloc(sizeof(xpdata_t) * TargetDim);
  size_t ii;

  for (ii=0; ii<nCnt; ii++) {
    applyAffine(tmp, vals + ii*nDim, nDim, tAffine);
    pnts[ii].x = tmp[0];
    pnts[ii].y = tmp[1];
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
  size_t npnts = 8;
  size_t ndim = 3;

  xpdata_t cube[8*3] =
      { +1, +1, +1,
        +1, +1, -1,
        +1, -1, +1,
        +1, -1, -1,
        -1, +1, +1,
        -1, +1, -1,
        -1, -1, +1,
        -1, -1, -1 };
  XPoint* pnts = calloc(npnts, sizeof(pnts));
  static size_t bcnt = 0;

  // offset vector + projecting to TargetDim
  xpdata_t* proj1 = calloc(ndim * (1 + TargetDim), sizeof(xpdata_t));

  proj1[0] = 50;
  proj1[1] = 50;
  proj1[2] = 50;
  proj1[3] = 20 * cos(0.1);
  proj1[4] = 20 * sin(0.1);
  proj1[5] = 1;
  proj1[6] = 20 * -sin(0.1);
  proj1[7] = 20 * cos(0.1);
  proj1[8] = 1;

  //projectToX(pnts, cube, ndim, npnts, proj1);

  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
          unsigned long color = 0xff;
          XSetForeground(disp, gc, color);
          XDrawLine(disp, win, gc, 0, 0, 100, 200);

          //XDrawLines(disp, win, gc, pnts, npnts, CoordModeOrigin);

          break;
        }
      case ButtonPress:
        bcnt++;
        proj1[3] = 20 * cos(0.1*bcnt);
        proj1[4] = 20 * sin(0.1*bcnt);
        proj1[5] = 1;
        proj1[6] = 20 * -sin(0.1*bcnt);
        proj1[7] = 20 * cos(0.1*bcnt);
        proj1[8] = 1;
        //projectToX(pnts, cube, ndim, npnts, proj1);

        break;
      case KeyPress:
        cont = 0;
        break;
    }
  }

  XDestroyWindow(disp, win);
  XCloseDisplay(disp);
}
