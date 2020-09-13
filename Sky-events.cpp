#include "Sky.hpp"

/**********************************
 *
 * Input Events
 *
***********************************/




int Sky::keyPressEvent(XKeyEvent ev)
{
    //if (currentOperatingMode == BROWSING)
    if (cloudSelected != 0)
    {

        {
            XSetInputFocus(dpy, cloudSelected->getCoreWindowID(),  RevertToNone, CurrentTime);
            XAllowEvents(dpy, ReplayKeyboard, ev.time);
            XFlush(dpy);
        }
    }
    else
    {
        XAllowEvents(dpy, AsyncKeyboard, ev.time);
    }


    //KeySym ks = XKeycodeToKeysym(dpy, ev.keycode, 0);
    KeySym ks = XkbKeycodeToKeysym(dpy, ev.keycode, 0, 0);
    cout << "KS :  " << ks << endl;

    /*const char *s;
    if (ks != NoSymbol)
    {
        s = XKeysymToString (ks);
    }*/


    Control =  (ks == XK_Control_L)  || (ks== XK_Control_R) || Control;
    Alt = (ks == XK_Alt_L)  || (ks== XK_Alt_R) || Alt;
    Shift = (ks == XK_Shift_L)  || (ks== XK_Shift_R) || Shift;
    Super = (ks == XK_Super_L) || (ks == XK_Super_R) || Super;


    if (ks == XK_f && Super )
    {
        if(isScreenMaximized(highlightedCloud))
        {
            maximizeCloud(highlightedCloud, true);
        }
        else // space or not screen maximized
        {
            maximizeCloud(highlightedCloud);
        }

        superCounter++;
    }
    else if ((ks == XK_Up || ks == XK_Down || ks == XK_Left || ks== XK_Right) && !cloudSelected)
    {
        if(ks == XK_Right)
        {

            finalBrowsePointCounterX = (finalBrowsePointCounterX+(browseFactor*keyboardBrowseAceleration));
        }

        if(ks == XK_Left)
        {
            finalBrowsePointCounterX = (finalBrowsePointCounterX-(browseFactor*keyboardBrowseAceleration));
        }

        if(ks == XK_Up)
        {
            finalBrowsePointCounterY = (finalBrowsePointCounterY-(browseFactor*keyboardBrowseAceleration));
        }

        if(ks == XK_Down)
        {
            finalBrowsePointCounterY = (finalBrowsePointCounterY+(browseFactor*keyboardBrowseAceleration));
        }

        if(!Control)
        {
            if (keyboardBrowseAceleration < browseMoveThreshold)
            {
                keyboardBrowseAceleration++;
            }
        }
        //if( (!browsePointCounter.x() && !browsePointCounter.y()) || Control)
        {
            //updateSky(Control);
            triggerBrowseMovement();
        }

    }
    else if (ks == XK_Return && !cloudSelected)
    {
        if(!cloudSelected && highlightedCloud)
        {
            selectCloud(highlightedCloud);
        }
    }
    else if (ks == XK_Escape && Super)
    {
        if(cloudSelected)
        {
            deSelectCloud();
        }
        superCounter++;
    }
    else if (ks == XK_Tab && Alt)
    {
        historySelect();
    }
    else if(ks == XK_q && Super)
    {
        Exit();
    }
    else if(ks == XK_w && Super)
    {
        if(highlightedCloud)
        {
            closeCloud(highlightedCloud);
            setHighlightedCloud(NULL);
            deSelectCloud();
        }
    }
    else if(ks== XK_equal && Super)
    {
        // also center in the middle if less than 100% size factor otherwise 0,0
        int zoomFactorMaxLimit = 200;
        if(skyscope->getSizeFactor() < zoomFactorMaxLimit)
        {

            skyscope->setSizeFactor(skyscope->getSizeFactor()+20);
            if(skyscope->isHidden())
            {
                skyscope->Show();
            }
            skyscope->Update();
            superCounter++;
        }
        centerCloud(skyscope);

    }
    else if( (ks== XK_minus) && Super)
    {
        int zoomFactorMinLimit = 50;
        if(skyscope->getSizeFactor() > zoomFactorMinLimit)
        {
            skyscope->setSizeFactor(skyscope->getSizeFactor()-10);
            //skyscope->Update();
            if(skyscope->isHidden())
            {
                skyscope->Show();
            }
            skyscope->Update();
            superCounter++;
        }
        centerCloud(skyscope);

    }
    else if(ev.keycode == XKeysymToKeycode(dpy,'r') && ev.state & Mod4Mask)
    {
        superCounter++;
        Execute("/usr/bin/dmenu_run");
        //Execute("/usr/bin/rofi -show run -font 'snap 10' -fg '#505050' -bg '#000000' -hlfg '#ffb964' -hlbg '#000000' -o 60 -opacity 80");
    }


    return 0;
}


