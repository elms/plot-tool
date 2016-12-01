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

  for (size_t ii=0; ii < TargetDim; ii++) {
    res[ii] = 0;
    for (size_t jj=0; jj < nDim; jj++) {
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

  for (size_t ii=0; ii<nCnt; ii++) {
    applyAffine(tmp, vals + ii*nDim, nDim, tAffine);
    pnts[ii].x = tmp[0];
    pnts[ii].y = tmp[1];

    printf("pnt: %f %f %d %d\n", tmp[0], tmp[1], pnts[ii].x, pnts[ii].y);
  }

  free(tmp);
  return 0;
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
  XPoint* pnts = calloc(npnts, sizeof(pnts));
  static size_t bcnt = 0;

  // offset vector + projecting to TargetDim
  xpdata_t* proj1 = calloc(ndim * (1 + TargetDim), sizeof(xpdata_t));

  proj1[0] = 20;
  proj1[1] = 20;
  proj1[2] = 20;
  proj1[3] = 10 * cos(0.1);
  proj1[4] = 10 * sin(0.1);
  proj1[5] = 3;
  proj1[6] = 10 * -sin(0.1);
  proj1[7] = 10 * cos(0.1);
  proj1[8] = 3;

  projectToX(pnts, cube, ndim, npnts, proj1);

  if (1) {
    int min_kc;
    int max_kc;
    XDisplayKeycodes(disp, &min_kc, &max_kc);

    int cnt_kc = max_kc - min_kc + 1;
    int per = 0;
    KeySym* ks = XGetKeyboardMapping(disp, min_kc, cnt_kc, &per);

    for (int ii=0; ii < cnt_kc; ii++) {
        printf("%d\n", ii);
      for (int jj=0; jj< per; jj++) {
        printf("\t%d\t%#x\n", jj, ks[ii*per + jj]);
      }
    }

    XFree(ks);
  }
  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
          printf("expose\n");
          unsigned long color = 0xff;
          XSetForeground(disp, gc, color);
          XDrawLine(disp, win, gc, 0, 0, 100, 200);

          XDrawLines(disp, win, gc, pnts, npnts, CoordModeOrigin);

          break;
        }
      case ButtonPress:
        bcnt++;
        proj1[3] = 10 * cos(0.1*bcnt);
        proj1[4] = 10 * sin(0.1*bcnt);
        proj1[5] = 3;
        proj1[6] = 10 * -sin(0.1*bcnt);
        proj1[7] = 10 * cos(0.1*bcnt);
        proj1[8] = 3;
        projectToX(pnts, cube, ndim, npnts, proj1);
        XDrawLines(disp, win, gc, pnts, npnts, CoordModeOrigin);

        printf("button: %zu\n", bcnt);
        break;
      case KeyPress:
        {
          KeySym ksym = XLookupKeysym(&ev.xkey, 0);
          printf("keypress: %#x\t%#x\t%#x\n", ev.xkey.keycode, ev.xkey.state, ksym);
          {
            char string[25];
            int len;
            KeySym keysym;
            len = XLookupString(&ev.xkey, string, 25, &keysym, NULL);
            printf("%d %#x ", len, keysym, string);
            for (int ii=0; ii<len; ii++) {
              printf("%#x, ", string[ii]);
            }
            printf("\n");
          }
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
