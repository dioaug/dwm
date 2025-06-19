/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <pango/pangocairo.h>

#include "drw.h"
#include "util.h"

Drw *
drw_create(Display *dpy, int screen, Window root, unsigned int w, unsigned int h, Visual *visual, unsigned int depth, Colormap cmap)
{
	Drw *drw = ecalloc(1, sizeof(Drw));

	drw->dpy = dpy;
	drw->screen = screen;
	drw->root = root;
	drw->w = w;
	drw->h = h;
	drw->visual = visual;
	drw->depth = depth;
	drw->cmap = cmap;

	// drw->surface = cairo_xlib_surface_create(
    //     dpy, drw->root, visual, w, h
    // );
    // drw->cr = cairo_create(drw->surface);

	drw->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
	drw->cr = cairo_create(drw->surface);
	cairo_set_line_width(drw->cr, 1);
	
	drw->surface_bg = cairo_xlib_surface_create(dpy, root, visual, w, h);
	drw->cr_bg = cairo_create(drw->surface_bg);
	cairo_set_line_width(drw->cr_bg, 1);


	// drw->drawable = XCreatePixmap(dpy, root, w, h, depth);
	// drw->gc = XCreateGC(dpy, drw->drawable, 0, NULL);
	// XSetLineAttributes(dpy, drw->gc, 1, LineSolid, CapButt, JoinMiter);

	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h)
{
	if (!drw)
		return;

	drw->w = w;
	drw->h = h;
	// FIXME: surface is not being resized
	if (drw->surface)
		cairo_xlib_surface_set_size(drw->surface, w, h);
	if (drw->surface_bg)
		cairo_xlib_surface_set_size(drw->surface_bg, w, h);
}

void
drw_free(Drw *drw)
{
	cairo_destroy(drw->cr);
    cairo_surface_destroy(drw->surface);
	cairo_destroy(drw->cr_bg);
    cairo_surface_destroy(drw->surface_bg);

	free(drw);
}

void
drw_rect(cairo_t *cr, int x, int y, unsigned int w, unsigned int h, double bw)
{
    double r = 6;

    cairo_move_to(cr, x + r, y);
    cairo_line_to(cr, x + w - r, y);
    cairo_arc(cr, x + w - r, y + r, r, -M_PI/2, 0);
    cairo_line_to(cr, x + w, y + h - r);
    cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI/2);
    cairo_line_to(cr, x + r, y + h);
    cairo_arc(cr, x + r, y + h - r, r, M_PI/2, M_PI);
    cairo_line_to(cr, x, y + r);
    cairo_arc(cr, x + r, y + r, r, M_PI, 3*M_PI/2);
    cairo_close_path(cr);

    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 1.0);
	
    cairo_fill_preserve(cr);
    cairo_clip_preserve(cr);

    cairo_set_line_width(cr, bw * 2);
    cairo_set_source_rgba(cr, 0.2, 0.2, 0.2, 1.0); // border color
    cairo_stroke(cr);

    cairo_reset_clip(cr);
}

void
drw_text(Drw *drw, int x, int y, const char *text, const char *font_desc) {
	PangoLayout *layout = pango_cairo_create_layout(drw->cr);

    cairo_set_source_rgb(drw->cr, 1.0, 1.0, 1.0);
	pango_layout_set_text(layout, text, -1);
	PangoFontDescription *desc = pango_font_description_from_string(font_desc);
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	cairo_move_to(drw->cr, x, y);

	pango_cairo_show_layout(drw->cr, layout);

	g_object_unref(layout);
}

void clear_surface(cairo_t *cr, unsigned int w, unsigned int h) {
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);

	cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	cairo_rectangle(cr, 0, 0, w, h);
    cairo_fill(cr);

	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
}

void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h) {
	if(!drw)
		return;

	cairo_set_operator(drw->cr_bg, CAIRO_OPERATOR_OVER);

	cairo_set_source_surface(drw->cr_bg, drw->surface, 0, 0);
	cairo_paint(drw->cr_bg);

	cairo_xlib_surface_set_drawable(drw->surface_bg, win, w, h);

	// cairo_surface_write_to_png(drw->surface_bg, "./diowmsurfacebg.png");
	// cairo_surface_write_to_png(drw->surface, "./diowmsurface.png");

	XSync(drw->dpy, False);
}

DDimension
drw_get_textdim(Drw *drw, const char *text, const char *font_desc) {
	DDimension s;
	
	PangoLayout *layout = pango_cairo_create_layout(drw->cr);

    pango_layout_set_text(layout, text, -1);
    PangoFontDescription *desc = pango_font_description_from_string(font_desc);
    pango_layout_set_font_description(layout, desc);

    int width, height;
    pango_layout_get_size(layout, &width, &height);  // in Pango units (1 Pango unit = 1/1024 of a point)
	
    pango_font_description_free(desc);
    g_object_unref(layout);
	
	s.width = width / PANGO_SCALE;
	s.height = height / PANGO_SCALE;

    return s;
}

Cur *
drw_cur_create(Drw *drw, int shape)
{
	Cur *cur;

	if (!drw || !(cur = ecalloc(1, sizeof(Cur))))
		return NULL;

	cur->cursor = XCreateFontCursor(drw->dpy, shape);

	return cur;
}

void
drw_cur_free(Drw *drw, Cur *cursor)
{
	if (!cursor)
		return;

	XFreeCursor(drw->dpy, cursor->cursor);
	free(cursor);
}
