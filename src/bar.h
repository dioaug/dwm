#pragma once

#include <stdbool.h>

#include "core.h"
#include "draw.h"

enum alignment { Start, Center, End };

typedef struct {
	int ModuleGroupIndex;
	int ModuleIndex;
} ModuleId;

typedef struct {
	unsigned int border_width;
	unsigned int gap;
	enum alignment alignitems;
	DPadding padding;
} BoxStyle;

struct Module;
typedef Dimensions (*ModDrawFunc)(struct Module *mod, Monitor *m, Client *c, Vec2 pos, bool draw, bool updatecontent);

typedef struct {
	// char **elements;
	// size_t num_elements;
	BoxStyle style;
	char *content;
} ModuleChildren;

typedef struct Module {
	ModDrawFunc drawfunc;
	ModuleChildren children;
	double updateinterval;
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
extern pthread_t bar_intervalthreads;
extern bool bar_updating;

void drawbar(Monitor *m, ModuleId* updatespecificmodcontent);
void drawbars(void);
void setupbarmodules();
void bar_initupdateintervals();
void moduledraw(struct Module *mod, enum alignment alignment, Monitor *m, Client *c, bool draw, bool updatecontent);

Dimensions moduledraw_tags(struct Module *mod, Monitor *m, Client *c, Vec2 pos, bool draw, bool updatecontent);
Dimensions moduledraw_rect(struct Module *mod, Monitor *m, Client *c, Vec2 pos, bool draw, bool updatecontent);
Dimensions moduledraw_wintitle(struct Module *mod, Monitor *m, Client *c, Vec2 pos, bool draw, bool updatecontent);
Dimensions moduledraw_time(struct Module *mod, Monitor *m, Client *c, Vec2 pos, bool draw, bool updatecontent);
Dimensions moduledraw_systray(struct Module *mod, Monitor *m, Client *c, Vec2 pos, bool draw, bool updatecontent);