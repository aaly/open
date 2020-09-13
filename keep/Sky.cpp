#include "Sky.hpp"
#include  <QtX11Extras/QtX11Extras>
#include  <X11/cursorfont.h>
#include <X11/extensions/XTest.h>

bool mouseMovementReady;
Sky::Sky()
{
	backgroundtype = BG_SCALED;
	currentDivingLevel = 0;
	divingFactor = 1;
	highlightedCloud = NULL;
	scoping = false;
	allowCollision = false;
	scopeBackground = true;
	diveBackground = true;
	browsePoint = QPoint(0, 0);
	browseFactor = 20;
	zoomFactor = 50;
	cloudLock = false;
	leftClick = false;
	rightClick = false;
	middleClick = false;
	Control = false;
	Alternate = false;
	Shift =false;
	autoScope = false;

    divingTimeLine = new QTimeLine(1000);
	//connect(divingTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(Dive()));

    scopeTimeLine = new QTimeLine(1000);
    connect(scopeTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(Scope()));

    ////scopebrowser = new scopeBrowser(this);
    ////scopebrowser->setSizeFactor(10);
    ////scopebrowser->move(0, getSize().height() - scopebrowser->height());
    ////bgView = new QLabel();
    ////QTimer::singleShot(1, this, SLOT(smoothMovement()));
	ignoreCursorMovement = false;
////	cloudsShdow = new QGraphicsDropShadowEffect();
////	cloudsShdow->setColor(Qt::lightGray);
}

Sky::~Sky()
{
    //delete cloud1;
    //delete cloud2;
    //delete cloud3;

	delete scopeTimeLine;
    delete bgView;
    /////delete scopebrowser;

    XCloseDisplay(dpy);
	Exit();
}



int Sky::Resize(int Width, int Height)
{
    XResizeWindow(dpy, root, Width, Height);
    rootSize = QSize(Width, Height);
    setSpaceSize(Width+2000, Height+2000);
    //bgView->move(0, 0);
    ////bgView->resize(spaceSize);
   //// updateBackground(true);
    //bgView->setStyleSheet("QLabel { background-color : red; color : blue; }");
    return 0;
}

QSize Sky::getSize()
{
    return rootSize;
}
int Sky::Run()
{
    screenorientation = HORIZONTAL;
    //QtConcurrent::run(processEvents, this);
    return processEvents(this);
}

QPoint Sky::getBrowsePoint()
{
	return browsePoint;
}

int Sky::Scope()
{
	// update clouds
	QMapIterator<unsigned int, QVector<Cloud*> > it(clouds);
	while (it.hasNext())
	{
		it.next();
		if(it.key() != currentDivingLevel)
		{
			//FIX other level should also be moving with a slower factor, as long as they are not hidden levels ?
			continue;
		}

		for( int i = 0; i < it.value().size(); i++)
		{

			QPoint origin = it.value().at(i)->getOrigin();
			origin = origin - browsePoint;
			//it.value().at(i)->hide();
            it.value().at(i)->Move(origin);
			/*if ( (origin.x() > 0 && origin.x() < width()) &&
				 (origin.y() > 0 && origin.y() < height()) )
			{
				it.value().at(i)->show();
			}*/
			//it.value().at(i)->show();
		}
	}

	//update background
    updateSky();
	return 0;
}


int Sky::smoothScope()
{
	int moveX = scopeTimeLine->currentValue()*scopeToPoint.x();
	int moveY = scopeTimeLine->currentValue()*scopeToPoint.y();

	cout << "Move : " << moveX << ":" << moveY << endl;
	//scopeToPoint.setX(scopeToPoint.x()+(moveX/-1));
	//scopeToPoint.setY(scopeToPoint.y()+(moveY/-1));

	scopeToPoint.setX(scopeToPoint.x()-(moveX));
	scopeToPoint.setY(scopeToPoint.y()-(moveY));

	browsePoint.setX(browsePoint.x()+ (moveX*+1));
	browsePoint.setY(browsePoint.y()+ (moveY*+1));

	Scope();

	/*
	cout << "scopeToPoint : " << scopeToPoint.x() << ":" << scopeToPoint.y() << endl;


	// update clouds
	QMapIterator<unsigned int, QVector<Cloud*> > it(clouds);
	while (it.hasNext())
	{
		it.next();
		if(it.key() != currentDivingLevel)
		{
			//FIX other level should also be moving with a slower factor, as long as they are not hidden levels ?
			continue;
		}

		for( int i = 0; i < it.value().size(); i++)
		{
			QPoint moveto(it.value().at(i)->pos().x(), it.value().at(i)->pos().y());
			if(moveX != 0)
			{
				moveto.setX(moveto.x()+ moveX);
			}

			if(moveY != 0)
			{
				moveto.setY(moveto.y()+ moveY);
			}

			cout << "X: "  << moveto.x()+ moveX << "Y: " << moveto.y()+ moveY << endl;

			it.value().at(i)->move(moveto);
		}
	}

	//update background
	if(scopeBackground)

	{
		if(moveX != 0)
		{
			browsePoint.setX(browsePoint.x()+ (moveX*+1));
		}

		if(moveY != 0)
		{
			browsePoint.setY(browsePoint.y()+ (moveY*+1));
		}
		update();
	}*/

	if(scopeToPoint.x() == 0 && scopeToPoint.y() ==0)
	{
        cout << (highlightedCloud->getPosition()).x() << "[]" << (highlightedCloud->getPosition()).y() << endl;
		cout << "Stop scoping" <<endl;
		scopeTimeLine->stop();
	}

	return 0;
}


