#include "Cloud.hpp"

//FIX all the border issues

Cloud::Cloud(int windowID, Display* XDisplay, bool frame)
{
	highlighted= false;
    Hidden = true;

    BORDER_WIDTH = 1;

    windowChanges = 0;

    this->windowID = windowID;
    this->XDisplay = XDisplay;

    thumbnailMode = false;
    liveThumbnail = true;


    if(frame)
    {
        Frame();
    }
}

int Cloud::updateThumbnail()
{
    if(thumbnailPixmap)
    {
        XFreePixmap(XDisplay, thumbnailPixmap);
    }

    // Check if Composite extension is enabled
     int event_base_return;
     int error_base_return;

     int render_event;
     int render_error;

     if (!XCompositeQueryExtension (XDisplay, &event_base_return, &error_base_return))
     {
         cerr << "No Composite Extension !" << endl;
         return 1;
     }


     if (!XRenderQueryExtension (XDisplay, &render_event, &render_error))
     {
        cerr << "No Render Extension!" << endl;
        return 1;
     }




     // Requests the X server to direct the hierarchy starting at window to off-screen storage
     XCompositeRedirectWindow(XDisplay, windowID, CompositeRedirectAutomatic);
     XCompositeRedirectWindow(XDisplay, coreWindowID, CompositeRedirectAutomatic);
     // Preventing the backing pixmap from being freed when the window is hidden/destroyed
     // If you want the window contents to still be available after the window has been destroyed,
     // or after the window has been resized (but not yet redrawn), you can increment the backing
     // pixmaps ref count to prevent it from being deallocated.
     XSync(XDisplay, false);
     thumbnailPixmap = XCompositeNameWindowPixmap( XDisplay, windowID );


     XWindowAttributes attr;
       Status s = XGetWindowAttributes (XDisplay, windowID, &attr);
       if (s == 0)
         printf ("Fail to get window attributes!\n");

     XRenderPictFormat *format = XRenderFindVisualFormat (XDisplay, attr.visual);
     int width = attr.width;
     int height = attr.height;
     int depth = attr.depth;

    //XRenderPictFormat* format = XRenderFindVisualFormat( XDisplay, DefaultVisual( XDisplay, DefaultScreen(XDisplay) ) );

    XRenderPictureAttributes pa;
    pa.subwindow_mode = IncludeInferiors; // Don't clip child widgets
    Picture pic = XRenderCreatePicture( XDisplay, windowID, format, CPSubwindowMode, &pa );

    cout << "PICturE : " << pic << endl << flush;

    //XRenderComposite( XDisplay, PictOpSrc, pic, None, thumbnailPixmap, 0, 0, 0, 0, 0 , 0, getSize().width(), getSize().height() );
    //XRenderComposite( XDisplay, PictOpSrc, pic, None, thumbnailPixmap, 0, 0, 0, 0, 0 , 0, width, height);
    XCompositeUnredirectWindow(XDisplay, windowID, CompositeRedirectAutomatic);
    XCompositeUnredirectWindow(XDisplay, coreWindowID, CompositeRedirectAutomatic);
    XRenderFreePicture(XDisplay, pic);

    XFlush(XDisplay);

    return 0;
}

// highlight or draw border
/*int Cloud::Highlight(QPainter* painter)
{
    ////QPen pen;
    ////pen.setWidth(2);
    ////pen.setColor(Qt::blue);
    ////painter->setPen(pen);
    ////painter->drawRect(10, 10, width()-10, height()-10);

	return 0;
}*/


string Cloud::getWindowIconifyName()
{
    // https://www.debian.org/doc/manuals/intro-i18n/ch-examples.en.html
    int    status;
    XTextProperty text_prop;
    char **list;
    int    num;

    status = XGetWMIconName(XDisplay, coreWindowID, &text_prop);
    if (!status || !text_prop.value || !text_prop.nitems)
    {
        return string("");
    }

    status = XmbTextPropertyToTextList(XDisplay, &text_prop, &list, &num);
    if (status < Success || !num || !*list)
    {
        return string("");
    }

    XFree(text_prop.value);
    string windowTitle = (char *)strdup(*list);


    XFreeStringList(list);

    return windowTitle;
}


Pixmap Cloud::getWindowIcon()
{
    XWMHints* win_hints = XGetWMHints(XDisplay, coreWindowID);

    if(win_hints)
    {
        Pixmap pixmap = win_hints->icon_pixmap;

        XFree(win_hints);
        return pixmap;
    }

    return 0;
}

