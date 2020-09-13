#include "skyScope.hpp"

skyScope::skyScope(Sky* sky)
{
    this->sky = sky;
    XDisplay = sky->getDisplay();
    rootWindowID = sky->getRootWindow();
    windowID = XCreateSimpleWindow(XDisplay, rootWindowID, 0, 0, 100, 100, 0, 0, 0);
    skyBackground = 0;
    sizeFactor = 0;
    BORDER_WIDTH = 0;
    //setSizeFactor(100);


    showThumbnail = true;
    showTitle = true;
    showGrid = true;
    showScope = true;
    transparent = false;
    showIcon = true;
    gc = XCreateGC(XDisplay, rootWindowID ,0, NULL);
}

skyScope::~skyScope()
{
}


QSize skyScope::getPixmapSize(Pixmap pixmap)
{
    Window R;
    int x = 0;
    int y = 0;
    unsigned int width =0;
    unsigned int height = 0;
    unsigned int border = 0;
    unsigned int depth = 0;


    XGetGeometry(XDisplay, pixmap, &R, &x, &y, &width, &height, &border, &depth);

    return QSize(width, height);
}

Pixmap skyScope::Sharpen(Pixmap pixmap, unsigned int level)
{
    imlib_context_set_drawable(pixmap);
    // fix the size parameters and not leave it fixed
    Imlib_Image l = imlib_create_image_from_drawable(pixmap, 0, 0, getPixmapSize(pixmap).width(), getPixmapSize(pixmap).height(), 1);
    imlib_context_set_image(l);
    imlib_image_sharpen(level);
    imlib_render_image_on_drawable(0,0);
    imlib_free_image_and_decache();
    return pixmap;
}

Pixmap skyScope::Blur(Pixmap pixmap, unsigned int level)
{
    imlib_context_set_blend(0);
    imlib_context_set_drawable(pixmap);
    // fix the size parameters and not leave it fixed
    Imlib_Image l = imlib_create_image_from_drawable(pixmap, 0, 0, getPixmapSize(pixmap).width(), getPixmapSize(pixmap).height(), 1);
    imlib_context_set_image(l);
    imlib_image_blur(level);
    imlib_render_image_on_drawable(0,0);
    imlib_free_image_and_decache();
    return pixmap;
}

Pixmap skyScope::resizePixmap(Pixmap pixmap, int Width, int Height)
{
    if (pixmap)
    {
        Window R;
        int x = 0;
        int y = 0;
        unsigned int width =0;
        unsigned int height = 0;
        unsigned int border = 0;
        unsigned int depth = 0;


        XGetGeometry(XDisplay, pixmap, &R, &x, &y, &width, &height, &border, &depth);

        imlib_context_set_dither(0);

        //Imlib_Image resizedImage = imlib_create_image_from_drawable(pixmap, 0, 0, width, height, 0);
        Imlib_Image resizedImage = imlib_create_image(width, height);
        imlib_context_set_drawable(pixmap);
        imlib_context_set_image(resizedImage);
        imlib_copy_drawable_to_image(0, 0, 0, width, height, 0, 0, 0);

        Pixmap Resized = XCreatePixmap(XDisplay, rootWindowID, Width, Height, DefaultDepth(XDisplay, 0));
        imlib_context_set_image(resizedImage);
        imlib_context_set_drawable(Resized);
        imlib_render_image_on_drawable_at_size(0, 0, Width, Height);
        imlib_free_image_and_decache();
        XFreePixmap (XDisplay, pixmap);
        imlib_free_image(); // free resizedImage

        return Resized;
    }
    return 0;
}