int Sky::checkScope()
{
	if(scopeTimeLine->state() == QTimeLine::Running)
	{
		return 1;
		//scopeTimeLine->stop();
	}

	/*float x = 0, y =0;
	int screenW = QApplication::desktop()->width(), screenH = QApplication::desktop()->height();

	if(highlightedCloud->width() > screenW)
	{
		highlightedCloud->resize(screenW, highlightedCloud->height());
	}

	if(highlightedCloud->height() > screenH)
	{
		highlightedCloud->resize(highlightedCloud->width(), screenH);
	}


	if(highlightedCloud->x() + highlightedCloud->width() > screenW)
	{
		x = ((highlightedCloud->x() + highlightedCloud->width()) - screenW) * -1;
	}
	else if((highlightedCloud->x() + highlightedCloud->width() < screenW) && highlightedCloud->x()<0)
	{
		x = highlightedCloud->x() * -1;
	}

	if(highlightedCloud->y() + highlightedCloud->height() > screenH)
	{
		y = ((highlightedCloud->y() + highlightedCloud->height()) - screenH) * -1;
	}
	else if((highlightedCloud->y() + highlightedCloud->height() < screenH) && highlightedCloud->y() < 0)
	{
		y = highlightedCloud->y() * -1;
	}

	scopeToPoint.setX(x);
	scopeToPoint.setY(y);*/

	//cout << scopeToPoint.x() << ":" << scopeToPoint.y() << endl;
	//cout << (highlightedCloud->pos()).x() << ":" << (highlightedCloud->pos()).y() << endl;
	scopeTimeLine->setEasingCurve(QEasingCurve::InOutExpo);
	scopeTimeLine->setFrameRange(0, 100);
	scopeTimeLine->setDuration(800);
	connect(scopeTimeLine, SIGNAL(frameChanged(int)), this, SLOT(smoothScope()));
	if(scopeTimeLine->state() != QTimeLine::Running)
	{
		scopeTimeLine->start();
	}

	//close();


	return 0;
}

int Sky::addWindow(Window WindowID)
{
    Cloud* cloud = new Cloud(WindowID, dpy);
    cloud->Resize(500, 200);
    cloud->setOrigin(QPoint(400, 100));
    cloud->setDivingLevel(currentDivingLevel);
    return addCloud(cloud);
}

int Sky::addCloud(Cloud* cloud)
{
	if(!cloud || cloud == NULL)
	{
		return 1;
	}
	//FIX, check if out of space bouandries and fix
	if(clouds[cloud->getDivingLevel()].count() == 0)
	{
		//cloud->setHighlight(true);
		//
		highlightedCloud = cloud;
		checkScope();
	}

	if(cloud->getDivingLevel() != currentDivingLevel)
	{
        cloud->Lower();
	}
    else
    {
        cloud->Raise();
    }

	clouds[cloud->getDivingLevel()].push_back(cloud);
	return 0;
}

int Sky::deleteCloud(Cloud* cloud)
{
	if(!cloud || cloud == NULL)
	{
		return 1;
	}

	//clouds[cloud->getDivingLevel()].erase();
	return 0;
}

void Sky::Exit()
{
	QApplication::exit(0);
}


QSize Sky::getSpaceSize()
{
	return spaceSize;
}

int Sky::setSpaceSize(unsigned int width, unsigned int height)
{
	//check if given size is good bla bla bla ?
	// ....

	// set the space size :)
	spaceSize.setWidth(width);
	spaceSize.setHeight(height);
	//resize(width, height);

	//updateBackground(true);

	calcualteUniverseCenter();

	return 0;
}


int Sky::smoothMovement()
{
	mouseMovementReady = true;
    ////QTimer::singleShot(10, this, SLOT(smoothMovement()));
	return 0;
}

