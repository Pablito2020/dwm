/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h> // Necessary for hardware keys (volume keys)

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 0;   /* systray spacing */ 
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrains Mono:size=12", "JoyPixels:pixelsize=12:antialias=true:autohint=true" };
static const char dmenufont[]       = "monospace:size=10";
static const char col_gray1[]       = "#000000";
static const char col_gray2[]       = "#363636";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#ffffff";
static const char col_cyan[]        = "#005577";
static const char gruvbox_norm_foreground[]       = "#ebdbb2";
static const char gruvbox_norm_background[]       = "#1d2021";
static const char gruvbox_selection_foreground[]  = "#eeeeee";
static const char gruvbox_selection_background[]  = "#fb4934";

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { gruvbox_norm_foreground, gruvbox_norm_background, col_gray2 },
	[SchemeSel]  = { gruvbox_selection_foreground, gruvbox_selection_background, gruvbox_selection_background  },
    [SchemeTitle]  = { gruvbox_selection_background, gruvbox_norm_background,  gruvbox_selection_background  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    	NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Firefox", 	NULL,     NULL,           1 << 8,    0,          0,          -1,        -1 },
	{ "Alacritty",  NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      	NULL,     "Event Tester", 0,         1,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-c", "-l", "20", NULL};
static const char *termcmd[]  = { "alacritty", NULL };

/* Hardware Configuration */
static const char *mutecmd[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };
static const char *volupcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+5%", NULL };
static const char *voldowncmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-5%", NULL };
static const char *killdwmblocks[] = {"pkill","-RTMIN+10", "dwmblocks", NULL};
static const char *killdwmblockskeyboard[] = {"pkill","-RTMIN+12", "dwmblocks", NULL};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_m,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
    { MODKEY,                       XK_b,      spawn,          SHCMD("brave")},
    { MODKEY,                       XK_s,      spawn,          SHCMD("spotify") },
	{ MODKEY,                       XK_i,      spawn,          SHCMD("remap_keyboard") },
	{ MODKEY,                       XK_i,      spawn,          {.v = killdwmblockskeyboard } },
	{ MODKEY,                       XK_f,      spawn,          SHCMD("pcmanfm") },
	{ MODKEY,                       XK_p,      spawn,          SHCMD("pavucontrol") },
	{ MODKEY,                       XK_g,      spawn,          SHCMD("brave https://mail.google.com/mail/u/0/#inbox") },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          SHCMD("brave https://www.github.com/pablito2020") },
/*  { MODKEY|ShiftMask,             XK_Return, togglescratch,  {.ui = 0} },     */

    // Windows movements
    { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_w,      killclient,     {0} },

    // Layouts 
	{ MODKEY,                       XK_Tab,    cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  zoom,           {0} },               // Make current window the master
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },               // Put in normal layout
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },               // Hide status bar
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },        // Change different layouts inside the layout (lol)
	{ MODKEY|ShiftMask,             XK_d,      incnmaster,     {.i = +1 } },        // Same but different direction
/*  { MODKEY,                       XK_0,      view,           {.ui = ~0 } },    */
/* 	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },    */

	// Hardware Keys
    { 0,                            XF86XK_AudioMute,spawn,    {.v = mutecmd } },
    { 0,                            XF86XK_AudioMute,spawn,    {.v = killdwmblocks } },
    { 0,                            XF86XK_AudioLowerVolume,spawn, {.v = voldowncmd } },
    { 0,                            XF86XK_AudioLowerVolume,spawn, {.v = killdwmblocks } },
    { 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = volupcmd } },
    { 0,                            XF86XK_AudioRaiseVolume, spawn, {.v = killdwmblocks } },
    { 0,                            XF86XK_AudioPrev,		spawn,		SHCMD("playerctl previous --player=spotify") },
	{ 0,                            XF86XK_AudioNext,		spawn,		SHCMD("playerctl next --player=spotify") },
	{ 0,                            XF86XK_AudioPause,		spawn,		SHCMD("playerctl play-pause --player=spotify") },
	{ 0,                            XF86XK_AudioPlay,		spawn,		SHCMD("playerctl play-pause --player=spotify") },
	{ 0,                            XF86XK_AudioStop,		spawn,		SHCMD("playerctl play-pause --player=spotify") },
	{ 0,                            XK_Print,		        spawn,		SHCMD("screenshot") },

    // Tags
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

    // Exit dwm
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