Pixmap skyScope::resizePixmapBlend(Pixmap pixmap, int Width, int Height)
{
    if (pixmap)
    {
        Window R;
        int x = 0;
        int y = 0;
        unsigned int width =0;
        unsigned int height = 0;
        unsigned int border = 0;
        unsigned int depth = 0;


        XGetGeometry(XDisplay, pixmap, &R, &x, &y, &width, &height, &border, &depth);


        imlib_context_set_anti_alias(0);
         imlib_context_set_dither(0);
        //Imlib_Image resizedImage = imlib_create_image_from_drawable(pixmap, 0, 0, width, height, 0);
        Imlib_Image originalImage = imlib_create_image(width, height);
        imlib_context_set_drawable(pixmap);
        imlib_context_set_image(originalImage);
        imlib_copy_drawable_to_image(0, 0, 0, width, height, 0, 0, 0);


        Pixmap Resized = XCreatePixmap(XDisplay, rootWindowID, Width, Height, DefaultDepth(XDisplay, 0));
        //Imlib_Image resizedImage = imlib_create_image(Width, Height);
        Imlib_Image resizedImage = imlib_create_image_from_drawable(Resized, 0, 0, Width, Height, 0);
        imlib_context_set_blend(1);
        imlib_context_set_image(resizedImage);
        imlib_blend_image_onto_image(originalImage, 0, 0, 0, width, height, 0, 0, Width, Height);

        //imlib_context_set_drawable(Resized);
        //imlib_render_image_on_drawable(0, 0);

        XFreePixmap (XDisplay, pixmap);
        imlib_free_image(); // free resizedImage

        imlib_context_set_image(originalImage);
        imlib_free_image(); // free originalImage

        return Resized;
    }
    return 0;
}




Pixmap skyScope::getScreenShot(bool showGrid)
{
    if (XDisplay && rootWindowID)
    {
        Pixmap spaceShot = getSpaceShot(showGrid);

        if(spaceShot)
        {
            Pixmap screenShot = XCreatePixmap(XDisplay, rootWindowID, sky->getSize().width(), sky->getSize().height(), DefaultDepth(XDisplay, 0));
            QPoint fromPoint = sky->browsePoint;
            XCopyArea(XDisplay, spaceShot, screenShot, gc, fromPoint.x(), fromPoint.y(), sky->getSize().width(), sky->getSize().height(), 0, 0);
            XFreePixmap(XDisplay, spaceShot);            return screenShot;
        }
    }
    return 0;
}

Pixmap skyScope::getSpaceShot(bool showGrid)
{
    if (XDisplay && rootWindowID)
    {

        Pixmap spaceShotPixmap = XCreatePixmap(XDisplay, rootWindowID, sky->getSpaceSize().width(), sky->getSpaceSize().height(),
                                               DefaultDepth(XDisplay, 0));


        imlib_context_set_image(sky->localBackground);
        imlib_context_set_drawable(spaceShotPixmap);
        imlib_render_image_on_drawable(0, 0);
        //XCopyArea(XDisplay, sky->backgroundImage, spaceShotPixmap, gc, 0, 0, sky->getSpaceSize().width(), sky->getSpaceSize().height(), 0, 0);

        XColor solidColor;
        string colorSelection;
        Colormap colormap = DefaultColormap(XDisplay, 0);

        QSize spaceShotBox;
        spaceShotBox.setWidth((sky->getSpaceSize().width()*sky->getSpaceSize().width())/sky->getSpaceSize().width());
        spaceShotBox.setHeight((sky->getSpaceSize().height()*sky->getSpaceSize().height())/sky->getSpaceSize().height());

        float xx = (float)(sky->getBrowsePoint().x()*spaceShotBox.width())/sky->getSpaceSize().width();
        float yy = (float)(sky->getBrowsePoint().y()*spaceShotBox.height())/sky->getSpaceSize().height();

        if(xx > (int)xx)
        {
            xx += 2;
        }

        if(yy > (int)yy)
        {
            yy += 2;
        }


        QVector<Cloud*> clouds = sky->getCurrentClouds();

        // draw all clouds except highlighted
        for (int i =0; i <clouds.size(); i++)
        {
            if(clouds.at(i)->isHidden())
            {
                continue;
            }
            QPoint fromPoint;
            fromPoint.setX(((clouds.at(i)->getPosition().x()+sky->getBrowsePoint().x())*sky->getSpaceSize().width())/sky->getSpaceSize().width());
            fromPoint.setY(((clouds.at(i)->getPosition().y()+sky->getBrowsePoint().y())*sky->getSpaceSize().height())/sky->getSpaceSize().height());

            QPoint toPoint;
            toPoint.setX( ((toPoint.x()+clouds.at(i)->getSize().width()) *spaceShotBox.width()) / sky->getSpaceSize().width());
            toPoint.setY( ((toPoint.y()+clouds.at(i)->getSize().height()) *spaceShotBox.height()) / sky->getSpaceSize().height());

            // set color for normal window
            colorSelection = "#0059b3";
            XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
            XAllocColor(XDisplay, colormap, &solidColor);
            XSetForeground(XDisplay, gc, solidColor.pixel);
            XFillRectangle(XDisplay, spaceShotPixmap, gc, fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());



            // draw Thumbnail
            clouds.at(i)->updateThumbnail();
            Imlib_Image thumbnailImage = imlib_create_image(clouds.at(i)->getThumbnailSize().width(),clouds.at(i)->getThumbnailSize().height());
            imlib_context_set_drawable(clouds.at(i)->thumbnailPixmap);
            imlib_context_set_image(thumbnailImage);
            imlib_copy_drawable_to_image(0, 0, 0, clouds.at(i)->getThumbnailSize().width(), clouds.at(i)->getThumbnailSize().height(), 0, 0, 0);
            imlib_context_set_drawable(spaceShotPixmap);
            imlib_context_set_anti_alias(1);
            imlib_render_image_on_drawable_at_size(fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());
            imlib_free_image_and_decache();
        }

        return spaceShotPixmap;

    }
    return 0;
}


