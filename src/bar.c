#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "bar.h"
#include "config.h"
#include "util.h"
#include "draw.h"
#include "dynarray.h"

const char *font_desc_temp = "sans-serif bold 8";
pthread_t bar_intervalthreads;
bool bar_updating;

ModuleGroup modulegroups[3];
Bar bar = {
    .dimensions = {0,1},
    .style = (BoxStyle){
        // .border_width = 2, 
        .alignitems = Center,
        .padding = {2, 2, 2, 2},
    },
};


void
drawbar(Monitor *m, ModuleId* updatespecificmodcontent)
{
	int stw = 0;
	unsigned occ = 0, urg = 0;
	Client *c;

	if (!m->showbar || (bar_updating && updatespecificmodcontent == NULL))
		return;

	bar_updating = true;
	drw_clear(drw);

	cairo_set_source_rgba(drw->ctx, 0.1, 0.1, 0.1, 1.0);
	cairo_rectangle(drw->ctx, 0, 0, m->ww, bar.dimensions.height);
	cairo_fill(drw->ctx);

	for (int modulegroup = 0; modulegroup < LENGTH(modulegroups); modulegroup++) {
		modulegroups[modulegroup].dimensions = (Dimensions){0,0};
		modulegroups[modulegroup].nextelementxpos = 0;

		if (dynarray_length(modulegroups[modulegroup].modules) == 0)
			continue;
		
		// doing this to get the modulegroup dimensions before drawing the modules
		for (int i = 0; i < dynarray_length(modulegroups[modulegroup].modules); i++) {

			bool eita = false;

			if (updatespecificmodcontent != NULL &&
				updatespecificmodcontent->ModuleGroupIndex == modulegroup && 
				updatespecificmodcontent->ModuleIndex == i)
					eita = true;
			else
			 	eita = false;


			moduledraw(&modulegroups[modulegroup].modules[i], modulegroup, m, c, 0, eita);
		}

		if (modulegroups[modulegroup].dimensions.width == 0)
			continue;

		if (modulegroups[modulegroup].dimensions.height > bar.dimensions.height){
			int newbarheight = modulegroups[modulegroup].dimensions.height
			+ bar.style.padding.t + bar.style.padding.b;

			if (bar.dimensions.height != newbarheight) {
				bar.dimensions.height = newbarheight;
				updatebarpos(m);
				arrange(m);
			}
		}
		resizebarwin(m);

		for (c = m->clients; c; c = c->next) {
			occ |= c->tags;
			if (c->isurgent)
				urg |= c->tags;
		}

		modulegroups[modulegroup].dimensions.width += (modulegroups[modulegroup].style.border_width * 2) 
		+ modulegroups[modulegroup].style.padding.l + modulegroups[modulegroup].style.padding.r
		- modulegroups[modulegroup].style.gap;

		int modgroupx = 0;
		int modgroupy = 0;

		switch (modulegroup) {
			default:
			case ModuleGroupLeft:
				modgroupx = 0 + bar.style.padding.l;
				break;
			case ModuleGroupCenter:
				modgroupx = (m->ww / 2) - (modulegroups[modulegroup].dimensions.width / 2);
				break;
			case ModuleGroupRight:
				modgroupx = m->ww - modulegroups[modulegroup].dimensions.width - bar.style.padding.r;
				break;
		}

		switch (bar.style.alignitems) {
			case Start:
				modgroupy = 0;
				break;
			case Center:
				modgroupy = ((bar.dimensions.height - bar.style.padding.t - bar.style.padding.b) - modulegroups[modulegroup].dimensions.height) / 2;
				break;
			case End:
				modgroupy = (bar.dimensions.height - bar.style.padding.t - bar.style.padding.b) - modulegroups[modulegroup].dimensions.height;
				break;
		}

		drw_rect(drw,
			modgroupx, modgroupy + bar.style.padding.t,
			modulegroups[modulegroup].dimensions.width, modulegroups[modulegroup].dimensions.height, modulegroups[modulegroup].style.border_width,
			(Color){0.1, 0.1, 0.1, 0.4},
			(Color){0.2, 0.2, 0.2, 1.0}
		);
		
		for (int i = 0; i < dynarray_length(modulegroups[modulegroup].modules); i++) {
			bool eita = false;

			if (updatespecificmodcontent != NULL &&
				updatespecificmodcontent->ModuleGroupIndex == modulegroup && 
				updatespecificmodcontent->ModuleIndex == i)
					eita = true;
			else
			 	eita = false;

			moduledraw(&modulegroups[modulegroup].modules[i], modulegroup, m, c, 1, eita);
		}
	}

	drw_map(drw, m->barwin, 0, 0, m->ww, bar.dimensions.height);

	bar_updating = false;
}

