#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <X11/Xlib.h>
#include <alsa/asoundlib.h>

#define MAXBUF 1024

const char *status_datetime(const char *);
int status_battery();
int status_audio();
const char *status_wifi_ssid(const char *);

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
    len += snprintf(status+len, sizeof(status)-len,
                    "\U0001F4F6%s ",
                    status_wifi_ssid("wlp2s0"));
    len += snprintf(status+len, sizeof(status)-len,
                    "\U0001F509%d%% ",
                    status_audio());
    len += snprintf(status+len, sizeof(status)-len,
                    "\U0001F50B%d%% ",
                    status_battery());
    len += snprintf(status+len, sizeof(status)-len,
                    "\U0001F558%s ",
                    status_datetime("%T%z %a %F"));

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
  FILE *battery_fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
  if (!battery_fp)
    exit(1);

  int capacity;
  fscanf(battery_fp, "%i", &capacity);
  fclose(battery_fp);
  return capacity;
}

int status_audio() {
  int volume;
  snd_hctl_t *hctl;
  snd_ctl_elem_id_t *id;
  snd_ctl_elem_value_t *ctl;
  snd_hctl_elem_t *elem;

  /* To find card and subdevice */
  snd_hctl_open(&hctl, "hw:0", 0);
  snd_hctl_load(hctl);
  snd_ctl_elem_id_alloca(&id);
  snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_MIXER);

  /* amixer controls */
  snd_ctl_elem_id_set_name(id, "Master Playback Volume");
  elem = snd_hctl_find_elem(hctl, id);

  snd_ctl_elem_value_alloca(&ctl);
  snd_ctl_elem_value_set_id(ctl, id);

  snd_hctl_elem_read(elem, ctl);
  volume = (int)snd_ctl_elem_value_get_integer(ctl, 0);

  snd_hctl_close(hctl);
  return volume;
}

const char *status_wifi_ssid(const char *iface) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  struct iwreq wreq;

  memset(&wreq, 0, sizeof(struct iwreq));
  wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;
  snprintf(wreq.ifr_name, sizeof(wreq.ifr_name), "%s", iface);

  if (sockfd == -1)
    return NULL;

  wreq.u.essid.pointer = localbuf;
  if (ioctl(sockfd, SIOCGIWESSID, &wreq) == -1) {
    close(sockfd);
    return NULL;
  }
  close(sockfd);

  if (strcmp(localbuf, "") == 0)
    return NULL;
  return localbuf;
}