int skyScope::Update()
{
    if (XDisplay && rootWindowID)
    {
        //browserPixmap = resizePixmap(getSpaceShot(), getSize().width(), getSize().height());
        //browserPixmap = resizePixmapBlend(getSpaceShot(), getSize().width(), getSize().height());
        //browserPixmap = getSpaceShot();

        if(!browserPixmap)
        {
            return 1;
        }

        if(!transparent)
        {
            XCopyArea(XDisplay, skyBackground, browserPixmap, gc, 0, 0, getSize().width(), getSize().height(), 0, 0);
        }
        else
        {
            Pixmap temp = getScreenShot();
            int x = getPosition().x();
            int y = getPosition().y();
            XCopyArea(XDisplay, temp, browserPixmap, gc, x, y, getSize().width(), getSize().height(), 0, 0);
            XFreePixmap(XDisplay, temp);

            browserPixmap  = Blur(browserPixmap, 3);
        }

        cairo_surface_t* sfc = cairo_xlib_surface_create(XDisplay, browserPixmap,
                                                         DefaultVisual(XDisplay, DefaultScreen(XDisplay)), getSize().width(), getSize().height());
        cairo_xlib_surface_set_size(sfc, getSize().width(), getSize().height());
        cairo_t* ctx = cairo_create(sfc);

        XColor solidColor;
        string colorSelection= "#0059b3";
        Colormap colormap = DefaultColormap(XDisplay, 0);
        XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
        XAllocColor(XDisplay, colormap, &solidColor);
        XSetForeground(XDisplay, gc, solidColor.pixel);

        browserBox.setWidth((getSize().width()*sky->getSize().width())/sky->getSpaceSize().width());
        browserBox.setHeight((getSize().height()*sky->getSize().height())/sky->getSpaceSize().height());

        float xx = (sky->getBrowsePoint().x()*browserBox.width())/(float)sky->getSize().width();
        float yy = (sky->getBrowsePoint().y()*browserBox.height())/(float)sky->getSize().height();

        xx = ceil(xx);
        yy = ceil(yy);

        if(showGrid)
        {
            // draw grid
            int propotionalGridWidth = (sky->getSize().width()*getSize().width())/sky->getSpaceSize().width();
            int propotionalGridHeight = (sky->getSize().height()*getSize().height())/sky->getSpaceSize().height();

            float gridColumns = getSize().width()/(float)propotionalGridWidth;
            gridColumns = ceil(gridColumns);

            float gridRows = getSize().height()/(float)propotionalGridHeight;
            gridRows = ceil(gridRows);

            //Pixmap gridPixmap = XCreatePixmap(XDisplay, rootWindowID, getSize().width(), getSize().height(), 24);





            cairo_push_group(ctx);

            for(int i = 0; i < gridColumns; i++) // columns
            {
                for(int k = 0; k < gridRows; k++) // rows
                {
                    //colorSelection= "#B20000";
                    //XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
                    //XAllocColor(XDisplay, colormap, &solidColor);
                    //XSetForeground(XDisplay, gc, solidColor.pixel);

                    cairo_set_line_width (ctx, 2);
                    cairo_set_source_rgba(ctx, 178,0,0, 0.2);

                    if (xx < i*propotionalGridWidth+propotionalGridWidth && xx+browserBox.width() > i*propotionalGridWidth &&
                        yy < k*propotionalGridHeight+propotionalGridHeight && yy+browserBox.height() > k*propotionalGridHeight)
                    {
                        cairo_rectangle (ctx, i*propotionalGridWidth , k*propotionalGridHeight, propotionalGridWidth, propotionalGridHeight);
                        cairo_stroke (ctx);
                        cairo_set_source_rgba(ctx, 112,151,212, 0.2);
                        cairo_rectangle (ctx, i*propotionalGridWidth+1, k*propotionalGridHeight+1, propotionalGridWidth-2, propotionalGridHeight-2);
                        cairo_fill (ctx);
                    }
                    else
                    {
                        cairo_rectangle (ctx, i*propotionalGridWidth , k*propotionalGridHeight, propotionalGridWidth, propotionalGridHeight);
                        cairo_stroke (ctx);
                    }

                }
            }

            cairo_pop_group_to_source(ctx);
            cairo_paint(ctx);
            cairo_surface_flush(sfc);
        }

        // draw all clouds except highlighted
        QVector<Cloud*> clouds = sky->getCurrentClouds();
        for (int i =0; i <clouds.size(); i++)
        {


            QPoint fromPoint;
            fromPoint.setX(ceil(((clouds.at(i)->getPosition().x()+sky->getBrowsePoint().x())*getSize().width())/(float)sky->getSpaceSize().width()));
            fromPoint.setY(ceil(((clouds.at(i)->getPosition().y()+sky->getBrowsePoint().y())*getSize().height())/(float)sky->getSpaceSize().height()));

            QPoint toPoint;
            toPoint.setX( ceil(((toPoint.x()+clouds.at(i)->getSize().width()) *browserBox.width()) /(float)sky->getSize().width()));
            toPoint.setY( ceil(((toPoint.y()+clouds.at(i)->getSize().height()) *browserBox.height()) /(float)sky->getSize().height()));


            // set color forText
            colorSelection = "#FFFFFF";
            XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
            XAllocColor(XDisplay, colormap, &solidColor);
            XSetForeground(XDisplay, gc, solidColor.pixel);



            string windowTitle = clouds.at(i)->getWindowIconifyName().size() ? clouds.at(i)->getWindowIconifyName() : clouds.at(i)->getWindowName();
            //XFontStruct* font_info  = XLoadQueryFont( XDisplay, "-*-helvetica-medium-r-normal--20-*-*-*-p-100-iso8859-1" );;
            XFontStruct* font_info  = XQueryFont(XDisplay, (GContext)XGContextFromGC(gc));
            int textWidth = 0;

            if(windowTitle.size())
            {

                textWidth = XTextWidth(font_info,windowTitle.c_str(), windowTitle.size());
            }

            int characterWidth = textWidth > 0 ? textWidth/windowTitle.size() : 0;
            float charactersPerLine = characterWidth > 0 ? toPoint.x()/(float)characterWidth : 0;
            int textHeight = 0;

            if(font_info)
            {
                textHeight = font_info->ascent +font_info->descent;
            }

            XFreeFont(XDisplay, font_info);
            float numberOfLines = windowTitle.size()/(float)charactersPerLine;

            charactersPerLine = ceil(charactersPerLine);
            numberOfLines = ceil(numberOfLines);


            //int totalLinesHeight = font_info->ascent +font_info->descent;


            // draw Thumbnail
            if(showThumbnail)
            {
                clouds.at(i)->updateThumbnail();

                Imlib_Image thumbnailImage = imlib_create_image(clouds.at(i)->getThumbnailSize().width(),clouds.at(i)->getThumbnailSize().height());
                if(thumbnailImage)
                {
                    imlib_context_set_drawable(clouds.at(i)->thumbnailPixmap);
                    imlib_context_set_image(thumbnailImage);
                    imlib_copy_drawable_to_image(0, 0, 0, clouds.at(i)->getThumbnailSize().width(), clouds.at(i)->getThumbnailSize().height(), 0, 0, 0);
                    imlib_context_set_drawable(browserPixmap);
                    imlib_context_set_anti_alias(1);
                    imlib_context_set_dither(0);
                    imlib_render_image_on_drawable_at_size(fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());
                    imlib_free_image_and_decache();;
                }
            }
            else
            {
                cairo_set_source_rgba(ctx, 255,255,255, 0.8);
                if(clouds.at(i)->isHighlighted())
                {
                    cairo_set_source_rgba(ctx, 176,23,31, 0.8);
                }
                cairo_rectangle (ctx, fromPoint.x(), fromPoint.y(), toPoint.x(), toPoint.y());
                cairo_fill (ctx);
            }

            // draw Icon
            if(showIcon && clouds.at(i)->getIconSize().width())
            {
                int iconX = (toPoint.x()/2)-(clouds.at(i)->getIconSize().width()/2);
                int iconY = (toPoint.y()/2)-(clouds.at(i)->getIconSize().height()/2);;
                Imlib_Image iconImage = imlib_create_image(clouds.at(i)->getIconSize().width(), clouds.at(i)->getIconSize().height());
                if(iconImage)
                {
                    cout << "HERE 1 " << endl << flush;
                    imlib_context_set_drawable(clouds.at(i)->getWindowIcon());
                    //imlib_context_set_drawable(clouds.at(i)->thumbnailPixmap);
                    imlib_context_set_image(iconImage);
                    imlib_copy_drawable_to_image(0, 0, 0, clouds.at(i)->getIconSize().width(), clouds.at(i)->getIconSize().height(), 0, 0, 0);
                    imlib_context_set_drawable(browserPixmap);
                    imlib_context_set_anti_alias(1);
                    imlib_render_image_on_drawable_at_size(fromPoint.x()+iconX, fromPoint.y()+iconY, clouds.at(i)->getIconSize().width(), clouds.at(i)->getIconSize().height());
                    imlib_free_image_and_decache();
                    cout << "HERE 1 " << endl << flush;
                }
            }

            float possibleNumberOfLines = (toPoint.y()/2)/(float)textHeight;
            possibleNumberOfLines = ceil(possibleNumberOfLines);

            if(possibleNumberOfLines > numberOfLines)
            {
                possibleNumberOfLines = numberOfLines;
            }

            /*cout << "Text : " <<  windowTitle << endl << flush;
            cout << "Text Width : " << textWidth << endl;
            cout << "Width : " << toPoint.x() << endl;
            cout << "Title Characters : " << windowTitle.size() << endl;
            cout << "Characters per line : " << charactersPerLine << endl;
            cout << "Number of Lines : " << numberOfLines << endl;
            cout << "Possible Number of Lines : " << possibleNumberOfLines << endl << flush;
            cout << "New Client" << endl << flush;*/

            if(showTitle)
            {
                for(int k =1; k <= possibleNumberOfLines; k++)
                {
                    int toPointY = (fromPoint.y() + toPoint.y())  - ((possibleNumberOfLines-k)*textHeight);
                    string partialTitle = windowTitle.substr((k-1)*charactersPerLine, charactersPerLine);
                    //XDrawString(XDisplay, browserPixmap, gc, fromPoint.x(), toPointY, partialTitle.c_str(), partialTitle.size());
                    //XDrawString(XDisplay, browserPixmap, gc,  fromPoint.x() + ((toPoint.x()-(characterWidth*partialTitle.size()))/2), toPointY, partialTitle.c_str(), partialTitle.size());
                    XDrawImageString(XDisplay, browserPixmap, gc,  fromPoint.x() + ((toPoint.x()-(characterWidth*partialTitle.size()))/2), toPointY, partialTitle.c_str(), partialTitle.size());
                    //cairo_move_to (ctx, fromPoint.x() + ((toPoint.x()-(characterWidth*partialTitle.size()))/2), toPointY);
                    //cairo_show_text(ctx, partialTitle.c_str());
                }
            }
        }


        // draw scoping rectangle
        if(showScope && !(browserBox.width() > sky->getSize().width() || browserBox.height() > sky->getSize().height()))
        {
            //colorSelection = "#C5C5C5";
            //XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
            //XAllocColor(XDisplay, colormap, &solidColor);
            //XSetForeground(XDisplay, gc, solidColor.pixel);
            //XDrawRectangle(XDisplay, browserPixmap, gc, xx, yy, browserBox.width(), browserBox.height());
            //XDrawRectangle(XDisplay, browserPixmap, gc, xx-1, yy-1, browserBox.width()+2, browserBox.height()+2);


            //cairo_set_line_width (ctx, 0.1);
            cairo_set_line_width (ctx, 2);
            cairo_set_source_rgba(ctx, 197,197,197, 0.9);
            cairo_rectangle (ctx, xx, yy, browserBox.width(), browserBox.height());
            cairo_stroke (ctx);
            cairo_surface_destroy(sfc);
        }

        //if(clouds.size())
        if (false)
        {
            // center of the cloud
            QPoint B;
            B.setX(clouds.at(0)->getPosition().x() + (clouds.at(0)->getSize().width()/2));
            B.setY(clouds.at(0)->getPosition().y() + (clouds.at(0)->getSize().height()/2));

            QPoint p;

            if( (B.x() >= 0 && B.x() <= sky->getSize().width()))
            {
                p.setX( (B.x()*getSize().width()) / sky->getSize().width());
            }
            else if(B.x() < sky->getBrowsePoint().x())
            {
                p.setX(-200);
            }
            else if(B.x() > sky->getBrowsePoint().x() + sky->getSize().width())
            {
                p.setX(getSize().width()+200);
            }


            if((B.y() >= 0 && B.y() <= sky->getSize().height()))
            {
                p.setY( (B.y()*getSize().height()) / sky->getSize().height());
            }
            else if(B.y() < sky->getBrowsePoint().y())
            {
                p.setY(-200);
            }
            else if(B.y() > sky->getBrowsePoint().y() + + sky->getSize().height())
            {
                p.setY(getSize().height()+200);
            }


            cout << "draw" << endl;

            cout << p.x() << ":" << p.y() << endl << flush;
            cairo_set_source_rgba(ctx, 200,0,0, 0.2);
            cairo_arc(ctx, p.x(), p.y(), 300, 0, 2*M_PI);
            cairo_stroke_preserve(ctx);
            cairo_fill(ctx);
        }

        XMapWindow(XDisplay, windowID);

        if ( (getSize().width() > sky->getSize().width() || getSize().height() > sky->getSize().height()) )
        {
            float newX =0;
            float newY = 0;

            if (!(browserBox.width() > sky->getSize().width() || browserBox.height() > sky->getSize().height())) // if box is not bigger than screen
            {
                newX = xx - ((sky->getSize().width()-browserBox.width())/2);
                newY = yy - ((sky->getSize().height()-browserBox.height())/2);
            }
            else
            {
                newX = xx;
                newY = yy;
            }

            if(newX < 0)
            {
                newX = 0;
            }

            if(newY < 0)
            {
                newY = 0;
            }


            if(newX + sky->getSize().width() > getSize().width())
            {
                newX = getSize().width() -  sky->getSize().width();
            }

            if(newY +  sky->getSize().height() >  getSize().height())
            {
                newY = getSize().height() -  sky->getSize().height();
            }

            XCopyArea(XDisplay, browserPixmap, windowID, gc, newX, newY, getSize().width(), getSize().height(), 0, 0);
        }
        else
        {
            XCopyArea(XDisplay, browserPixmap, windowID, gc, 0, 0, getSize().width(), getSize().height(), 0, 0);
        }





        XFlush(XDisplay);

        colorSelection = "#151515";
        XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
        XAllocColor(XDisplay, colormap, &solidColor);
        XSetWindowColormap(XDisplay, windowID, colormap);
        XSetWindowBorderWidth(XDisplay, windowID, borderSize);
        XSetWindowBorder(XDisplay, windowID, solidColor.pixel);


        //Show();
        Raise();
    }
    return 0;
}


