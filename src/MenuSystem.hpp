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

#pragma once

#include "spmod.hpp"

// using MenuItemCallback_t = void (*)(Menu *menu, size_t item, int player);
// using MenuHandler_t = void (*)(Menu *menu, size_t item, int player);

void UTIL_ShowMenu(edict_t* pEdict, int slots, int time, char *menu, int mlen);

struct MenuItem
{
    MenuItem(std::string n,
             MenuItemCallback_t c,
             SourcePawn::IPluginFunction *f) : name(n),
                                               callback(c),
                                               pluginCallback(f)
    {}
    std::string name;
    MenuItemCallback_t callback;
    SourcePawn::IPluginFunction *pluginCallback;
};

class Menu: public IMenu
{
public:
    Menu(size_t id,
         MenuHandler_t handler);
    Menu(size_t id,
         SourcePawn::IPluginFunction *handler);

    ~Menu() {}

    void Display(int player, int page, int time) const;
    void Close(...) const;

    void SetTitle(std::string_view text);
    void SetItemsPerPage(int value);
    int GetItemsPerPage() const;

    void AppendItem(std::string_view name,
                    MenuItemCallback_t callback);
    void AppendItem(std::string_view name,
                    SourcePawn::IPluginFunction *pluginCallback);

    bool InsertItem(size_t position,
                    std::string_view name,
                    MenuItemCallback_t callback);
    bool InsertItem(size_t position,
                    std::string_view name,
                    SourcePawn::IPluginFunction *pluginCallback);

    bool RemoveItem(size_t position);
    void RemoveAllItems();

    void AddHandler(MenuHandler_t func);
    void AddPluginHandler(SourcePawn::IPluginFunction *func);

    size_t getId() const;
private:
    void addItem(int pos,
                 std::string_view name,
                 MenuItemCallback_t callback,
                 SourcePawn::IPluginFunction *pluginCallback);
private:
    size_t m_id;
    std::string m_title;
    int m_itemsPerPage;
    
    MenuHandler_t m_handler;
    SourcePawn::IPluginFunction *m_pluginHandler;

    std::vector<MenuItem> m_items;
};

class MenuManager
{
public:
    MenuManager() {}
    ~MenuManager() {}

    std::shared_ptr<Menu> registerMenu(MenuHandler_t handler);
    std::shared_ptr<Menu> registerMenu(SourcePawn::IPluginFunction *func);
    
    void destroyMenu(size_t index);
    std::shared_ptr<Menu> findMenu(size_t index) const;
    void clearMenus();
private:
    size_t m_mid;
    std::vector<std::shared_ptr<Menu>> m_menus;
};
