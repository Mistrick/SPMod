/*
 *  Copyright (C) 2018-2020 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  SPMod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with SPMod.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

namespace SPMod
{
    class IPlayer;

    enum class NavigationType : int8_t
    {
        None = 0,
        Back = -1,
        Next = -2,
        Exit = -3
    };

    class IMenu
    {
    public:
        enum class Style : int8_t
        {
            Item = 0,
            Text
        };

        class IItem
        {
        public:
            enum class Status : int8_t
            {
                Enabled = 0,
                Disabled,
                Hide
            };

            using Callback =
                std::function<Status(IMenu *const menu, IItem *const item, IPlayer *const player, std::any data)>;

            virtual std::string_view getName() const = 0;
            virtual void setName(std::string_view name) = 0;

            virtual std::any getData() const = 0;
            virtual void setData(std::any data) = 0;

            virtual NavigationType getNavType() const = 0;

            virtual void setCallback(Callback func, std::any data) = 0;

            virtual ~IItem() = default;
        };

        using ItemHandler =
            std::function<void(IMenu *const menu, IItem *const item, IPlayer *const player, std::any data)>;
        using TextHandler =
            std::function<void(IMenu *const menu, std::uint32_t key, IPlayer *const player, std::any data)>;

        virtual void display(IPlayer *player, std::uint32_t page, std::uint32_t time) = 0;
        virtual bool getGlobal() const = 0;
        virtual Style getStyle() const = 0;

        virtual void setText(std::string_view text) = 0;
        virtual void setKeys(std::uint32_t keys) = 0;

        virtual void setTitle(std::string_view text) = 0;

        virtual void setItemsPerPage(std::size_t value) = 0;
        virtual std::size_t getItemsPerPage() const = 0;

        virtual std::uint32_t getTime() const = 0;
        virtual std::uint32_t getKeys() const = 0;

        virtual IItem *appendItem(std::string_view name, IItem::Callback callback, std::any cbData, std::any data) = 0;

        virtual IItem *insertItem(std::size_t position,
                                  std::string_view name,
                                  IItem::Callback callback,
                                  std::any cbData,
                                  std::any data) = 0;

        virtual IItem *setStaticItem(std::size_t position,
                                     std::string_view name,
                                     IItem::Callback callback,
                                     std::any cbData,
                                     std::any data) = 0;

        virtual bool removeItem(std::size_t position) = 0;
        virtual void removeAllItems() = 0;

        virtual IItem *getItem(std::size_t position) const = 0;

        virtual std::uint32_t getItemIndex(const IItem *item) const = 0;
        virtual void setNumberFormat(std::string_view format) = 0;

        virtual std::size_t getItems() const = 0;

        virtual ~IMenu() = default;
    };

    class IMenuMngr : public ISPModInterface
    {
    public:
        static constexpr std::uint16_t MAJOR_VERSION = 0;
        static constexpr std::uint16_t MINOR_VERSION = 0;

        static constexpr std::uint32_t VERSION = (MAJOR_VERSION << 16 | MINOR_VERSION);
        /**
         * @brief Gets interface's name.
         *
         * @return        Interface's name.
         */
        std::string_view getName() const override
        {
            return "IMenuMngr";
        }

        /**
         * @brief Gets interface's version.
         *
         * @note The first 16 most significant bits represent major version, the rest represent minor version.
         *
         * @return        Interface's version.
         */
        std::uint32_t getVersion() const override
        {
            return VERSION;
        }

        virtual IMenu *registerMenu(const std::variant<IMenu::ItemHandler, IMenu::TextHandler> &handler,
                                    std::any data,
                                    bool global) = 0;
        virtual void destroyMenu(const IMenu *menu) = 0;
        virtual ~IMenuMngr() = default;
    };
} // namespace SPMod
