#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>

#include "config.h"

int main(int argc, char *argv[]) {
  Display *dpy = XOpenDisplay(NULL);
  const struct timespec sleeptime = {
    SLEEP_INTERVAL_SEC,
    SLEEP_INTERVAL_NANOSEC
  };
  size_t len;
  char status[MAXBUF];

  if (!dpy) {
    fprintf(stderr, "ERROR: cannot open display\n");
    return 1;
  }

  while (1) {
    len = 0;
    len += snprintf(status+len, sizeof(status)-len,
                    " ");
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ",
                    status_memory_available());
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ",
                    status_disk_available());
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ",
                    status_wifi_ssid());
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ",
                    status_audio());
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ",
                    status_backlight());
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ", 
                    status_battery());
    len += snprintf(status+len, sizeof(status)-len,
                    "%s ",
                    status_datetime());

    XStoreName(dpy, DefaultRootWindow(dpy), status);
    XSync(dpy, 0);

    nanosleep(&sleeptime, NULL);
  }

  XCloseDisplay(dpy);
  return 0;
}
