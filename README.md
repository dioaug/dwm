dwm - dynamic window manager
============================
dwm is an extremely fast, small, and dynamic window manager for X.

Patches
-------
- [smartborders](https://dwm.suckless.org/patches/smartborders/dwm-smartborders-6.2.diff)
- [pertag](https://dwm.suckless.org/patches/pertag/dwm-pertag_with_sel-20231003-9f88553.diff)
- [status2d](https://dwm.suckless.org/patches/status2d/dwm-status2d-systray-6.4.diff)
- [statuscmd](https://dwm.suckless.org/patches/statuscmd/dwm-statuscmd-status2d-20210405-60bb3df.diff)
- [systrayiconsize](https://gitlab.com/-/snippets/2184056)
- [alwayscenter](https://dwm.suckless.org/patches/alwayscenter/dwm-alwayscenter-20200625-f04cac6.diff)
- [focusonclick](https://dwm.suckless.org/patches/focusonclick/dwm-focusonclick-20200110-61bb8b2.diff)
- [fixborders](https://dwm.suckless.org/patches/alpha/dwm-fixborders-6.2.diff)
- [tilegap](https://dwm.suckless.org/patches/tilegap/dwm-tilegap-6.4.diff)
- [focusonnetactive](https://dwm.suckless.org/patches/focusonnetactive/dwm-focusonnetactive-6.2.diff)
- [ewmhtags](https://dwm.suckless.org/patches/ewmhtags/dwm-ewmhtags-6.2.diff)
- [actualfullscreen](https://dwm.suckless.org/patches/actualfullscreen/dwm-actualfullscreen-20211013-cb3f58a.diff)
- [scratchpads](https://dwm.suckless.org/patches/scratchpads/dwm-scratchpads-20200414-728d397b.diff)

Requirements
------------
In order to build dwm you need the Xlib header files.


Installation
------------
Edit config.mk to match your local setup (dwm is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install dwm (if
necessary as root):

    make clean install


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

In order to display status info in the bar, you can do something
like this in your .xinitrc:

    while xsetroot -name "`date` `uptime | sed 's/.*,//'`"
    do
    	sleep 1
    done &
    exec dwm


Configuration
-------------
The configuration of dwm is done by creating a custom config.h
and (re)compiling the source code.