QSize Cloud::getIconSize()
{
    Pixmap icon = getWindowIcon();
    if (icon)
    {
        Window R;
        int x = 0;
        int y = 0;
        unsigned int width =0;
        unsigned int height = 0;
        unsigned int border = 0;
        unsigned int depth = 0;


        XGetGeometry(XDisplay, icon, &R, &x, &y, &width, &height, &border, &depth);
        return QSize(width, height);
    }

    return QSize(0, 0);
}


QSize Cloud::getThumbnailSize()
{
    if (thumbnailPixmap)
    {
        Window R;
        int x = 0;
        int y = 0;
        unsigned int width =0;
        unsigned int height = 0;
        unsigned int border = 0;
        unsigned int depth = 0;


        XGetGeometry(XDisplay, thumbnailPixmap, &R, &x, &y, &width, &height, &border, &depth);
        return QSize(width, height);
    }

    return QSize(-1, -1);
}

string Cloud::getWindowName()
{
    // https://www.debian.org/doc/manuals/intro-i18n/ch-examples.en.html
    int    status;
    XTextProperty text_prop;
    char **list;
    int    num;

    status = XGetWMName(XDisplay, coreWindowID, &text_prop);
    if (!status || !text_prop.value || !text_prop.nitems)
    {
        return string("");
    }

    status = XmbTextPropertyToTextList(XDisplay, &text_prop, &list, &num);
    if (status < Success || !num || !*list)
    {
        return string("");
    }

    XFree(text_prop.value);
    string windowTitle = (char *)strdup(*list);


    XFreeStringList(list);

    return windowTitle;
}

int Cloud::setHighlight(bool cond)
{
	highlighted = cond;

    XColor solidColor;
    string colorSelection;
    Colormap colormap = DefaultColormap(XDisplay, 0);

    if(cond)
    {
        colorSelection = "#ff0000";
    }
    else
    {
        colorSelection = "#000000";
    }
    XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
    XAllocColor(XDisplay, colormap, &solidColor);
    XSetWindowColormap(XDisplay, windowID, colormap);
    XSetWindowBorderWidth(XDisplay, windowID, 2);
    XSetWindowBorder(XDisplay, windowID, solidColor.pixel);
	return cond;
}

bool Cloud::isHighlighted()
{
	return highlighted;
}

int Cloud::Raise()
{
    return XRaiseWindow(XDisplay, windowID);
}

int Cloud::Lower()
{
    return XLowerWindow(XDisplay, windowID);
}

int Cloud::Resize(int Width, int Height)
{
    thumbnailPixmap = XCreatePixmap(XDisplay,
                                          windowID,
                                      Width,
                                      Height,
                                      DefaultDepth(XDisplay, 0));
    //Width -= BORDER_WIDTH;
    //Height -= BORDER_WIDTH;

    XResizeWindow(XDisplay, windowID, Width-(BORDER_WIDTH*4), Height-(BORDER_WIDTH*4));
    return XResizeWindow(XDisplay, coreWindowID, Width, Height);
}

int Cloud::Move(QPoint pos)
{
    return XMoveWindow(XDisplay, windowID, pos.x(), pos.y());
}

int Cloud::Move(int X,int Y)
{
    return XMoveWindow(XDisplay, windowID, X-BORDER_WIDTH, Y-BORDER_WIDTH);
}

QPoint Cloud::getPosition()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return QPoint(attributes.x, attributes.y);
}

QPoint Cloud::getBorderedPosition()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return QPoint(attributes.x-BORDER_WIDTH, attributes.y-BORDER_WIDTH);
}


QSize Cloud::getSize()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return QSize(attributes.width+(BORDER_WIDTH*2), attributes.height+(BORDER_WIDTH*2));
}

bool Cloud::isLowered()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return attributes.map_state; // ????????
}


unsigned int Cloud::getWindowID()
{
    return windowID;
}

unsigned int Cloud::getCoreWindowID()
{
    return coreWindowID;
}

int Cloud::Hide(bool subWindows)
{
    if(!Hidden)
    {
        XUnmapWindow(XDisplay, windowID);
        if(subWindows)
        {
           // XUnmapWindow(XDisplay, coreWindowID);
            XUnmapSubwindows(XDisplay, windowID);
        }

        XFlush(XDisplay);
        Hidden = true;
    }
    return 0;
}

