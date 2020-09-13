#ifndef SKYNOTIFIER_HPP
#define SKYNOTIFIER_HPP

#include "Common.hpp"
#include "Sky.hpp"

class Sky;

static int counter;

class skyNotify
{
public:
    skyNotify();

    typedef enum
    {
        NOTIFICATION_POS_TOP =0,
        NOTIFICATION_POS_BTTOM,
        NOTIFICATION_POS_RIGHT,
        NOTIFICATION_POS_LEFT,
        NOTIFICATION_POS_TOPLEFT,
        NOTIFICATION_POS_TOPRIGHT,
        NOTIFICATION_POS_BOTTOMLEFT,
        NOTIFICATION_POS_BOTTOMRIGHT,
        NOTIFICATION_POS_NONE
    }
    notificationPosition;

    typedef enum
    {
        NOTIFICATION_SHAPE_Curve =0,
        NOTIFICATION_SHAPE_Circle,
        NOTIFICATION_SHAPE_NONE
    }
    notificationShape;

    typedef enum
    {
        NOTIFICATION_PHASE_FADEIN = 0,
        NOTIFICATION_PHASE_FADEOUT,
        NOTIFICATION_PHASE_NONE
    }
    notificationPhase;

    notificationPosition position;
    notificationShape shape;
    notificationPhase phase;

    // curve/circle points
    QPoint startPoint;
    QPoint middlePoint1;
    QPoint middlePoint2;
    QPoint endPoint;
    double currentStep;

};

class skyNotifier
{
    public:
        explicit skyNotifier(Sky *sky);
        ~skyNotifier();
        int setSizeFactor(unsigned int size);
        int setBorderSize(unsigned int size);
        int Update();

        Pixmap getScreenShot(bool showGrid = false);
        Pixmap getSpaceShot(bool showGrid = false);

        Pixmap resizePixmap(Pixmap pixmap, int Width, int Height);
        Pixmap resizePixmapBlend(Pixmap pixmap, int Width, int Height);

        int setShowScope(bool cond);
        bool getShowScope();
        int setShowIcon(bool cond);
        bool getShowIcon();
        int setShowTitle(bool cond);
        bool getShowTitle();
        int setShowGrid(bool cond);
        bool getShowGrid();
        int setShowThumbnail(bool cond);
        bool getShowThumbnail();

        int getSizeFactor();

        skyNotify addHotCloud(Cloud* cloud);


        QSize getPixmapSize(Pixmap pixmap);




    private:
        Pixmap notifierPixmap;

        Sky* sky;
        QSize browserBox;
        //QGraphicsOpacityEffect* of;
        Window rootWindowID;
        Window overlayWindow;

        unsigned int sizeFactor;

        Pixmap Blur(Pixmap pixmap, unsigned int level = 20);
        Pixmap Sharpen(Pixmap pixmap, unsigned int level =20);
        int displayNotify(cairo_t* ctx, skyNotify* notify);

        QVector<skyNotify> notifications;

        Display* XDisplay;
        GC gc;

        int releaseOverlayWindow();
        Window requestOverlayWindow();
};

#endif // skyNotifier_HPP
