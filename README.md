Radium Window Manager
============================

Requirements
------------
In order to build radium you need the Xlib header files.

Testing
-------
    Xephyr :1 -screen 1280x720 -ac -br -noreset &

    DISPLAY=:1 ./build/radium

Installation
------------
Edit config.mk to match your local setup (radium is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install radium (if
necessary as root):

    make clean install


Running radium
-----------
Add the following line to your .xinitrc to start radium using startx:

    exec radium