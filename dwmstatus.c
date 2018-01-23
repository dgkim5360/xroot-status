#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <linux/wireless.h>
#include <X11/Xlib.h>
#include <alsa/asoundlib.h>

#define MAXBUF 1024

const char *status_datetime(const char *);
const char *status_battery();
const char *status_backlight();
const char *status_audio();
const char *status_wifi_ssid(const char *);
const char *status_memory_available();
const char *status_disk_available();

int main(int argc, char *argv[]) {
  Display *dpy = XOpenDisplay(NULL);
  const struct timespec sleeptime = {5, 0L};
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
                    status_wifi_ssid("wlp2s0"));
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
                    status_datetime("%R%z \U0001F4C6%a %b %d %Y"));

    XStoreName(dpy, DefaultRootWindow(dpy), status);
    XSync(dpy, 0);

    nanosleep(&sleeptime, NULL);
  }

  XCloseDisplay(dpy);
  return 0;
}

static char localbuf[MAXBUF/2];
const static char *clocks[12] = {
  "\U0001F55B%s", "\U0001F550%s", "\U0001F551%s", "\U0001F552%s",
  "\U0001F553%s", "\U0001F554%s", "\U0001F555%s", "\U0001F556%s",
  "\U0001F557%s", "\U0001F558%s", "\U0001F559%s", "\U0001F55A%s"};

const char *status_datetime(const char *fmt) {
  time_t t = time(NULL);
  struct tm *tm_localtime = localtime(&t);
  char strft[50];
  int hour;

  if (strftime(strft, sizeof(localbuf), fmt, tm_localtime) == 0)
    return NULL;
  hour = tm_localtime->tm_hour%12;
  sprintf(localbuf, clocks[hour], strft);
  return localbuf;
}

const char *status_battery() {
  FILE *charging_fp = fopen("/sys/class/power_supply/BAT0/status", "r"),
       *battery_fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
  if (!charging_fp && !battery_fp)
    return NULL;

  int capacity;
  char c = fgetc(charging_fp);
  fscanf(battery_fp, "%i", &capacity);
  fclose(charging_fp);
  fclose(battery_fp);

  if (c == 'C') {
    sprintf(localbuf, "\U0001F50C%d%%", capacity);
    return localbuf;
  }
  if (capacity > 80)
    sprintf(localbuf, "\uA70D%d%%", capacity);
  else if (capacity > 60)
    sprintf(localbuf, "\uA70E%d%%", capacity);
  else if (capacity > 40)
    sprintf(localbuf, "\uA70F%d%%", capacity);
  else if (capacity > 20)
    sprintf(localbuf, "\uA710%d%%", capacity);
  else
    sprintf(localbuf, "\uA711%d%%", capacity);
  return localbuf;
}

const char *status_backlight() {
  FILE *max_fp = fopen("/sys/class/backlight/intel_backlight/max_brightness",
                       "r"),
       *now_fp = fopen("/sys/class/backlight/intel_backlight/brightness", "r");
  int max, now, p;
  fscanf(max_fp, "%i", &max);
  fscanf(now_fp, "%i", &now);

  p = now*100/max;
  sprintf(localbuf, (p > 50)? "\U0001F506%d%%" : "\U0001F505%d%%", p);

  fclose(max_fp);
  fclose(now_fp);
  return localbuf;
}

const char *status_audio() {
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
  /* Check the switch is on */
  snd_ctl_elem_id_set_name(id, "Master Playback Switch");
  elem = snd_hctl_find_elem(hctl, id);

  snd_ctl_elem_value_alloca(&ctl);
  snd_ctl_elem_value_set_id(ctl, id);

  snd_hctl_elem_read(elem, ctl);
  if (!snd_ctl_elem_value_get_boolean(ctl, 0)) {
    sprintf(localbuf, "\U0001F507");
    return localbuf;
  }

  /* Check the master volume */
  snd_ctl_elem_id_set_name(id, "Master Playback Volume");
  elem = snd_hctl_find_elem(hctl, id);

  snd_ctl_elem_value_alloca(&ctl);
  snd_ctl_elem_value_set_id(ctl, id);

  snd_hctl_elem_read(elem, ctl);
  volume = (int)snd_ctl_elem_value_get_integer(ctl, 0);


  snd_hctl_close(hctl);
  if (volume > 50)
    sprintf(localbuf, "\U0001F50A%d%%", volume);
  else
    sprintf(localbuf, "\U0001F509%d%%", volume);
  return localbuf;
}

const char *status_wifi_ssid(const char *iface) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0), offset;
  struct iwreq wreq;

  memset(&wreq, 0, sizeof(struct iwreq));
  wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;
  snprintf(wreq.ifr_name, sizeof(wreq.ifr_name), "%s", iface);

  if (sockfd == -1)
    return "\u26C8";

  offset = sprintf(localbuf, "\U0001F4F6");
  wreq.u.essid.pointer = localbuf+offset;
  if (ioctl(sockfd, SIOCGIWESSID, &wreq) == -1) {
    close(sockfd);
    localbuf[offset] = '\0';
    return localbuf;
  }
  close(sockfd);

  if (strcmp(localbuf+offset, "") == 0)
    sprintf(localbuf+offset, "NOT CONNECTED");
  return localbuf;
}

const char *status_memory_available() {
  FILE *mem_fp;
  long mem_avail;
  int nscan;

  if (!(mem_fp = fopen("/proc/meminfo", "r")))
    exit(1);
  nscan = fscanf(mem_fp,
                "MemTotal: %ld kB\n"
                "MemFree: %ld kB\n"
                "MemAvailable: %ld kB\n",
                &mem_avail, &mem_avail, &mem_avail);
  fclose(mem_fp);

  mem_avail = (nscan == 3)? mem_avail/1024 : 0;
  sprintf(localbuf, "MEM+%dMB", (int)mem_avail);
  return localbuf;
}

const char *status_disk_available() {
  struct statvfs fs;

  if (statvfs("/", &fs) < 0)
    return NULL;
  sprintf(localbuf, "\U0001F4BF+%ldGB", (fs.f_bsize/1024)*fs.f_bavail/1024/1024);
  return localbuf;
}
