#include "Sky.hpp"

bool mouseMovementReady;
Sky::Sky()
{
    XInitThreads();

    dpy = XOpenDisplay(0x0);

    if(!dpy)
    {
        cerr << "Can not open display !!!!" << endl <<flush;
        Exit();
    }


    root = DefaultRootWindow(dpy);
    Screen* defaultScreen = DefaultScreenOfDisplay( dpy );
    Resize(defaultScreen->width, defaultScreen->height);
    XSetErrorHandler(OnXError);
    XSetIOErrorHandler(OnXIOError);

    browseDuration = 6; // duration in milliseconds
    liveThumbnailMode = false;
    Timer = 0;
    CONFIG_GRAVITY = true;
	backgroundtype = BG_SCALED;
	currentDivingLevel = 0;
	divingFactor = 1;
	highlightedCloud = NULL;
	scoping = false;
	allowCollision = false;
	scopeBackground = true;
	diveBackground = true;
    browsePoint = QPoint(0, 0);
    fixedBrowseFactor = 25;
    browseFactor = fixedBrowseFactor;
    zoomFactor = 20;
	cloudLock = false;

    rootOverlay = 0;
    rootOverlayCounter = 0;

    finalBrowsePointCounterY = 0;
    finalBrowsePointCounterX = 0;
    //currentBrowsePointCounterX = 0;
    //currentBrowsePointCounterY = 0;

	autoScope = false;
    browseMoveThreshold = 5;
    keyboardBrowseAceleration = 1;
    mouseMoveCounter = 0;
    CONFIG_ONSCREENHISTORY=false;
    cloudSelected = NULL;
    ignoreCursorMovement = false;


    leftClick = 0;
    rightClick = 0;
    middleClick = false;
    Control = false;
    Super  = false;
    Alt = false;
    Alternate = false;
    Shift =false;
    superCounter = 0;
    controlCounter = 0;
    altCounter = 0;
    shiftCounter = 0;
}

Sky::~Sky()
{
    XCloseDisplay(dpy);
	Exit();
}


int Sky::Resize(int Width, int Height)
{
    XResizeWindow(dpy, root, Width, Height);
    rootSize = QSize(Width, Height);
    updateBackground(true);
    calcualteUniverseCenter();
    return 0;
}

