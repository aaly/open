#include "skyNotifier.hpp"

skyNotify::skyNotify() : position(skyNotify::NOTIFICATION_POS_NONE),
                         shape(skyNotify::NOTIFICATION_SHAPE_NONE),
                         phase(skyNotify::NOTIFICATION_PHASE_FADEIN),
                         currentStep(0)
{

}


int skyNotifier::Update()
{
    //if (XDisplay && overlayWindow)
    if (XDisplay)
    {
        counter = 1;
        overlayWindow = sky->requestOverlayWindow();
        sky->releaseOverlayWindow();

        if(!overlayWindow)
        {
            cerr << "[ERROR] : Failed to get overlay window !" << endl << flush;
            return 1;
        }

        cairo_surface_t* sfc = cairo_xlib_surface_create(XDisplay, overlayWindow,
                                                        DefaultVisual(XDisplay, DefaultScreen(XDisplay)), sky->getSize().width(), sky->getSize().height());
        cairo_xlib_surface_set_size(sfc, sky->getSize().width(), sky->getSize().height());
        cairo_t* ctx = cairo_create(sfc);

        //test only
        if(sky->getCurrentClouds().size())
        {
            if(!sky->isOnScreen(sky->getCurrentClouds().at(0)))
            {
                addHotCloud(sky->getCurrentClouds().at(0));
            }
        }

        while(notifications.size())
        {
            //XClearWindow(XDisplay, overlayWindow);
            //XCopyArea(XDisplay, pix, overlayWindow, gc, 0, 0, sky->getSize().width(), sky->getSize().height(), 0, 0);
            //
            //XCopyArea(XDisplay, pix, overlayWindow, gc, 0, 0, sky->getSize().width(), sky->getSize().height(), 0, 0);
            //Pixmap pix = XCreatePixmap(XDisplay, windowID, sky->getSize().width(), sky->getSize().height(), 32);
            //XFreePixmap(XDisplay,pix);

            //XCopyArea(XDisplay, rootWindowID, pix, gc, 0, 0, sky->getSize().width(), sky->getSize().height(), 0, 0);
            //XCopyArea(XDisplay, sky->getCurrentClouds().at(0)->coreWindowID, overlayWindow, gc, 0, 0, sky->getSize().width(), sky->getSize().height(), 0, 0);

            cerr << "[INFO] : STARTIGN LOOP" << endl << flush;

            for (int i =0; i < notifications.size(); i++)
            {
                displayNotify(ctx, &notifications[i]);
                if(notifications.at(i).phase == skyNotify::NOTIFICATION_PHASE_NONE)
                {
                    notifications.remove(i);
                    i--;
                }
            }

            cerr << "[INFO] : ENDING LOOP" << endl << flush;
            cairo_surface_flush(sfc);
            XSync(XDisplay, False);
            //usleep(1000000/25);
            ////this_thread::sleep_for(chrono::milliseconds(1000000/25));
            cerr << "[INFO] : ENDING LOOP" << endl << flush;
        }

        cerr << "[INFO] : ENDING LOOP2" << endl << flush;
        releaseOverlayWindow();
        cairo_destroy(ctx);
        cairo_surface_destroy(sfc);
        cerr << "[INFO] : ENDING LOOP3" << endl << flush;
    }
    return 0;
}




Window skyNotifier::requestOverlayWindow()
{

        XCompositeRedirectSubwindows(XDisplay, rootWindowID, CompositeRedirectManual);

        overlayWindow = XCompositeGetOverlayWindow(XDisplay, XRootWindow(XDisplay, XDefaultScreen(XDisplay)));
        XSync(XDisplay, false);

        if(overlayWindow == 0)
        {
            return 0;
        }

        XserverRegion region = XFixesCreateRegion (XDisplay, NULL, 0);
        XFixesSetWindowShapeRegion (XDisplay, overlayWindow, ShapeBounding, 0, 0, 0);
        XFixesSetWindowShapeRegion (XDisplay, overlayWindow, ShapeInput, 0, 0, region);
        XFixesDestroyRegion (XDisplay, region);

        //XSelectInput(dpy, rootOverlay, ExposureMask);



    return overlayWindow;
}

int skyNotifier::releaseOverlayWindow()
{

            //XUnmapWindow(dpy, rootOverlay);
            XCompositeUnredirectSubwindows (XDisplay, rootWindowID, CompositeRedirectManual);
            XCompositeReleaseOverlayWindow(XDisplay, overlayWindow);
            overlayWindow = 0;
            XSync(XDisplay, false);

    return 0;
}


