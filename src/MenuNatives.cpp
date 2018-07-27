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

    pMenu->appendItemCore(name, ctx->GetFunctionById(params[arg_callback]), params[arg_data]);

    return 1;
}

// native void AddStaticItem(int position, const char name[], any data = 0, CallbackHandler callback = 0);
static cell_t MenuAddStaticItem(SourcePawn::IPluginContext *ctx,
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

    pMenu->setStaticItem(static_cast<size_t>(params[arg_position]), name, ctx->GetFunctionById(params[arg_callback]), params[arg_data]);

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

    pMenu->insertItemCore(static_cast<size_t>(params[arg_position]), name, ctx->GetFunctionById(params[arg_callback]), params[arg_data]);

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

    menuManager->displayMenu(pMenu, params[arg_player], params[arg_page], params[arg_time]);

    return 1;
}

// native void Menu.Destroy();
static cell_t MenuDestroy(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1 };
    
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

    menuManager->destroyMenu(pMenu->getId());

    return 1;
}

// native int Menu.Items.get();
static cell_t MenuItemsGet(SourcePawn::IPluginContext *ctx,
                           const cell_t *params)
{
    enum { arg_index = 1 };
    
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

    return static_cast<cell_t>(pMenu->getItems());
}

// native int Menu.ItemsPerPage.set(int value);
static cell_t MenuItemsPerPageSet(SourcePawn::IPluginContext *ctx,
                           const cell_t *params)
{
    enum { arg_index = 1, arg_value };
    
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

    pMenu->setItemsPerPage(static_cast<size_t>(params[arg_value]));
    
    return 1;
}

// native int Menu.ItemsPerPage.get();
static cell_t MenuItemsPerPageGet(SourcePawn::IPluginContext *ctx,
                           const cell_t *params)
{
    enum { arg_index = 1 };
    
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

    return static_cast<cell_t>(pMenu->getItemsPerPage());
}

enum
{
    MProp_NumberFormat
};

// native void SetProp(MenuProp prop, any ...);
static cell_t MenuSetProp(SourcePawn::IPluginContext *ctx,
                          const cell_t *params)
{
    enum { arg_index = 1, arg_prop, arg_args };
    
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

    switch(params[arg_prop])
    {
    case MProp_NumberFormat:
        // TODO: find "#num" before call func
        char *format;
        ctx->LocalToString(params[arg_args], &format);
        pMenu->setNumberFormat(format);
        break;
    default:
        break;
    }

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

    char menu[] = "\n";
    UTIL_ShowMenu(INDEXENT(player), 0, 0, menu, strlen(menu));

    return 1;
}

// native void MenuItem.SetName(const char[] name);
static cell_t MenuItemSetName(SourcePawn::IPluginContext *ctx,
                              const cell_t *params)
{
    enum { arg_item = 1, arg_name };
    
    cell_t packedItem = params[arg_item];

    // MenuExit, Next, Back
    if(packedItem < 0)
        return 0;

    cell_t menuId, itemId;
    UNPACK_ITEM(packedItem, menuId, itemId);

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

    pMenu->setItemName(itemId, name);

    return 1;
}

// native void MenuItem.GetName(char[] name, int size);
static cell_t MenuItemGetName(SourcePawn::IPluginContext *ctx,
                              const cell_t *params)
{
    enum { arg_item = 1, arg_name, arg_size };
    
    cell_t packedItem = params[arg_item];

    // MenuExit, Next, Back
    // TODO: throw error?
    if(packedItem < 0)
        return 0;
    
    cell_t menuId, itemId;
    UNPACK_ITEM(packedItem, menuId, itemId);

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

    std::string str = pMenu->getItemName(itemId).data();

    #if defined __STDC_LIB_EXT1__ || defined SP_MSVC
    #if defined SP_MSVC
    strncpy_s(name, params[arg_size], str.c_str(), _TRUNCATE);
    #else
    strncpy_s(name, params[arg_size], str.c_str(), params[arg_size] - 1);
    #endif
    #else
    std::strncpy(name, str.c_str(), params[arg_size]);
    name[params[arg_size] - 1] = '\0';
    #endif

    pMenu->setItemName(itemId, name);

    return 1;
}

// native void MenuItem.SetData(any data);
static cell_t MenuItemSetData(SourcePawn::IPluginContext *ctx,
                              const cell_t *params)
{
    enum { arg_item = 1, arg_data };
    
    cell_t packedItem = params[arg_item];

    // MenuExit, Next, Back
    if(packedItem < 0)
        return 0;
    
    cell_t menuId, itemId;
    UNPACK_ITEM(packedItem, menuId, itemId);

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

    pMenu->setItemData(itemId, params[arg_data]);

    return 1;
}

// native any MenuItem.GetData();
static cell_t MenuItemGetData(SourcePawn::IPluginContext *ctx,
                              const cell_t *params)
{
    enum { arg_item = 1, arg_data };
    
    cell_t packedItem = params[arg_item];

    // MenuExit, Next, Back
    if(packedItem < 0)
        return 0;
    
    cell_t menuId, itemId;
    UNPACK_ITEM(packedItem, menuId, itemId);

    if (menuId < 0)
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

    return pMenu->getItemData(itemId);
}

sp_nativeinfo_t gMenuNatives[] =
{
    {   "Menu.Menu",                MenuCreate              },
    {   "Menu.SetTitle",            MenuSetTitle            },
    {   "Menu.AddItem",             MenuAddItem             },
    {   "Menu.AddStaticItem",       MenuAddStaticItem       },
    {   "Menu.InsertItem",          MenuInsertItem          },
    {   "Menu.RemoveItem",          MenuRemoveItem          },
    {   "Menu.RemoveAllItems",      MenuRemoveAllItems      },
    {   "Menu.SetProp",             MenuSetProp             },
    {   "Menu.Display",             MenuDisplay             },
    {   "Menu.Destroy",             MenuDestroy             },
    {   "Menu.Items.get",           MenuItemsGet            },
    {   "Menu.ItemsPerPage.set",    MenuItemsPerPageSet     },
    {   "Menu.ItemsPerPage.get",    MenuItemsPerPageGet     },

    {   "CloseMenu",                MenuClose               },

    {   "MenuItem.SetName",         MenuItemSetName         },
    {   "MenuItem.GetName",         MenuItemGetName         },
    {   "MenuItem.SetData",         MenuItemSetData         },
    {   "MenuItem.GetData",         MenuItemGetData         },

    {   nullptr,                    nullptr                 }
};