void
drawbars(void)
{
	Monitor *m;

	for (m = mons; m; m = m->next)
		drawbar(m, NULL);
}

void
setupbarmodules()
{
	for (int i = 0; i < LENGTH(modulegroups); i++) {
		modulegroups[i] = (ModuleGroup){
			.modules = dynarray_create(Module),
			.dimensions = {0,0},
			.nextelementxpos = 0,
			.style = {
				.border_width = 2, 
				.padding = {2, 2, 2, 2},
				.alignitems = Center,
				.gap = 4,
			},
		};
	}

	Module module_tags = (Module){
		.drawfunc = moduledraw_tags,
		.updateinterval = 0,
		.children = {
			// .elements = boxtest,
			// .num_elements = LENGTH(boxtest),
			.style = {
				.border_width = 2, 
				.padding = {1, 6, 1, 6},
				.alignitems = Start,
				.gap = 0,
			},
		},
		.modulesize = {.x=0,.y=0},
		.style = {
			.border_width = 2, 
			.padding = {2, 2, 2, 2},
			.alignitems = Start,
			.gap = 2,
		}
	};

	Module module_rect = (Module){
		.drawfunc = moduledraw_rect,
		.updateinterval = 0,
		.children = {
			// .elements = boxtest,
			// .num_elements = LENGTH(boxtest),
			.style = {
				.border_width = 2, 
				.padding = {1, 6, 1, 6},
				.alignitems = Start,
				.gap = 0,
			},
		},
		.modulesize = {.x=0,.y=0},
		.style = {
			.border_width = 2, 
			.padding = {2, 2, 2, 2},
			.alignitems = Start,
			.gap = 2,
		}
	};

	Module module_wintitle = (Module){
		.drawfunc = moduledraw_wintitle,
		.updateinterval = 0,
		.children = {
			// .elements = boxtest,
			// .num_elements = LENGTH(boxtest),
			.style = {
				.border_width = 2, 
				.padding = {1, 6, 1, 6},
				.alignitems = Start,
				.gap = 0,
			},
		},
		.modulesize = {.x=0,.y=0},
		.style = {
			.border_width = 2, 
			.padding = {2, 2, 2, 2},
			.alignitems = Start,
			.gap = 2,
		}
	};

	Module module_time = (Module){
		.drawfunc = moduledraw_time,
		.updateinterval = 1,
		.children = {
			.content = (char *)malloc(sizeof(char) * 100),
			// .elements = boxtest,
			// .num_elements = LENGTH(boxtest),
			.style = {
				.border_width = 2, 
				.padding = {1, 6, 1, 6},
				.alignitems = Start,
				.gap = 0,
			},
		},
		.modulesize = {.x=0,.y=0},
		.style = {
			.border_width = 2, 
			.padding = {2, 2, 2, 2},
			.alignitems = Start,
			.gap = 2,
		}
	};
	snprintf(module_time.children.content, 100, "0");

	Module module_systray = (Module){
		.drawfunc = moduledraw_systray,
		.updateinterval = 0,
		.children = {
			// .elements = boxtest,
			// .num_elements = LENGTH(boxtest),
			.style = {
				.border_width = 2, 
				.padding = {1, 6, 1, 6},
				.alignitems = Start,
				.gap = 0,
			},
		},
		.modulesize = {.x=0,.y=0},
		.style = {
			.border_width = 2, 
			.padding = {0, 0, 0, 0},
			.alignitems = Start,
			.gap = 2,
		}
	};

	dynarray_push(modulegroups[ModuleGroupLeft].modules, module_tags);
	dynarray_push(modulegroups[ModuleGroupCenter].modules, module_wintitle);
	dynarray_push(modulegroups[ModuleGroupRight].modules, module_systray);
	dynarray_push(modulegroups[ModuleGroupRight].modules, module_time);
}

