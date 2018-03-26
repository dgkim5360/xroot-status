#ifndef _XROOTSTATUS_CONFIG
#define _XROOTSTATUS_CONFIG

#define MAXBUF 1024
#define SLEEP_INTERVAL_SEC 5
#define SLEEP_INTERVAL_NANOSEC 0L  // 1 sec == 1,000,000,000 nsec
#define MAXVOLUME 87

#define FORMAT_DATETIME "%R%z %a %b %d %Y"
#define SYS_BATTERY_STATUS "/sys/class/power_supply/BAT0/status"
#define SYS_BATTERY_CAPACITY "/sys/class/power_supply/BAT0/capacity"
#define SYS_BACKLIGHT_MAX "/sys/class/backlight/intel_backlight/max_brightness"
#define SYS_BACKLIGHT_NOW "/sys/class/backlight/intel_backlight/brightness"
#define INTERFACE_WIRELESS "wlp2s0"
#define PROC_MEMINFO "/proc/meminfo"

const char *status_datetime();
const char *status_battery();
const char *status_backlight();
const char *status_audio();
const char *status_wifi_ssid();
const char *status_memory_available();
const char *status_disk_available();
#endif