int Sky::processEvents(Sky* Root)
{
    if(!(dpy = XOpenDisplay(0x0)))
    {
        cout << "Can not open display !!!!" << endl;
        return 1;
    }
    root = DefaultRootWindow(dpy);
    //XSelectInput( dpy, root, SubstructureRedirectMask | SubstructureNotifyMask | StructureNotifyMask);
    ////XSelectInput( dpy, root, SubstructureNotifyMask | StructureNotifyMask);
    ////XSync(dpy, false);
    //XGrabServer(dpy);
    XGrabKey(dpy, AnyKey, AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
    ////XSelectInput(dpy, root, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | PointerMotionHintMask);
    //XGrabKeyboard(dpy, root, True, GrabModeAsync, GrabModeAsync, CurrentTime);
    //XGrabButton(dpy, AnyButton, AnyModifier, root, True, SubstructureNotifyMask, GrabModeAsync, GrabModeAsync, None, None);
    //XGrabButton(dpy, AnyButton, AnyModifier, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    //XGrabButton(dpy, 3, Mod1Mask, root, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);

    Root->Resize(QApplication::desktop()->width(), 	QApplication::desktop()->height());

    XColor bgSolidColor;
    char bgSolidColorSelection[] = "#0059b3";
    Colormap colormap = DefaultColormap(dpy, 0);
    GC green_gc = XCreateGC(dpy, root, 0, 0);
    XParseColor(dpy, colormap, bgSolidColorSelection, &bgSolidColor);
    XAllocColor(dpy, colormap, &bgSolidColor);
    XSetForeground(dpy, green_gc, bgSolidColor.pixel);
    XFillRectangle(dpy, root, green_gc, 1, 1, Root->getSize().width(), Root->getSize().height());

    XDefineCursor(dpy,root,XCreateFontCursor(dpy,XC_arrow));

    while(true)
    {
        XNextEvent(dpy, &ev);
        if(ev.type == CreateNotify)
        {
            cout << "New window !!!" << ev.xcreatewindow.window << endl;
            Root->addWindow(ev.xcreatewindow.window);
            //XRaiseWindow(dpy,ev.xcreatewindow.window);
            XMapRaised(dpy,ev.xcreatewindow.window);
            //XMapWindow(dpy,ev.xcreatewindow.window);
        }
        if(ev.type == DestroyNotify)
        {
            cout << "destroy window !!!" << ev.xdestroywindow.window << endl;
            XDestroyWindow(dpy,  ev.xdestroywindow.window);
        }
        //else if(ev.type == KeyPress && ev.xkey.subwindow != None)
        else if(ev.type == KeyPress || ev.type == KeyRelease)
        {
            cout << "Key pess!!!!" << endl;
            if(ev.xkey.keycode == XKeysymToKeycode(dpy,'q') && ev.xkey.state & Mod4Mask)
            {
                Root->Exit();
                break;
            }
            if (ev.xkey.subwindow != None)
            {
                //XRaiseWindow(dpy, ev.xkey.subwindow);
                //XUngrabKeyboard(dpy, ev.xkey.time);
                XUngrabKeyboard(dpy, 0);

                XAllowEvents(dpy, ReplayKeyboard, 0);
                //int ret =XSendEvent(dpy, ev.xbutton.subwindow,True, ev.xkey.type, &ev);
                XTestFakeKeyEvent(dpy, XKeysymToKeycode (dpy, XK_A), True, 0);
                XTestFakeKeyEvent(dpy, XKeysymToKeycode (dpy, XK_A), False, 0);

                //XFlush(dpy);

                //Window current_focus_window;
                //int current_focus_revert;
                 //XGetInputFocus(dpy, &current_focus_window, &current_focus_revert);
                 ////int ret =XSendEvent(dpy, InputFocus,True, ev.xkey.type, &ev);
                //int ret =XSendEvent(dpy, ev.xbutton.subwindow,True, ev.xkey.type, &ev);
                //cerr << "Return : " << ret << endl;
                cerr << "Return : " <<  ev.xbutton.window << endl;
            }
            //XSync(dpy, false);
        }
        else if(ev.type == ButtonPress && ev.xbutton.subwindow != None)
        {
            XGrabPointer(dpy, ev.xbutton.subwindow, True,
                        PointerMotionMask|ButtonReleaseMask, GrabModeAsync,
                        GrabModeAsync, None, None, CurrentTime);
            XGetWindowAttributes(dpy, ev.xbutton.subwindow, &attr);
            start = ev.xbutton;
        }
        else if(ev.type == MotionNotify)
        {
            int xdiff, ydiff;
            while(XCheckTypedEvent(dpy, MotionNotify, &ev));
            xdiff = ev.xbutton.x_root - start.x_root;
            ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(dpy, ev.xmotion.window,
                    attr.x + (start.button==1 ? xdiff : 0),
                    attr.y + (start.button==1 ? ydiff : 0),
                    MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
                    MAX(1, attr.height + (start.button==3 ? ydiff : 0)));
        }
        else if(ev.type == ButtonRelease)
        {
            XUngrabPointer(dpy, CurrentTime);
        }
    }
    return 0;
}



int Sky::calcualteUniverseCenter()
{
	float percent = 0.004;
    universeCenter.setX( (rootSize.width()/2) - (rootSize.width()*percent));
    universeCenter.setY( (rootSize.height()/2) - (rootSize.height()*percent));

    universeCenter.setWidth((rootSize.width()*percent)*2);
    universeCenter.setHeight((rootSize.height()*percent)*2);

	return 0;
}

int Sky::setBackground(const QString &bg)
{
	if(QFile::exists(bg))
	{
		backgroundPath = bg;
        return updateBackground(true);
	}
	return 1;
}

/*const QPixmap Sky::getBackground()
{
    return background;
}*/

int Sky::setBackgroundType(const Sky::backgroundType& type)
{
	backgroundtype = type;
	return updateBackground(true);
}

int Sky::updateBackground(bool refresh)
{
    return 0;
}

/*
int Sky::updateBackground(bool refresh)
{
    QPixmap tmpBackground;
	if(refresh)
	{
		tmpBackground.load(backgroundPath);
		if(tmpBackground.isNull())
		{
			return 1;
		}
	}
	else
	{
        tmpBackground = background;
        tmpBackground.copy(background.rect());

	}

    background = QPixmap(spaceSize.width(), spaceSize.height());
    QPainter* painter = new QPainter(&background);

    //painter->setRenderHint(QPainter::HighQualityAntialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

	if(refresh)
	{
        painter->fillRect(0, 0, background.width(), background.height(), Qt::black);
	}

	if(backgroundtype == BG_SCALED)
	{
        painter->drawPixmap(0, 0, background.width(), background.height(), tmpBackground);
	}
	else if(backgroundtype == BG_CLIPPED)
	{
        painter->drawPixmap(0, 0, tmpBackground, 0, 0, tmpBackground.width(), tmpBackground.height());
	}
	else if(backgroundtype == BG_REPEATED)
	{
        int cwidth = background.width()-tmpBackground.width();
		if(cwidth > 0)
		{
			cwidth = (float)cwidth/tmpBackground.width() == cwidth/tmpBackground.width() ? cwidth/tmpBackground.width() : cwidth/tmpBackground.width()+2;
		}
		else
		{
			cwidth = 1;
		}

		int cheight  = background.height()-tmpBackground.height();
		if(cheight > 0)
		{
			cheight = (float)cheight/tmpBackground.height() == cheight/tmpBackground.height() ? cheight/tmpBackground.height() : cheight/tmpBackground.height()+1;
		}
		else
		{
			cheight = 0;
		}

		for ( int i =0; i < cwidth; i++)
		{
			painter->drawPixmap(i*tmpBackground.width(), 0, tmpBackground, 0, 0, tmpBackground.width(), tmpBackground.height());
			for ( int k =0; k <= cheight; k++)
			{
				painter->drawPixmap(i*tmpBackground.width(), k*tmpBackground.height(), tmpBackground, 0, 0, tmpBackground.width(), tmpBackground.height());
			}
		}
	}

	delete painter;

    bgView->setPixmap(background);
    bgView->setWindowFlags(Qt::WindowTransparentForInput);
    bgView->show();
    //XSetWindowBackgroundPixmap(dpy, root, static_cast<const Pixmap*>(tmpBackground.handle()));
    updateSky();
	return 0;
}*/

void Sky::drawBackground(QPainter *painter)
{
	if(scopeBackground)
	{
        ////painter->drawPixmap(0, 0, rootSize.width(), rootSize.height(), background, browsePoint.x(), browsePoint.y(), rootSize.width(),rootSize.height());
	}
	else
	{
        ////painter->drawPixmap(0, 0, rootSize.width(), rootSize.height(), background);
	}
}



/*
bool Sky::handelMouseMovement(QEvent* event)
{
	QMouseEvent* mevent = static_cast<QMouseEvent *>(event);

	oldMousePosition.setX(mevent->X);

}*/

/*
void Sky::mouseMoveEvent(QMouseEvent *event)
{
	unsigned int diveMovementFactor = 8;

	if ( !((event->pos().x() > universeCenter.x() && event->pos().y() > universeCenter.y()) &&
		  (event->pos().x() < universeCenter.x()+universeCenter.width() && event->pos().y() < universeCenter.y()+universeCenter.height())))
	{
		if(event->pos().x() > spaceSize.width()/2)
		{
			horizontalScrollBar()->setValue(horizontalScrollBar()->value()+diveMovementFactor);
		}
		else
		{
			horizontalScrollBar()->setValue(horizontalScrollBar()->value()-diveMovementFactor);
		}

		if(event->pos().y() > spaceSize.height()/2)
		{
			verticalScrollBar()->setValue(verticalScrollBar()->value()+diveMovementFactor);
		}
		else
		{
			verticalScrollBar()->setValue(verticalScrollBar()->value()-diveMovementFactor);
		}

	}
}
*/


int Sky::Browse(const int& direction, bool smooth)
{
	QPoint searchPoint(0 ,0);
	QPoint browseToPoint(browseFactor ,browseFactor);

	if(direction & UP)
	{
		searchPoint.setY(0);
		browseToPoint.setY(browseFactor*-1);
	}
	else if(direction & DOWN)
	{
		searchPoint.setX(spaceSize.height());
	}

	if(direction & LEFT)
	{
		searchPoint.setX(0);
		browseToPoint.setX(browseFactor*-1);
	}
	else if(direction & RIGHT)
	{
		searchPoint.setX(spaceSize.width());
	}


	if(!cloudLock)
	//if(false)
	{
		Cloud* oldHighlightedCloud = highlightedCloud;

		QMapIterator<unsigned int, QVector<Cloud*> > it(clouds);
		while (it.hasNext())
		{
			it.next();
			if(it.key() != currentDivingLevel)
			{
				continue;
			}


			for( int i = 0; i < it.value().size(); i++)
			{
                if (it.value().at(i)->isLowered()) // is not needed since we are searchig one level only ?
				{
					continue;
				}
				bool push = false;

				if(screenorientation == VERTICAL)
				{
					if(direction & UP)
					{
                        if(it.value().at(i)->getPosition().y() < oldHighlightedCloud->getPosition().y() &&
                                it.value().at(i)->getPosition().y() > searchPoint.y() )
						{
                            searchPoint.setY(it.value().at(i)->getPosition().y());
							push = true;
						}
					}
					else if(direction & DOWN)
					{
                        if(it.value().at(i)->getPosition().y() > oldHighlightedCloud->getPosition().y() &&
                                it.value().at(i)->getPosition().y() < searchPoint.y() )
						{
                            searchPoint.setY(it.value().at(i)->getPosition().y());
							push = true;
						}
					}

				}
				else if(screenorientation == HORIZONTAL)
				{
					if(direction & LEFT)
					{
                        if(it.value().at(i)->getPosition().x() < oldHighlightedCloud->getPosition().x() &&
                                it.value().at(i)->getPosition().x() > searchPoint.x() )
						{
                            searchPoint.setX(it.value().at(i)->getPosition().x());
							push = true;
						}
					}
					else if(direction & RIGHT)
					{
                        if(it.value().at(i)->getPosition().x() > oldHighlightedCloud->getPosition().x() &&
                                it.value().at(i)->getPosition().x() < searchPoint.x() )
						{
                            searchPoint.setX(it.value().at(i)->getPosition().x());
							push = true;
						}
					}
				}

				// out of screen out of my dream
                if(it.value().at(i)->getOrigin().x()+ it.value().at(i)->getSize().width() >= browsePoint.x()+ rootSize.width() || it.value().at(i)->getOrigin().x()+ it.value().at(i)->getSize().width()  <= browsePoint.x())
				{
					push = false;
				}

                if(it.value().at(i)->getOrigin().y() + it.value().at(i)->getSize().height() >= browsePoint.y()+ rootSize.height() || it.value().at(i)->getOrigin().y()+ it.value().at(i)->getSize().height()  <= browsePoint.y())
				{
					push = false;
				}

				if(push)
				{
					highlightedCloud = it.value().at(i);
				}
			}

			if(it.key() == currentDivingLevel)
			{
				break;
			}
		}

		oldHighlightedCloud->setHighlight(false);
		highlightedCloud->setHighlight(true);
	}

	/*if(oldHighlightedCloud != highlightedCloud)
	{
		cout <<highlightedCloud->pos().x() << ":::" << highlightedCloud->pos().y() << endl;
		checkScope();
	}*/

	/*while( (browsePoint.x()+browseToPoint.x()) + width() >  spaceSize.width() || (browsePoint.x()+browseToPoint.x()) < 0 )
	{
		browseToPoint.setX(browseToPoint.x() + (browseToPoint.x()/browseToPoint.x()));
	}

	while( (browsePoint.y()+browseToPoint.y()) + height() >  spaceSize.height() || (browsePoint.y()+browseToPoint.y()) < 0 )
	{
		browseToPoint.setY(browseToPoint.y() + (browseToPoint.y()/browseToPoint.y()));
	}*/



	if(smooth)
	{
		scopeToPoint += browseToPoint;

		if(browsePoint.x() + scopeToPoint.x() < 0)
		{
			scopeToPoint.setX(0);
		}
        else if (browsePoint.x() + scopeToPoint.x() + rootSize.width() > spaceSize.width())
		{
            scopeToPoint.setX(spaceSize.width() - rootSize.width() - browsePoint.x());
		}

		if(browsePoint.y() + scopeToPoint.y() < 0)
		{
			scopeToPoint.setY(0);
		}
        else if (browsePoint.y() + scopeToPoint.y() + rootSize.height() > spaceSize.height())
		{
            scopeToPoint.setY(spaceSize.height() - rootSize.height() - browsePoint.y());
		}

		checkScope();
	}
	else
	{
		browsePoint += browseToPoint;

		if(browsePoint.x() < 0)
		{
			browsePoint.setX(0);
		}
        else if (browsePoint.x() + rootSize.width() > spaceSize.width())
		{
            browsePoint.setX(spaceSize.width() - rootSize.width());
		}

		if(browsePoint.y() < 0)
		{
			browsePoint.setY(0);
		}
        else if (browsePoint.y() + rootSize.height() > spaceSize.height())
		{
            browsePoint.setY(spaceSize.height() - rootSize.height());
		}
		Scope();
	}

    /////scopebrowser->update();

	return 0;
}


int Sky::resetCursor()
{

	int universeXMidPoint = universeCenter.x()+(universeCenter.width()/2);
	int universeYMidPoint = universeCenter.y()+(universeCenter.height()/2);
	ignoreCursorMovement = true;
	QCursor::setPos(universeXMidPoint, universeYMidPoint);
    return 0;
}





/**********************************
 *
 * Manipulation
 *
***********************************/



bool Sky::cloudContainable(int x, int y, int width, int height)
{
	if(x >= 0 && y >= 0 && x+width <= spaceSize.width() && y+height <= spaceSize.height())
	{
		return true;
	}

	return false;
}

int Sky::moveClouds(int direction)
{
	if(!Control || !leftClick || highlightedCloud == NULL)
	{
		return 1;
	}

	cloudLock = true;
	//int oldBFactor = browseFactor;
	//browseFactor = 1;
    if(highlightedCloud->getPosition().x() <= browseFactor || highlightedCloud->getPosition().x()+highlightedCloud->getSize().width() +browseFactor >= rootSize.width()
            || highlightedCloud->getPosition().y() <= browseFactor || highlightedCloud->getPosition().y()+highlightedCloud->getSize().height() +browseFactor >= rootSize.height() )
	{
		Browse(direction, true);
	}


	//int moveFactor = browseFactor;
	int moveFactor = 2;

	int xMoveFactor = moveFactor;
	int yMoveFactor = moveFactor;

	if(direction & UP)
	{
		yMoveFactor *= -1;
	}
	if(direction & LEFT)
	{
		xMoveFactor *= -1;
	}


    if(highlightedCloud->getPosition().x()+xMoveFactor < 0)
	{
        xMoveFactor = -highlightedCloud->getPosition().x();
	}
    else if(highlightedCloud->getPosition().x()+xMoveFactor+highlightedCloud->getSize().width() > rootSize.width())
	{
        xMoveFactor = rootSize.width() - (highlightedCloud->getPosition().x()+xMoveFactor+highlightedCloud->getSize().width());
	}

    if(highlightedCloud->getPosition().y()+yMoveFactor < 0)
	{
        yMoveFactor = -highlightedCloud->getPosition().y();
	}
    else if(highlightedCloud->getPosition().y()+yMoveFactor+highlightedCloud->getSize().height() > rootSize.height())
	{
        yMoveFactor = rootSize.height() - (highlightedCloud->getPosition().y()+yMoveFactor+highlightedCloud->getSize().height());
	}

	if(yMoveFactor != 0 || xMoveFactor != 0)
	{
		//highlightedCloud->resize(highlightedCloud->size().width() + resizeFactor, highlightedCloud->size().height() + resizeFactor);
        highlightedCloud->Move(highlightedCloud->getPosition().x() +xMoveFactor, highlightedCloud->getPosition().y() +yMoveFactor);
        highlightedCloud->setOrigin(QPoint(highlightedCloud->getPosition().x() +browsePoint.x() , highlightedCloud->getPosition().y() +browsePoint.y()));
		//Browse(direction);
	}

	cloudLock = false;
	return 0;
}



int Sky::resizeClouds(int direction)
{
	if(!Control || !rightClick || highlightedCloud == NULL)
	{
		return 1;
	}

	int xResizeFactor = 8;
	int yResizeFactor = 8;

	if(direction & LEFT)
	{
		xResizeFactor *= -1;
	}

	if(direction & UP)
	{
		yResizeFactor *= -1;
	}

	cloudLock = true;

    if (cloudContainable(highlightedCloud->getOrigin().x(), highlightedCloud->getOrigin().y(), highlightedCloud->getSize().width() + xResizeFactor, highlightedCloud->getSize().height() + yResizeFactor))
	{
        highlightedCloud->Resize(highlightedCloud->getSize().width() + xResizeFactor, highlightedCloud->getSize().height() + yResizeFactor);
	}
	//highlightedCloud->move(highlightedCloud->pos().x() +((resizeFactor*-1)/2), highlightedCloud->pos().y() +((resizeFactor*-1)/2));
	//highlightedCloud->setOrigin(QPoint(highlightedCloud->pos().x() +((resizeFactor*-1)/2), highlightedCloud->pos().y() +((resizeFactor*-1)/2)));

	//int oldbrowsing = browseFactor;
	//browseFactor = xResizeFactor < yResizeFactor ? yResizeFactor : xResizeFactor ;
	//browseFactor = browseFactor < oldbrowsing ? oldbrowsing : browseFactor ;
	//browseFactor = oldbrowsing;

    if ( (highlightedCloud->getSize().width()+highlightedCloud->getSize().width() > rootSize.width() || highlightedCloud->getPosition().y()+highlightedCloud->getSize().height() > rootSize.height())
		 && (direction & RIGHT || direction & DOWN) )
		//if (highlightedCloud->getOrigin().x()+highlightedCloud->size().width() > width() || highlightedCloud->getOrigin().y()+highlightedCloud->size().height() > height())
	{
		Browse(direction, true);
	}

    else if ( (highlightedCloud->getPosition().x()<0 && direction & LEFT) || (highlightedCloud->getPosition().y()<0 && direction & UP) )
	{
		Browse(direction, true);
	}


    //repaint();
    updateSky();

	cloudLock = false;
	return 0;
}

/**********************************
 *
 * Events
 *
***********************************/


void Sky::mouseMoveEvent(QMouseEvent *event)
{
	if (ignoreCursorMovement)
	{
		ignoreCursorMovement = false;
		return;
	}

	if(!mouseMovementReady)
	{
		return;
	}
	mouseMovementReady = false;


	int direction = 0;

	int universeXMidPoint = universeCenter.x()+(universeCenter.width()/2);
	int universeYMidPoint = universeCenter.y()+(universeCenter.height()/2);

	if(event->pos().x() > universeXMidPoint)
	{
		direction = direction | RIGHT;
	}
	else
	{
		direction = direction | LEFT;
	}

	if(event->pos().y() > universeYMidPoint)
	{
		direction = direction | DOWN;
	}
	else
	{
		direction = direction | UP;
	}



	if(resizeClouds(direction) && moveClouds(direction))
	{
		Browse(direction, false);
	}


    oldMousePosition.setX(rootSize.width()/2);
    oldMousePosition.setY(rootSize.height()/2);
	resetCursor();
}

void Sky::keyPressEvent(QKeyEvent* event)
{
	if(event->key()==Qt::Key_Alt)
	{
		Alternate = true;
	}

	if(event->key()==Qt::Key_Control)
	{
		Control = true;
	}

	if(event->key()==Qt::Key_Shift)
	{
		Shift = true;
	}
}

//void Sky::keyReleaseEvent(QKeyEvent* event)
void   Sky::keyReleaseEvent(QKeyEvent* event)
{
	if(event->key()==Qt::Key_Alt)
	{
		Alternate = false;
	}

	if(event->key()==Qt::Key_Control)
	{
		Control = false;
	}

	if(event->key()==Qt::Key_Shift)
	{
		Shift = false;
	}

}


void Sky::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;

	if (numSteps > 0)
	{
		if(Control)
		{
			Zoom(zoomFactor);
		}
		else
		{
			checkDiving(IN);
		}
	}
	else if (numSteps < 0)
	{
		if(Control)
		{
			Zoom(zoomFactor*-1);
		}
		{
			checkDiving(OUT);
		}
	}
	event->accept();

}

