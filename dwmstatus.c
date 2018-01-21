#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>

int main(int argc, char *argv[]) {
  Display *dpy = XOpenDisplay(NULL);
  const struct timespec sleeptime = {1, 0L};

  if (!dpy) {
    fprintf(stderr, "ERROR: cannot open display\n");
     return 1;
  }

  while (1) {
    XStoreName(dpy, DefaultRootWindow(dpy), "hello!");
    XSync(dpy, 0);

    nanosleep(&sleeptime, NULL);
  }

  XCloseDisplay(dpy);
  return 0;
}