void* bar_moduleroutine(void* arg) {
	ModuleId mID = *(ModuleId*)arg;
	free(arg);

	while (1) {
		while (bar_updating)
			usleep(10000);
		drawbar(selmon, &mID);

		usleep(modulegroups[mID.ModuleGroupIndex].modules[mID.ModuleIndex].updateinterval * 1000000);
	}

	return NULL;
}

void
bar_initupdateintervals()
{
	ModuleId *validintervals = dynarray_create(ModuleId);

	for (int modulegroup = 0; modulegroup < LENGTH(modulegroups); modulegroup++) {
		for (int i = 0; i < dynarray_length(modulegroups[modulegroup].modules); i++) {
			if (modulegroups[modulegroup].modules[i].updateinterval > 0) {
				ModuleId mId = (ModuleId){.ModuleGroupIndex = modulegroup, .ModuleIndex = i};
				dynarray_push(validintervals, mId);
			}
		}
	}

	for (size_t i = 0; i < dynarray_length(validintervals); ++i) {
		ModuleId* modinfo = malloc(sizeof(ModuleId));

		*modinfo = validintervals[i];
		if (pthread_create(&bar_intervalthreads, NULL, bar_moduleroutine, modinfo) != 0) {
			perror("Failed to create module interval thread");
			free(modinfo);
		}
	}

	dynarray_destroy(validintervals);
}

void
moduledraw(struct Module *mod, enum alignment alignment, Monitor *m, Client *c, int shoulddraw, bool updatecontent)
{
	Dimensions modsize = mod->drawfunc(mod, m, c, (Vec2){0,0}, 0, updatecontent);
	if (!shoulddraw) {
		if (modsize.width != 0 || modsize.width != 0) {
			int moduleheight = modsize.height + (mod->style.border_width * 2) + (modulegroups[alignment].style.border_width * 2) +
			modulegroups[alignment].style.padding.t + modulegroups[alignment].style.padding.b +
			mod->style.padding.t + mod->style.padding.b;
			
			if (moduleheight > modulegroups[alignment].dimensions.height)
				modulegroups[alignment].dimensions.height = moduleheight;
		
			int modulewidth = modsize.width + (mod->style.border_width * 2)
			+ mod->style.padding.l + mod->style.padding.r;
			
			modulegroups[alignment].dimensions.width += modulewidth + modulegroups[alignment].style.gap;
		}
		return;
	}

	unsigned int groupalignmentoffset = 0;
	
	int moduleheight = modsize.height
	+ (mod->style.border_width * 2) + (modulegroups[alignment].style.border_width * 2)
	+ mod->style.padding.t + mod->style.padding.b;

	int baralignitemsyoffset = 0;
	int alignitemsyoffset = 0;

	switch (bar.style.alignitems) {
		case Start:
			baralignitemsyoffset = 0;
			break;
		case Center:
			baralignitemsyoffset = ((bar.dimensions.height - bar.style.padding.t - bar.style.padding.b) - modulegroups[alignment].dimensions.height) / 2;;
			break;
		case End:
			baralignitemsyoffset = (bar.dimensions.height - bar.style.padding.t - bar.style.padding.b) - modulegroups[alignment].dimensions.height;
			break;
	}

	int test = (modulegroups[alignment].dimensions.height 
		- (modulegroups[alignment].style.padding.t)
		- (modulegroups[alignment].style.padding.b)) - moduleheight;

	switch (modulegroups[alignment].style.alignitems) {
		case Start:
			alignitemsyoffset = 0 + bar.style.padding.t;
			break;
		case Center:
			alignitemsyoffset = test / 2 + bar.style.padding.t;
			break;
		case End:
			alignitemsyoffset = test + bar.style.padding.t;
			break;
	}

	switch (alignment) {
		case ModuleGroupLeft:
			groupalignmentoffset = 0 + bar.style.padding.l;
			break;
		case ModuleGroupCenter:
			groupalignmentoffset = (m->ww / 2) - (modulegroups[alignment].dimensions.width / 2);
			break;
		case ModuleGroupRight:
			groupalignmentoffset = m->ww - modulegroups[alignment].dimensions.width 
			- bar.style.padding.r;
			break;
	}
	
	Vec2 modpos = (Vec2){
		.x = modulegroups[alignment].nextelementxpos + mod->style.border_width + modulegroups[alignment].style.border_width +
		mod->style.padding.l + modulegroups[alignment].style.padding.l +
		groupalignmentoffset,
		.y = mod->style.border_width + modulegroups[alignment].style.border_width +
		mod->style.padding.t + modulegroups[alignment].style.padding.t + 
		alignitemsyoffset + baralignitemsyoffset,
	};
	
	if (modsize.width != 0 || modsize.width != 0) {
		drw_rect(drw,
			modulegroups[alignment].nextelementxpos + modulegroups[alignment].style.border_width + 
			modulegroups[alignment].style.padding.l +
			groupalignmentoffset
			,
			modulegroups[alignment].style.border_width + 
			modulegroups[alignment].style.padding.t +
			alignitemsyoffset + baralignitemsyoffset
			,
			modsize.width + (mod->style.border_width * 2) +
			mod->style.padding.l + mod->style.padding.r
			,
			modsize.height + (mod->style.border_width * 2) +
			mod->style.padding.t + mod->style.padding.b
			,
			mod->style.border_width,
			(Color){0.1, 0.1, 0.1, 0.4},
			(Color){0.2, 0.2, 0.2, 1.0}
		);
		
		int modulewidth = modsize.width + (mod->style.border_width * 2)
			+ mod->style.padding.l + mod->style.padding.r;
	
		modulegroups[alignment].nextelementxpos += modulewidth + modulegroups[alignment].style.gap;
	}

	Dimensions _modsize = mod->drawfunc(mod, m, c, modpos, shoulddraw, updatecontent);
}

