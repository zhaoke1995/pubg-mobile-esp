#include "D3DMenu.h"

void D3DMenu::AddItem(char *txt, int *var, char **opt, int maxval, int typ)
{
	if (noitems >= (maxitems - 3)) return;
	MENU[noitems]->typ = typ;
	MENU[noitems]->txt = txt;
	MENU[noitems]->opt = opt;
	MENU[noitems]->var = var;
	MENU[noitems]->maxval = maxval;
	noitems++;
	totheight = (noitems*height) + titleheight;
}

void D3DMenu::AddGroup(char *txt, int *var, char **opt, int maxval)
{
	AddItem(txt, var, opt, maxval, MENUGROUP);
}

void D3DMenu::AddGroup1(char *txt, int *var, char **opt, int maxval)
{
	AddItem(txt, var, opt, maxval, MENUGROUP1);
}


void D3DMenu::AddText(char *txt, char *opt)
{
	AddItem(txt, 0, (char **)opt, 0, MENUTEXT);
}

void D3DMenu::Show()
{
	int i, val, cy;
	DWORD color;

	if (!visible)
		return;

	cy = y;
	if (title) {
		dxm->DrawString((float)(x + totwidth / 4), (float)cy - 3, col_title, dxm->pFontMenu,title);
		cy += titleheight;
	}
	for (i = 0; i < noitems; i++) {
		if (MENU[i]->typ == MENUTEXT)
		{
			dxm->DrawString((float)x, (float)cy, col_on, dxm->pFontMenu,MENU[i]->txt);
			if (MENU[i]->opt)
			{
				dxm->DrawString((float)(x + ofs), (float)cy, col_on, dxm->pFontMenu,(char *)MENU[i]->opt);
			}
		}
		else
		{
			val = (MENU[i]->var) ? (*MENU[i]->var) : 0;
			if (i == cur)
				color = col_current;
			else if (MENU[i]->typ == MENUGROUP)
				color = col_group;
			else if (MENU[i]->typ == MENUGROUP1)
				color = (val) ? col_on : col_off;
			else
				color = (val) ? col_on : col_off;
			dxm->DrawString((float)x, (float)cy, color, dxm->pFontMenu,MENU[i]->txt);
			if (MENU[i]->opt)
			{
				dxm->DrawString((float)(x + ofs), (float)cy, color, dxm->pFontMenu, (char *)MENU[i]->opt[val]);
			}
		}
		cy += height;
	}
}

void D3DMenu::Nav(void)
{
	if (GetAsyncKeyState(VK_HOME) & 1) visible = (!visible);

	if (!visible) return;

	if (visible)
	{
		if (GetAsyncKeyState(VK_CONTROL))
		{
			if (GetAsyncKeyState(VK_UP) & 1) y -= 10;
			if (GetAsyncKeyState(VK_DOWN) & 1) y += 10;
			if (GetAsyncKeyState(VK_LEFT) & 1) x -= 10;
			if (GetAsyncKeyState(VK_RIGHT) & 1) x += 10;
		}
		else
		{
			if (GetAsyncKeyState(VK_UP) & 1) {
				do {
					cur--;
					if (cur < 0)  cur = noitems - 1;
				} while (MENU[cur]->typ == MENUTEXT);
			}
			else if (GetAsyncKeyState(VK_DOWN) & 1) {
				do {
					cur++;
					if (cur == noitems) cur = 0;
				} while (MENU[cur]->typ == MENUTEXT);
			}
			else if (MENU[cur]->var) {
				int dir = 0;
				if (GetAsyncKeyState(VK_LEFT) & 1 && *MENU[cur]->var > 0)
					dir = -1;
				if (GetAsyncKeyState(VK_RIGHT) & 1 && *MENU[cur]->var < (MENU[cur]->maxval - 1))
					dir = 1;
				if (dir) {
					*MENU[cur]->var += dir;
					if (MENU[cur]->typ == MENUGROUP) noitems = 0;
					if (MENU[cur]->typ == MENUGROUP1) noitems = 0;
				}
			}
		}
	}
}