skyNotifier::skyNotifier(Sky* sky)
{
    this->sky = sky;
    XDisplay = sky->getDisplay();
    rootWindowID = sky->getRootWindow();
    sizeFactor = 0;
    gc = XCreateGC(XDisplay, rootWindowID ,0, NULL);

    static int		xfixes_event, xfixes_error;
    static int		xshape_event, xshape_error;


    if (!XFixesQueryExtension (XDisplay, &xfixes_event, &xfixes_error))
    {
        fprintf (stderr, "No XFixes extension\n");
        exit (1);
    }

    //rootWindowID = XCompositeGetOverlayWindow(XDisplay, XRootWindow(XDisplay, XDefaultScreen(XDisplay)));


    //XserverRegion region = XFixesCreateRegion (XDisplay, NULL, 0);
    //XFixesSetWindowShapeRegion (XDisplay, rootWindowID, ShapeBounding, 0, 0, 0);
    //XFixesSetWindowShapeRegion (XDisplay, rootWindowID, ShapeInput, 0, 0, region);
    //XFixesDestroyRegion (XDisplay, region);

    //overlayWindow = sky->requestOverlayWindow();
    //sky->releaseOverlayWindow();

    setSizeFactor(100);
}

skyNotifier::~skyNotifier()
{
}


skyNotify skyNotifier::addHotCloud(Cloud* cloud)
{

    skyNotify notify;

    // Cloud center point
    QPoint cloudCenter;
    cloudCenter.setX(cloud->getPosition().x() + (cloud->getSize().width()/2));
    cloudCenter.setY(cloud->getPosition().y() + (cloud->getSize().height()/2));

    cout << "CLOUD : " << cloud->getPosition().x() << ":" << cloud->getPosition().y() << endl;
    cout << "CLOUD : " << cloudCenter.x() << ":" << cloudCenter.y() << endl;
    cout << "BROWSE : " << sky->getBrowsePoint().x() << ":" << sky->getBrowsePoint().y() << endl;

    // browsing box !
    //QPoint rectTopLeft = sky->getBrowsePoint();
    QPoint rectTopLeft;
    QPoint rectTopRight(rectTopLeft.x()+sky->getSize().width(), rectTopLeft.y());
    QPoint rectBottomLeft(rectTopLeft.x(), rectTopLeft.y()+sky->getSize().height());
    QPoint rectBottomRight(rectTopLeft.x()+sky->getSize().width(), rectTopLeft.y()+sky->getSize().height());

    if( (cloudCenter.x() > rectTopLeft.x()) && (cloudCenter.x() < rectTopRight.x()) ) //top or bottom
    {
        cout << "top bottom" << endl;
        notify.middlePoint1.setX ( ((cloudCenter.x()*sky->getSize().width()) / sky->getSize().width()) - 5 );
        notify.middlePoint2.setX ( ((cloudCenter.x()*sky->getSize().width()) / sky->getSize().width()) + 5 );

        if(cloudCenter.y() < rectTopLeft.y()) //top
        {
            notify.shape = skyNotify::NOTIFICATION_SHAPE_Curve;
            cout << "TOP" << endl << flush;
            notify.startPoint.setX(0);
            notify.startPoint.setY(0);

            notify.middlePoint1.setY( 30  );
            notify.middlePoint2.setY( 28 );

            notify.endPoint.setX(sky->getSize().width());
            notify.endPoint.setY(0);

        }
        else if(cloudCenter.y() > rectBottomLeft.y()) // bottom
        {
            notify.shape = skyNotify::NOTIFICATION_SHAPE_Curve;
            cout << "bottom" << endl;
            notify.startPoint.setX(0);
            notify.startPoint.setY(sky->getSize().height());

            notify.middlePoint1.setY( sky->getSize().height() - 30  );
            notify.middlePoint2.setY( sky->getSize().height() - 28 );

            notify.endPoint.setX(sky->getSize().width());
            notify.endPoint.setY(sky->getSize().height());

        }
    }
    else if ( (cloudCenter.y() > rectTopLeft.y()) && (cloudCenter.y() < rectBottomLeft.y())) // right left
    {
        cout << "right left" << endl;
        notify.middlePoint1.setY( ((cloudCenter.y()*sky->getSize().height()) / sky->getSize().height())  );
        notify.middlePoint2.setY( ((cloudCenter.y()*sky->getSize().height()) / sky->getSize().height())  );

        if(cloudCenter.x() < rectTopLeft.x()) // left
        {
            notify.shape = skyNotify::NOTIFICATION_SHAPE_Curve;
            cout << "left" << endl;
            notify.startPoint.setX(0);
            notify.startPoint.setY(0);

            notify.middlePoint1.setX ( 30 );
            notify.middlePoint2.setX ( 30);

            notify.endPoint.setX(0);
            notify.endPoint.setY(sky->getSize().height());

        }
        else if(cloudCenter.x() > rectTopRight.x()) // right
        {
            notify.shape = skyNotify::NOTIFICATION_SHAPE_Curve;
            cout << "right:" << endl;

            notify.startPoint.setX(sky->getSize().width());
            notify.startPoint.setY(0);

            notify.middlePoint1.setX (sky->getSize().width() - 30 );
            notify.middlePoint2.setX (sky->getSize().width() - 30);

            notify.endPoint.setX(sky->getSize().width());
            notify.endPoint.setY(sky->getSize().height());
        }
    }

    if(notify.shape == skyNotify::NOTIFICATION_SHAPE_NONE)
    {
        notify.shape = skyNotify::NOTIFICATION_SHAPE_Circle;

        if(cloudCenter.x() < 0 && cloudCenter.y() < 0) //top left
        {
            notify.startPoint.setX(0);
            notify.startPoint.setY(0);
        }
        else if(cloudCenter.x() < 0 && cloudCenter.y() > sky->getSize().height()) //bottom left
        {
            notify.startPoint.setX(0);
            notify.startPoint.setY(sky->getSize().height());

        }
        else if(cloudCenter.x() > sky->getSize().width() && cloudCenter.y() < 0) //top right
        {
            notify.startPoint.setX(sky->getSize().width());
            notify.startPoint.setY(0);
        }
        else if(cloudCenter.x() > sky->getSize().width() && cloudCenter.y() > sky->getSize().height()) //bottom right
        {
            notify.startPoint.setX(sky->getSize().width());
            notify.startPoint.setY(sky->getSize().height());
        }
        else
        {
            notify.shape = skyNotify::NOTIFICATION_SHAPE_NONE;

        }
    }

    cout << "Start : " <<  notify.startPoint.x() << ":" <<  notify.startPoint.y() << endl;
    cout << "Middle " <<  notify.middlePoint1.x() << ":" <<  notify.middlePoint1.y() << endl;
    cout << "Middle " <<  notify.middlePoint2.x() << ":" <<  notify.middlePoint2.y() << endl;
    cout << "End : " <<  notify.endPoint.x() << ":" <<  notify.endPoint.y() << endl;
    cout << "SHAPE : " << notify.shape << endl;
    notifications.push_back(notify);
    return notify;
}

