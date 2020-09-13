#ifndef SKYITEM_HPP
#define SKYITEM_HPP


#include "Common.hpp"

class skyItem
{
	public:
        explicit skyItem();

		unsigned int getDivingLevel();
		unsigned int setDivingLevel(unsigned int);

        int setOrigin(const QPoint &point);
        QPoint getOrigin();

	private:
		//virtual int Highlight() = 0;
		unsigned int divingLevel;
        QPoint origin;
		//bool visible;
		
};

#endif // SKYITEM_HPP
