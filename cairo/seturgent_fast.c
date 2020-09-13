#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(void)
{
    const char* window_idstring = getenv("WINDOWID");
    if( window_idstring == NULL )
    {
        fprintf(stderr, "No WINDOWID set\n");
        return 1;
    }
    Window w = atoi(window_idstring);
    if( w <= 0 )
    {
        fprintf(stderr, "Couldn't parse window id '%s'\n",
                window_idstring);
        return 1;
    }


    Display* display;
    const char* displaystring = getenv("DISPLAY");
    if( displaystring == NULL )
    {
        fprintf(stderr, "No DISPLAY set\n");
        return 1;
    }

    display = XOpenDisplay(displaystring);
    if( display == NULL )
    {
        fprintf(stderr, "Couldn't open display '%s\n", displaystring);
        return 1;
    }

    XWMHints* hints = XGetWMHints(display, w);
    if( hints == NULL )
    {
        fprintf(stderr, "Couldn't retrieve hints\n");
        return 1;
    }

    hints->flags |= XUrgencyHint;
//    hints->flags = 0;

    XSetWMHints(display, w, hints);

    XFree(hints);
    XFlush(display);
    XCloseDisplay(display);
    return 0;
}
