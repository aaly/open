XEPHYR=$(whereis -b Xephyr | cut -f2 -d' ')
gdb --args xinit ./xinitrc -- "$XEPHYR"  :100 -ac -screen 2000x1000  -resizeable -host-cursor
#xinit ./xinitrc -- "$XEPHYR"  :100 -ac -fullscreen  -screen 2000x1000 -resizeable -host-cursor

