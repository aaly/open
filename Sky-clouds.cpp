#include "Sky.hpp"

/**********************************
 *
 * Clouds Manipulations
 *
***********************************/

int Sky::deSelectCloud()
{
    cloudSelected = 0;

    XSetInputFocus(dpy, root,  RevertToNone, CurrentTime);
    //XUngrabPointer(dpy, CurrentTime);
    XSync(dpy, false);
    //XAllowEvents(dpy, AsyncPointer, ev.xmotion.time);
    selectEvents(root);
    //currentOperatingMode = BROWSING;

    Control = false;

    return 0;
}

int Sky::selectCloud(Cloud* cloud)
{
    if(highlightedCloud != cloud)
    {
        setHighlightedCloud(cloud);
    }

    cloudSelected = cloud;
    cloud->Raise();
    XUngrabPointer(dpy, CurrentTime);

    Cursor c;
    c = XCreateFontCursor(dpy, XC_arrow);
    XDefineCursor(dpy, root, c);
    XSync(dpy, false);
    XAllowEvents(dpy, AsyncPointer, ev.xmotion.time);

    XSetInputFocus(dpy, cloud->getCoreWindowID(),  RevertToNone, CurrentTime);

    //currentOperatingMode == SELECTING;

    return 0;
}


int Sky::maximizeCloud(Cloud* cloud, bool fullSpace)
{
    if(fullSpace)
    {
        cloud->Resize(getSpaceSize().width(), getSpaceSize().height());
        //cloud->Move(0,0);
    }
    else
    {
        cloud->Resize(getSize().width(), getSize().height());
        cloud->Move(0,0);
    }

}

int Sky::resizeCloud(bool resizeOnSceen)
{
    QSize currentSize = highlightedCloud->getSize();
    QSize newSize(0,0);

    unsigned int  cloudControlSpeed = 20;

    if(finalBrowsePointCounterX < 0)
    {
        newSize.setWidth(-1*cloudControlSpeed);
    }
    else if(finalBrowsePointCounterX > 0)
    {
        newSize.setWidth(cloudControlSpeed);
    }

    if(finalBrowsePointCounterY < 0)
    {
        newSize.setHeight(-1*cloudControlSpeed);
    }
    else if(finalBrowsePointCounterY > 0)
    {
        newSize.setHeight(cloudControlSpeed);
    }

    newSize += currentSize;

    if(resizeOnSceen)
    {
        if (newSize.width() > getSize().width())
        {
            newSize.setWidth(getSize().width());
        }
        else if(newSize.width()  < 0)
        {
            newSize.setWidth(20);
        }

        if (newSize.height() > getSize().height())
        {
            newSize.setHeight(getSize().height());
        }
        else if(newSize.height()  < 0)
        {
            newSize.setHeight(20);
        }
    }
    else // move in the whole space
    {
        //calculate if out of space or not
    }

    //resize around center
    QPoint newPosition(highlightedCloud->getPosition().x() - (newSize.width()-currentSize.width())/2,
                       highlightedCloud->getPosition().y()- (newSize.height()-currentSize.height())/2);

    if (newPosition.x() < 0 || newSize.width() >= getSize().width() )
    {
        newPosition.setX(0);
    }

    if (newPosition.y() < 0 || newSize.height() >= getSize().height())
    {
        newPosition.setY(0);
    }

    // resize !
    highlightedCloud->Resize(newSize.width(), newSize.height());
    highlightedCloud->Move(newPosition);


    return 0;
}

