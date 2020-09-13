#include "Cloud.hpp"
#include <QPainter>

#include <iostream>
using namespace std;

QPixmap* bg;
Cloud::Cloud(int windowID, Display* XDisplay)
{
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);
    ////setWindowFlags(Qt::X11BypassWindowManagerHint);
	highlighted= false;
	//setWindowFlags();
	//setMouseTracking(false);
    ////setEnabled(false);

    ////setAttribute(Qt::WA_TransparentForMouseEvents);

	bg = new QPixmap("nice.jpg");

    windowID = windowID;
    XDisplay = XDisplay;
}

void Cloud::paintEvent(QPaintEvent * event)
{

	//QImage image(size(), QImage::Format_ARGB32_Premultiplied);
    ////QPainter painter(this);

    ////painter.fillRect(0, 0, width(), height(),QBrush(Qt::red));
	if(highlighted)
	{
        ////Highlight(&painter);
	}


    /////painter.drawPixmap(0, 0, width(), height(), *bg);
}


// highlight or draw border
int Cloud::Highlight(QPainter* painter)
{
    ////QPen pen;
    ////pen.setWidth(2);
    ////pen.setColor(Qt::blue);
    ////painter->setPen(pen);
    ////painter->drawRect(10, 10, width()-10, height()-10);

	return 0;
}

int Cloud::setHighlight(bool cond)
{
	highlighted = cond;
    ////update();
	return cond;
}

bool Cloud::isHighlighted()
{
	return highlighted;
}

void Cloud::mouseMoveEvent(QMouseEvent* event)
{
	//event->ignore();
}


/*bool Cloud::event(QEvent *event)
{
	return false;
}*/

void Cloud::enterEvent(QEvent *)
{

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
    return XResizeWindow(XDisplay, windowID, Width, Height);
}

int Cloud::Move(QPoint pos)
{
    return XMoveWindow(XDisplay, windowID, pos.x(), pos.y());
}

int Cloud::Move(int X,int Y)
{
    return XMoveWindow(XDisplay, windowID, X, Y);
}

QPoint Cloud::getPosition()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return QPoint(attributes.x, attributes.y);
}

QPoint Cloud::getSize()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return QPoint(attributes.width, attributes.height);
}

bool Cloud::isLowered()
{
    XWindowAttributes attributes;
    XGetWindowAttributes(XDisplay, windowID, &attributes);
    return attributes.map_state; // ????????
}