QSize Sky::getSize()
{
    return rootSize;
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

int Sky::setBackgroundType(const Sky::backgroundType& type)
{
    backgroundtype = type;
    if(backgroundPath.size())
    {
        return updateBackground(true);
    }
    return 0;
}

int Sky::moveBackground()
{
    if(!dpy)
    {
        return 1;
    }

    Imlib_Image tempBackground = imlib_create_image(getSize().width(), getSize().height());
    if(!tempBackground)
    {
        cerr << "Imlib2 : Ooooops , null image !" << endl;
    }
    imlib_context_set_image(tempBackground);
    imlib_context_set_color(255, 255, 255, 255);
    imlib_image_fill_rectangle(0, 0, getSize().width(), getSize().height());
    imlib_context_set_blend(1);
    imlib_blend_image_onto_image(localBackground, 0,
                                 browsePoint.x(), browsePoint.y(), getSize().width(), getSize().height(),
                                 0, 0, getSize().width(), getSize().height());

    Pixmap canvas = XCreatePixmap(dpy, root, getSize().width(), getSize().height(), DefaultDepth(dpy, 0));

    imlib_context_set_drawable(canvas);
    imlib_render_image_on_drawable(0,0);
    XSetWindowBackgroundPixmap(dpy, root, canvas);
    imlib_context_set_blend(0);
    imlib_free_image_and_decache();
    tempBackground = NULL;

    XFreePixmap(dpy, canvas);
    XClearWindow(dpy, root);
    return 0;
}


int Sky::updateBackground(bool refresh)
{
    if(!dpy)
    {
        return 1;
    }

    if(refresh)
    {
        Imlib_Load_Error      er;
        backgroundImage = imlib_load_image_with_error_return(backgroundPath.toStdString().c_str(), &er);
        if (!backgroundImage)
        {
            cerr << "imlib2 : Can not set backgound image : " << backgroundPath.toStdString().c_str() << endl << flush;
            return 1;
        }
    }

    XColor bgSolidColor;
    char bgSolidColorSelection[] = "#0059b3";
    Colormap colormap = DefaultColormap(dpy, 0);
    GC green_gc = XCreateGC(dpy, root, 0, 0);
    XParseColor(dpy, colormap, bgSolidColorSelection, &bgSolidColor);
    XAllocColor(dpy, colormap, &bgSolidColor);
    XSetForeground(dpy, green_gc, bgSolidColor.pixel);
    XFillRectangle(dpy, root, green_gc, 1, 1, getSize().width(), getSize().height());

    imlib_context_set_image(backgroundImage);
    int backgroundWidth = imlib_image_get_width();
    int backgroundHeight = imlib_image_get_height();

    imlib_context_set_display(dpy);
    imlib_context_set_visual(DefaultVisual(dpy, DefaultScreen(dpy)));
    imlib_context_set_colormap(DefaultColormap(dpy, DefaultScreen(dpy)));
    imlib_set_cache_size(1000);
    imlib_set_color_usage(256);
    imlib_context_set_dither(0);
    //blank_screen(s, BLANK_COLOR, &canvas);
    localBackground = imlib_create_image(getSpaceSize().width(), getSpaceSize().height());
    imlib_context_set_image(localBackground);
    imlib_context_set_color(255, 255, 255, 255);
    imlib_image_fill_rectangle(0, 0, getSpaceSize().width(), getSpaceSize().height());
    imlib_context_set_blend(1);

    if(backgroundtype == BG_SCALED)
    {
        imlib_blend_image_onto_image(backgroundImage, 0,
                                     0, 0, backgroundWidth,  backgroundHeight,
                                     0, 0, getSpaceSize().width(), getSpaceSize().height());

    }
    else if(backgroundtype == BG_CLIPPED)
    {
        imlib_blend_image_onto_image(backgroundImage, 0,
                                     0, 0, backgroundWidth,  backgroundHeight,
                                     0, 0, backgroundWidth,  backgroundHeight);
    }
    else if(backgroundtype == BG_REPEATED)
    {
        for (int yi = 0; yi <= getSpaceSize().height(); yi += backgroundHeight)
        {
            for (int xi = 0; xi <= getSpaceSize().width(); xi += backgroundWidth)
            {
                imlib_blend_image_onto_image(backgroundImage, 0,
                                             0, 0, backgroundWidth,  backgroundHeight,
                                             xi, yi, backgroundWidth,  backgroundHeight);
            }
        }
    }
    else if(backgroundtype == BG_CENTERED)
    {
        int screenCenterX = ( getSpaceSize().width()  - backgroundWidth)  / 2;
        int screenCenterY = (getSpaceSize().height() - backgroundHeight) / 2;
        imlib_blend_image_onto_image(backgroundImage, 0,
                                     0, 0, backgroundWidth,  backgroundHeight,
                                     screenCenterX, screenCenterY, backgroundWidth,  backgroundHeight);
    }
    return moveBackground();
}

int Sky::Run()
{
    cout << flush;
    cerr << flush;

    // Get the list of input devices that are attached to the
    // display now.
       int              numDevices;
       XDeviceInfoPtr list = (XDeviceInfoPtr) XListInputDevices(dpy, &numDevices);

       // Now run through the list looking for the dial+button
       // device.
       XDevice* device = NULL;
       for (int i = 0; (i < numDevices) && (device == NULL); i++)
       {
         // Open the device - the device id is set at runtime.
           cerr << list[i].name << endl;

           char* deviceName = XGetAtomName(dpy, list[i].type);
           if(deviceName)
           {
               cerr <<  deviceName << endl << flush;
               XFree(deviceName);
           }

           XFlush(dpy);
         //if (strcmp(list[i].name, DEVICE_NAME) == 0) {
          // device = XOpenDevice(display, list[i].id);
         //}
       }


    XDefineCursor(dpy,root,0);
    screenorientation = HORIZONTAL;
    moveBackground();

    skynotifier = new skyNotifier(this);
    //skynotifier->setSizeFactor(10);
    //skynotifier->Move(0, 0);
    //skynotifier->Hide();
    //skynotifier->Update();

    skyscope = new skyScope(this);
    skyscope->setBorderSize(2);
    skyscope->setSizeFactor(100);
    skyscope->Move(0, 0);


    scopebrowser = new skyScope(this);
    scopebrowser->setShowTitle(false);
    scopebrowser->setShowGrid(false);
    scopebrowser->setShowThumbnail(false);
    scopebrowser->setTransparent(false);
    scopebrowser->setBorderSize(2);
    scopebrowser->setSizeFactor(10);
    scopebrowser->Move(0, getSize().height() - scopebrowser->getSize().height());
    scopebrowser->Show();
    scopebrowser->Update();


    //QtConcurrent::run(processEvents, this);
    //thread(&Sky::browsingThreadFunction, this).detach();
    /////thread(&Sky::notifierThreadFunction, this).detach();
    setCurrentOperation(BROWSING);
    return processEvents();
}

Window Sky::requestOverlayWindow()
{
    if(!rootOverlay)
    {
        //XCompositeRedirectSubwindows(dpy, root, CompositeRedirectAutomatic);
        //XSync(dpy, false);
        rootOverlay = XCompositeGetOverlayWindow(dpy, XRootWindow(dpy, XDefaultScreen(dpy)));

        if(rootOverlay == 0)
        {
            return 0;
        }

        XserverRegion region = XFixesCreateRegion (dpy, NULL, 0);
        XFixesSetWindowShapeRegion (dpy, rootOverlay, ShapeBounding, 0, 0, 0);
        XFixesSetWindowShapeRegion (dpy, rootOverlay, ShapeInput, 0, 0, region);
        XFixesDestroyRegion (dpy, region);

        //XSelectInput(dpy, rootOverlay, ExposureMask);
    }

    if(rootOverlayCounter > 0)
    {
        //XCompositeRedirectSubwindows(dpy, root, CompositeRedirectAutomatic);
        //XMapWindow(dpy, rootOverlay);
    }

    rootOverlayCounter++;
    return rootOverlay;
}

int Sky::releaseOverlayWindow()
{

    if(rootOverlayCounter > 0)
    {
        rootOverlayCounter--;

        if(rootOverlayCounter == 0)
        {
            //XUnmapWindow(dpy, rootOverlay);
            XCompositeUnredirectSubwindows (dpy, root, CompositeRedirectAutomatic);
            XCompositeReleaseOverlayWindow(dpy, rootOverlay);
            rootOverlay = 0;
            selectEvents(root);

        }
    }

    return rootOverlayCounter;
}

QPoint Sky::getBrowsePoint()
{
	return browsePoint;
}

Display* Sky::getDisplay()
{
    return dpy;
}

Window Sky::getRootWindow()
{
    return root;
}

int Sky::addWindow(Window WindowID)
{
    //if (findCloudByWindow(ev.xcreatewindow.window) || findCloudByWindow(ev.xcreatewindow.parent))
    if (findCloudByID(WindowID))
    {
        return 1;
    }

    Cloud* cloud = new Cloud(WindowID, dpy);
    cloud->setDivingLevel(currentDivingLevel);
    cloud->setLiveThumbnailMode(liveThumbnailMode);
    //XSync(dpy, false);
    return addCloud(cloud);
}

int Sky::removeWindow(Window WindowID)
{
    Cloud* cloud = NULL;
    if((cloud = findCloudByCoreWindow(WindowID)))
    {
        //TODO : we shuld set the window to a crash state ?
        cloud->coreWindowID = -1;
        cout << "Closing window : " << WindowID << " With frame : " << cloud->windowID << endl << flush;
        XDestroyWindow(dpy, cloud->windowID);
        XSync(dpy, false);
        //return 0;
    }
    else if((cloud =findCloudByWindow(WindowID)))
    {
        cout << "Closing frame : " << cloud->windowID << endl  << flush;;
        //XDestroySubwindows(dpy, WindowID);
        for(int i = 0; i < clouds[currentDivingLevel].size(); i++)
        {
            if(clouds[currentDivingLevel].at(i)->getCoreWindowID() == WindowID ||
                    clouds[currentDivingLevel].at(i)->getWindowID() == WindowID)
            {
                XSync(dpy, false);

                //FIX
                if(clouds[currentDivingLevel].at(i))
                {
                    //delete clouds[currentDivingLevel].at(i);
                }

                clouds[currentDivingLevel].erase(clouds[currentDivingLevel].begin()+i);
                //updateSky();
                break;
                //return 0;
            }
        }
    }

    if(cloud == cloudSelected)
    {
        deSelectCloud();
    }

    for(int i =0; i < cloudsHistory.size(); i++)
    {
        if (cloud == cloudsHistory.at(i))
        {
            cloudsHistory.remove(i);
            break;
        }
    }

    skyscope->Hide();
    selectEvents(root);
    XSync(dpy, false);
    updateSky(true);
    return 1;
}

QPoint Sky::findAvailableSpace(Cloud* cloud)
{
    if(clouds[currentDivingLevel].size())
    {
        Cloud* lastCloud = clouds[currentDivingLevel].at(clouds[currentDivingLevel].size()-1);
        if (spaceSize.width() - (lastCloud->getPosition().x()+browsePoint.x() +lastCloud->getSize().width()) > cloud->getSize().width())
        {
            //return QPoint(lastCloud->getPosition().x()+browsePoint.x() +lastCloud->getSize().width(), 0);
        }
        else
        {
            //return QPoint(0, lastCloud->getPosition().y()+browsePoint.x() +lastCloud->getSize().height());
        }
    }
    return QPoint(clouds[currentDivingLevel].size()*400, 0);
    return QPoint(0,0);
}

Cloud* Sky::findCloudByID(Window WindowID)
{
    for(int i = 0; i < clouds[currentDivingLevel].size(); i++)
    {
        if(clouds[currentDivingLevel].at(i)->getWindowID() == WindowID ||
           clouds[currentDivingLevel].at(i)->getCoreWindowID() == WindowID)
        {
            return clouds[currentDivingLevel].at(i);
        }
    }
    return NULL;
}


Cloud* Sky::findCloudByWindow(Window WindowID)
{
    for(int i = 0; i < clouds[currentDivingLevel].size(); i++)
    {
        //if(clouds[currentDivingLevel].at(i)->getWindowID() == WindowID ||
           //clouds[currentDivingLevel].at(i)->getCoreWindowID() == WindowID)
        if (clouds[currentDivingLevel].at(i)->getWindowID() == WindowID)
        {
            return clouds[currentDivingLevel].at(i);
        }
    }
    return NULL;
}

Cloud* Sky::findCloudByCoreWindow(Window WindowID)
{
    for(int i = 0; i < clouds[currentDivingLevel].size(); i++)
    {
        if (clouds[currentDivingLevel].at(i)->getCoreWindowID() == WindowID)
        {
            return clouds[currentDivingLevel].at(i);
        }
    }
    return NULL;
}




void Sky::Exit()
{
    //QApplication::exit(0);
    exit(0);
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

    if(backgroundPath.size())
    {
        updateBackground(true);
    }

    //calcualteUniverseCenter();

	return 0;
}

int Sky::selectEvents(Window windowID)
{
    Cursor cursor;
    Pixmap csr;
    XColor xcolor;
    static char csr_bits[] = {0x00};

    csr= XCreateBitmapFromData(dpy,windowID,csr_bits,1,1);
    cursor= XCreatePixmapCursor(dpy,csr,csr,&xcolor,&xcolor,1,1);

    XGrabKey(dpy, AnyKey, AnyModifier, windowID, True, GrabModeSync, GrabModeSync);
    XGrabPointer(dpy, windowID, True, PointerMotionMask|ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);
    XSelectInput(dpy, windowID, StructureNotifyMask | SubstructureNotifyMask | SubstructureRedirectMask | KeyPressMask |KeyReleaseMask | PointerMotionMask);

    //XSelectInput(dpy, windowID, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | PointerMotionHintMask | StructureNotifyMask | ResizeRedirectMask | SubstructureNotifyMask | SubstructureRedirectMask | GraphicsExpose);
    //XGrabButton(dpy, AnyButton, AnyModifier, windowID, True, SubstructureNotifyMask, GrabModeAsync, GrabModeAsync, None, None);
    //XGrabButton(dpy, AnyButton, AnyModifier, windowID, True, ButtonPressMask | ButtonReleaseMask, GrabModeSync, GrabModeSync, None, None);
    ////XGrabButton(dpy, AnyButton, AnyModifier, windowID, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);
    ////XGrabButton(dpy, AnyButton, AnyModifier, windowID, True, ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None);
    ////XGrabPointer(dpy, windowID, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | EnterWindowMask | LeaveWindowMas,
    ///              GrabModeSync, GrabModeSync, None, None, CurrentTime);
    //XGrabButton(dpy, 3, Mod1Mask, windowID, True, ButtonPressMask, GrabModeAsync, GrabModeAsync, None, None);

    return 0;
}


int Sky::processEvents()
{
    ////XAutoRepeatOff(dpy);
    //cout << "CASE : " << XkbSetDetectableAutoRepeat(dpy, 1, 0) << endl;
    selectEvents(root);


    while(true)
    {
        XNextEvent(dpy, &ev);
        //if (ev.type == KeyRelease || ev.type == KeyPress)
        {
            cout << "Event : " << XEvent_Name[ev.type] << " -> type :" << ev.type  << endl  << flush;
        }

        if (ev.type == ConfigureNotify)
        {
            Cloud* cloud = findCloudByCoreWindow(ev.xconfigure.window);
            if(cloud)
            {
                cloud->Resize(ev.xconfigure.width, ev.xconfigure.height);
            }

        }
        else if(ev.type == damage_event + XDamageNotify)
        {
            XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent*>(&ev);


            cout << "damage" << endl;
            //if(liveThumbnailMode)
            {
                Cloud* cloud = findCloudByWindow(e->drawable);
                if(cloud)
                {
                    cloud->updateThumbnail();
                }
                updateScopes();
            }

            XDamageSubtract( dpy, e->damage, None, None );
        }
        else if (ev.type == ClientMessage)
        {
            //cout << ev.xclient.window << endl;
            //cout << ev.xclient.data.b<< endl;
            //cout << ev.xclient.data.l<< endl;
            //cout << ev.xclient.data.s<< endl;
        }

        else if (ev.type == PropertyNotify)
        {
            cout << "PROPERTYYYYYYYYYYY NOTIFY" << endl << flush;
        }
        else if (ev.type == Expose)
        {
            cout << "EXPOSEEEEEEEEEE : " << ev.xexpose.window << endl;
            Cloud* cloud = findCloudByWindow(ev.xexpose.window);
            if(cloud)
            {
                //cloud->drawCloud();
            }

        }
        else if (ev.type == ReparentNotify)
        {
            //cout <<"reparent wndow req : " <<  ev.xreparent.window << endl;
            //cout <<"reparent wndow parent req : " <<  ev.xreparent.parent << endl;
        }
        /*else if(ev.type == GraphicsExpose && ev.xexpose.window == root)
        {
            XColor bgSolidColor;
            char bgSolidColorSelection[] = "#0059b3";
            Colormap colormap = DefaultColormap(dpy, 0);
            GC green_gc = XCreateGC(dpy, root, 0, 0);
            XParseColor(dpy, colormap, bgSolidColorSelection, &bgSolidColor);
            XAllocColor(dpy, colormap, &bgSolidColor);
            XSetForeground(dpy, green_gc, bgSolidColor.pixel);
            XFillRectangle(dpy, root, green_gc, 1, 1, getSize().width(), getSize().height());
        }*/
        else if(ev.type == CreateNotify)
        {
            cout << "New window !!!" << ev.xcreatewindow.window << endl;
            //addWindow(ev.xcreatewindow.window);
        }
        else if (ev.type == MapRequest)
        {
            cout << "MAP REQUEST : " << ev.xmaprequest.window << endl;
            Cloud* cloud = findCloudByID(ev.xmaprequest.window);
            if(cloud)
            {
                if(cloud->isHidden())
                {
                    cloud->Show(false);
                    updateSky(true);
                }
            }
            else
            {
                addWindow(ev.xmaprequest.window);
            }
        }
        /*else if (ev.type == MapNotify)
        {
            Cloud* cloud = findCloudByID(ev.xmap.window);
            if(cloud)
            {
                if(cloud->isHidden())
                {
                    cloud->Show(false);
                    updateSky(true);
                }
            }
        }
        else if (ev.type == UnmapNotify)
        {
            cout << "unmap " << ev.xunmap.window << endl;
            Cloud* cloud = findCloudByCoreWindow(ev.xunmap.window);
            if (cloud)
            {
                if(!cloud->isHidden())
                {
                    cloud->Hide(false);
                    updateSky(true);
                }
            }
        }*/
        else if (ev.type == DestroyNotify)
        {
            removeWindow(ev.xdestroywindow.window);
        }
        else if (ev.type == KeyRelease)
        {
            keyReleaseEvent(ev.xkey);
        }
        else if (ev.type == KeyPress)
        {
            keyPressEvent(ev.xkey);
        }
        else if(ev.type == ButtonPress)
        {
            buttonPressEvent(ev);
        }
        else if(ev.type == ButtonRelease)
        {
            buttonReleaseEvent(ev);
        }
        else if(ev.type == MotionNotify)
        {
            mouseMoveEvent(ev.xmotion);
        }

    }
    return 0;
}

int Sky::setLiveThumbnailMode(bool cond)
{
    liveThumbnailMode = cond;

    if(cond)
    {
        if(!dpy)
        {
            cerr << "[Error] : null Display object while init Damage Query Extenstion" << endl << flush;
            return false;
        }
        if(!XDamageQueryExtension(dpy, &damage_event, &damage_error ))
        {
            cerr << "[Error] : Damage Query Extenstion not loaded " << endl << flush;
            return false;
        }
    }


    for (int i =0; i < clouds[currentDivingLevel].size(); i++)
    {
        clouds[currentDivingLevel].at(i)->setLiveThumbnailMode(cond);
    }

    return 0;
}




int Sky::setHighlightedCloud(Cloud* cloud)
{
    if(highlightedCloud)
    {
        highlightedCloud->setHighlight(false);
        highlightedCloud = NULL;
    }

    if(cloud)
    {
        highlightedCloud = cloud;
        highlightedCloud->setHighlight(true);
        //////highlightedCloud->Raise();
        //updateSky();
        return 0;
    }

    updateSky(true);
    return 1;
}


const map<unsigned int, QVector<Cloud*> >& Sky::getClouds()
{
	return clouds;
}

const QVector<Cloud *> &Sky::getCurrentClouds()
{
	return clouds[currentDivingLevel];
}

int Sky::calcualteUniverseCenter()
{
    float percent = 0.01;
    universeCenter.setX( (getSize().width()/2) - (getSize().width()*percent));
    universeCenter.setY( (getSize().height()/2) - (getSize().height()*percent));

    universeCenter.setWidth((getSize().width()*percent)*2);
    universeCenter.setHeight((getSize().height()*percent)*2);

    return 0;
}

int Sky::resetCursor()
{
    int universeXMidPoint = universeCenter.x()+(universeCenter.width()/2);
    int universeYMidPoint = universeCenter.y()+(universeCenter.height()/2);

    //XUndefineCursor(dpy,root);
    XWarpPointer(dpy, None, root, 0, 0, 0, 0, universeXMidPoint, universeYMidPoint);
    //XDefineCursor(dpy,root,None);
    //XChangeActivePointerGrab(dpy, PointerMotionMask, None, CurrentTime);
    //XUndefineCursor(dpy,root);
    XFlush(dpy);

    /*Pixmap bm_no;
            Colormap cmap;
            Cursor no_ptr;
            XColor black, dummy;
            static char bm_no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};


            cmap = DefaultColormap(dpy, DefaultScreen(dpy));
            XAllocNamedColor(dpy, cmap, "black", &black, &dummy);
            bm_no = XCreateBitmapFromData(dpy, root, bm_no_data, 8, 8);
            no_ptr = XCreatePixmapCursor(dpy, bm_no, bm_no, &black, &black, 0, 0);

            XDefineCursor(dpy, root, no_ptr);
            XFreeCursor(dpy, no_ptr);
            if (bm_no != None)
                    XFreePixmap(dpy, bm_no);
            XFreeColors(dpy, cmap, &black.pixel, 1, 0);*/

    ignoreCursorMovement = true;
    return 0;
}
int Sky::updateHighLights()
{
    QPoint searchPoint(0,0);
    int Direction = 0;
    if(finalBrowsePointCounterY < 0) // UP
    {
        Direction = Direction | UP;
        searchPoint.setY(0);
    }
    else if(finalBrowsePointCounterY > 0) // DOWN
    {
        Direction = Direction | DOWN;
        searchPoint.setX(spaceSize.height());
    }

    if(finalBrowsePointCounterX < 0) // LEFT
    {
        Direction = Direction | LEFT;
        searchPoint.setX(0);
    }
    else if(finalBrowsePointCounterX > 0) // RIGHT
    {
        Direction = Direction | RIGHT;
        searchPoint.setX(spaceSize.width());
    }


    if(!onScreenClouds.size())
    {
        if(highlightedCloud)
        {
            setHighlightedCloud(NULL);
        }
        return 1;
    }


    Cloud* canditateHighlight = highlightedCloud;

    for (int i =0; i < onScreenClouds.size(); i++)
    {
        if(highlightedCloud == onScreenClouds.at(i))
        {
            continue;
        }

        if(!canditateHighlight)
        {
           canditateHighlight = onScreenClouds.at(i);
           continue;
        }

        if(Direction & LEFT)
        {
            if (onScreenClouds.at(i)->getPosition().x() <= canditateHighlight->getPosition().x())
            {
                canditateHighlight = onScreenClouds.at(i);
            }
        }
        else if(Direction & RIGHT)
        {
            if (onScreenClouds.at(i)->getPosition().x() >= canditateHighlight->getPosition().x())
            {
                canditateHighlight = onScreenClouds.at(i);
            }
        }

        if(Direction & UP)
        {
            if (onScreenClouds.at(i)->getPosition().y() <= canditateHighlight->getPosition().y())
            {
                canditateHighlight = onScreenClouds.at(i);
            }
        }
        else if(Direction & DOWN)
        {
            if (onScreenClouds.at(i)->getPosition().y() >= canditateHighlight->getPosition().y())
            {
                canditateHighlight = onScreenClouds.at(i);
            }
        }
    }

    if(!canditateHighlight)
    {
        return 1;
    }


    setHighlightedCloud(canditateHighlight);


    if(canditateHighlight == highlightedCloud)
    {
        for (int i =0; i < onScreenClouds.size(); i++)
        {
            if(highlightedCloud == onScreenClouds.at(i))
            {
                return 0;
            }
        }
    }
    setHighlightedCloud(NULL);
    return 1;

}



int Sky::centerAtCloud(Cloud* cloud)
{
    float targetedX = ceil((getSize().width()-cloud->getSize().width())/(float)2);
    if (targetedX < 0 || targetedX > getSize().width())
    {
        targetedX = cloud->getBorderedPosition().x();

    }
    else
    {
        targetedX = targetedX-cloud->getBorderedPosition().x();
    }

    float targetedY = ceil((getSize().height()-cloud->getSize().height())/(float)2);
    if (targetedY < 0 || targetedY > getSize().height())
    {
        targetedY = cloud->getBorderedPosition().y();
    }
    else
    {
        targetedY = targetedY-cloud->getBorderedPosition().y();
    }

    ////targetedX = cloud->getBorderedPosition().x();
    ////targetedY = cloud->getBorderedPosition().y();

    //targetedX *= -1;
    //targetedY *= -1;


    finalBrowsePointCounterX = targetedX*-1;
    finalBrowsePointCounterY = targetedY*-1;

    //browsePointCounter.setX(targetedX);
    //browsePointCounter.setY(targetedY);

    //FIX : call triggerbrowse
    int oldBrowseDuration = browseDuration;
    //browseDuration = 50;
    browseDuration = ((abs(finalBrowsePointCounterX+finalBrowsePointCounterY)) *40)/abs(browsePoint.x()+browsePoint.y());

    cout << "DURATION : " << browseDuration << endl << flush;

    //updateSky(false);
    triggerBrowseMovement();

    //Browse();
    browseDuration = oldBrowseDuration;

    return 0;
}

void Sky::notifierThreadFunction()
{
    //while(true)
    //{
        ////Mutex.unlock();
        //setThumbnailMode(false);
        //Mutex.lock();
        //Mutex.lock();
    //}

}

void Sky::browsingThreadFunction()
{
    //while(true)
    //{
        ////Mutex.unlock();
        //fix this, make duration and t global and modifiable in a function
        Timer = 0; // start
        int timerStep = 1;

        //setThumbnailMode(true);
        QPoint previousBrowsePointCounter;
        QPoint currentBrowsePointCounter;
        while (Timer <= browseDuration)
        {
            Timer += timerStep;

            currentBrowsePointCounter.setX(ceil(easeInOutSine(Timer, 0, finalBrowsePointCounterX, browseDuration)));
            currentBrowsePointCounter.setY(ceil(easeInOutSine(Timer, 0, finalBrowsePointCounterY, browseDuration)));

            browsePointCounter.setX(currentBrowsePointCounter.x() - previousBrowsePointCounter.x());
            browsePointCounter.setY(currentBrowsePointCounter.y() - previousBrowsePointCounter.y());
            Browse();
            previousBrowsePointCounter.setX(currentBrowsePointCounter.x());
            previousBrowsePointCounter.setY(currentBrowsePointCounter.y());


            if(currentBrowsePointCounter.x() == finalBrowsePointCounterX &&
               currentBrowsePointCounter.y() == finalBrowsePointCounterY)
            {
                break;
            }

            //usleep(timerStep);
        }

        //cout << finalBrowsePointCounterX << ":" << finalBrowsePointCounterY << endl;

        //setThumbnailMode(false);
        //Mutex.lock();
        //Mutex.lock();
    //}

        finalBrowsePointCounterY = 0;
        finalBrowsePointCounterX = 0;
}



bool Sky::isOnScreen(Cloud* cloud)
{
    updateOnScreenClouds();
    for (int i =0; i < onScreenClouds.size(); i++)
    {
        if ( onScreenClouds.at(i) == cloud)
        {
            return true;
        }
    }
    return false;
}

int Sky::historySelect()
{
    //1- if on screen then push to back
    if (((isOnScreen(highlightedCloud) && CONFIG_ONSCREENHISTORY) ||
        !CONFIG_ONSCREENHISTORY) && altCounter == 0 )
    {
        for(int i =0; i < cloudsHistory.size(); i++)
        {

            if (cloudsHistory.at(i)== highlightedCloud)
            {
                cloudsHistory.removeAt(i);
            }
        }
        cloudsHistory.push_back(highlightedCloud);
    }


    //2- highlight cloud at history.size()-altCounter if on screen, otherwise increase and do the same
    if(cloudsHistory.size() > 1)
    {
        Cloud* cloud = cloudsHistory.at(altCounter%cloudsHistory.size());
        if ((isOnScreen(cloud) && CONFIG_ONSCREENHISTORY) ||
             !CONFIG_ONSCREENHISTORY)
        {
            setHighlightedCloud(cloud);
            //cloudsHistory.remove(cloudsHistory.size()-(altCounter%cloudsHistory.size()));

            if(cloudSelected)
            {
                selectCloud(cloud);
                if(!isOnScreen(cloud))
                {
                    centerAtCloud(cloud);
                }
            }

            scopebrowser->Update();
            altCounter++;
            return 0;
        }
    }
    altCounter++;
    return 1;
}



// call when treshold has been crossed by mouse movement or keyboard press or touch movement
int Sky::triggerBrowseMovement()
{
    if(leftClick && highlightedCloud) // move cloud
    {
        setCurrentOperation(MOVE);
        moveCloud(!Super);
        updateScopes();

    }
    else if(rightClick && highlightedCloud) //resize cloud
    {
        setCurrentOperation(RESIZE);
        resizeCloud(!Super);
        updateScopes();
    }
    else
    {
        updateSky(Control);
        skynotifier->Update();
        //thread(&skyNotifier::Update, skynotifier).detach();
        ////thread(&skyNotifier::Update, skynotifier).detach();
        //Mutex.unlock();
    }

    mouseMoveCounter = 0;
    //browsePointCounter = QPoint(0,0);
    finalBrowsePointCounterX = finalBrowsePointCounterY = 0;
    //currentBrowsePointCounterX = currentBrowsePointCounterY = 0;
    return 0;
}


bool Sky::isSpaceMaximized(Cloud* cloud)
{
    if(cloud->getSize().width() == getSpaceSize().width() && cloud->getSize().height() == getSpaceSize().height())
    {
        return true;
    }

    return false;
}

bool Sky::isScreenMaximized(Cloud* cloud)
{
    //cout << cloud->getSize().width() << ":" << cloud->getSize().height() << endl;
    //cout << getSize().width() << ":" << getSize().height() << endl;
    if(cloud->getSize().width() == getSize().width() && cloud->getSize().height() == getSize().height())
    {
        return true;
    }

    return false;
}


int Sky::callThread()
{
    thread(&Sky::browsingThreadFunction, this).detach();
    return 0;
}

int Sky::updateScopes()
{
    if(skyscope->isHidden())
    {
        if(scopebrowser->isHidden())
        {
            scopebrowser->Show(true);
        }
        scopebrowser->Raise();
        scopebrowser->Update();
    }
    else
    {
        if(!scopebrowser->isHidden())
        {
            scopebrowser->Hide();
        }

        skyscope->Raise();
        skyscope->Update();
    }
    return 0;
}

int Sky::updateSky(bool ignoreScope)
{
    if(!ignoreScope)
    {
        browsingThreadFunction();
    }

    // highlight next cloud
    updateOnScreenClouds();
    updateHighLights();
    //skynotifier->Update();
    updateScopes();

    mouseMoveCounter = 0;
    //browsePointCounter = QPoint(0,0);

    return 0;
}

int Sky::updateOnScreenClouds()
{
    for( int i = 0; i < clouds[currentDivingLevel].size(); i++)
    {
        QPoint position =  clouds[currentDivingLevel].at(i)->getPosition();
        //QSize size =  clouds[currentDivingLevel].at(i)->getSize();
        int xAxisVisabiliy = position.x()+ clouds[currentDivingLevel].at(i)->getSize().width();
        int yAxisVisabiliy = position.y()+ clouds[currentDivingLevel].at(i)->getSize().height();

        //if ( ((position.x()  >= 0 &&  xAxisVisabiliy/2 < getSize().width()) && (position.y() >= 0 &&  yAxisVisabiliy/2 < getSize().height()))
//            ||((position.x()+ (size.width()/2) < size.width() &&  xAxisVisabiliy >=  0) && ( position.y()+ (size.height()/2) < size.height() &&  yAxisVisabiliy/2 >= 0 )) )
        if ( (position.x()  >= 0 &&  xAxisVisabiliy < getSize().width()) && (position.y() >= 0 &&  yAxisVisabiliy < getSize().height()))
        {
            for( int k = 0; k < onScreenClouds.size(); k++)
            {
                if(onScreenClouds.at(k) == clouds[currentDivingLevel].at(i))
                {
                    onScreenClouds.remove(k);
                    break;
                }
            }
            onScreenClouds.push_back(clouds[currentDivingLevel].at(i));
        }
        else
        {
            for( int k = 0; k < onScreenClouds.size(); k++)
            {
                if(onScreenClouds.at(k) == clouds[currentDivingLevel].at(i))
                {
                    onScreenClouds.remove(k);
                    break;
                }
            }
        }
    }

    if(CONFIG_GRAVITY && onScreenClouds.size())
    {
        ///int screenClouds = onScreenClouds.size() +1;
        //int screenClouds = 2;
        //browseFactor = fixedBrowseFactor / screenClouds;
    }
    else
    {
        //browseFactor = fixedBrowseFactor;
    }

    //cout << "SIZE : " << onScreenClouds.size() << endl;
    return 0;
}

int Sky::Browse() // also browsing
{
    QPoint oldBrowsePoint = browsePoint;
    browsePoint += browsePointCounter;

    if(browsePoint.x() < 0)
    {
        //browsePointCounter.setX(browsePointCounter.x()+(browsePoint.x()*-1));
        //cout << "X LESS THAN 0 : " << browsePoint.y() << endl << flush;
        browsePoint.setX(0);
    }
    else if (browsePoint.x() + getSize().width() > spaceSize.width())
    {
        browsePoint.setX(spaceSize.width() - getSize().width());
        //browsePointCounter.setX(0);
        //cout << "X MORE THAN WIDTH " << endl << flush;
    }

    if(browsePoint.y() < 0)
    {
        //browsePointCounter.setY(browsePointCounter.y()+(browsePoint.y()*-1));
        //cout << "Y LESS THAN 0 : " <<browsePoint.y() << endl << flush;
        browsePoint.setY(0);
    }
    else if (browsePoint.y() + getSize().height() > spaceSize.height())
    {
        browsePoint.setY(spaceSize.height() - getSize().height());
        //cout << "Y MORE THAN HEIGHT " << endl << flush;
    }

    if (browsePoint - oldBrowsePoint != browsePointCounter)
    {
        browsePointCounter = browsePoint - oldBrowsePoint;
    }

    //if(resizeClouds(direction) && moveClouds(direction))
    if(oldBrowsePoint != browsePoint)
    {
        if(Control)
        {
            browsePoint = oldBrowsePoint;
        }
    }

    // update clouds
    for (map<unsigned int, QVector<Cloud*>>::iterator it = clouds.begin() ; it != clouds.end(); ++it)
    {
        if(it->first != currentDivingLevel)
        {
            //FIX other level should also be moving with a slower factor, as long as they are not hidden levels ?
            continue;
        }

        for( int i = 0; i < it->second.size(); i++)
        {
            QPoint position = it->second.at(i)->getPosition()  + (browsePointCounter*-1);
            it->second.at(i)->Move(position);
        }
    }

    //update background
    moveBackground();

    return 0;
}

void Sky::Execute(string cmd, string arg)
{
    if(fork() == 0)
    {
       // if(fork() == 0)
        {
            if(dpy)
            {
               close(ConnectionNumber(dpy));
            }

            setsid();
            char *cstr = new char[cmd.length() + 1];
            strcpy(cstr, cmd.c_str());

            char* arg2[] = {cstr, NULL, NULL};
            execvp(arg2[0],arg2);
            delete [] cstr;
        }
        exit(0);
    }
}

int Sky::setCurrentOperation(unsigned int op)
{
    currentOperation = op;
    return 0;
}


