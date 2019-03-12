#pragma once
#include <Windows.h>
#include "DxManager.h"
// Setting Color
#define DARKORANGE  D3DCOLOR_ARGB(255,255,127,000)
#define TextWhite   D3DCOLOR_ARGB(255,255,255,255) 
#define TextRed     D3DCOLOR_ARGB(255,255,0,0) 
#define TextGreen   D3DCOLOR_ARGB(255,0,255,0)
#define TextBlue    D3DCOLOR_ARGB(255,0,0,255)
#define TextBlack   D3DCOLOR_ARGB(255,0,0,0)
#define TextPurple  D3DCOLOR_ARGB(255,125,0,255)
#define TextGrey    D3DCOLOR_ARGB(255,128,128,128)
#define TextYellow  D3DCOLOR_ARGB(255,255,255,0)
#define TextOrange  D3DCOLOR_ARGB(255,255,140,0)
#define TextCyan    D3DCOLOR_ARGB(255,0,139,200)
#define TextPink    D3DCOLOR_ARGB(255,255,192,203)

// Menu Color
#define TextBorder  D3DCOLOR_ARGB(255,128,128,128)
#define TextFound   D3DCOLOR_ARGB(255,0,139,200)

#ifndef D3DFONT_RIGHT
#define D3DFONT_RIGHT   0x0008
#endif
#ifndef D3DFONT_SHADOW
#define D3DFONT_SHADOW  0x0010
#endif
#define MENUGROUP       1
#define MENUTEXT        2
#define MENUITEM        3
#define MENUGROUP1      4

#define MCOLOR_TITLE    TextGreen
#define MCOLOR_CURRENT  TextRed  //当前选中的行的颜色 
#define MCOLOR_GROUP    TextYellow //组的颜色
#define MCOLOR_TEXT     TextWhite//行的文字颜色
#define MCOLOR_OFF      TextWhite
#define MCOLOR_ON       TextGreen  


typedef struct {
	int  typ;         // type of menu line, folder, item
	char *txt;    // text to show
	char **opt;   // array of options
	int  *var;    // variable containing current status
	int  maxval;    // maximum value,  normally 1  gives  0=off  1=on
} tMENU;

class D3DMenu
{
public:
	D3DMenu(char *Name = 0, int maxentries = 130, int maxwidth = 120)
	{
		title = Name;
		maxitems = maxentries;
		cur = noitems = visible = 0;
		x = 5;
		y = 20;
		totwidth = ofs = maxwidth;
		height = 15;
		titleheight = totheight = height + 4;
		col_title = MCOLOR_TITLE;
		col_group = MCOLOR_GROUP;
		col_text = MCOLOR_TEXT;
		col_off = MCOLOR_OFF;
		col_on = MCOLOR_ON;
		col_current = MCOLOR_CURRENT;
		MENU = (tMENU **)malloc(sizeof(LPVOID) * maxitems);
		for (int i = 0; i < maxitems; i++)
			MENU[i] = (tMENU *)malloc(sizeof(tMENU));
	}
	~D3DMenu() {
		for (int i = 0; i < maxitems; i++) free(MENU[i]);
		free(MENU);
	}

	DWORD       col_title;
	DWORD       col_group;
	DWORD       col_group1;
	DWORD       col_text;
	DWORD       col_off;
	DWORD       col_on;
	DWORD       col_current;

	int         x, y;               // current position of the menu
	int         totwidth, totheight;    // total width and height of menu
	int         height;             // height of 1 menu line
	int         titleheight;        // some extra height for a title
	int         ofs;                // offset for option text
	// menu vars
	char        *title;             // some menu title
	int         cur;                // current highlighted menu item    
	int         noitems;            // number of menu items
	int         visible;            // 1 = menu is visible

	tMENU       **MENU;

	void AddItem(char *txt, int *var, char **opt, int maxvalue = 2, int typ = MENUITEM);
	void AddGroup(char *txt, int *var, char **opt, int maxvalue = 2);
	void AddGroup1(char *txt, int *var, char **opt, int maxvalue = 2);
	void AddText(char *txt, char *opt = "");
	void Show(DxManager *pDxm);
	void Nav(void);

private:
	int         maxitems;
};

