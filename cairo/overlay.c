//gcc x11drawoverlay.c -o x11drawoverlay -lX11 -lXfixes -lXcomposite -lcairo -I /usr/include/cairo 

static int		xfixes_event, xfixes_error;
static int		damage_event, damage_error;
static int		composite_event, composite_error;
static int		render_event, render_error;
static int		xshape_event, xshape_error;

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <X11/Xlib.h>

#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <stdlib.h>

#include <unistd.h>

using namespace std;

Display *d;
Window overlay;
Window root;
int width, height;
int counter;

void
 allow_input_passthrough (Window w)
{
    XserverRegion region = XFixesCreateRegion (d, NULL, 0);

    XFixesSetWindowShapeRegion (d, w, ShapeBounding, 0, 0, 0);
    XFixesSetWindowShapeRegion (d, w, ShapeInput, 0, 0, region);

    XFixesDestroyRegion (d, region);
}

void
prep_overlay (void)
{
	XCompositeRedirectSubwindows (d, root, CompositeRedirectAutomatic);
    overlay = XCompositeGetOverlayWindow (d, root);
    cout << "OVERLAY::::::::::" << endl << flush;
    cout << overlay << endl;
    
        
    allow_input_passthrough (overlay);
}

void draw(cairo_t *cr) {
    int quarter_w = width / 4;
    int quarter_h = height / 4;
    //cairo_set_source_rgb(cr, 100, 0.0, 0.0);
    
    
    XClearWindow(d, overlay);
    
    cairo_set_source_rgba(cr, counter%250, 0, 0, 0.5);
    //cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    //cairo_rectangle(cr, quarter_w, quarter_h, quarter_w * 2, quarter_h * 2);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    //cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    //cairo_rectangle(cr, quarter_w, quarter_h, quarter_w * 2, quarter_h * 2);
    cairo_rectangle(cr, 0, 0, width, height);
    //cairo_fill(cr);
    
    //XClearWindow(d, overlay);
    //XFlush(d);
    
    
           //cairo_paint(cr);
	counter+= 5;
	
	cout << "CALLED : " << counter << endl << flush;
}

int main() {
    struct timespec ts = {0, 5000000};


counter = 0;

    d = XOpenDisplay(NULL);

    int s = DefaultScreen(d);
    root = RootWindow(d, s);

    
    if (!XFixesQueryExtension (d, &xfixes_event, &xfixes_error))
    {
	fprintf (stderr, "No XFixes extension\n");
	exit (1);
    }
    if (!XShapeQueryExtension (d, &xshape_event, &xshape_error))
    {
	fprintf (stderr, "No XShape extension\n");
}
	

    
    XSelectInput (d, root, SubstructureNotifyMask);

    width = DisplayWidth(d, s);
    height = DisplayHeight(d, s);

    prep_overlay();

    cairo_surface_t *surf = cairo_xlib_surface_create(d, overlay,
                                  DefaultVisual(d, s),
                                  width, height);
    cairo_t *cr = cairo_create(surf);
    
           

    XSelectInput(d, overlay, ExposureMask);

    
    while(1) {
      //overlay = XCompositeGetOverlayWindow (d, root);
      //XClearWindow(d, overlay);
      draw(cr);
      //XCompositeReleaseOverlayWindow (d, root);
      //nanosleep(&ts, NULL);
      sleep(1);
      
    }

    XEvent ev;
    while (1) {
		draw(cr);
    XNextEvent(d, &ev);
        if (ev.type == Expose) {
            draw(cr);
        }
    }


    
    XCompositeUnredirectSubwindows (d, root, CompositeRedirectAutomatic);

    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    XCloseDisplay(d);
    return 0;
}
