#pragma once

#include <X11/X.h>

#include "util.h"
#include "draw.h"
#include "core.h"

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
							* MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLE(C)            ((C->tags & C->mon->tagset[C->mon->seltags]))
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define TAGMASK                 ((1 << LENGTH(tags)) - 1)
#define TAGSLENGTH              (LENGTH(tags))
#define OPAQUE                  0xffU
#define _NET_SYSTEM_TRAY_ORIENTATION_HORZ 0

#define SYSTEM_TRAY_REQUEST_DOCK    0
/* XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY      0
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_FOCUS_IN             4
#define XEMBED_MODALITY_ON         10
#define XEMBED_MAPPED              (1 << 0)
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_WINDOW_DEACTIVATE    2
#define VERSION_MAJOR               0
#define VERSION_MINOR               0
#define XEMBED_EMBEDDED_VERSION (VERSION_MAJOR << 16) | VERSION_MINOR

/* enums */
enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel, SchemeSec, SchemeSecInv }; /* color schemes */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
	NetSystemTray, NetSystemTrayOP, NetSystemTrayOrientation, NetSystemTrayOrientationHorz, NetSystemTrayVisual,
	NetWMFullscreen, NetActiveWindow, NetWMWindowType,
	NetWMWindowTypeDialog, NetWMWindowTypeDock, NetClientList, NetDesktopNames, NetDesktopViewport, NetNumberOfDesktops, NetCurrentDesktop, NetLast }; /* EWMH atoms */
enum { Manager, Xembed, XembedInfo, XLast }; /* Xembed atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
enum { ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
	ClkClientWin, ClkRootWin, ClkLast }; /* clicks */

typedef union {
	int i;
	unsigned int ui;
	float f;
	const void *v;
} Arg;

typedef struct {
	unsigned int click;
	unsigned int mask;
	unsigned int button;
	void (*func)(const Arg *arg);
	const Arg arg;
} Button;

typedef struct Monitor Monitor;
typedef struct Client Client;
struct Client {
	char name[256];
	float mina, maxa;
	int x, y, w, h;
	int oldx, oldy, oldw, oldh;
	int basew, baseh, incw, inch, maxw, maxh, minw, minh, hintsvalid;
	int bw, oldbw;
	unsigned int tags;
	int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen;
	Client *next;
	Client *snext;
	Monitor *mon;
	Window win;
};