int Sky::moveCloud(bool moveOnSceen)
{
    QPoint currentPosition = highlightedCloud->getPosition();
    QPoint newPosition(0,0);

    unsigned int  cloudControlSpeed = 40;

    if(finalBrowsePointCounterX < 0)
    {
        newPosition.setX(-1*cloudControlSpeed);
    }
    else if(finalBrowsePointCounterX > 0)
    {
        newPosition.setX(cloudControlSpeed);
    }

    if(finalBrowsePointCounterY < 0)
    {
        newPosition.setY(-1*cloudControlSpeed);
    }
    else if(finalBrowsePointCounterY > 0)
    {
        newPosition.setY(cloudControlSpeed);
    }

    newPosition += currentPosition;

    QSize highlightedCloudSize = highlightedCloud->getSize();

    if(moveOnSceen)
    {
        if (newPosition.x() + highlightedCloudSize.width() > getSize().width())
        {
            newPosition.setX(getSize().width() - highlightedCloudSize.width());
        }
        else if(newPosition.x()  < 0)
        {
            newPosition.setX(0);
        }

        if (newPosition.y() + highlightedCloudSize.height() > getSize().height())
        {
            newPosition.setY(getSize().height()- highlightedCloudSize.height());
        }
        else if(newPosition.y()  < 0)
        {
            newPosition.setY(0);
        }
    }
    else // move in the whole space
    {
        //calculate if out of space or not
        if (newPosition.x() + highlightedCloudSize.width() > getSpaceSize().width())
        {
            newPosition.setX(getSpaceSize().width() - highlightedCloudSize.width());
        }
        else if(newPosition.x()  < 0)
        {
            newPosition.setX(0);
        }

        if (newPosition.y() + highlightedCloudSize.height() > getSpaceSize().height())
        {
            newPosition.setY(getSpaceSize().height()- highlightedCloudSize.height());
        }
        else if(newPosition.y()  < 0)
        {
            newPosition.setY(0);
        }

        skyscope->setShowScope(false);
        skyscope->Show();

        if(skyscope->getSize().width() > getSpaceSize().width() && skyscope->getSize().height() > getSpaceSize().height())
        {
            scopebrowser->Show();
        }
        else
        {
            scopebrowser->Hide();
        }
    }

    // move !
    highlightedCloud->Move(newPosition);
    return 0;
}


int Sky::centerCloud(Cloud* cloud)
{
    int targetedX = (getSize().width()-cloud->getSize().width())/2;
    if (targetedX < 0 || targetedX > getSize().width())
    {
        targetedX = 0;
    }

    int targetedY = (getSize().height()-cloud->getSize().height())/2;
    if (targetedY < 0 || targetedY > getSize().height())
    {
        targetedY = 0;
    }

    return cloud->Move(targetedX, targetedY);
}

int Sky::closeCloud(Cloud* cloud)
{
    XEvent ke;
    ke.type = ClientMessage;
    ke.xclient.window = cloud->getCoreWindowID();
    ke.xclient.message_type = XInternAtom(dpy, "WM_PROTOCOLS", True);
    ke.xclient.format = 32;
    ke.xclient.data.l[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
    ke.xclient.data.l[1] = CurrentTime;
    return XSendEvent(dpy, cloud->getCoreWindowID(), False, NoEventMask, &ke);
}


int Sky::addCloud(Cloud* cloud)
{
    if(!cloud || cloud == NULL)
    {
        return 1;
    }
    //FIX, check if out of space bouandries and fix
    if(clouds[cloud->getDivingLevel()].size() == 0)
    {
        highlightedCloud = cloud;
    }

    if(cloud->getDivingLevel() != currentDivingLevel)
    {
        //cloud->Lower();
    }
    else
    {
        //cloud->Raise();
    }


    //selectEvents(cloud->getWindowID());


    //cloud->Move((clouds[cloud->getDivingLevel()].size()-1)*100, 0);
    //cloud->Move(0, 0);
    //cloud->Hide();
    //cloud->Resize(600,400);
    //cloud->Show();

    //XSelectInput(dpy, cloud->getWindowID(), StructureNotifyMask | SubstructureNotifyMask);

    clouds[cloud->getDivingLevel()].push_back(cloud);
    cloudsHistory.push_front(cloud);
    return updateSky(true);
}

int Sky::deleteCloud(Cloud* cloud)
{
    if(!cloud || cloud == NULL)
    {
        return 1;
    }

    //clouds[cloud->getDivingLevel()].erase();
    updateSky(true);
    return 0;
}
