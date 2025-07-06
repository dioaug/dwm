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

	if(!drw)
		return NULL;
	drw->dpy = dpy;
	drw->screen = screen;
	drw->w = w;
	drw->h = h;

	drw->sur = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, w, h);
	drw->ctx = cairo_create(drw->sur);
	cairo_set_line_width(drw->ctx, 1);

	drw->sur_b = cairo_xlib_surface_create(dpy, root, DefaultVisual(dpy, screen), w, h);
	drw->ctx_b = cairo_create(drw->sur_b);
	cairo_surface_destroy(drw->sur_b);

	return drw;
}

void
drw_resize(Drw *drw, unsigned int w, unsigned int h)
{
	if(!drw)
		return;
	drw->w = w;
	drw->h = h;
}

void
drw_free(Drw *drw)
{
	cairo_surface_finish(drw->sur);
	cairo_destroy(drw->ctx);
	cairo_destroy(drw->ctx_b);
	free(drw);
}

void
drw_rect(Drw *drw, int x, int y, unsigned int w, unsigned int h, double bw, Color bg_color, Color bd_color)
{
    double r = 6;

    cairo_move_to(drw->ctx, x + r, y);
    cairo_line_to(drw->ctx, x + w - r, y);
    cairo_arc(drw->ctx, x + w - r, y + r, r, -M_PI/2, 0);
    cairo_line_to(drw->ctx, x + w, y + h - r);
    cairo_arc(drw->ctx, x + w - r, y + h - r, r, 0, M_PI/2);
    cairo_line_to(drw->ctx, x + r, y + h);
    cairo_arc(drw->ctx, x + r, y + h - r, r, M_PI/2, M_PI);
    cairo_line_to(drw->ctx, x, y + r);
    cairo_arc(drw->ctx, x + r, y + r, r, M_PI, 3*M_PI/2);
    cairo_close_path(drw->ctx);

    cairo_set_source_rgba(drw->ctx, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
	
    cairo_fill_preserve(drw->ctx);
    cairo_clip_preserve(drw->ctx);

    cairo_set_line_width(drw->ctx, bw * 2);
    cairo_set_source_rgba(drw->ctx, bd_color.r, bd_color.g, bd_color.b, bd_color.a);
    cairo_stroke(drw->ctx);

    cairo_reset_clip(drw->ctx);
}

void
drw_text(Drw *drw, int x, int y, const char *text, const char *font_desc, Color color) {
	if (text == NULL || *text == '\0')
        return;

	PangoLayout *layout = pango_cairo_create_layout(drw->ctx);

    cairo_set_source_rgb(drw->ctx, color.r, color.g, color.b);
	pango_layout_set_text(layout, text, -1);
	PangoFontDescription *desc = pango_font_description_from_string(font_desc);
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	cairo_move_to(drw->ctx, x, y);

	pango_cairo_show_layout(drw->ctx, layout);

	g_object_unref(layout);
}

void
drw_clear(Drw *drw) {
	if(!drw)
		return;
	cairo_save(drw->ctx);
	cairo_set_operator(drw->ctx, CAIRO_OPERATOR_CLEAR);
	cairo_paint(drw->ctx);
	cairo_restore(drw->ctx);
}

void
drw_map(Drw *drw, Window win, int x, int y, unsigned int w, unsigned int h) {
	if(!drw)
		return;
	cairo_xlib_surface_set_drawable(drw->sur_b, win, w, h);
	cairo_set_source_surface(drw->ctx_b, drw->sur, x, y);
	cairo_paint(drw->ctx_b);
	XSync(drw->dpy, False);
}

Dimensions
drw_get_textdim(Drw *drw, const char *text, const char *font_desc) {
	if (text == NULL || *text == '\0')
        return (Dimensions){0,0};
	
	Dimensions s;
	
	PangoLayout *layout = pango_cairo_create_layout(drw->ctx);

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
