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
std::string replace(const std::string& str, const std::string& from, const std::string& to);

#define PACK_ITEM(menuid, itemid) (menuid << 16 | itemid)
#define UNPACK_ITEM(index, menuid, itemid) menuid = index >> 16; itemid = index & 0xFFFF

class Menu: public IMenu
{
public:
    struct MenuItem
    {
        MenuItem(std::string n,
                 std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&c,
                 std::variant<cell_t, void *> d) : name(n),
                                                   callback(c),
                                                   data(d)
        {}
        ItemStatus execCallback(Menu *menu, size_t i, int player)
        {
            ItemStatus result = ItemEnabled;
            
            try
            {
                auto *func = std::get<SourcePawn::IPluginFunction *>(callback);
                if(func && func->IsRunnable())
                {
                    func->PushCell(static_cast<cell_t>(menu->getId()));
                    func->PushCell(static_cast<cell_t>(PACK_ITEM(menu->getId(), i)));
                    func->PushCell(static_cast<cell_t>(player));
                    func->Execute(reinterpret_cast<cell_t*>(&result));
                }
            }
            catch (const std::bad_variant_access &e [[maybe_unused]])
            {
                auto func = std::get<MenuItemCallback_t>(callback);
                if(func)
                {
                    result = func(menu, i, player);
                }
            }

            return result;
        }
        std::string name;
        std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> callback;
        std::variant<cell_t, void *> data;
    };

public:
    Menu(size_t id,
         std::variant<SourcePawn::IPluginFunction *, MenuHandler_t> &&handler,
         bool global);

    ~Menu() {}

    void display(int player,
                 int page,
                 int time) override;
    
    void close(...) const override;

    bool getGlobal() const;

    void setTitle(const char *text) override;
    void setTitleCore(std::string_view text);

    void setItemsPerPage(int value);
    int getItemsPerPage() const;

    void setNumberFormat(std::string_view format);

    int getTime() const;
    int getKeys() const;
    int keyToSlot(int key) const;

    void appendItem(const char *name,
                    MenuItemCallback_t callback,
                    void *data) override;

    void appendItemCore(std::string_view name,
                        std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                        std::variant<cell_t, void *> &&data);

    bool insertItemCore(size_t position,
                        std::string_view name,
                        std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                        std::variant<cell_t, void *> &&data);

    bool setStaticItem(size_t positon,
                       std::string_view name,
                       std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                       std::variant<cell_t, void *> &&data);

    bool removeItem(size_t position);
    void removeAllItems();

    bool setItemName(size_t item,
                     std::string_view name);
    
    std::string_view getItemName(size_t item) const;

    void setItemData(size_t item,
                     std::variant<cell_t, void *> &&data);
    cell_t getItemData(size_t item);

    void setHandler(std::variant<SourcePawn::IPluginFunction *, MenuHandler_t> &&func);

    void execHandler(int player, int item);

    size_t getId() const;
private:
    void _addItem(int position,
                  std::string_view name,
                  std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                  std::variant<cell_t, void *> &&data);
private:
    size_t m_id;
    bool m_global;
    std::string m_title;
    std::string m_numberFormat;
    int m_time;
    int m_itemsPerPage;
    int m_keys;
    int m_slots[10];
    std::shared_ptr<MenuItem> m_staticSlots[7];

    std::variant<SourcePawn::IPluginFunction *, MenuHandler_t> m_handler;

    std::vector<MenuItem> m_items;
};

class MenuManager : public IMenuManager
{
public:
    MenuManager();
    ~MenuManager() = default;

    IMenu *registerMenu(MenuHandler_t handler, bool global) override;
    IMenu *registerMenu(SourcePawn::IPluginFunction *func, bool global) override;
    IMenu *findMenu(size_t mid) const;

    template<typename ...Args>
    std::shared_ptr<Menu> registerMenuCore(Args... args)
    {
        std::shared_ptr<Menu> menu = std::make_shared<Menu>(m_mid++, std::forward<Args>(args)...);
        m_menus.push_back(menu);
        return menu;
    }
    std::shared_ptr<Menu> findMenuCore(size_t index) const;

    void destroyMenu(size_t index);
    void clearMenus();

    void attachMenu(int player, size_t menuId, int page);
    void closeMenu(int player);

    META_RES ClientCommand(edict_t *pEntity);
    void ClientDisconnected(edict_t *pEntity);
private:
    size_t m_mid = 0;
    std::vector<std::shared_ptr<Menu>> m_menus;

    int m_playerMenu[33];
    int m_playerPage[33];
};
