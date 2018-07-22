/*
 *  Copyright (C) 2018 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#if defined _menus_included
    #endinput
#endif
#define _menus_included

enum Menu
{
    INVALID_MENU = -1
};

enum MenuItem
{
    MenuExit = -3,
    MenuNext = -2,
    MenuBack = -1
};

enum ItemStatus
{
	ItemEnabled,
	ItemDisabled
};

typedef MenuHandler = function int (Menu menu, MenuItem item, int player);
typedef CallbackHandler = function ItemStatus (Menu menu, MenuItem item, int player);

methodmap Menu
{
    public native Menu(MenuHandler handler);
    public native void Display(int player, int time = -1, int page = 0);
    public native void SetTitle(const char[] title);
    public native void AddItem(const char[] name);
}
