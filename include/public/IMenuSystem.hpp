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



namespace SPMod
{
    enum
    {
        MENU_EXIT = -3,
        MENU_NEXT = -2,
        MENU_BACK = -1
    };

    enum ItemStatus
    {
        ItemEnabled,
        ItemDisabled,
        ItemHide
    };
    
    class IMenu SPMOD_FINAL
    {
    public:
        using MenuItemCallback_t = ItemStatus (*)(IMenu *const menu, size_t item, int player);
        using MenuHandler_t = void (*)(IMenu *const menu, size_t item, int player);

        virtual void display(int player,
                            int page,
                            int time) = 0;
        virtual void close(...) const = 0;
        virtual void setTitle(const char *text) = 0;
        virtual void appendItem(const char *name,
                                MenuItemCallback_t callback,
                                void *data) = 0;
    protected:
        virtual ~IMenu() {}
    };

    class IMenuManager SPMOD_FINAL
    {
    public:
        using MenuHandler_t = void (*)(IMenu *const menu, size_t item, int player);
        virtual IMenu *registerMenu(MenuHandler_t handler, bool global) = 0;
        virtual IMenu *registerMenu(SourcePawn::IPluginFunction *func, bool global) = 0;
        virtual IMenu *findMenu(size_t mid) const = 0;
    protected:
        virtual ~IMenuManager() {}
    };
}

