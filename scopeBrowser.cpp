#include "scopeBrowser.hpp"


scopeBrowser::scopeBrowser(Sky* sky)// : windowID(-1), XDisplay(sky->getDisplay()),frame(False)
{
    this->sky = sky;
    XDisplay = sky->getDisplay();
    rootWindowID = sky->getRootWindow();
    windowID = XCreateSimpleWindow(XDisplay, rootWindowID, 0, 0, 100, 100, 0, 0, 0);
}

scopeBrowser::~scopeBrowser()
{
}


Pixmap scopeBrowser::Sharpen(Pixmap pixmap, unsigned int level)
{
    imlib_context_set_drawable(pixmap);
    // fix the size parameters and not leave it fixed
    Imlib_Image l = imlib_create_image_from_drawable(pixmap, 0, 0, getSize().width(), getSize().height(), 1);
    imlib_context_set_image(l);
    imlib_image_sharpen(level);
    imlib_render_image_on_drawable(0,0);
    imlib_free_image_and_decache();
    return pixmap;
}

Pixmap scopeBrowser::Blur(Pixmap pixmap, unsigned int level)
{
    imlib_context_set_drawable(pixmap);
    // fix the size parameters and not leave it fixed
    Imlib_Image l = imlib_create_image_from_drawable(pixmap, 0, 0, getSize().width(), getSize().height(), 1);
    imlib_context_set_image(l);
    imlib_image_blur(level);
    imlib_render_image_on_drawable(0,0);
    imlib_free_image_and_decache();
    return pixmap;
}

