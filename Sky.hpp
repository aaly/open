#ifndef OPEN_HPP
#define OPEN_HPP

#include "Common.hpp"
#include "Cloud.hpp"


#include "scopeBrowser.hpp"
#include "skyScope.hpp"
#include "skyNotifier.hpp"



#ifndef PI
#define PI  3.14159265
#endif

#define MAX(a, b) ((a) > (b) ? (a) : (b))

class scopeBrowser;
class skyScope;
class skyNotifier;

class Sky
{
public:

    // must be declared before all or get segfaut in xlib funcations !
    std::mutex Mutex;
    atomic<int> Timer;
    atomic<int> finalBrowsePointCounterX;
    atomic<int> finalBrowsePointCounterY;
    //atomic<int> rootOverlayCounter;
    int rootOverlayCounter;

    //atomic<int> currentBrowsePointCounterX;
    //atomic<int> currentBrowsePointCounterY;
    //QPoint finalBrowsePointCounter;
    //QPoint currentBrowsePointCounter;
    //int Timer;




    //static int OnXError(Display* display, XErrorEvent* e);
    //static int OnXIOError(Display* display);
    //static string XRequestCodeToString(unsigned char request_code);
    int Run();


    Display* getDisplay();
    Window getRootWindow();
    QPoint findAvailableSpace(Cloud* cloud);
    int selectEvents(Window windowID);
    Cloud* findCloudByID(Window WindowID);
    Cloud* findCloudByWindow(Window WindowID);
    Cloud* findCloudByCoreWindow(Window WindowID);
    QVector<Cloud*> onScreenClouds;
    int processEvents();
    int updateOnScreenClouds();
    //explicit Sky(QObject *parent = 0);
    explicit Sky();
	~Sky();
	int setSpaceSize(unsigned int width, unsigned int height);
    QSize getSpaceSize();

    int setBackground(const QString& bg);
    //const QPixmap getBackground();

	typedef enum
	{
		BG_SCALED =0,
		BG_CLIPPED,
        BG_REPEATED,
        BG_CENTERED
	}
	backgroundType;

    typedef enum
    {
        BROWSING =0,
        SELECTING,
        HYBRID,
        DIVING
    }
    operatingMode;

    typedef enum
    {
        SCOPE =0,
        ZOOM,
        CLOSE,
        RUN,
        MOVE,
        RESIZE,
        SELECT,
        DIVE
    }
    Operations;




	int setBackgroundType(const backgroundType& type);
	backgroundType getBackgroundType();

	unsigned long getDivingLevel();

    QPoint getBrowsePoint();
    int moveBackground();
    const map<unsigned int, QVector<Cloud *> > &getClouds();
    const QVector<Cloud*>& getCurrentClouds();
    int     Resize(int Width, int Height);
    QSize   getSize();


    int calcualteUniverseCenter();
    int updateHighLights();
    int updateScopes();
    int updateSky(bool ignoreScope = false);
    int Zoom(int level);
    int addWindow(Window WindowID);
    int removeWindow(Window WindowID);
    int setHighlightedCloud(Cloud* cloud);
    int setLiveThumbnailMode(bool cond);
    int resizeClouds(int direction);
    int moveClouds(int direction);
    int checkDiving(int direction);
    int historySelect();
    bool isOnScreen(Cloud* cloud);
    int resetCursor();
    bool cloudContainable(int x, int y, int width, int height);
    void Execute(string cmd, string arg = "");
    int centerAtCloud(Cloud* cloud);
    Window requestOverlayWindow();
    int releaseOverlayWindow();

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

    //void drawBackground(QPainter *painter);
	int updateBackground(bool refresh);
    int triggerBrowseMovement();


    bool isSpaceMaximized(Cloud* cloud);
    bool isScreenMaximized(Cloud* cloud);
    int setCurrentOperation(unsigned int op);
public:
	void Exit();
    int Browse();
    void browsingThreadFunction();
    void notifierThreadFunction();
    int callThread();

Imlib_Image localBackground;
QPoint browsePoint;

    private:



    int browseDuration;

    bool CONFIG_ONSCREENHISTORY;

    Cloud* highlightedCloud;
    //unsigned int currentOperatingMode;
    unsigned int currentOperation;
    bool liveThumbnailMode;
    skyScope* skyscope;
    skyNotifier* skynotifier;
    //QTimeLine* scopeTimeLine;
    //QTimeLine* divingTimeLine;

    QSize rootSize;
    QSize spaceSize;
    QRectF universeCenter;
////	QVector<Spots*> spots;

    Imlib_Image backgroundImage;
    //QPixmap background;
    QString backgroundPath;
    backgroundType backgroundtype;


    QPoint oldMousePosition;

    unsigned long currentDivingLevel;
    unsigned long divingFactor;

    map<unsigned int, QVector<Cloud*> > clouds;
    QVector<Cloud*> cloudsHistory;

    screenOrientation screenorientation;


    bool scoping;
    QPoint scopeToPoint;

    bool allowCollision;
    bool scopeBackground;
    bool diveBackground;

    Cloud* cloudSelected;

    skyScope* scopebrowser;


    int browseFactor;

    QPoint browsePointCounter;


    unsigned int zoomFactor;


    bool Control;
    bool Alt;
    bool Alternate;
    bool Shift;
    bool Super;
    unsigned int rightClick;
    unsigned int leftClick;
    bool middleClick;
    int shiftCounter;

    bool cloudLock;
    bool autoScope;

    int controlCounter;
    int altCounter;
    int superCounter;

    bool ignoreCursorMovement;


    unsigned int browseMoveThreshold;
    unsigned int mouseMoveCounter;
    unsigned int keyboardBrowseAceleration;


    Display* dpy;
    Window root;
    XWindowAttributes attr;
    XButtonEvent start;
    XEvent ev;
    Window rootOverlay;

    bool CONFIG_GRAVITY;
    int fixedBrowseFactor;
    int damage_event, damage_error; // The event base is important here


   /**********************************
    *
    * Ease Functions
    *
   ***********************************/

    private:
    //https://github.com/jesusgollonet/ofpennereasing
    float easeInExpo (float t,float b , float c, float d);
    float easeOutExpo(float t,float b , float c, float d);
    float easeInOutExpo(float t,float b , float c, float d);
    float easeInCirc (float t,float b , float c, float d);
    float easeOutCirc(float t,float b , float c, float d);
    float easeInOutCirc(float t,float b , float c, float d);
    float easeInSine (float t,float b , float c, float d);
    float easeOutSine(float t,float b , float c, float d);
    float easeInOutSine(float t,float b , float c, float d);




   /**********************************
    *
    * Clouds
    *
   ***********************************/

   private:
    int moveCloud(bool moveOnScreen = false);
    int resizeCloud(bool resizeOnSceen  = false);
    int maximizeCloud(Cloud* cloud, bool fullSpace = false);
    int centerCloud(Cloud* cloud);
    int addCloud(Cloud* cloud);
    int deleteCloud(Cloud* cloud);
    int selectCloud(Cloud* cloud);
    int deSelectCloud();
    int closeCloud(Cloud* cloud);



   /**********************************
    *
    * Events
    *
   ***********************************/
    private:
        int mouseMoveEvent(XMotionEvent ev);
        int keyPressEvent(XKeyEvent ev);
        int keyReleaseEvent(XKeyEvent ev);
        int buttonReleaseEvent(XEvent ev);
        int buttonPressEvent(XEvent ev);


};

#endif // OPEN_HPP