void Sky::mousePressEvent(QMouseEvent* event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		leftClick = true;
	}

	if(event->buttons() & Qt::RightButton)
	{
		rightClick = true;
	}

	if(event->buttons() & Qt::MiddleButton)
	{
		middleClick = true;
	}
	//event->accept();
}

void Sky::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		leftClick = false;
	}

	if(event->button() == Qt::RightButton)
	{
		rightClick = false;
	}

	if(event->button() == Qt::MiddleButton)
	{
		middleClick = false;
	}
	//event->accept();
}

bool Sky::event(QEvent *event)
{	
	if(event->type() == QEvent::Close)
	{
		Exit();
	}
    return 0;
}

void Sky::paintEvent(QPaintEvent *event)
{
	//QImage image(size(), QImage::Format_ARGB4444_Premultiplied);
	//QPixmap image(size());
    ////QPainter painter();
    ////painter.setRenderHint(QPainter::Antialiasing, true);
	//painter.setRenderHint(QPainter::SmoothPixmapTransform);
	//painter.setBackgroundMode(Qt::TransparentMode);
    ////drawBackground(&painter);
    ////painter.drawRect(universeCenter);
	//QWidget::paintEvent(event);
	//QPainter realPainter(this);
	//realPainter.drawPixmap(0, 0, image);

//#define SkyShadowEffect
#ifdef SkyShadowEffect
	for(unsigned int i =0; i < clouds[currentDivingLevel].size(); i++)
	{
		Cloud* cloud = clouds[currentDivingLevel].at(i);
		QPen pen;
		pen.setWidth(2);
		pen.setColor(Qt::blue);
		realPainter.setPen(pen);
		realPainter.drawLine(cloud->x(), cloud->y()+cloud->height(), cloud->x()+cloud->width(),  cloud->y()+cloud->height());
	}
#endif

	//delete image;

}




