#ifndef _T3_TINT3_H_
#define _T3_TINT3_H_

#include <X11/Xft/Xft.h>
typedef struct ColorSet{
    unsigned long FG;
    unsigned long BG;
    XftColor FG_xft;
} ColorSet;

typedef struct {
    int x, y, w, h;
    int text_offset_y;
    int color_border_pixels;
    int border_width;
    XSetWindowAttributes wa;
    Display *dpy;
    GC gc;
    Pixmap canvas;
    Pixmap empty;
    XftDraw *xftdraw;
    struct {
        int ascent;
        int descent;
        int height;
        XFontSet set;
        XFontStruct *xfont;
        XftFont *xft_font;
    } font;
} DC;

extern Window root;

void drawmenu(void);

int get_x11_cardinal_property(Atom at, Atom type);

ColorSet *make_baritem_colours(char *fg, char *bg);

Atom NET_NUMBER_DESKTOPS
    ,NET_CURRENT_DESKTOP
    ,NET_DESKTOP_NAMES
    ;

Atom _CARDINAL_
    ,_UTF8_STRING_
    ;

#endif
