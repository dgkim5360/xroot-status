#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>

#define MAXBUF 1024

const char *status_datetime(const char *);
int status_battery();

int main(int argc, char *argv[]) {
  Display *dpy = XOpenDisplay(NULL);
  const struct timespec sleeptime = {1, 0L};
  size_t len;
  char status[MAXBUF];

  if (!dpy) {
    fprintf(stderr, "ERROR: cannot open display\n");
      return 1;
  }

  while (1) {
    len = 0;
    len += snprintf(status, sizeof(status),
                    "\u23F0%s ",
                    status_datetime("%T%z %a %F"));
    len += snprintf(status+len, sizeof(status)-len,
                    "\U0001F50B%d%% ",
                    status_battery());

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

int status_battery() {
  FILE *fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
  if (!fp)
    exit(1);

  int capacity;
  fscanf(fp, "%i", &capacity);
  fclose(fp);
  return capacity;
}
