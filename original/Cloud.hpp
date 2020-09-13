#ifndef SPOT_HPP
#define SPOT_HPP

#include <QPainter>
#include "skyItem.hpp"
#include "OWindow.hpp"
#include <QMouseEvent>

class Cloud : public skyItem
{
//	Q_OBJECT
public:
    explicit Cloud(int windowID = -1, Display* XDisplay = NULL);

	bool isHighlighted();

    int Raise();
    int Lower();
    int Resize(int Width, int Height);
    int Move(int X,int Y);
    int Move(QPoint pos);
    QPoint getPosition();
    QPoint getSize();
    bool isLowered();
	
signals:
	
public slots:
		 int setHighlight(bool);

	private:
		QVector<OWindow> windows;
		int Highlight(QPainter* painter);
		bool highlighted;

        Window windowID;
        Display* XDisplay;

	protected:
		void paintEvent(QPaintEvent *event);
		//void moveEvent(QMoveEvent *);
		void mouseMoveEvent(QMouseEvent* event);
		//bool event(QEvent *event);
		void enterEvent(QEvent *);
};

#endif // SPOT_HPP
