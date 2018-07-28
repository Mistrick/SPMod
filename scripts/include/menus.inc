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

enum MenuStyle
{
    MenuItemStyle,
    MenuTextStyle
}

enum MenuProp
{
    /**
     * @param format            Should contain "#num", example "\\r[#num]"
     */
    MProp_NumberFormat
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
	ItemDisabled,
    ItemHide
};

typeset MenuHandler
{
	// text style
	function void(
		Menu menu,
		int key,
		int player
	);

	// item style
	function void(
		Menu menu,
		MenuItem item,
		int player
	);
};

typedef CallbackHandler = function ItemStatus (Menu menu, MenuItem item, int player);

methodmap Menu
{
    // text style always global
    public native Menu(MenuHandler handler, MenuStyle style = MenuItemStyle, bool global = false);
    public native void Display(int player, int page = 0, int time = -1);
    // use only for global menu
    public native void Destroy();

    // only for item style
    public native void SetTitle(const char[] title);
    public native void AddItem(const char[] name, any data = 0, CallbackHandler callback = INVALID_FUNCTION);
    public native void AddStaticItem(int position, const char[] name, any data = 0, CallbackHandler callback = INVALID_FUNCTION);
    public native void InsertItem(int position, const char[] name, any data = 0, CallbackHandler callback = INVALID_FUNCTION);
    public native void RemoveItem(int position);
    public native void RemoveAllItems();
    public native void SetProp(MenuProp prop, any ...);
    property int Items {
        public native get();
    }
    property int ItemsPerPage {
        public native get();
        public native set(int value);
    }
    
    // only for text tyle
    public native void SetText(const char[] text);
    public native void SetKeys(int keys);
}

methodmap MenuItem
{
    public native void GetName(char[] name, int size);
    public native void SetName(const char[] name);
    public native any GetData();
    public native void SetData(any data);
}

native void CloseMenu(int player);
