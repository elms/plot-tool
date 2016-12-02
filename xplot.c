#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "xplot.h"
#include "projective.h"
#include "plotting.h"

#if 0
int readdata(const char* fname, xpdata_t* data, size_t* pdim, size_t* num) {
  size_t cnt = 0;
  size_t dim = 0;
  struct stat sb;

  int df = open(fname, O_RDONLY);

  if (fstat(fd, &sb) == -1) {
    //handle_error("fstat");
  }

  data = calloc(1, bufsb.st_size);

  if (-1 == read(fd, buf, sb.st_size)) {
    //
  }

  return 0;
}
#endif


void plot() { //xpdata_t* data, size_t npnts) {
    Display* disp = XOpenDisplay(NULL);
  if (NULL == disp) {
    fprintf(stderr, "unable to open display\n");
    exit(1);
  }

  int screen = DefaultScreen(disp);

  int width = 800;
  int height = 400;

  unsigned long black = BlackPixel(disp, screen);
  unsigned long white = WhitePixel(disp, screen);

  Window win = XCreateSimpleWindow(disp, RootWindow(disp, screen),
				   0, 0, width, height, 0, white, black);

  XSelectInput(disp, win, ExposureMask | KeyPressMask | ButtonPressMask);
  XMapWindow(disp, win);

  GC gc = XCreateGC(disp, win, 0, NULL);

  XMapRaised(disp, win);

#define NTEST 1000
  size_t npnts = NTEST;
  size_t xnpnts = 0;
  xpdata_t data[2*NTEST];
  for (int ii=0; ii<2*NTEST; ii++) {
    data[ii] = rand()%10;
  }
  data[513] = 20;
  XPoint* xpnt = calloc(npnts, sizeof(XPoint));
  xpgeo_t geo =
      {
     .width  = 200,
     .height = 200
      };
  plotmax(xpnt, data, npnts, &geo, -1, 0, &xnpnts);


  XEvent ev;
  int cont = 1;
  while (cont) {
    XNextEvent(disp, &ev);
    switch (ev.type) {
      case Expose:
        {
          unsigned long color = 0x00ff00;
          XSetForeground(disp, gc, color);

          XDrawLines(disp, win, gc, xpnt, xnpnts, CoordModeOrigin);

          break;
        }
      case ButtonPress:
      case KeyPress:
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

int main(int argc, char** argv) {


  //  readdata();

  plot();


}
