#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <QWidget>
#include "Common.hpp"

class OWindow : public QWidget
{
	Q_OBJECT
public:
	explicit OWindow(QWidget *parent = 0);
	
signals:
	void thumbnailUpdate();
	void windowUpdate();

public slots:

private:
	//X11Window   XWindow;
	QPixmap* thumbnail;
	QPixmap* icon;
	QPixmap* systrayIcon;
};

#endif // WINDOW_HPP
