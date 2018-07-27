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

#include "spmod.hpp"

int gmsgShowMenu = 0;
int gmsgVGUIMenu = 0;

Menu::Menu(size_t id,
           std::variant<SourcePawn::IPluginFunction *, MenuHandler_t> &&handler,
           bool global) : m_id(id),
                          m_global(global),
                          m_title(""),
                          m_numberFormat("\\r#num."),
                          m_time(-1),
                          m_itemsPerPage(7),
                          m_keys(0),
                          m_handler(handler)
{}

void Menu::display(int player, int page, int time)
{
    // format and show menu
    int keys = 0;

    // cache this?
    std::stringstream text;

    text << m_title << "\n\n";

    size_t start = page * m_itemsPerPage;
    size_t hidden = 0;
    
    if(page)
    {
        for(size_t j = 0; j < start; j++)
        {
            if(m_items[j].execCallback(this, j, player) == ItemHide)
                hidden++;
        }
        start += hidden;

        size_t statics = 0;
        for(size_t j = 0; j < m_itemsPerPage; j++)
        {
            if(m_staticSlots[j])
                statics++;
        }
        start -= statics * page;
    }

    size_t slot = 0;
    

    auto addItem = [&](ItemStatus r, size_t s, std::string n)
    {
        text << replace(m_numberFormat, "#num", std::to_string(s + 1 == 10 ? 0 : s + 1));

        if(r == ItemEnabled)
        {
            text << " \\w" << n << "\n";
            keys |= (1 << s);
        }
        else
        {
            text << " \\d" << n << "\n";
        }
    };

    ItemStatus ret = ItemEnabled;

    size_t i = start;
    while(slot < m_itemsPerPage && i < m_items.size())
    {
        MenuItem &item = m_staticSlots[slot] ? *m_staticSlots[slot] : m_items[i];

        ret = item.execCallback(this, m_staticSlots[slot] ? m_items.size() + slot : i, player);

        if(ret == ItemHide)
        {
            if(!m_staticSlots[slot])
                ++i;
            continue;
        }

        addItem(ret, slot, item.name);
        m_slots[slot] = m_staticSlots[slot] ? m_items.size() + slot : i;

        if(!m_staticSlots[slot])
            ++i;
        
        slot++;
    }

    text << "\n";

    while(slot < 7)
    {
        // check for static
        if(m_staticSlots[slot])
        {
            MenuItem &item = *m_staticSlots[slot];
            ret = item.execCallback(this, m_items.size() + slot, player);

            if(ret != ItemHide)
            {
                addItem(ret, slot, item.name);
                m_slots[slot++] = m_items.size() + slot;
            }
        }

        slot++;
        text << "\n";
    }

    if(m_items.size() - hidden > i)
    {
        addItem(ItemEnabled, slot, "Next");
        m_slots[slot++] = MENU_NEXT;
    }
    else
    {
        slot++;
        text << "\n";
    }

    if(page)
    {
        addItem(ItemEnabled, slot, "Back");
        m_slots[slot++] = MENU_BACK;
    }
    else
    {
        slot++;
        text << "\n";
    }

    addItem(ItemEnabled, slot, "Exit");
    m_slots[slot] = MENU_EXIT;

    // TODO: add color autodetect (hl don't show colors)
    // TODO: color tags, remove if game mode unsupport it

    char buffer[512];

#if defined __STDC_LIB_EXT1__ || defined SP_MSVC
    #if defined SP_MSVC
    strncpy_s(buffer, sizeof(buffer), text.str().c_str(), _TRUNCATE);
    #else
    strncpy_s(buffer, sizeof(buffer), text.str().c_str(), sizeof(buffer) - 1);
    #endif
#else
    std::strncpy(buffer, text.str().c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';
#endif

    m_keys = keys;
    m_time = time;

    //show
    UTIL_ShowMenu(INDEXENT(player), keys, time, buffer, strlen(buffer));
}

bool Menu::getGlobal() const
{
    return m_global;
}

void Menu::setTitle(const char *text)
{
    setTitleCore(text);
}

void Menu::setTitleCore(std::string_view text)
{
    m_title = text;
}
void Menu::setItemsPerPage(size_t value)
{
    m_itemsPerPage = value;
}
size_t Menu::getItemsPerPage() const
{
    return m_itemsPerPage;
}

void Menu::setNumberFormat(std::string_view format)
{
    m_numberFormat = format;
}

int Menu::getTime() const
{
    return m_time;
}

int Menu::getKeys() const
{
    return m_keys;
}

int Menu::keyToSlot(int key) const
{
    return m_slots[key];
}

void Menu::appendItem(const char *name,
                      MenuItemCallback_t callback,
                      void *data)
{
    appendItemCore(name, callback, data);
}

void Menu::appendItemCore(std::string_view name,
                          std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                          std::variant<cell_t, void *> &&data)
{
    _addItem(-1, name, std::move(callback), std::move(data));
}

bool Menu::insertItemCore(size_t position,
                          std::string_view name,
                          std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                          std::variant<cell_t, void *> &&data)
{
    if(position < 0 || position >= m_items.size())
        return false;
    
    _addItem(position, name, std::move(callback), std::move(data));

    return true;
}

bool Menu::setStaticItem(size_t position,
                   std::string_view name,
                   std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                   std::variant<cell_t, void *> &&data)
{
    if(position >= m_itemsPerPage)
        return false;
    
    m_staticSlots[position] = std::make_unique<MenuItem>(name.data(), std::move(callback), std::move(data));

    return true;
}

bool Menu::removeItem(size_t position)
{
    if(position < 0 || position >= m_items.size())
        return false;
    
    m_items.erase(m_items.begin() + position);

    return true;
}
void Menu::removeAllItems()
{
    m_items.clear();
}

size_t Menu::getItems() const
{
    return m_items.size();
}

bool Menu::setItemName(size_t item,
                       std::string_view name)
{
    if(item >= m_items.size() + 10)
        return false;
    
    if(item >= m_items.size())
        m_staticSlots[item - m_items.size()].get()->name = name.data();
    else
        m_items[item].name = name.data();

    return true;
}

std::string_view Menu::getItemName(size_t item) const
{
    if(item >= m_items.size() + 10)
        return "";

    if(item >= m_items.size())
        return m_staticSlots[item - m_items.size()].get()->name;
    else
        return m_items[item].name;
}

void Menu::setItemData(size_t item,
                       std::variant<cell_t, void *> &&data)
{
    if(item >= m_items.size())
        m_staticSlots[item - m_items.size()].get()->data;
    else
        m_items[item].data = data;
}

cell_t Menu::getItemData(size_t item)
{
    try
    {
        if(item >= m_items.size())
            return std::get<cell_t>(m_staticSlots[item - m_items.size()].get()->data);
        else 
            return std::get<cell_t>(m_items[item].data);
    }
    catch (const std::bad_variant_access &e [[maybe_unused]])
    {
        return 0;
    }
}

void Menu::setHandler(std::variant<SourcePawn::IPluginFunction *, MenuHandler_t> &&func)
{
    m_handler = func;
}

void Menu::execHandler(int player, int item)
{
    try
    {
        auto *func = std::get<SourcePawn::IPluginFunction *>(m_handler);
        if(func && func->IsRunnable())
        {
            cell_t packedItem = item >= 0 ? PACK_ITEM(m_id, item) : item;
            func->PushCell(static_cast<cell_t>(m_id));
            func->PushCell(packedItem);
            func->PushCell(static_cast<cell_t>(player));
            func->Execute(nullptr);
        }
    }
    catch (const std::bad_variant_access &e [[maybe_unused]])
    {
        auto func = std::get<MenuHandler_t>(m_handler);
        func(this, item, player);
    }
}

size_t Menu::getId() const
{
    return m_id;
}

void Menu::_addItem(int position,
                    std::string_view name,
                    std::variant<SourcePawn::IPluginFunction *, MenuItemCallback_t> &&callback,
                    std::variant<cell_t, void *> &&data)
{
    MenuItem item(name.data(), std::move(callback), std::move(data));
    if(position == -1)
    {
        m_items.push_back(item);
    }
    else
    {
        m_items.insert(m_items.begin() + position, item);
    }
}

MenuManager::MenuManager()
{
    for(int i = 1; i < 33; i++)
    {
        m_playerMenu[i] = -1;
        m_playerPage[i] = 0;
    }
}

IMenu *MenuManager::registerMenu(MenuHandler_t handler, bool global)
{
    return registerMenuCore(handler, global).get();
}
IMenu *MenuManager::registerMenu(SourcePawn::IPluginFunction *func, bool global)
{
    return registerMenuCore(func, global).get();
}

IMenu *MenuManager::findMenu(size_t mid) const
{
    return findMenuCore(mid).get();
}

void MenuManager::destroyMenu(size_t index)
{
    // close menu for any player
    for(int i = 1; i < 33; i++)
    {
        if(m_playerMenu[i] == static_cast<int>(index))
            closeMenu(i);
    }

    auto iter = m_menus.begin();
    while (iter != m_menus.end())
    {
        if ((*iter)->getId() == index)
        {
            m_menus.erase(iter);
            break;
        }
        ++iter;
    }
}

std::shared_ptr<Menu> MenuManager::findMenuCore(size_t index) const
{
    for(auto menu : m_menus)
    {
        if(menu->getId() == index)
            return menu;
    }
    return nullptr;
}

void MenuManager::clearMenus()
{
    m_menus.clear();
    m_mid = 0;
}

void MenuManager::displayMenu(std::shared_ptr<Menu> menu, int player, int page, int time)
{
    closeMenu(player);

    m_playerMenu[player] = menu->getId();
    m_playerPage[player] = page;

    menu->display(player, page, time);
}

void MenuManager::closeMenu(int player)
{
    if(m_playerMenu[player] == -1)
        return;
    
    std::shared_ptr<Menu> pMenu = findMenuCore(m_playerMenu[player]);

    m_playerMenu[player] = -1;
    pMenu->execHandler(player, MENU_EXIT);
}

META_RES MenuManager::ClientCommand(edict_t *pEntity)
{
    int pressedKey = std::stoi(CMD_ARGV(1), nullptr, 0) - 1;
    int player = ENTINDEX(pEntity);

    if(m_playerMenu[player] == -1)
        return MRES_IGNORED;
    
    std::shared_ptr<Menu> pMenu = findMenuCore(m_playerMenu[player]);
    
    if(pMenu->getKeys() & (1 << pressedKey))
    {
        m_playerMenu[player] = -1;

        int slot = pMenu->keyToSlot(pressedKey);

        pMenu->execHandler(player, slot);

        if(slot == MENU_BACK)
        {
            displayMenu(pMenu, player, m_playerPage[player] - 1, pMenu->getTime());
        }
        else if(slot == MENU_NEXT)
        {
            displayMenu(pMenu, player, m_playerPage[player] + 1, pMenu->getTime());
        }
        else if(!pMenu->getGlobal())
        {
            destroyMenu(pMenu->getId());
        }

        return MRES_SUPERCEDE;
    }

    return MRES_IGNORED;
}

void MenuManager::ClientDisconnected(edict_t *pEntity)
{
    closeMenu(ENTINDEX(pEntity));
}

// TODO: move to util.cpp or same
/* warning - don't pass here const string */
void UTIL_ShowMenu(edict_t* pEdict, int slots, int time, char *menu, int mlen)
{
    char *n = menu;
    char c = 0;
    int a;

    if (!gmsgShowMenu)
        return; // some games don't support ShowMenu (Firearms)

    do
    {
        a = mlen;
        if (a > 175) a = 175;
        mlen -= a;
        c = *(n+=a);
        *n = 0;
        
        MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, nullptr, pEdict);
        WRITE_SHORT(slots);
        WRITE_CHAR(time);
        WRITE_BYTE(c ? true : false);
        WRITE_STRING(menu);
        MESSAGE_END();
        *n = c;
        menu = n;
    }
    while (*n);
}

std::string replace(const std::string& str, const std::string& from, const std::string& to)
{
    std::string temp(str);
    size_t start_pos = temp.find(from);
    if(start_pos == std::string::npos)
        return std::string("");
    temp.replace(start_pos, from.length(), to);
    return temp;
}
