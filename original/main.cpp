#include <QApplication>
#include "Sky.hpp"
#include <QProcess>


int main(int argc, char *argv[])
{
    //QProcess proc;
    //proc.startDetached("/usr/bin/urxvt");
    QApplication a(argc, argv);
	a.setDesktopSettingsAware(false);
	a.setOverrideCursor( QCursor( Qt::BlankCursor ) );

    Sky w;
    //w.setSpaceSize(1440, 900);
    w.setBackground("bg.jpg");
    w.setBackgroundType(Sky::BG_REPEATED);
    w.Run();
    //w.show();
    return a.exec();
}
