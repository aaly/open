#include "scopeBrowser.hpp"
#include "Sky.hpp"

scopeBrowser::scopeBrowser(QWidget *parent) :
	QWidget(parent)
{
	sky = (Sky*)parent;
	setWindowOpacity(0.1);
	QGraphicsOpacityEffect* of = new QGraphicsOpacityEffect(this);
	of->setOpacity(0.5);
	setGraphicsEffect(of);

	setStyleSheet("scopeBrowser {border: 0px;}");
}

scopeBrowser::~scopeBrowser()
{

}

void scopeBrowser::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(0, 0, width(), height(), Qt::darkGray);
	QWidget::paintEvent(event);

	browserBox.setWidth((width()*sky->width())/sky->getSpaceSize().width());
	browserBox.setHeight((height()*sky->height())/sky->getSpaceSize().height());

	float x = (float)(sky->getBrowsePoint().x()*browserBox.width())/sky->width();
	float y = (float)(sky->getBrowsePoint().y()*browserBox.height())/sky->height();

	if(x > (int)x)
	{
		x+= 2;
	}

	if(y > (int)y)
	{
		y += 2;
	}
	painter.fillRect(x, y, browserBox.width(), browserBox.height(), Qt::white);

	QVector<Cloud*> clouds = sky->getCurrentClouds();

	QPen pen;
	pen.setWidth(5);

	painter.setPen(pen);
	for (int i =0; i <clouds.size(); i++)
	{
		QPoint fromPoint;
		fromPoint.setX((clouds.at(i)->getOrigin().x()*width()) / sky->getSpaceSize().width());
		fromPoint.setY((clouds.at(i)->getOrigin().y()*height()) / sky->getSpaceSize().height());
		QPoint toPoint(fromPoint);
		toPoint.setX(toPoint.x());
		toPoint.setY(toPoint.y());
		if(clouds.at(i)->isHighlighted())
		{
			pen.setColor(Qt::green);
		}
		painter.setPen(pen);
		painter.drawLine(fromPoint, fromPoint);//, toPoint);
		pen.setColor(Qt::black);
	}
}

int scopeBrowser::setSizeFactor(unsigned int size)
{
	float width = (float)(sky->width()*size)/100;
	float height = (float)(sky->height()*size)/100;

	if(width > (int)width)
	{
		width += 1;
	}

	resize(width , height);
	return size;
}