int skyNotifier::displayNotify(cairo_t* ctx, skyNotify* notify)
{

    //cairo_save(ctx);
    //cairo_set_source_rgba(ctx, 100, 0, 0, 0);
    //cairo_set_operator(ctx, CAIRO_OPERATOR_CLEAR);
    //cairo_paint(ctx);
    //cairo_restore(ctx);


    cout<< "start display" << endl << flush;
    double alphaStep = 0.015;
    double alpha = 0.15;

    double red = 105;
    double green = 105;
    double blue = 105;


    //cairo_set_operator(ctx, CAIRO_OPERATOR_OVERLAY);
    //cairo_set_operator(ctx, CAIRO_OPERATOR_ADD);
    cairo_set_source_rgba(ctx, red, green, blue, notify->currentStep);
    //cairo_set_source_rgba(ctx, red, green, blue, 0.9);

    cairo_arc(ctx, 50 * counter, 500,  100, 0, 2*M_PI);
    cairo_arc(ctx, 50 , 500,  100, 0, 2*M_PI);
    cairo_stroke_preserve(ctx);
    cairo_fill(ctx);

    counter++;

    if(notify->phase == skyNotify::NOTIFICATION_PHASE_FADEIN)
    {
        if(notify->currentStep+alphaStep > alpha)
        {
            //notify->currentStep = 0;
            notify->phase = skyNotify::NOTIFICATION_PHASE_FADEOUT;

        }
        else
        {
            notify->currentStep += alphaStep;
        }

    }
    else if(notify->phase == skyNotify::NOTIFICATION_PHASE_FADEOUT)
    {
        cout << "OUT STEP : " << notify->currentStep << endl;
        notify->currentStep -= alphaStep;
        if(notify->currentStep < 0 )
        {
            notify->currentStep = 0;
            notify->phase = skyNotify::NOTIFICATION_PHASE_NONE;
            counter = 1;
            cout<< "set to none" << endl << flush;
        }


    }

    if(notify->shape == skyNotify::NOTIFICATION_SHAPE_Curve)
    {
        cairo_move_to (ctx, notify->startPoint.x(), notify->startPoint.y());
        cairo_curve_to (ctx, notify->middlePoint1.x(), notify->middlePoint1.y(), notify->middlePoint2.x(), notify->middlePoint2.y(),
                        notify->endPoint.x(), notify->endPoint.y());
        cairo_set_line_width (ctx, 1.0);
        cairo_close_path(ctx);
        cairo_fill(ctx);
    }
    else if(notify->shape == skyNotify::NOTIFICATION_SHAPE_Circle)
    {
        //cout << "CIRCLE" << endl << flush;
        cairo_arc(ctx, notify->startPoint.x(), notify->startPoint.y(), 50, 0, 2*M_PI);
        cairo_stroke_preserve(ctx);
        cairo_fill(ctx);
    }

    return 0;
}