int scopeBrowser::Update()
{
    if (XDisplay && rootWindowID)
    {
        int x = getPosition().x();
        int y = getPosition().y();

        Move(-getSize().width(), -getSize().height());
        //Hide();
        XImage* img = XGetImage(XDisplay, rootWindowID, x, y,  getSize().width(), getSize().height(), AllPlanes, ZPixmap);
        Move(x, y);
        //Show();


        GC  gc = XCreateGC(XDisplay, rootWindowID ,0, NULL);

        Pixmap browserPixmap = XCreatePixmap(XDisplay,
                                              rootWindowID,
                                          getSize().width(),
                                          getSize().height(),
                                          DefaultDepth(XDisplay, 0));


        //XCopyArea(XDisplay, rootWindowID, browserPixmap, gc, 0, 0, getSize().width(), getSize().height(), 0, 0);
        XPutImage(XDisplay, browserPixmap, gc, img, 0, 0, 0, 0, getSize().width(), getSize().height());
        XFree(img);


        XColor solidColor;
        string colorSelection= "#0059b3";
        Colormap colormap = DefaultColormap(XDisplay, 0);
        XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
        XAllocColor(XDisplay, colormap, &solidColor);
        XSetForeground(XDisplay, gc, solidColor.pixel);


        //browserPixmap  = Sharpen(browserPixmap);
        browserPixmap  = Blur(browserPixmap, 1);

        browserBox.setWidth((getSize().width()*sky->getSize().width())/sky->getSpaceSize().width());
        browserBox.setHeight((getSize().height()*sky->getSize().height())/sky->getSpaceSize().height());

        float xx = (float)(sky->getBrowsePoint().x()*browserBox.width())/sky->getSize().width();
        float yy = (float)(sky->getBrowsePoint().y()*browserBox.height())/sky->getSize().height();

        if(xx > (int)xx)
        {
            xx += 2;
        }

        if(yy > (int)yy)
        {
            yy += 2;
        }

        // Browsing recangle
        XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
        XAllocColor(XDisplay, colormap, &solidColor);
        XSetForeground(XDisplay, gc, solidColor.pixel);
        XDrawRectangle(XDisplay, browserPixmap, gc, xx, yy, browserBox.width(), browserBox.height());

        QVector<Cloud*> clouds = sky->getCurrentClouds();

        for (int i =0; i <clouds.size(); i++)
        {
            //if(clouds.at(i)->isHidden() || clouds.at(i)->getWindowID() == getWindowID())
            if(clouds.at(i)->isHidden())
            {
                continue;
            }
            QPoint fromPoint;
            fromPoint.setX(((clouds.at(i)->getPosition().x()+sky->getBrowsePoint().x())*getSize().width())/sky->getSpaceSize().width());
            fromPoint.setY(((clouds.at(i)->getPosition().y()+sky->getBrowsePoint().y())*getSize().height())/sky->getSpaceSize().height());


            QPoint toPoint;
            toPoint.setX( ((toPoint.x()+clouds.at(i)->getSize().width()) *browserBox.width()) / sky->getSize().width());
            toPoint.setY( ((toPoint.y()+clouds.at(i)->getSize().height()) *browserBox.height()) / sky->getSize().height());

            colorSelection = "#0059b3";
            XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
            XAllocColor(XDisplay, colormap, &solidColor);
            XSetForeground(XDisplay, gc, solidColor.pixel);

            XFillRectangle(XDisplay, browserPixmap, gc, fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());

            // draw Icon
            int iconX = (toPoint.x()/2)-(clouds.at(i)->getIconSize().width()/2);
            int iconY = (toPoint.y()/2)-(clouds.at(i)->getIconSize().height()/2);;
            Imlib_Image iconImage = imlib_create_image(clouds.at(i)->getIconSize().width(), clouds.at(i)->getIconSize().height());
            imlib_context_set_drawable(clouds.at(i)->getWindowIcon());
            //imlib_context_set_drawable(clouds.at(i)->thumbnailPixmap);
            imlib_context_set_image(iconImage);
            imlib_copy_drawable_to_image(0, 0, 0, clouds.at(i)->getIconSize().width(), clouds.at(i)->getIconSize().height(), 0, 0, 0);
            imlib_context_set_drawable(browserPixmap);
            imlib_context_set_anti_alias(1);
            imlib_render_image_on_drawable_at_size(fromPoint.x()+iconX, fromPoint.y()+iconY, clouds.at(i)->getIconSize().width(), clouds.at(i)->getIconSize().height());
            imlib_free_image_and_decache();

            //for (unsigned int i = penSize; i >0 ; i--)
            //{
//                XFillRectangle(XDisplay, browserPixmap, gc, fromPoint.x()+i, fromPoint.y()+i, penSize-i, penSize-i);
//            }
        }

        // draw highlighted last
        if(sky->highlightedCloud)
        {
            QPoint fromPoint;
            fromPoint.setX(((sky->highlightedCloud->getPosition().x()+sky->getBrowsePoint().x())*getSize().width())/sky->getSpaceSize().width());
            fromPoint.setY(((sky->highlightedCloud->getPosition().y()+sky->getBrowsePoint().y())*getSize().height())/sky->getSpaceSize().height());
            QPoint toPoint;
            toPoint.setX( ((toPoint.x()+sky->highlightedCloud->getSize().width()) *browserBox.width()) / sky->getSize().width());
            toPoint.setY( ((toPoint.y()+sky->highlightedCloud->getSize().height()) *browserBox.height()) / sky->getSize().height());
            colorSelection = "#00FF00";
            XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
            XAllocColor(XDisplay, colormap, &solidColor);
            XSetForeground(XDisplay, gc, solidColor.pixel);
            XFillRectangle(XDisplay, browserPixmap, gc, fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());

            // draw Icon
            int iconX = (toPoint.x()/2)-(sky->highlightedCloud->getIconSize().width()/2);
            int iconY = (toPoint.y()/2)-(sky->highlightedCloud->getIconSize().height()/2);;
            Imlib_Image iconImage = imlib_create_image(sky->highlightedCloud->getIconSize().width(), sky->highlightedCloud->getIconSize().height());
            imlib_context_set_drawable(sky->highlightedCloud->getWindowIcon());
            //imlib_context_set_drawable(clouds.at(i)->thumbnailPixmap);
            imlib_context_set_image(iconImage);
            imlib_copy_drawable_to_image(0, 0, 0, sky->highlightedCloud->getIconSize().width(), sky->highlightedCloud->getIconSize().height(), 0, 0, 0);
            imlib_context_set_drawable(browserPixmap);
            imlib_context_set_anti_alias(1);
            imlib_render_image_on_drawable_at_size(fromPoint.x()+iconX, fromPoint.y()+iconY, sky->highlightedCloud->getIconSize().width(), sky->highlightedCloud->getIconSize().height());
            imlib_free_image_and_decache();
        }

        //Show();
        //browserPixmap  = Blur(browserPixmap, 1);
        //XMapWindow(XDisplay, windowID);

        Show();
        XCopyArea(XDisplay, browserPixmap, windowID, gc, 0, 0, getSize().width(), getSize().height(), 0, 0);
        //XSetWindowBackgroundPixmap(XDisplay, windowID, browserPixmap);
        XFlush(XDisplay);


        colorSelection = "#151515";
        XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
        XAllocColor(XDisplay, colormap, &solidColor);
        XSetWindowColormap(XDisplay, windowID, colormap);
        XSetWindowBorderWidth(XDisplay, windowID, borderSize);
        XSetWindowBorder(XDisplay, windowID, solidColor.pixel);
        XFreePixmap(XDisplay, browserPixmap);


        Raise();
        //XRaiseWindow(XDisplay, windowID);
    }
    return 0;
}


int scopeBrowser::setBorderSize(unsigned int size)
{
    borderSize = size;
    return size;
}

int scopeBrowser::setSizeFactor(unsigned int size)
{
    float width = (float)(sky->getSize().width()*size)/100;
    float height = (float)(sky->getSize().height()*size)/100;

	if(width > (int)width)
	{
		width += 1;
	}


    return Resize(width , height);;
}



