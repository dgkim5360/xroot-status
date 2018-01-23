xroot-status
============

.. image:: screenshot.png

This program ceaselessly puts the various system information into the
name of the root window in X with a regular time interval. The provided
information includes:

1. Datetime
2. Battery status/capacity (%)
3. Backlight brightness (%)
4. Audio status/volume (%)
5. Connected wireless SSID (provided)
6. Available disk volume (GB)
7. Available memory (MB)

Additionally it contains somewhat "responsive" unicode characters to
represent the information.


Installation
------------

1. Clone it,
2. Sudo make clean install
3. Use $(DESTDIR)$(PREFIX)/bin/xroot-status for whatever you want.
   In my case I need this program for my laptop
   (Lenovo Ideapad 500S-14ISK) running Arch Linux + dwm.


License
-------

Please see the COPYING file.