const QMap<unsigned int, QVector<Cloud*> >& Sky::getClouds()
{
	return clouds;
}

const QVector<Cloud*>& Sky::getCurrentClouds()
{
	return clouds[currentDivingLevel];
}










/**********************************
 *
 * Zooming
 *
***********************************/
int Sky::Zoom(int level)
{

	QMapIterator<unsigned int, QVector<Cloud*> > it(clouds);
	while (it.hasNext())
	{
		it.next();
		if(it.key() != currentDivingLevel)
		{
			//FIX other level should also be moving with a slower factor, as long as they are not hidden levels ?
			continue;
		}

		for( int i = 0; i < it.value().size(); i++)
		{
            it.value().at(i)->Resize(it.value().at(i)->getSize().width() + level, it.value().at(i)->getSize().height() + level);
            it.value().at(i)->Move(it.value().at(i)->getPosition().x() +((level*-1)/2), it.value().at(i)->getPosition().y() +((level*-1)/2));
		}
	}

	setSpaceSize(spaceSize.width() + level,spaceSize.height() + level);

	backgroundType tmpbgType = backgroundtype;
	backgroundtype = BG_SCALED;
	updateBackground(false);
	backgroundtype = tmpbgType;


	Browse(RIGHT & LEFT & UP & DOWN, true);
    updateSky();
	return level;
}