int Sky::keyReleaseEvent(XKeyEvent ev)
{
    KeySym ks = XKeycodeToKeysym(dpy, ev.keycode, 0);
    const char *s;
    if (ks != NoSymbol)
    {
        s = XKeysymToString (ks);
    }

    cout << endl << "Char : " << s << endl << flush;
    cerr << ks << endl << flush;

    //if (ev.xkey.subwindow != None && cloudSelected)
    if (cloudSelected)
    {
        XSetInputFocus(dpy, highlightedCloud->getWindowID(), RevertToNone, CurrentTime);
        XSync(dpy, False);
        XAllowEvents(dpy, ReplayKeyboard, ev.time);
        XFlush(dpy); /* don't forget! */
    }
    else
    {
        XAllowEvents(dpy, AsyncKeyboard, ev.time);
    }


    if ( (ks == XK_Super_L) || (ks == XK_Super_R))
    {
        if(!superCounter)
        {
            if(skyscope->isHidden())
            {
                if(skyscope->getSizeFactor() > 100)
                {
                    scopebrowser->Show();
                }
                skyscope->Show();
            }
            else
            {
                skyscope->Hide(true);
            }
            updateSky(false);
        }
        superCounter = 0;
        Super = false;
    }

    if ( (ks == XK_Control_L)  || (ks== XK_Control_R))
    {
        Control = false;
        controlCounter = 0;
    }

    if ((ks == XK_Alt_L)  || (ks== XK_Alt_R))
    {
        if(altCounter)
        {
            if(!isOnScreen(highlightedCloud))
            {
                centerAtCloud(highlightedCloud);
            }
        }

        Alt = false;
        altCounter = 0;
    }

    if ( (ks == XK_Shift_L)  || (ks== XK_Shift_R))// || (ev.xkey.state & ControlMask))
    {
        Shift = false;
        shiftCounter = 0;
    }

    if ((ks == XK_Up || ks == XK_Down || ks == XK_Left || ks== XK_Right) && !cloudSelected)
    {
        keyboardBrowseAceleration = 1;
    }

    return 0;
}


int Sky::buttonPressEvent(XEvent ev)
{
    if(!cloudSelected)
    {
        leftClick += (ev.xbutton.button == Button1 ? 1 : leftClick);
        rightClick += (ev.xbutton.button == Button3 ? 1 : rightClick);
    }
    else
    {
        XAllowEvents(dpy, ReplayPointer, ev.xmotion.time);
        XFlush(dpy);
    }
    return 0;
}

int Sky::buttonReleaseEvent(XEvent ev)
{
    if(!cloudSelected)
    {
        if( ev.xbutton.button == Button1)
        {
            if(currentOperation != MOVE && currentOperation != RESIZE)
            {
                if(cloudSelected && (cloudSelected != findCloudByCoreWindow(ev.xbutton.subwindow)) )
                {
                    setHighlightedCloud(findCloudByCoreWindow(ev.xbutton.subwindow));
                }

                if(highlightedCloud)
                {
                    selectCloud(highlightedCloud);
                }
            }
            leftClick = false;
        }
        else if( ev.xbutton.button == Button3)
        {
            rightClick = false;
        }

        setCurrentOperation(SCOPE);
    }
    else
    {
        setCurrentOperation(SELECT);
    }

   /* cout << ev.xbutton.button << endl << flush;
    if(!cloudSelected)
    {
        XUngrabPointer(dpy, CurrentTime);
        XSync(dpy, false);
        XAllowEvents(dpy, AsyncPointer, ev.xmotion.time);
    }
    else
    {
        XAllowEvents(dpy, ReplayPointer, ev.xmotion.time);
        XFlush(dpy);
    }*/

    return 0;
}



int Sky::mouseMoveEvent(XMotionEvent ev)
{
    if (ignoreCursorMovement)
    {
        ignoreCursorMovement = false;
        return 1;
    }

    //update scoping position
    if(!cloudSelected)
    {
        XAllowEvents(dpy, AsyncPointer, ev.time);
        //finalBrowsePointCounter.setX(0);
        //finalBrowsePointCounter.setY(0);
        //finalBrowsePointCounter = currentBrowsePointCounter;

        int universeXMidPoint = universeCenter.x()+(universeCenter.width()/2);
        int universeYMidPoint = universeCenter.y()+(universeCenter.height()/2);

        if(ev.x_root != oldMousePosition.x())
        {
            ev.x_root > universeXMidPoint ? finalBrowsePointCounterX += browseFactor :
                                            finalBrowsePointCounterX += browseFactor*-1;

//            if (ev.x_root > universeXMidPoint)
//            {
//                finalBrowsePointCounterX += browseFactor;
//            }
//            else if (ev.x_root < universeXMidPoint)
//            {
//                finalBrowsePointCounterX -= browseFactor;
//            }
        }

        if(ev.y_root != oldMousePosition.y())
        {
            ev.y_root > universeYMidPoint ? finalBrowsePointCounterY += browseFactor :
                                            finalBrowsePointCounterY += browseFactor*-1;

//            if(ev.y_root > universeYMidPoint)
//            {
//                finalBrowsePointCounterY += browseFactor;
//            }
//            else if (ev.y_root < universeYMidPoint)
//            {
//                finalBrowsePointCounterY -= browseFactor;
//            }
        }

        if(++mouseMoveCounter == browseMoveThreshold)
        {
            triggerBrowseMovement();
        }

        oldMousePosition.setX(ev.x_root);
        oldMousePosition.setY(ev.y_root);

        return resetCursor();
    }
    else
    {
        XAllowEvents(dpy, ReplayPointer, ev.time);
        XFlush(dpy);
    }

    return 0;
}


