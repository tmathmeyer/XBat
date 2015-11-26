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
static XSetWindowAttributes wa;
static Display *disp;
static GC gc;
static Pixmap canvas;


// get a color from a string, and save it into the context
ulng _getcolor(const char *colstr) {
    XColor color;
    if(!XAllocNamedColor(disp, wa.colormap, colstr, &color, &color)) {
        printf("cannot allocate color '%s'\n", colstr);
        return 0;
    }
    return color.pixel;
}

ulng alphaset(ulng color, uint8_t alpha) {
    uint32_t mod = alpha;
    mod <<= 24;
    return (0x00ffffff & color) | mod;
}

uint8_t hex(char c) {
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c-'0');
    }
    if (c >= 'a' && c <= 'f') {
        return (uint8_t)(10+c-'a');
    }
    if (c >= 'A' && c <= 'F') {
        return (uint8_t)(10+c-'A');
    }
    return 0;
}

ulng getcolor(const char *colstr) {
    char *rgbcs = strdup(colstr);
    char *freeme = rgbcs;
    uint8_t value;
    switch(strlen(colstr)) {
        case 4: // #rgb
        case 7: // #rrggbb
            free(freeme);
            return _getcolor(colstr);
        case 9: // #aarrggbb
            value = 16*hex(colstr[1]) + hex(colstr[2]);
            rgbcs += 2;
            break;
        case 5: // #argb
            value = hex(colstr[1]) * 17;
            rgbcs += 1;
            break;
    }

    rgbcs[0] = '#';
    ulng result = _getcolor(rgbcs);
    result = alphaset(result, value);
    free(freeme);
    return result;
}

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

void mapdc() {
    XClearArea(disp, win, 0, 0, width, height, 0);
    XCopyArea(disp, canvas, win, gc, 0, 0, width, height, 0, 0);
    XFlush(disp);
}

void run(void) {
    XEvent xe;
    puts("fuck");
    while(1){
        while(QLength(disp)) {
            XNextEvent(disp, &xe);
        }

        battery = (battery+1)%101;
        unsigned long color = getcolor("#00ccffcc");
        unsigned long color2 = getcolor("#00ff0000");
        drawrect(x, y, width, height,         1, color);
        drawrect(x, y, width, height-battery, 1, color2);
       
        mapdc();

        puts("fuck");
        usleep(50000);
    }
}

void setup() {
    /* create a new draw ctx */
    disp = XOpenDisplay(NULL);
    
    /* set to allow transparency */
    XVisualInfo vinfo;
    XMatchVisualInfo(disp, DefaultScreen(disp), 32, TrueColor, &vinfo);
    wa.colormap = XCreateColormap(disp,
            DefaultRootWindow(disp),
            vinfo.visual,
            AllocNone);
    wa.border_pixel = 0;
    wa.background_pixel = 0;

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
    gc = XCreateGC(disp, win, 0, NULL);

    canvas=XCreatePixmap(disp, root, width, height, vinfo.depth);
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
