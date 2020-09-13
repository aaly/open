#ifndef OPEN_HPP
#define OPEN_HPP

#include <QWidget>
#include "Common.hpp"
#include "Cloud.hpp"
#include <QLabel>
#include <QMouseEvent>
#include <QMap>
#include <QTimeLine>
#include <QDesktopWidget>
#include <QTimer>
#include <QApplication>
#include "scopeBrowser.hpp"
#include <QProcess>

#include <string>
#include <QThread>



#define MAX(a, b) ((a) > (b) ? (a) : (b))

static Display* dpy;
static Window root;
static XWindowAttributes attr;
static XButtonEvent start;
static XEvent ev;


class scopeBrowser;

class Sky : public QWidget
{
    Q_OBJECT
    
public:
    int Run();
    static int processEvents(Sky *);
	explicit Sky(QWidget *parent = 0);
	~Sky();
	int setSpaceSize(unsigned int width, unsigned int height);
	QSize getSpaceSize();

	int setBackground(const QString& bg);
	const QPixmap getBackground();

	typedef enum
	{
		BG_SCALED =0,
		BG_CLIPPED,
		BG_REPEATED
	}
	backgroundType;

	int setBackgroundType(const backgroundType& type);
	backgroundType getBackgroundType();


	Cloud* cloud1;
	Cloud* cloud2;
	Cloud* cloud3;


	unsigned long getDivingLevel();

	QPoint getBrowsePoint();

	const QMap<unsigned int, QVector<Cloud*> >& getClouds();
	const QVector<Cloud*>& getCurrentClouds();

private:
	QSize spaceSize;
	QRectF universeCenter;
////	QVector<Spots*> spots;

	QPixmap background;
	QString backgroundPath;
	backgroundType backgroundtype;


	QPointF oldMousePosition;

	unsigned long currentDivingLevel;
	unsigned long divingFactor;



	int calcualteUniverseCenter();

	QMap<unsigned int, QVector<Cloud*> > clouds;



	int addCloud(Cloud* cloud);
	int deleteCloud(Cloud* cloud);

	typedef enum
	{
		RIGHT =1,
		LEFT = 1 << 1,
		UP = 1 << 2,
		DOWN = 1 << 3,
		IN = 1 << 4,
		OUT = 1 << 5
	}
	movementDirection;

	typedef enum
	{
		VERTICAL =0,
		HORIZONTAL
	}
	screenOrientation;

	screenOrientation screenorientation;

	QLabel* label;

	Cloud* highlightedCloud;

	QTimeLine* scopeTimeLine;

	QTimeLine* divingTimeLine;

	bool scoping;
	QPointF scopeToPoint;

	bool allowCollision;
	bool scopeBackground;
	bool diveBackground;

	scopeBrowser* scopebrowser;

	int Browse(const int& direction, bool smooth);
	int browseFactor;
	QPoint browsePoint;


	unsigned int zoomFactor;

	int Zoom(int level);

    int addWindow(Window WindowID);
	bool Control;
	bool Alternate;
	bool Shift;
	bool rightClick;
	bool leftClick;
	bool middleClick;

	bool cloudLock;

	bool autoScope;

	int resizeClouds(int direction);
	int moveClouds(int direction);

	int checkDiving(int direction);

	bool ignoreCursorMovement;
	int resetCursor();

	bool cloudContainable(int x, int y, int width, int height);

	QGraphicsDropShadowEffect* cloudsShdow;

protected:
	void drawBackground(QPainter *painter);
	int updateBackground(bool refresh);
	bool event(QEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent *event);

	void paintEvent(QPaintEvent *event);

	void keyReleaseEvent(QKeyEvent* event);
	void keyPressEvent(QKeyEvent* event);




public slots:
	void Exit();
	int smoothMovement();
	int Scope();
	int smoothScope();
	int checkScope();

	unsigned long Dive(int level);
};

#endif // OPEN_HPP

