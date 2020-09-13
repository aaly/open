#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

void* TaskCode(void* parg)
{
    int keycode = *((int*)parg);
    cout<< "\n\n" << keycode << "\n\n";
    system("espeak -v en " "\"a\"");
    delete (int*)parg;
    return 0;
}

void SendKeyEvent(Display *display, XEvent event)
{
    Window current_focus_window;
    XKeyEvent& xkey = event.xkey;

    int current_focus_revert;
    XGetInputFocus(display, &current_focus_window, &current_focus_revert);       
    xkey.state = Mod2Mask;

    XSendEvent(display, InputFocus,  True, xkey.type, (XEvent *)(&event));
}

int GrabKey(Display* display, Window grab_window, int keycode)
{
    unsigned int    modifiers       = Mod2Mask; // numlock on
    //Window          grab_window     = DefaultRootWindow(display);
    Bool            owner_events    = True;
    int             pointer_mode    = GrabModeAsync;
    int             keyboard_mode   = GrabModeAsync;

    XGrabKey(display, keycode, modifiers, grab_window, owner_events, pointer_mode, keyboard_mode);
    return keycode;
}

void UngrabKey(Display* display, Window grab_window, int keycode)
{
    unsigned int    modifiers       = Mod2Mask; // numlock on

   // Window grab_window = DefaultRootWindow(display);
    XUngrabKey(display,keycode,modifiers,grab_window);
}


void Action(int keycode)
{
    pthread_t thread;
    int* pthread_arg = new int;

    *pthread_arg = keycode;
    pthread_create(&thread,0, TaskCode, (void*) pthread_arg);
}

int main()
{
    Display*    display = XOpenDisplay(0);
    Window      root    = DefaultRootWindow(display);
    XEvent      event;

    int keycode = XKeysymToKeycode(display,'a');
    GrabKey(display,root,keycode);

    XSelectInput(display, root, KeyPressMask | KeyReleaseMask);
    while(true)
    {
        XNextEvent(display, &event);
        switch(event.type)
        {
            case KeyPress:
                Action(event.xkey.keycode);
            case KeyRelease:
                SendKeyEvent(display,event);
            default:
                break;
        }
    }

    XCloseDisplay(display);
}
