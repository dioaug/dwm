#pragma once

#include "core.h"

enum alignment { Start, Center, End };

typedef struct {
	unsigned int border_width;
	unsigned int gap;
	enum alignment alignitems;
	DPadding padding;
} BoxStyle;

struct Module;
typedef Dimensions (*ModDrawFunc)(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw);

typedef struct {
	// char **elements;
	// size_t num_elements;
	BoxStyle style;
} ModuleChildren;

typedef struct Module {
	ModDrawFunc drawfunc;
	ModuleChildren children;
	Vec2 modulesize;
	BoxStyle style;
} Module;

typedef struct {
	Module *modules;
	Dimensions dimensions;
	int nextelementxpos;
	BoxStyle style;
} ModuleGroup;

typedef struct {
	Dimensions dimensions;
	BoxStyle style;
} Bar;

enum ModuleGroupType {
	ModuleGroupLeft,
	ModuleGroupCenter,
	ModuleGroupRight,
};

extern ModuleGroup modulegroups[3];
extern Bar bar;

void setupbarmodules();
void moduledraw(struct Module *mod, enum alignment alignment, Monitor *m, Client *c, int shoulddraw);

Dimensions moduledraw_tags(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw);
Dimensions moduledraw_rect(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw);
Dimensions moduledraw_textrect(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw);
Dimensions moduledraw_systray(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw);