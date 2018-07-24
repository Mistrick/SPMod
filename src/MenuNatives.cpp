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

// native Menu(MenuHandler handler);
static cell_t MenuCreate(SourcePawn::IPluginContext *ctx,
                         const cell_t *params)
{
    enum { arg_handler = 1 };
    
    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();

    std::shared_ptr<Menu> pMenu;
    pMenu = menuManager->registerMenuCore(ctx->GetFunctionById(params[arg_handler]));

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
    auto pMenu = menuManager->findMenu(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    char *title;
    ctx->LocalToString(params[arg_title], &title);

    pMenu->SetTitle(title);

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
    auto pMenu = menuManager->findMenu(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    char *name;
    ctx->LocalToString(params[arg_name], &name);

    // TODO: implement arg_data, callback
    pMenu->AppendItemCore(name, reinterpret_cast<SourcePawn::IPluginFunction *>(nullptr) /* ctx->GetFunctionById(params[arg_callback]) */);

    return 1;
}

// native void Display(int player, int time = -1, int page = 0);
static cell_t MenuDisplay(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1, arg_player, arg_time, arg_page };
    
    cell_t menuId = params[arg_index];
    if (menuId  < 0)
    {
        ctx->ReportError("Invalid menu index!");
        return 0;
    }

    const std::unique_ptr<MenuManager> &menuManager = gSPGlobal->getMenuManagerCore();
    auto pMenu = menuManager->findMenu(menuId);

    if(!pMenu)
    {
        ctx->ReportError("Menu not found!");
        return 0;
    }

    pMenu->Display(params[arg_player], params[arg_page], params[arg_time]);

    return 1;
}

sp_nativeinfo_t gMenuNatives[] =
{
    {   "Menu.Menu",            MenuCreate      },
    {   "Menu.SetTitle",        MenuSetTitle    },
    {   "Menu.AddItem",         MenuAddItem     },
    {   "Menu.Display",         MenuDisplay     },
    
    {   nullptr,                nullptr         }
};