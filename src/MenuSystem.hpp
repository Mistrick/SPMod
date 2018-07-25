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
    /* MenuItem(std::string n,
             MenuItemCallback_t c,
             SourcePawn::IPluginFunction *f) : name(n),
                                               callback(c),
                                               pluginCallback(f)
    {} */
    MenuItem(std::string s) : name(s) {}
    std::string name;
    // MenuItemCallback_t callback;
    // SourcePawn::IPluginFunction *pluginCallback;
};

class Menu: public IMenu
{
public:
    Menu(size_t id,
         MenuHandler_t handler);
    Menu(size_t id,
         SourcePawn::IPluginFunction *handler);

    ~Menu() {}

    void Display(int player,
                 int page,
                 int time);
    
    void Close(...) const;

    void SetTitle(const char *text) override
    {
        SetTitleCore(text);
    }
    void SetTitleCore(std::string_view text);

    void SetItemsPerPage(int value);
    int GetItemsPerPage() const;

    int GetTime() const
    {
        return m_time;
    }

    int GetKeys() const
    {
        return m_keys;
    }

    int KeyToSlot(int key) const
    {
        return m_slots[key];
    }

    virtual void AppendItem(const char *name,
                            MenuItemCallback_t callback) override
    {
        AppendItemCore(name, callback);
    }

    void AppendItemCore(std::string_view name,
                    MenuItemCallback_t callback);
    void AppendItemCore(std::string_view name,
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


    void ExecHandler(int player, int item);

    size_t getId() const;
private:
    void addItem(int pos,
                 std::string_view name,
                 MenuItemCallback_t callback,
                 SourcePawn::IPluginFunction *pluginCallback);
private:
    size_t m_id;
    std::string m_title;
    int m_time;
    int m_itemsPerPage;
    int m_keys;
    int m_slots[10];

    MenuHandler_t m_handler;
    SourcePawn::IPluginFunction *m_pluginHandler;

    std::vector<MenuItem> m_items;
};

class MenuManager : public IMenuManager
{
public:
    MenuManager()
    {
        for(int i = 1; i < 33; i++)
        {
            m_playerMenu[i] = -1;
            m_playerPage[i] = 0;
        }
    }
    ~MenuManager() = default;

    IMenu *registerMenu(MenuHandler_t handler) override
    {
        return registerMenuCore(handler).get();
    }
    IMenu *registerMenu(SourcePawn::IPluginFunction *func) override
    {
        return registerMenuCore(func).get();
    }
    
    std::shared_ptr<Menu> registerMenuCore(MenuHandler_t handler);
    std::shared_ptr<Menu> registerMenuCore(SourcePawn::IPluginFunction *func);
    
    void destroyMenu(size_t index);
    std::shared_ptr<Menu> findMenu(size_t index) const;
    void clearMenus();

    void AttachMenu(int player, size_t menuId, int page);
    META_RES ClientCommand(edict_t *pEntity);
    void ClientDisconnected(edict_t *pEntity);
    void CloseMenu(edict_t *pEntity);
private:
    size_t m_mid = 0;
    std::vector<std::shared_ptr<Menu>> m_menus;

    int m_playerMenu[33];
    int m_playerPage[33];
};