int Sky::updateSky()
{
    return 0;
}

/**********************************
 *
 * Diving
 *
***********************************/

int Sky::checkDiving(int direction)
{
	if(divingTimeLine->state() == QTimeLine::Running)
	{
		divingTimeLine->stop();
	}

	divingTimeLine->setEasingCurve(QEasingCurve::InQuint);
	divingTimeLine->setFrameRange(0, 100);
	divingTimeLine->setDuration(200);
	connect(divingTimeLine, SIGNAL(frameChanged(int)), this, SLOT(Dive()));
	divingTimeLine->start();


	//close();

	return 0;
}

unsigned long Sky::Dive(int level)
{
	currentDivingLevel += level;
	level = level*divingFactor;

	//if( value < 0 && value*-1 > zoom)
	{
		//zoom =0;
	}
	//else
	{
		currentDivingLevel += level;
		for (unsigned int i =0; i < clouds.count(); i++)
		{
			/*QGraphicsEllipseItem * eitem = (QGraphicsEllipseItem*)items().at(i);
			QRectF rect = eitem->rect();

			if(eitem->x()+ rect.x() + rect.width() < width()/2)
			{
				eitem->moveBy(value, 0);
			}
			else
			{
				eitem->moveBy(value, 0);

			}

			if(eitem->y()+ rect.y() + rect.height() < height()/2)
			{
				eitem->moveBy(0, value);
			}
			else
			{
				eitem->moveBy(0, value);
			}
			cout << "POS:" << eitem->x() << ":" << eitem->y() << endl;
			cout << "POS:" << eitem->pos().x() << ":" << eitem->pos().y() << endl;
			cout << "POS:" << eitem->scenePos().x() << ":" << eitem->scenePos().y() << endl;
			cout << "POS:" << eitem->mapToScene(0, 0).x() << ":" << eitem->mapToScene(0, 0).y() << endl;
			//cout << "SIZE:" << eitem->boundingRect().width() << ":" << eitem->boundingRect().height() << endl;
			*/
		}
	}
    updateSky();
	return level;
}

unsigned long Sky::getDivingLevel()
{
	return currentDivingLevel;
}




/*int Sky::OnXError(Display* display, XErrorEvent* e)
{
  const int MAX_ERROR_TEXT_LENGTH = 1024;
  char error_text[MAX_ERROR_TEXT_LENGTH];
  XGetErrorText(display, e->error_code, error_text, sizeof(error_text));
  cerr << "Received X error:\n"
             << "    Request: " << int(e->request_code)
             << " - " << XRequestCodeToString(e->request_code) << "\n"
             << "    Error code: " << int(e->error_code)
             << " - " << error_text << "\n"
             << "    Resource ID: " << e->resourceid;
  // The return value is ignored.
  return 0;
}*/
