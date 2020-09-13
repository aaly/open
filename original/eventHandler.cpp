#include "eventHandler.hpp"

eventHandler::eventHandler(QObject *parent) : QObject(parent)
{
	doneInit = false;
	Display *dpy = NULL;
	Window rootwin = -1;

	Init();
}


int eventHandler::Init()
{
	if(doneInit)
	{
		return 2;
	}


	if(!(dpy=XOpenDisplay(NULL)))
	{
		//fprintf(stderr, "ERROR: Could not open display\n");
		return 1;
	}

	////scr=DefaultScreen(dpy);
	//rootwin=RootWindow(dpy, scr);
	////cerr << "root window " << rootwin << ":" << winId() << flush;


	return 0;
}

int eventHandler::X11HandleEvents()
{

	while (0)
	{
		/*XEvent e;
		XNextEvent (text_box.display, & e);
		if (e.type == Expose) {
			draw_screen ();
		}
		else if (e.type == ConfigureNotify) {
			XConfigureEvent xce = e.xconfigure;
			if (xce.width != text_box.width ||
				xce.height != text_box.height) {
				text_box.width = xce.width;
				text_box.height = xce.height;
			}
			draw_screen ();
		}
		else if (e.type == ClientMessage) {
			if ((Atom)e.xclient.data.l[0] == wm_delete_window) {
				break;
			}
		}*/
	}
}
