XEPHYR=$(whereis -b Xephyr | cut -f2 -d' ')
xinit ./xinitrc -- \
    "$XEPHYR" \
        :100 \
        -ac \
	-fullscreen \
        -host-cursor

#        -screen 800x600 \

