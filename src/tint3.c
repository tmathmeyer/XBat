/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include <pwd.h>
#include "draw.h"
#include "lwbi.h"
#include "tint3.h"

#define uchr unsigned char
#define unt unsigned int
#define ulng unsigned long

static void run(void);
static void setup(void);

static int battery = 0;
static int width = 70;
static int height = 200;
static int x = 1850;
static int y = 880;

static Window win;
static Display *disp;
static GC gc;
static Pixmap canvas;


void drawrect(unt x, unt y, unt w, unt h, Bool fill, ulng color) {
    XRectangle rect = {x, y, w, h};
    XSetForeground(disp, gc, color);

    (fill?XFillRectangles:XDrawRectangles)(disp, canvas, gc, &rect, 1);
}

int main() {
    XInitThreads();
    setup();
    run();
    return EXIT_FAILURE;
}

void run(void) {
    XEvent xe;
    while(1){
        while(QLength(disp)) {
            XNextEvent(disp, &xe);
        }

        battery = (battery+1)%101;
        unsigned long color = getcolor(dc, "#00ccffcc");
        unsigned long color2 = getcolor(dc, "#00ff0000");
        drawrect_modifier(dc, dc->x, dc->y, width, height, 1, color);
        drawrect_modifier(dc, dc->x, dc->y, width, height-battery, 1, color2);
       
        mapdc(dc, win, width, height);

        usleep(50000);
    }
}

void setup() {
    /* create a new draw ctx */
    dc = initdc();
    disp = disp;

    /* set to allow transparency */
    XVisualInfo vinfo;
    XMatchVisualInfo(disp, DefaultScreen(disp), 32, TrueColor, &vinfo);
    XSetWindowAttributes wa;
    wa.colormap = XCreateColormap(disp,
            DefaultRootWindow(disp),
            vinfo.visual,
            AllocNone);
    wa.border_pixel = 0;
    wa.background_pixel = 0;
    dc->wa = wa;



    unsigned long window_flags = 
        CWOverrideRedirect|CWEventMask|CWColormap|CWBorderPixel|CWBackPixel;

    int screen = DefaultScreen(disp);
    Window root = RootWindow(disp, screen);

    wa.override_redirect = True;
    wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    win = XCreateWindow(disp, root, x, y, width, height, 0,
            vinfo.depth, InputOutput,
            vinfo.visual,
            window_flags
            ,&wa);
    dc->gc = XCreateGC(disp, win, 0, NULL);

    resizedc(dc, width, height, &vinfo, &wa);
    XMapRaised(disp, win);

    /* set window in dock mode, so that WM wont fuck with it */
    long pval = XInternAtom (disp, "_NET_WM_WINDOW_TYPE_DOCK", False);
    long prop = XInternAtom (disp, "_NET_WM_WINDOW_TYPE", False);
    XChangeProperty(disp,win,prop,XA_ATOM,32,PropModeReplace,(uchr*)&pval,1);

    /* reserve space on the screen */
    prop = XInternAtom(disp, "_NET_WM_STRUT_PARTIAL", False);
    long ptyp = XInternAtom(disp, "CARDINAL", False);
    int16_t strut[12] = {0, 0, height+y, 0, 0, 0, 0, 0, 0, width, 0, 0};
    XChangeProperty(disp,win,prop,ptyp,16,PropModeReplace,(uchr*)strut,12);

    /* This is for support with legacy WMs */
    prop = XInternAtom(disp, "_NET_WM_STRUT", False);
    unsigned long strut_s[4] = {0, height, 0, 0};
    XChangeProperty(disp,win,prop,ptyp,32,PropModeReplace,(uchr*)strut_s,4);

    /* Appear on all desktops */
    prop = XInternAtom(disp, "_NET_WM_DESKTOP", False);
    long adsk = 0xffffffff;
    XChangeProperty(disp,win,prop,ptyp,32,PropModeReplace,(uchr*)&adsk,1);


    XSelectInput(disp, win, ExposureMask);
}
