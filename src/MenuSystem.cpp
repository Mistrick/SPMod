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

int gmsgShowMenu;
int gmsgVGUIMenu;

Menu::Menu(size_t id,
           MenuHandler_t handler) : m_id(id),
                                    m_title(""),
                                    m_time(-1),
                                    m_itemsPerPage(7),
                                    m_keys(0),
                                    m_handler(handler),
                                    m_pluginHandler(nullptr)
{}
Menu::Menu(size_t id,
           SourcePawn::IPluginFunction *handler) : m_id(id),
                                                   m_title(""),
                                                   m_time(-1),
                                                   m_itemsPerPage(7),
                                                   m_keys(0),
                                                   m_handler(nullptr),
                                                   m_pluginHandler(handler)
{}

void Menu::Display(int player, int page, int time)
{
    // TODO: close last menu before override
    gSPGlobal->getMenuManagerCore()->CloseMenu(INDEXENT(player));

    // format and show menu
    int keys = 0;

    // cache this?
    std::stringstream text;

    text << m_title << "\n\n";

    size_t start = page * m_itemsPerPage;
    size_t end = start + m_itemsPerPage < m_items.size() ? start + m_itemsPerPage : m_items.size();
    size_t slot = 0;

    // TODO: add color autodetect (hl don't show colors)
    for(size_t i = start; i < end; i++)
    {
        // TODO: number format
        // TODO: item callback
        text << "\\r" << slot + 1 << ". \\w" << m_items[i].name << "\n";
        keys |= (1 << slot);
        m_slots[slot++] = i;
    }

    text << "\n";

    // TODO: make it safe
    while(slot < 7)
    {
        slot++;
        text << "\n";
    }

    if(m_items.size() > end)
    {
        keys |= (1 << slot);
        m_slots[slot] = MENU_NEXT;
        text << "\\r" << ++slot << ". \\w" << "Next" << "\n";
    }
    else
    {
        slot++;
        text << "\n";
    }

    if(page)
    {
        keys |= (1 << slot);
        m_slots[slot] = MENU_BACK;
        text << "\\r" << ++slot << ". \\w"<< "Back" << "\n";
    }
    else
    {
        slot++;
        text << "\n";
    }

    keys |= (1 << slot);
    m_slots[slot] = MENU_EXIT;
    text << "\\r" << (++slot == 10 ? 0 : slot) << ". \\w" << "Exit" << "\n";

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

    gSPGlobal->getMenuManagerCore()->AttachMenu(player, m_id, page);

    m_keys = keys;
    m_time = time;

    //show
    UTIL_ShowMenu(INDEXENT(player), keys, time, buffer, strlen(buffer));
}
void Menu::Close(...) const
{
    // code
}

void Menu::SetTitleCore(std::string_view text)
{
    m_title = text;
}
void Menu::SetItemsPerPage(int value)
{
    m_itemsPerPage = value;
}
int Menu::GetItemsPerPage() const
{
    return m_itemsPerPage;
}

void Menu::AppendItemCore(std::string_view name,
                      MenuItemCallback_t callback)
{
    addItem(-1, name, callback, nullptr);
}
void Menu::AppendItemCore(std::string_view name,
                      SourcePawn::IPluginFunction *pluginCallback)
{
    addItem(-1, name, nullptr, pluginCallback);
}

bool Menu::InsertItem(size_t position,
                      std::string_view name,
                      MenuItemCallback_t callback)
{
    if(position < 0 || position >= m_items.size())
        return false;
    
    addItem(position, name, callback, nullptr);

    return true;
}
bool Menu::InsertItem(size_t position,
                       std::string_view name,
                       SourcePawn::IPluginFunction *pluginCallback)
{
    if(position < 0 || position >= m_items.size())
        return false;
    
    addItem(position, name, nullptr, pluginCallback);

    return true;
}

bool Menu::RemoveItem(size_t position)
{
    if(position < 0 || position >= m_items.size())
        return false;
    
    m_items.erase(m_items.begin() + position);

    return true;
}
void Menu::RemoveAllItems()
{
    m_items.clear();
}

void Menu::AddHandler(MenuHandler_t func)
{
    m_handler = func;
}
void Menu::AddPluginHandler(SourcePawn::IPluginFunction *func)
{
    m_pluginHandler = func;
}

void Menu::ExecHandler(int player, int item)
{
    if(m_handler)
    {
        m_handler(this, item, player);
    }
    if(m_pluginHandler)
    {
        m_pluginHandler->PushCell(static_cast<cell_t>(m_id));
        m_pluginHandler->PushCell(static_cast<cell_t>(item));
        m_pluginHandler->PushCell(static_cast<cell_t>(player));
        m_pluginHandler->Execute(nullptr);
    }
}

size_t Menu::getId() const
{
    return m_id;
}

void Menu::addItem(int pos,
                   std::string_view name,
                   MenuItemCallback_t callback,
                   SourcePawn::IPluginFunction *pluginCallback)
{
    MenuItem item(name.data()/* , callback, pluginCallback */);
    if(pos == -1)
    {
        m_items.push_back(item);
    }
    else
    {
        m_items.insert(m_items.begin() + pos, item);
    }
}

std::shared_ptr<Menu> MenuManager::registerMenuCore(MenuHandler_t handler)
{
    std::shared_ptr<Menu> menu = std::make_shared<Menu>(m_mid++, handler);
    m_menus.push_back(menu);
    return menu;
}
std::shared_ptr<Menu> MenuManager::registerMenuCore(SourcePawn::IPluginFunction *func)
{
    std::shared_ptr<Menu> menu = std::make_shared<Menu>(m_mid++, func);
    m_menus.push_back(menu);
    return menu;
}

void MenuManager::destroyMenu(size_t index)
{
    /* for(auto menu : m_menus)
    {
        if(menu->getId() == index)
            m_menus.erase(menu);
    } */
}

std::shared_ptr<Menu> MenuManager::findMenu(size_t index) const
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

void MenuManager::AttachMenu(int player, size_t menuId, int page)
{
    m_playerMenu[player] = menuId;
    m_playerPage[player] = page;
}

META_RES MenuManager::ClientCommand(edict_t *pEntity)
{
    int pressedKey = std::stoi(CMD_ARGV(1), nullptr, 0) - 1;
    int player = ENTINDEX(pEntity);

    if(m_playerMenu[player] == -1)
        return MRES_IGNORED;
    
    auto pMenu = findMenu(m_playerMenu[player]);

    if(pMenu->GetKeys() & (1 << pressedKey))
    {
        m_playerMenu[player] = -1;

        int slot = pMenu->KeyToSlot(pressedKey);

        if(slot == MENU_BACK)
        {
            pMenu->Display(player, m_playerPage[player] - 1, pMenu->GetTime());
        }
        else if(slot == MENU_NEXT)
        {
            pMenu->Display(player, m_playerPage[player] + 1, pMenu->GetTime());
        }

        pMenu->ExecHandler(player, slot);

        return MRES_SUPERCEDE;
    }

    return MRES_IGNORED;
}

void MenuManager::ClientDisconnected(edict_t *pEntity)
{
    // TODO: close menu if exist
    CloseMenu(pEntity);
}

void MenuManager::CloseMenu(edict_t *pEntity)
{
    int player = ENTINDEX(pEntity);

    if(m_playerMenu[player] == -1)
        return;
    
    auto pMenu = findMenu(m_playerMenu[player]);

    m_playerMenu[player] = -1;
    pMenu->ExecHandler(player, MENU_EXIT);
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