QSize skyNotifier::getPixmapSize(Pixmap pixmap)
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

Pixmap skyNotifier::Sharpen(Pixmap pixmap, unsigned int level)
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

Pixmap skyNotifier::Blur(Pixmap pixmap, unsigned int level)
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

Pixmap skyNotifier::resizePixmap(Pixmap pixmap, int Width, int Height)
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


Pixmap skyNotifier::resizePixmapBlend(Pixmap pixmap, int Width, int Height)
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




Pixmap skyNotifier::getScreenShot(bool showGrid)
{
    if (XDisplay && rootWindowID)
    {
        Pixmap spaceShot = getSpaceShot(showGrid);

        if(spaceShot)
        {
            Pixmap screenShot = XCreatePixmap(XDisplay, rootWindowID, sky->getSize().width(), sky->getSize().height(), DefaultDepth(XDisplay, 0));
            QPoint fromPoint = sky->browsePoint;
            XCopyArea(XDisplay, spaceShot, screenShot, gc, fromPoint.x(), fromPoint.y(), sky->getSize().width(), sky->getSize().height(), 0, 0);
            XFreePixmap(XDisplay, spaceShot);
            return screenShot;
        }
    }
    return 0;
}

Pixmap skyNotifier::getSpaceShot(bool showGrid)
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

QPoint pointOnRect(QPoint from, float minX, float minY, float maxX, float maxY)
{
    float midX = (minX + maxX) / 2;
    float midY = (minY + maxY) / 2;
    // if (midX - x == 0) -> m == ±Inf -> minYx/maxYx == x (because value / ±Inf = ±0)
    float m = (midY - from.y()) / (midX - from.x());

    if (from.x() <= midX) { // check "left" side
        float minXy = m * (minX - from.x()) + from.y();
        if (minY < minXy && minXy < maxY)
            return QPoint(minX, minXy);
    }

    if (from.x() >= midX) { // check "right" side
        float maxXy = m * (maxX - from.x()) + from.y();
        if (minY < maxXy && maxXy < maxY)
            return QPoint(maxX, maxXy);
    }

    if (from.y() <= midY) { // check "top" side
        float minYx = (minY - from.y()) / m + from.x();
        if (minX < minYx && minYx < maxX)
            return QPoint(minYx,minY);
    }

    if (from.y() >= midY) { // check "bottom" side
        float maxYx = (maxY - from.y()) / m + from.x();
        if (minX < maxYx && maxYx < maxX)
            return QPoint(maxYx, maxY);
    }
}

float lineSlope(QPoint A, QPoint B)
{
    if(A.x()-B.x() == 0 || B.x()-A.x() == 0)
    {
        cout << A.x() << ":" << B.x() << endl;
        cout << "OOOPS !!! SLOP IS 0" << endl << flush;
        return 0;
    }


    if(A.x() > B.x() && A.y() > B.y())
    {
        return A.y()-B.y()/A.x()-B.x();
    }
    else
    {
        return B.y()-A.y()/B.x()-A.x();
    }
}