Dimensions
moduledraw_rect(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw, bool updatecontent)
{
	unsigned int width = 16, height = 16;
	if (shoulddraw) {
		drw_rect(drw, pos.x, pos.y, width, height, 1, 
			(Color){0.1, 0.1, 0.1, 0.4},
			(Color){0.2, 0.2, 0.2, 1.0});
	}

	return (Dimensions){
		.width = width,
		.height = height,
	};
}

Dimensions
moduledraw_tags(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw, bool updatecontent)
{
	unsigned int width = 0, height = 0;
	unsigned int tagsgap = mod->style.gap;

	unsigned int i, occ = 0, urg = 0;

	for (int i = 0; i < LENGTH(tags); i++) {
		Dimensions textdim = drw_get_textdim(drw, tags[i], font_desc_temp); 

		if (shoulddraw) {
		
			for (c = m->clients; c; c = c->next) {
				occ |= c->tags;
				if (c->isurgent)
					urg |= c->tags;
			}

			Color text_color;
			Color bg_color;
			Color bd_color;

			if (m->tagset[m->seltags] & 1 << i) {
				text_color = (Color){1.0, 1.0, 1.0, 1.0};
				bg_color   = (Color){0.2, 0.2, 0.2, 1.0};
				bd_color   = (Color){0.2, 0.2, 0.2, 1.0};
			} else if (occ & 1 << i) {
				text_color = (Color){1.0, 1.0, 1.0, 1.0};
				bg_color   = (Color){0.1, 0.1, 0.1, 0.0};
				bd_color   = (Color){0.2, 0.2, 0.2, 0.0};
			} else {
				text_color = (Color){0.3686, 0.3686, 0.3686, 1.0};
				bg_color   = (Color){0.1, 0.1, 0.1, 0.0};
				bd_color   = (Color){0.2, 0.2, 0.2, 0.0};
			}

			drw_rect(drw, pos.x + width, pos.y,
				textdim.width + (mod->children.style.border_width * 2) +
				mod->children.style.padding.l + mod->children.style.padding.r
				,
				textdim.height + (mod->children.style.border_width * 2) +
				mod->children.style.padding.b + mod->children.style.padding.t
				, mod->children.style.border_width, bg_color, bd_color);
			drw_text(drw, pos.x +
				width + 
				mod->children.style.padding.l +
				mod->children.style.border_width
				,
				pos.y + mod->children.style.padding.t + mod->children.style.border_width
				, tags[i], font_desc_temp, text_color);

		}

		if (i == LENGTH(tags) - 1) tagsgap = 0;

		width += textdim.width + mod->children.style.padding.r + mod->children.style.padding.l + (mod->children.style.border_width * 2) + tagsgap;
		height = textdim.height + mod->children.style.padding.t + mod->children.style.padding.b + (mod->children.style.border_width * 2);
	}

	return (Dimensions){
		.width = width,
		.height = height,
	};
}