bool Cloud::isHidden()
{
    return Hidden;
}

int Cloud::Show(bool subWindows)
{
    if(Hidden)
    {
        XMapWindow(XDisplay, windowID);
        if(subWindows)
        {
            //XMapWindow(XDisplay, coreWindowID);
            XMapSubwindows(XDisplay, windowID);
        }
        XFlush(XDisplay);
        Hidden = false;
    }

    return 0;
}

int Cloud::Frame()
{
    if(!XDisplay || windowID == -1)
    {
        return 1;
    }

  // Visual properties of the frame to create.

  const unsigned long BORDER_COLOR = 0xff0000;
  const unsigned long BG_COLOR = 0xff0000;

  // 1. Retrieve attributes of window to frame.
  XWindowAttributes x_window_attrs;
  XGetWindowAttributes(XDisplay, windowID, &x_window_attrs);
  // 2. Create frame.
  const Window frame = XCreateSimpleWindow( XDisplay, XDefaultRootWindow(XDisplay),
                                            x_window_attrs.x, x_window_attrs.y, x_window_attrs.width, x_window_attrs.height,
                                            BORDER_WIDTH, BORDER_COLOR, BG_COLOR);

  cout << "Frame : " << frame << endl;

  // 4. Add client to save set, so that it will be restored and kept alive if we crash.
  XAddToSaveSet(XDisplay, windowID);
  // 5. Reparent client window.
 // XSync(XDisplay, true);

  //XSync(XDisplay, false);
  XReparentWindow( XDisplay, windowID, frame, 0, 0);  // Offset of client window within frame.
  XMoveWindow(XDisplay, windowID, 0, 0);
  //XResizeWindow(XDisplay, frame, x_window_attrs.width, x_window_attrs.height);
  XSelectInput(XDisplay, frame, StructureNotifyMask | SubstructureNotifyMask | PropertyChangeMask);
  //XSelectInput(XDisplay, windowID, VisibilityChangeMask | SubstructureNotifyMask);
  XSync(XDisplay, false);
  // 7. Save frame handle.
  coreWindowID = windowID;
  windowID = frame;
  Show();

  thumbnailPixmap = XCreatePixmap(XDisplay,
                                        windowID,
                                    getSize().width(),
                                    getSize().height(),
                                    DefaultDepth(XDisplay, 0));

  // 6. Map frame.
  //XMapWindow(XDisplay, frame);
  return windowID;
}

/*
void WindowManager::Unframe(Window w) {
  CHECK(clients_.count(w));

  // We reverse the steps taken in Frame().
  const Window frame = clients_[w];
  // 1. Unmap frame.
  XUnmapWindow(display_, frame);
  // 2. Reparent client window.
  XReparentWindow(
      display_,
      w,
      root_,
      0, 0);  // Offset of client window within root.
  // 3. Remove client window from save set, as it is now unrelated to us.
  XRemoveFromSaveSet(display_, w);
  // 4. Destroy frame.
  XDestroyWindow(display_, frame);
  // 5. Drop reference to frame handle.
  clients_.erase(w);

  LOG(INFO) << "Unframed window " << w << " [" << frame << "]";
}*/


int Cloud::moveAndResize(int x, int y, int width, int height)
{
    XMoveResizeWindow(XDisplay,windowID, x, y, width, height);
    return XResizeWindow(XDisplay, coreWindowID, width, height);
}

int Cloud::setLiveThumbnailMode(bool cond)
{
    if(cond)
    {
        if(!windowChanges)
        {
            // Create a damage handle for the window, and specify that we want an event whenever the
            // damage state changes from not damaged to damaged.
            windowChanges = XDamageCreate(XDisplay, coreWindowID, XDamageReportNonEmpty| XDamageReportBoundingBox |  XDamageReportRawRectangles | XDamageReportDeltaRectangles );
        }
    }
    else
    {
        if(windowChanges)
        {
            XDamageDestroy(XDisplay, windowChanges);
        }
    }

    thumbnailMode = cond;
    return cond;
}

int Cloud::drawCloud()
{
    if (thumbnailMode)
    {
        if(liveThumbnail)
        {
            updateThumbnail();
        }
        gc = XCreateGC(XDisplay, windowID ,0, NULL);
        XCopyArea(XDisplay, thumbnailPixmap, windowID, gc, 0, 0, getSize().width(), getSize().height(), 0, 0);
    }
    return 0;
}
