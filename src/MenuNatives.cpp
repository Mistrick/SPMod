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

// native Menu(MenuHandler handler, bool global = false);
static cell_t MenuCreate(SourcePawn::IPluginContext *ctx,
                         const cell_t *params)
{
    enum { arg_handler = 1, arg_global };
    
    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();

    std::shared_ptr<Menu> pMenu;
    pMenu = menuManager->registerMenuCore(ctx->GetFunctionById(params[arg_handler]), params[arg_global]);

    if (!pMenu)
        return -1;

    return pMenu->getId();
}

// native void SetTitle(const char title[]);
static cell_t MenuSetTitle(SourcePawn::IPluginContext *ctx,
                           const cell_t *params)
{
    enum { arg_index = 1, arg_title };
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    std::shared_ptr<Menu> pMenu = menuManager->findMenuCore(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    char *title;
    ctx->LocalToString(params[arg_title], &title);

    pMenu->setTitle(title);

    return 1;
}

// native void AddItem(const char name[], any data = 0, CallbackHandler callback = 0);
static cell_t MenuAddItem(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1, arg_name, arg_data, arg_callback };
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    std::shared_ptr<Menu> pMenu = menuManager->findMenuCore(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    char *name;
    ctx->LocalToString(params[arg_name], &name);

    // TODO: implement arg_data, callback
    pMenu->appendItemCore(name, ctx->GetFunctionById(params[arg_callback]));

    return 1;
}

// native void InsertItem(int position, const char name[], any data = 0, CallbackHandler callback = 0);
static cell_t MenuInsertItem(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1, arg_position, arg_name, arg_data, arg_callback };
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    std::shared_ptr<Menu> pMenu = menuManager->findMenuCore(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    char *name;
    ctx->LocalToString(params[arg_name], &name);

    // TODO: implement arg_data
    pMenu->insertItemCore(static_cast<size_t>(params[arg_position]), name, ctx->GetFunctionById(params[arg_callback]));

    return 1;
}

// native void RemoveItem(int position);
static cell_t MenuRemoveItem(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1, arg_position };
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    std::shared_ptr<Menu> pMenu = menuManager->findMenuCore(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    pMenu->removeItem(static_cast<size_t>(params[arg_position]));

    return 1;
}

// native void RemoveAllItems();
static cell_t MenuRemoveAllItems(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1};
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    std::shared_ptr<Menu> pMenu = menuManager->findMenuCore(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    pMenu->removeAllItems();

    return 1;
}

// native void Display(int player, int time = -1, int page = 0);
static cell_t MenuDisplay(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1, arg_player, arg_page, arg_time };
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    std::shared_ptr<Menu> pMenu = menuManager->findMenuCore(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    pMenu->display(params[arg_player], params[arg_page], params[arg_time]);

    return 1;
}

// native void CloseMenu(int player);
static cell_t MenuClose(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_player = 1 };
    
    int player = params[arg_player];
    
    if (player  < 0 || player > gpGlobals->maxClients)
    {
        ctx->ReportError("Invalid player index! %d", player);
        return 0;
    }

    // TODO: make loop if player == 0
    gSPGlobal->getMenuManagerCore()->closeMenu(player);

    UTIL_ShowMenu(INDEXENT(player), 0, 0, "\n", 1);

    return 1;
}

sp_nativeinfo_t gMenuNatives[] =
{
    {   "Menu.Menu",                MenuCreate          },
    {   "Menu.SetTitle",            MenuSetTitle        },
    {   "Menu.AddItem",             MenuAddItem         },
    {   "Menu.InsertItem",          MenuInsertItem      },
    {   "Menu.RemoveItem",          MenuRemoveItem      },
    {   "Menu.RemoveAllItems",      MenuRemoveAllItems  },
    {   "Menu.Display",             MenuDisplay         },

    {   "CloseMenu",                MenuClose           },

    {   nullptr,                    nullptr             }
};