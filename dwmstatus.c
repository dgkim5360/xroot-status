#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>

#define MAXBUF 1024

const char *status_datetime(const char *);

int main(int argc, char *argv[]) {
  Display *dpy = XOpenDisplay(NULL);
  const struct timespec sleeptime = {1, 0L};
  char status[MAXBUF];

  if (!dpy) {
    fprintf(stderr, "ERROR: cannot open display\n");
      return 1;
  }

  while (1) {
    snprintf(status, sizeof(status), "\u23F0 %s ", status_datetime("%T %z %a %F"));

    XStoreName(dpy, DefaultRootWindow(dpy), status);
    XSync(dpy, 0);

    nanosleep(&sleeptime, NULL);
  }

  XCloseDisplay(dpy);
  return 0;
}

static char localbuf[MAXBUF];

const char *status_datetime(const char *fmt) {
  time_t t = time(NULL);

  if (strftime(localbuf, sizeof(localbuf), fmt, localtime(&t)) == 0)
    return NULL;
  return localbuf;
}
