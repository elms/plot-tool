#include <X11/Xlib.h>

int main(int argc, char** argv) {

  Display* disp = XOpenDisplay(NULL);
  int screen = DefaultScreen(disp);

  int width=800;
  int height = 600; 

  unsigned long black = BlackPixel(disp, screen);
  unsigned long white = WhitePixel(disp, screen);

  Window win = XCreateSimpleWindow(disp, RootWindow(disp, screen),
				   0, 0, width, height, 0, white, black);


  XMapWindow(disp, win);
  GC gc = XCreateGC(disp, win, 0, NULL);
  XDrawLine(disp, win, gc, 0, 0, 100, 200);
}
