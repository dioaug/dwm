#pragma once

/* See LICENSE file for copyright and license details. */
#include <X11/Xlib.h>
#include <cairo/cairo.h>

typedef struct {
	Cursor cursor;
} Cur;

typedef struct {
	int width;
	int height;
} Dimensions;

typedef struct {
	double r;
	double g;
	double b;
	double a;
} Color;

typedef struct {
	int t;
	int r;
	int b;
	int l;
} DPadding;

enum { ColFg, ColBg, ColBorder }; /* Clr scheme index */
// typedef XftColor Clr;

typedef struct {
	unsigned int w, h;
	Display *dpy;
	int screen;
	Window root;
	Visual *visual;
	unsigned int depth;
	Colormap cmap;
	
	cairo_t *ctx;
	cairo_surface_t *sur;
	cairo_t *ctx_b;
	cairo_surface_t *sur_b;
	
} Drw;

/* Drawable abstraction */
Drw *drw_create(Display *dpy, int screen, Window win, unsigned int w, unsigned int h, Visual *visual, unsigned int depth, Colormap cmap);
void drw_resize(Drw *drw, unsigned int w, unsigned int h);
void drw_free(Drw *drw);

/* Cursor abstraction */
Cur *drw_cur_create(Drw *drw, int shape);
void drw_cur_free(Drw *drw, Cur *cursor);

/* Drawing functions */
void drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h, double bw, Color bg_color, Color bd_color);
void drw_text(Drw *drw, int x, int y, const char *text, const char *font_desc, Color color);

Dimensions drw_get_textdim(Drw *drw, const char *text, const char *font_desc);

/* Map functions */
void drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h);
void drw_clear(Drw *drw);