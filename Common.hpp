#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
using namespace std;

#include <map>

//#include <QLabel>
#include <QtConcurrent/QtConcurrent>
//#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMapIterator>
#include <QWindow>
//#include <QGraphicsDropShadowEffect>
//#include <QWidget>
//#include <QGraphicsOpacityEffect>

#include<QSize>
#include <QPoint>
#include<QRectF>

#include <Imlib2.h>
#include <QString>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>

//#include <X11/Xutil.h>
extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include  <X11/cursorfont.h>
#include <X11/extensions/XTest.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XI.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

}

#include  <cairo/cairo.h>
#include  <cairo/cairo-xlib.h>

#include <thread>
#include <mutex>
#include <chrono>

#include <limits>

#include <math.h>




#include "dataStructuers.hpp"

//#include  <QtX11Extras/QtX11Extras>

static string XRequestCodeToString(unsigned char request_code)
{
  static const char* const X_REQUEST_CODE_NAMES[] = {
      "",
      "CreateWindow",
      "ChangeWindowAttributes",
      "GetWindowAttributes",
      "DestroyWindow",
      "DestroySubwindows",
      "ChangeSaveSet",
      "ReparentWindow",
      "MapWindow",
      "MapSubwindows",
      "UnmapWindow",
      "UnmapSubwindows",
      "ConfigureWindow",
      "CirculateWindow",
      "GetGeometry",
      "QueryTree",
      "InternAtom",
      "GetAtomName",
      "ChangeProperty",
      "DeleteProperty",
      "GetProperty",
      "ListProperties",
      "SetSelectionOwner",
      "GetSelectionOwner",
      "ConvertSelection",
      "SendEvent",
      "GrabPointer",
      "UngrabPointer",
      "GrabButton",
      "UngrabButton",
      "ChangeActivePointerGrab",
      "GrabKeyboard",
      "UngrabKeyboard",
      "GrabKey",
      "UngrabKey",
      "AllowEvents",
      "GrabServer",
      "UngrabServer",
      "QueryPointer",
      "GetMotionEvents",
      "TranslateCoords",
      "WarpPointer",
      "SetInputFocus",
      "GetInputFocus",
      "QueryKeymap",
      "OpenFont",
      "CloseFont",
      "QueryFont",
      "QueryTextExtents",
      "ListFonts",
      "ListFontsWithInfo",
      "SetFontPath",
      "GetFontPath",
      "CreatePixmap",
      "FreePixmap",
      "CreateGC",
      "ChangeGC",
      "CopyGC",
      "SetDashes",
      "SetClipRectangles",
      "FreeGC",
      "ClearArea",
      "CopyArea",
      "CopyPlane",
      "PolyPoint",
      "PolyLine",
      "PolySegment",
      "PolyRectangle",
      "PolyArc",
      "FillPoly",
      "PolyFillRectangle",
      "PolyFillArc",
      "PutImage",
      "GetImage",
      "PolyText8",
      "PolyText16",
      "ImageText8",
      "ImageText16",
      "CreateColormap",
      "FreeColormap",
      "CopyColormapAndFree",
      "InstallColormap",
      "UninstallColormap",
      "ListInstalledColormaps",
      "AllocColor",
      "AllocNamedColor",
      "AllocColorCells",
      "AllocColorPlanes",
      "FreeColors",
      "StoreColors",
      "StoreNamedColor",
      "QueryColors",
      "LookupColor",
      "CreateCursor",
      "CreateGlyphCursor",
      "FreeCursor",
      "RecolorCursor",
      "QueryBestSize",
      "QueryExtension",
      "ListExtensions",
      "ChangeKeyboardMapping",
      "GetKeyboardMapping",
      "ChangeKeyboardControl",
      "GetKeyboardControl",
      "Bell",
      "ChangePointerControl",
      "GetPointerControl",
      "SetScreenSaver",
      "GetScreenSaver",
      "ChangeHosts",
      "ListHosts",
      "SetAccessControl",
      "SetCloseDownMode",
      "KillClient",
      "RotateProperties",
      "ForceScreenSaver",
      "SetPointerMapping",
      "GetPointerMapping",
      "SetModifierMapping",
      "GetModifierMapping",
      "NoOperation",
  };
  return X_REQUEST_CODE_NAMES[request_code];
}


static int OnXError(Display* display, XErrorEvent* e)
{
  const int MAX_ERROR_TEXT_LENGTH = 1024;
  char error_text[MAX_ERROR_TEXT_LENGTH];
  XGetErrorText(display, e->error_code, error_text, sizeof(error_text));
  return 1;
  cerr << "Received X error:\n"
             << "    Request: " << int(e->request_code)
             << " - " << XRequestCodeToString(e->request_code) << "\n"
             << "    Error code: " << int(e->error_code)
             << " - " << error_text << "\n"
             << "    Resource ID: " << e->resourceid << endl << flush;
  // The return value is ignored.
  return 0;
}


static int OnXIOError(Display* display)
{
    if(display)
    {
        cerr << "XIOError"<< endl << flush;
    }
    return 0;
}


//static const char* XEvent_Name[] = {
static std::map<int, std::string> XEvent_Name = {{0,""}};
/*
   1,"",
   1,"KeyPress",
   1,"KeyRelease",
   1,"ButtonPress",
   1,"ButtonRelease",
   1,"MotionNotify",
   1,"EnterNotify",
   1,"LeaveNotify",
   1,"FocusIn",
   1,"FocusOut",
   1,"KeymapNotify",
   1,"Expose",
   1,"GraphicsExpose",
   1,"NoExpose",
   1,"VisibilityNotify",
   1,"CreateNotify",
   1,"DestroyNotify",
   1,"UnmapNotify",
   1,"MapNotify",
   1,"MapRequest",
   1,"ReparentNotify",
   1,"ConfigureNotify",
   1,"ConfigureRequest",
   1,"GravityNotify",
   1,"ResizeRequest",
   1,"CirculateNotify",
   1,"CirculateRequest",
   1,"PropertyNotify",
   1,"SelectionClear",
   1,"SelectionRequest",
   "SelectionNotify",
   "ColormapNotify",
   "ClientMessage",
   "MappingNotify"
};*/


#endif // COMMON_HPP
