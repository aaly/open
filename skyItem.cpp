#include "skyItem.hpp"


skyItem::skyItem()
{
}


/*int skyItem::Highlight()
{
	return 0;
}

int skyItem::setHighlight(bool cond)
{
	return 0;
}*/

unsigned int skyItem::setDivingLevel(unsigned int level)
{
	divingLevel = level;
	return divingLevel;
}


unsigned int skyItem::getDivingLevel()
{
	return divingLevel;
}


int skyItem::setOrigin(const QPoint& point)
{
	origin = point;
    //move(point);
	return 0;
}

QPoint skyItem::getOrigin()
{
	return origin;
}
