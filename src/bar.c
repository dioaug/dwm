#include "bar.h"
#include "config.h"
#include "util.h"
#include "draw.h"
#include "dynarray.h"

const char *font_desc_temp = "sans-serif bold 8";

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

	Module module_textrect = (Module){
		.drawfunc = moduledraw_textrect,
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

	Module module_systray = (Module){
		.drawfunc = moduledraw_systray,
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
	dynarray_push(modulegroups[ModuleGroupCenter].modules, module_textrect);
	dynarray_push(modulegroups[ModuleGroupRight].modules, module_systray);
}

void
moduledraw(struct Module *mod, enum alignment alignment, Monitor *m, Client *c, int shoulddraw)
{
	Dimensions modsize = mod->drawfunc(mod, m, c, (Vec2){0,0}, 0);
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

	Dimensions _modsize = mod->drawfunc(mod, m, c, modpos, shoulddraw);
}

Dimensions
moduledraw_rect(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw)
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
moduledraw_tags(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw)
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
moduledraw_textrect(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw)
{
	unsigned int width = 0, height = 0;

	const char *text = m->sel->name;

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
moduledraw_systray(struct Module *mod, Monitor *m, Client *c, Vec2 pos, int shoulddraw)
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
