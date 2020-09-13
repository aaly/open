#ifndef SCOPEBROWSER_HPP
#define SCOPEBROWSER_HPP


#include "Common.hpp"
#include "Sky.hpp"

class Sky;

class scopeBrowser : public Cloud
{
	public:
        explicit scopeBrowser(Sky *sky);
		~scopeBrowser();
		int setSizeFactor(unsigned int size);
        int setBorderSize(unsigned int size);
        int Update();

		
	private:
		Sky* sky;
		QSize browserBox;
        //QGraphicsOpacityEffect* of;
        Window rootWindowID;

        unsigned int borderSize;

        Pixmap Blur(Pixmap pixmap, unsigned int level = 20);
        Pixmap Sharpen(Pixmap pixmap, unsigned int level =20)
;
};




#endif // SCOPEBROWSER_HPP
