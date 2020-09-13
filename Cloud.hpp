#ifndef SPOT_HPP
#define SPOT_HPP

#include "skyItem.hpp"
#include "Common.hpp"

class Cloud : public skyItem
{
public:
    explicit Cloud(int windowID = -1, Display* XDisplay = NULL, bool frame = true);

	bool isHighlighted();

    string getWindowName();
    string getWindowIconifyName();
    Pixmap getWindowIcon();
    QSize   getIconSize();
    QSize getThumbnailSize();
    int moveAndResize(int x, int y, int width, int height);
    int Frame();
    int Raise();
    int Lower();
    int updateThumbnail();
    bool isHidden();
    int Hide(bool subWindows = true);
    int Show(bool subWindows = true);
    int Resize(int Width, int Height);
    int Move(int X,int Y);
    int Move(QPoint pos);
    QPoint getPosition();
    QPoint getBorderedPosition();
    QSize getSize();
    bool isLowered();
    int setHighlight(bool);
    unsigned int getWindowID();
    unsigned int getCoreWindowID();

    GC  gc;

    //int updateThumbnail();

    public:
//		int Highlight(QPainter* painter);
		bool highlighted;
        bool Hidden;
        Window windowID;
        Window coreWindowID;
        Display* XDisplay;
        //QWidget* parent;
        Pixmap thumbnailPixmap;
        unsigned int BORDER_WIDTH;
        int setLiveThumbnailMode(bool cond);
        bool liveThumbnail;
        bool thumbnailMode;
        int drawCloud();

        Damage windowChanges;

	protected:
//		void paintEvent(QPaintEvent *event);
		//void moveEvent(QMoveEvent *);
//		void mouseMoveEvent(QMouseEvent* event);
		//bool event(QEvent *event);
//		void enterEvent(QEvent *);
};

#endif // SPOT_HPP