QPoint findLinesIntersection2(QPoint A, QPoint B, QPoint C, QPoint D)
{
    if (( (A.x()-B.x())*(C.y()-D.y()) - (A.y()-B.y())*(C.x()-D.x())  ) == 0)
    {
        return QPoint();
    }

    int x = (((A.x()*B.y()-A.y()*B.x())*(C.x()-D.x())) - ((A.x()-B.x())*(C.x()*D.y()-C.y()*D.x())) ) /
            ( (A.x()-B.x())*(C.y()-D.y()) - (A.y()-B.y())*(C.x()-D.x())  );
    int y = (((A.x()*B.y()-A.y()*B.x())*(C.y()-D.y())) - ((A.y()-B.y())*(C.x()*D.y()-C.y()*D.x())) ) /
            ( (A.x()-B.x())*(C.y()-D.y()) - (A.y()-B.y())*(C.x()-D.x())  );

    return QPoint(x,y);

}

QPoint findLinesIntersection(QPoint A, QPoint B, QPoint C, QPoint D)
{
    int firstLineSlope = lineSlope(A,B);
    int firstLineYIntercept = A.y()-(firstLineSlope*A.x());
    int secondLineSlope = lineSlope(C, D);
    int secondLineYIntercept = C.y()-(secondLineSlope*C.x());

    cout << "DONEEEEEEEEEEEEEEE" << endl << flush;

    if(firstLineSlope == secondLineSlope) // parallel lines
    {
        cout << "DONEEEEEEEEEEEEEEE1" << endl << flush;
        return QPoint();
    }

    if(firstLineYIntercept != secondLineYIntercept) //  the lines are different and there is no intersection
    {
        //cout << "DONEEEEEEEEEEEEEEE2" << endl << flush;
        //return QPoint();
    }



    int x = (secondLineYIntercept-firstLineYIntercept) / (firstLineSlope-secondLineSlope);

    cout << "DONEEEEEEEEEEEEEEE2" << endl << flush;

    int y = (firstLineSlope*secondLineYIntercept) - (secondLineSlope*firstLineYIntercept);


    y /= (firstLineSlope-secondLineSlope);

    cout << "DONEEEEEEEEEEEEEEE" << endl << flush;



    return QPoint(x, y);
}



QPoint findRectLineIntersection(QPoint A, QPoint B, QPoint rectTopLeft, int rectWidth, int rectHeight)
{

    QPoint rectTopRight(rectTopLeft.x()+rectWidth, rectTopLeft.y());
    QPoint rectBottomLeft(rectTopLeft.x(), rectTopLeft.y()+rectHeight);
    QPoint rectBottomRight(rectTopLeft.x()+rectWidth, rectTopLeft.y()+rectHeight);

    //int topLineSlope = lineSlope(rectTopLeft,rectTopRight);
    //int bottomLineSlope = lineSlope(rectBottomLeft,rectBottomRight);
    //int leftLineSlope = lineSlope(rectTopLeft,rectBottomLeft);
    //int rightLineSlope = lineSlope(rectTopRight,rectBottomRight);

    cout << "DONEEEEEEEEEEEEEEE" << endl << flush;

    QPoint AA = findLinesIntersection2(A, B, rectTopLeft, rectTopRight);
    QPoint BB = findLinesIntersection2(A, B, rectBottomLeft, rectBottomRight);
    QPoint C = findLinesIntersection2(A, B, rectTopLeft, rectBottomLeft);
    QPoint D = findLinesIntersection2(A, B, rectTopRight, rectBottomRight);



    if(AA != QPoint())
    {
        return AA;
    }
    else if(BB != QPoint())
    {
        return BB;
    }
    else if(C != QPoint())
    {
        return C;
    }
    else if(D != QPoint())
    {
        return D;
    }

    return QPoint();
}


int skyNotifier::setSizeFactor(unsigned int size)
{
    float width = ceil((sky->getSize().width()*size)/(float)100);
    float height = ceil((sky->getSize().height()*size)/(float)100);


    notifierPixmap = XCreatePixmap(XDisplay, rootWindowID, width, height,
                                  //DefaultDepth(XDisplay, 0));
                                   32);
    return 0;
}

int skyNotifier::getSizeFactor()
{
    return sizeFactor;
}