int skyScope::setBorderSize(unsigned int size)
{
    borderSize = size;
    return size;
}

int skyScope::setSizeFactor(unsigned int size)
{
    float width = ceil((sky->getSize().width()*size)/(float)100);
    float height = ceil((sky->getSize().height()*size)/(float)100);

    if(skyBackground)
    {
        XFreePixmap(XDisplay, skyBackground);
    }

    skyBackground = XCreatePixmap(XDisplay, rootWindowID, width, height, DefaultDepth(XDisplay, 0));

    imlib_context_set_image(sky->localBackground);
    imlib_context_set_drawable(skyBackground);
    imlib_render_image_on_drawable_at_size(0, 0, width, height);

    skyBackground  = Blur(skyBackground, 8);
    sizeFactor = size;

    browserPixmap = XCreatePixmap(XDisplay, rootWindowID, width, height,
                                  DefaultDepth(XDisplay, 0));

    Resize(width , height);

    return 0;
}

int skyScope::setShowScope(bool cond)
{
    showScope = cond;
    Update();
    return cond;
}

bool skyScope::getShowScope()
{
    return showScope;
}




int skyScope::setShowIcon(bool cond)
{
    showIcon = cond;
    Update();
    return cond;
}

bool skyScope::getShowIcon()
{
    return showIcon;
}