typedef struct {
	unsigned int mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Key;

typedef struct {
	const char *symbol;
	void (*arrange)(Monitor *);
} Layout;

typedef struct Pertag Pertag;
struct Monitor {
	char ltsymbol[16];
	float mfact;
	int nmaster;
	int num;
	int by;               /* bar geometry */
	int mx, my, mw, mh;   /* screen size */
	int wx, wy, ww, wh;   /* window area  */
	unsigned int seltags;
	unsigned int sellt;
	unsigned int tagset[2];
	int showbar;
	int createfconly;
	int topbar;
	Client *clients;
	Client *sel;
	Client *stack;
	Monitor *next;
	Window barwin;
	const Layout *lt[2];
	Pertag *pertag;
};

typedef struct {
	const char *rclass;
	const char *instance;
	const char *title;
	unsigned int tags;
	int isfloating;
	int monitor;
} Rule;

typedef struct Systray   Systray;
struct Systray {
	Window win;
	Client *icons;
};

/* function declarations */
void applyrules(Client *c);
int applysizehints(Client *c, int *x, int *y, int *w, int *h, int *bw, int interact);
void arrange(Monitor *m);
void arrangemon(Monitor *m);
void attach(Client *c);
void attachbottom(Client *c);
void attachstack(Client *c);
void buttonpress(XEvent *e);
void cleanupmon(Monitor *mon);
void clientmessage(XEvent *e);
void configure(Client *c);
void configurenotify(XEvent *e);
void configurerequest(XEvent *e);
Monitor *createmon(void);
void destroynotify(XEvent *e);
void detach(Client *c);
void detachstack(Client *c);
Monitor *dirtomon(int dir);
void expose(XEvent *e);
void focus(Client *c);
void focusin(XEvent *e);
Atom getatomprop(Client *c, Atom prop);
int getrootptr(int *x, int *y);
long getstate(Window w);
pid_t getstatusbarpid();
unsigned int getsystraywidth();
int gettextprop(Window w, Atom atom, char *text, unsigned int size);
void grabbuttons(Client *c, int focused);
void grabkeys(void);
void keypress(XEvent *e);
void manage(Window w, XWindowAttributes *wa);
void mappingnotify(XEvent *e);
void maprequest(XEvent *e);
Client *nexttiled(Client *c);
void pop(Client *c);
void propertynotify(XEvent *e);
Monitor *recttomon(int x, int y, int w, int h);
void removesystrayicon(Client *i);
void resize(Client *c, int x, int y, int w, int h, int bw, int interact);
void resizeclient(Client *c, int x, int y, int w, int h, int bw);
void resizebarwin(Monitor *m);
void resizerequest(XEvent *e);
void restack(Monitor *m);
int sendevent(Window w, Atom proto, int m, long d0, long d1, long d2, long d3, long d4);
void sendmon(Client *c, Monitor *m);
void setclientstate(Client *c, long state);
void setcurrentdesktop(void);
void setdesktopnames(void);
void setfocus(Client *c);
void setfullscreen(Client *c, int fullscreen);
void setnumdesktops(void);
void setviewport(void);
void seturgent(Client *c, int urg);
void showhide(Client *c);
Monitor *systraytomon(Monitor *m);

void unfocus(Client *c, int setfocus);
void unmanage(Client *c, int destroyed);
void unmapnotify(XEvent *e);
void updatecurrentdesktop(void);
void updatebarpos(Monitor *m);
void updatebars(void);
void updateclientlist(void);
int updategeom(void);
void updatenumlockmask(void);
void updatesizehints(Client *c);
void updatestatus(void);
void updatesystray(int updatebar);
void updatesystrayicongeom(Client *i, int w, int h);
void updatesystrayiconstate(Client *i, XPropertyEvent *ev);
void updatetitle(Client *c);
void updatewindowtype(Client *c);
void updatewmhints(Client *c);
Client *wintoclient(Window w);
Monitor *wintomon(Window w);
Client *wintosystrayicon(Window w);
int xerror(Display *dpy, XErrorEvent *ee);
int xerrordummy(Display *dpy, XErrorEvent *ee);
int xerrorstart(Display *dpy, XErrorEvent *ee);
void xinitvisual();

// USED IN CONFIG
void spawn(const Arg *arg);
void togglebar(const Arg *arg);
void togglefloating(const Arg *arg);
void togglefullscr(const Arg *arg);
void focusdir(const Arg *arg);
void placedir(const Arg *arg);
void setmfact(const Arg *arg);
void incnmaster(const Arg *arg);
void zoom(const Arg *arg);
void view(const Arg *arg);
void killclient(const Arg *arg);
void setlayout(const Arg *arg);
void togglecreatefconly(const Arg *arg);
void tag(const Arg *arg);
void focusmon(const Arg *arg);
void tagmon(const Arg *arg);
void quit(const Arg *arg);
void toggletag(const Arg *arg);
void toggleview(const Arg *arg);
void sigstatusbar(const Arg *arg);
void movemouse(const Arg *arg);
void resizemouse(const Arg *arg);

// LAYOUTS
void spiral(Monitor *mon);
void monocle(Monitor *m);

// MAIN FUNCTIONS
void checkotherwm(void);
void setup(void);
void scan(void);
void run(void);
void cleanup(void);

extern Display *dpy;
extern Visual *visual;
extern int depth;
extern Colormap cmap;

// Bar drw
extern Drw *drw;

extern Monitor *mons, *selmon;

// SYSTRAY
extern Systray *systray;
extern Vec2 systraypos;
extern unsigned long systrayorientation;



