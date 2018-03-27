#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <linux/wireless.h>
#include <alsa/asoundlib.h>

#include "config.h"

static char localbuf[MAXBUF/2];

const char *status_datetime(const char *fmt) {
  time_t t = time(NULL);
  struct tm *tm_localtime = localtime(&t);

  strftime(localbuf, sizeof(localbuf),
           FORMAT_DATETIME, tm_localtime);
  return localbuf;
}

const char *status_battery() {
  FILE *battery_fp = fopen(SYS_BATTERY_CAPACITY, "r");
  if (!battery_fp)
    return NULL;

  int capacity;
  fscanf(battery_fp, "%i", &capacity);
  fclose(battery_fp);

  sprintf(localbuf, "BAT%d%%", capacity);
  return localbuf;
}

const char *status_backlight() {
  FILE *max_fp = fopen(SYS_BACKLIGHT_MAX, "r"),
       *now_fp = fopen(SYS_BACKLIGHT_NOW, "r");
  int max, now, p;
  fscanf(max_fp, "%i", &max);
  fscanf(now_fp, "%i", &now);

  p = now*100/max;
  sprintf(localbuf, "LIGHT%d%%", p);

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
    sprintf(localbuf, "MUTED");
    return localbuf;
  }

  /* Check the master volume */
  snd_ctl_elem_id_set_name(id, "Master Playback Volume");
  elem = snd_hctl_find_elem(hctl, id);

  snd_ctl_elem_value_alloca(&ctl);
  snd_ctl_elem_value_set_id(ctl, id);

  snd_hctl_elem_read(elem, ctl);
  volume = (int)snd_ctl_elem_value_get_integer(ctl, 0)*100/MAXVOLUME;


  snd_hctl_close(hctl);
  sprintf(localbuf, "SOUND%d%%", volume);
  return localbuf;
}

const char *status_wifi_ssid() {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0), offset;
  struct iwreq wreq;

  memset(&wreq, 0, sizeof(struct iwreq));
  wreq.u.essid.length = IW_ESSID_MAX_SIZE+1;
  snprintf(wreq.ifr_name, sizeof(wreq.ifr_name),
           "%s", INTERFACE_WIRELESS);

  if (sockfd == -1) {
    sprintf(localbuf, "ETHERNET");
    return localbuf;
  }

  offset = sprintf(localbuf, "WIRELESS:");
  wreq.u.essid.pointer = localbuf+offset;
  if (ioctl(sockfd, SIOCGIWESSID, &wreq) == -1) {
    close(sockfd);
    localbuf[offset] = '\0';
    return localbuf;
  }
  close(sockfd);

  if (strcmp(localbuf+offset, "") == 0)
    sprintf(localbuf+offset, "IDLE");
  return localbuf;
}

const char *status_memory_available() {
  FILE *mem_fp;
  long mem_avail;
  int nscan;

  if (!(mem_fp = fopen(PROC_MEMINFO, "r")))
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
  sprintf(localbuf, "SSD+%ldGB",
          (fs.f_bsize/1024)*fs.f_bavail/1024/1024);
  return localbuf;
}
