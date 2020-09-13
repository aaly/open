#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include <QObject>
#include <iostream>

using namespace std;

#include<X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>

class eventHandler : public QObject
{
		Q_OBJECT
	public:
		explicit eventHandler(QObject *parent = 0);
		int Init();
		
	signals:
		
	public slots:

	private:
		bool doneInit;
		Display *dpy;
		Window rootwin;

		int X11HandleEvents();
		
};

#endif // EVENTHANDLER_HPP
