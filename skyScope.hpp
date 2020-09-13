#ifndef SKYSCOPE_HPP
#define SKYSCOPE_HPP

#include "Common.hpp"
#include "Sky.hpp"

class Sky;

class skyScope : public Cloud
{
    public:
        explicit skyScope(Sky *sky);
        ~skyScope();
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
        int setTransparent(bool cond);
        bool isTransparent();

        int getSizeFactor();


        QSize getPixmapSize(Pixmap pixmap);



    private:
        bool showThumbnail;
        bool showGrid;
        bool showTitle;
        bool showScope;
        bool showIcon;
        bool transparent;
        Pixmap skyBackground;
        Pixmap browserPixmap;

        Sky* sky;
        QSize browserBox;
        //QGraphicsOpacityEffect* of;
        Window rootWindowID;

        unsigned int borderSize;
        unsigned int sizeFactor;

        Pixmap Blur(Pixmap pixmap, unsigned int level = 20);
        Pixmap Sharpen(Pixmap pixmap, unsigned int level =20)
;
};

#endif // SKYSCOPE_HPP