Dimensions
moduledraw_time(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw, bool updatecontent)
{
	unsigned int width = 0, height = 0;

	if (updatecontent) {
		time_t raw_time;
		struct tm * time_info;
	
		time(&raw_time);
		time_info = localtime(&raw_time);
	
		char buffer[9];
		strftime(buffer, sizeof(buffer), "%H:%M:%S", time_info);
	
		snprintf(mod->children.content, 100, "%s", buffer);
	}

	Dimensions textdim = drw_get_textdim(drw, mod->children.content, font_desc_temp); 

	width = textdim.width + (mod->children.style.border_width * 2) 
	+ mod->children.style.padding.l + mod->children.style.padding.r;
	height = textdim.height + (mod->children.style.border_width * 2) 
	+ mod->children.style.padding.b + mod->children.style.padding.t;

	if (shoulddraw) {
		drw_rect(drw, pos.x, pos.y, width, height, mod->children.style.border_width,
			(Color){0.1, 0.1, 0.1, 0.4},
			(Color){0.2, 0.2, 0.2, 1.0});
		drw_text(drw, 
			pos.x + mod->children.style.padding.l + mod->children.style.border_width, 
			pos.y + mod->children.style.padding.t + mod->children.style.border_width, 
			mod->children.content, font_desc_temp,
			(Color){1.0, 1.0, 1.0, 1.0});
	}

	return (Dimensions){
		.width = width,
		.height = height,
	};
}

Dimensions
moduledraw_wintitle(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw, bool updatecontent)
{
	unsigned int width = 0, height = 0;

	char *text = m->sel->name;

	if (text == NULL || *text == '\0')
		return (Dimensions){0,0};

	Dimensions textdim = drw_get_textdim(drw, text, font_desc_temp); 

	width = textdim.width + (mod->children.style.border_width * 2) 
	+ mod->children.style.padding.l + mod->children.style.padding.r;
	height = textdim.height + (mod->children.style.border_width * 2) 
	+ mod->children.style.padding.b + mod->children.style.padding.t;

	if (shoulddraw) {
		drw_rect(drw, pos.x, pos.y, width, height, mod->children.style.border_width,
			(Color){0.1, 0.1, 0.1, 0.4},
			(Color){0.2, 0.2, 0.2, 1.0});
		drw_text(drw, 
			pos.x + mod->children.style.padding.l + mod->children.style.border_width, 
			pos.y + mod->children.style.padding.t + mod->children.style.border_width, 
			text, font_desc_temp,
			(Color){1.0, 1.0, 1.0, 1.0});
	}

	return (Dimensions){
		.width = width,
		.height = height,
	};
}

Dimensions
moduledraw_systray(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw, bool updatecontent)
{
	unsigned int width = getsystraywidth(), height = 16;

	systraypos = (Vec2){pos.x, pos.y};
	if (shoulddraw && getsystraywidth() > 1) {
		drw_rect(drw, pos.x, pos.y, width, height, 1,
			(Color){0.1, 0.1, 0.1, 0.4},
			(Color){0.2, 0.2, 0.2, 1.0});
	}

	if (getsystraywidth() == 1)
		width = 0;
	
	return (Dimensions){
		.width = width,
		.height = height,
	};
}