int skyScope::setShowTitle(bool cond)
{
    showTitle = cond;
    Update();
    return cond;
}

bool skyScope::getShowTitle()
{
    return showTitle;
}

int skyScope::setShowGrid(bool cond)
{
    showGrid = cond;
    Update();
    return cond;
}

bool skyScope::getShowGrid()
{
    return showGrid;
}

int skyScope::setShowThumbnail(bool cond)
{
    showThumbnail = cond;
    Update();
    return cond;
}

bool skyScope::getShowThumbnail()
{
    return showThumbnail;
}

int skyScope::setTransparent(bool cond)
{
    transparent = cond;
    Update();
    return cond;
}

bool skyScope::isTransparent()
{
    return transparent;
}

int skyScope::getSizeFactor()
{
    return sizeFactor;
}



/*int skyScope::setAutoBrowsingFactor(bool cond)
{

}*/



/*

if(showGrid)
{
    // draw grid
    int propotionalGridWidth = (sky->getSize().width()*getSize().width())/sky->getSpaceSize().width();
    int propotionalGridHeight = (sky->getSize().height()*getSize().height())/sky->getSpaceSize().height();

    float gridColumns = getSize().width()/(float)propotionalGridWidth;
    gridColumns = ceil(gridColumns);

    float gridRows = getSize().height()/(float)propotionalGridHeight;
    gridRows = ceil(gridRows);

    for(int i = 0; i < gridColumns; i++) // columns
    {
        for(int k = 0; k < gridRows; k++) // rows
        {
            colorSelection= "#B20000";
            XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
            XAllocColor(XDisplay, colormap, &solidColor);
            XSetForeground(XDisplay, gc, solidColor.pixel);

            if (xx < i*propotionalGridWidth+propotionalGridWidth && xx+browserBox.width() > i*propotionalGridWidth &&
                yy < k*propotionalGridHeight+propotionalGridHeight && yy+browserBox.height() > k*propotionalGridHeight)
            {
                XDrawRectangle(XDisplay, browserPixmap, gc, i*propotionalGridWidth , k*propotionalGridHeight, propotionalGridWidth, propotionalGridHeight);
                XDrawRectangle(XDisplay, browserPixmap, gc, i*propotionalGridWidth+1, k*propotionalGridHeight+1, propotionalGridWidth-1, propotionalGridHeight-1);


                colorSelection= "#0059b3";
                XParseColor(XDisplay, colormap, colorSelection.c_str(), &solidColor);
                XAllocColor(XDisplay, colormap, &solidColor);
                XSetForeground(XDisplay, gc, solidColor.pixel);
                XFillRectangle(XDisplay, browserPixmap, gc, i*propotionalGridWidth+2, k*propotionalGridHeight+2, propotionalGridWidth-2, propotionalGridHeight-2);
            }
            else
            {
                XDrawRectangle(XDisplay, browserPixmap, gc, i*propotionalGridWidth, k*propotionalGridHeight, propotionalGridWidth, propotionalGridHeight);
                XDrawRectangle(XDisplay, browserPixmap, gc, i*propotionalGridWidth +1, k*propotionalGridHeight +1, propotionalGridWidth-1, propotionalGridHeight-1);
                XDrawRectangle(XDisplay, browserPixmap, gc, i*propotionalGridWidth +2, k*propotionalGridHeight +2, propotionalGridWidth-2, propotionalGridHeight-2);
            }

        }
    }
}*/
