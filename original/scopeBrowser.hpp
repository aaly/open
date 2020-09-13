#ifndef SCOPEBROWSER_HPP
#define SCOPEBROWSER_HPP

#include <QWidget>
#include <QPainter>
#include <QGraphicsOpacityEffect>
//#include "Sky.hpp"

class Sky;

class scopeBrowser : public QWidget
{
		Q_OBJECT
	public:
		explicit scopeBrowser(QWidget *parent = 0);
		~scopeBrowser();
		int setSizeFactor(unsigned int size);
		
	signals:
		
	public slots:
	protected:
		void paintEvent(QPaintEvent *event);
	private:
		Sky* sky;
		QSize browserBox;
};

#endif // SCOPEBROWSER_HPP
