/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 10;       /* gap pixel between windows */
static const unsigned int snap      = 4;        /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft  = 0;   /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 4;   /* systray spacing */
static const unsigned int systrayiconsize = 15; /* systray icon size in px */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int barheight          = 22;       /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const int titlemaxlength     = 60;
static const int focusonwheel       = 0;
static const char *fonts[]          = { "CaskaydiaCove Nerd Font:style:bold:size=12", "monospace:size=12" };
static const char dmenufont[]       = "CaskaydiaCove Nerd Font:style:bold:size=12";
static const char col_gray1[]       = "#121212";
static const char col_gray2[]       = "#1c1c1c";
static const char col_gray3[]       = "#7a7a7a";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#90ff40";
static const unsigned int baralpha = 0x45;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm]   = { col_gray4, col_gray1, col_gray2 },
	[SchemeSel]    = { col_gray1, col_cyan,  col_cyan  },
	[SchemeSec]    = { col_gray3, col_gray1, col_gray1  },
	[SchemeSecInv] = { col_gray1, col_gray3, col_gray3  },
};
static const unsigned int alphas[][3]      = {
    /*               fg      bg        border*/
    [SchemeNorm]   = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]    = { OPAQUE, baralpha, borderalpha },
	[SchemeSec]    = { OPAQUE, baralpha, borderalpha },
	[SchemeSecInv] = { OPAQUE, baralpha, borderalpha },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"nemo", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spfilemanager",   spcmd1},
};

/* tagging */
static char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	//{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	//{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ NULL,		        "nemo",		    NULL,		SPTAG(0),		1,			 -1 },
};

/* layout(s) */
static const float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "",       spiral },    /* first entry is default */
	{ "",      NULL },    /* no layout function means floating behavior */
	{ "",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define PrtScn 0x0000ff61
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *roficmd[]  = { "rofi", "-show", "run", "-show-icons", NULL };
static const char *rofiwndcmd[]  = { "rofi", "-show", "window", NULL };
static const char *printscreencmd[]  = { "flameshot", "gui", NULL };
static const char *volumeraisecmd[]  = { "diowm-sb-volume", "raise", NULL };
static const char *volumelowercmd[]  = { "diowm-sb-volume", "lower", NULL };
static const char *volumemutecmd[]  = { "diowm-sb-volume", "togglemute", NULL };
static const char *termcmd[]  = { "alacritty", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = roficmd } },
	{ MODKEY,                       XK_Tab,    spawn,          {.v = rofiwndcmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ 0,							PrtScn,    spawn,          {.v = printscreencmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },

	{ MODKEY,                       XK_h,      focusdir,       {.i = 0 } }, // left
	{ MODKEY,                       XK_l,      focusdir,       {.i = 1 } }, // right
	{ MODKEY,                       XK_k,      focusdir,       {.i = 2 } }, // up
	{ MODKEY,                       XK_j,      focusdir,       {.i = 3 } }, // down
	{ MODKEY,                       XK_Left,   focusdir,       {.i = 0 } }, // left
	{ MODKEY,                       XK_Right,  focusdir,       {.i = 1 } }, // right
	{ MODKEY,                       XK_Up,     focusdir,       {.i = 2 } }, // up
	{ MODKEY,                       XK_Down,   focusdir,       {.i = 3 } }, // down

	{ MODKEY|ShiftMask,             XK_h,      placedir,       {.i = 0 } }, // left
	{ MODKEY|ShiftMask,             XK_l,      placedir,       {.i = 1 } }, // right
	{ MODKEY|ShiftMask,             XK_k,      placedir,       {.i = 2 } }, // up
	{ MODKEY|ShiftMask,             XK_j,      placedir,       {.i = 3 } }, // down
	{ MODKEY|ShiftMask,             XK_Left,   placedir,       {.i = 0 } }, // left
	{ MODKEY|ShiftMask,             XK_Right,  placedir,       {.i = 1 } }, // right
	{ MODKEY|ShiftMask,             XK_Up,     placedir,       {.i = 2 } }, // up
	{ MODKEY|ShiftMask,             XK_Down,   placedir,       {.i = 3 } }, // down
	
	{ MODKEY|ControlMask,           XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY|ControlMask,           XK_l,      setmfact,       {.f = +0.05} },

	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_p,      incnmaster,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_v,      view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	//{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_f,      togglecreatefconly,  {0} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,            			XK_c,  	   togglescratch,  {.ui = 0 } },
	{ 0,            			    XF86XK_AudioRaiseVolume,   spawn,  {.v = volumeraisecmd } },
	{ 0,            			    XF86XK_AudioLowerVolume,   spawn,  {.v = volumelowercmd } },
	{ 0,            			    XF86XK_AudioMute,          spawn,  {.v = volumemutecmd } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigstatusbar,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigstatusbar,   {.i = 5} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
