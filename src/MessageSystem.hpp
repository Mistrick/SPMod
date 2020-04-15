/*
 *  Copyright (C) 2018 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  This program is free software: you can redistribute it and/or modify
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

class Message : public IMessage
{
public:
    struct MessageParam
    {
        MsgParamType type;
        std::variant<int, float, std::string> data;
    };

    Message() = default;
    ~Message() = default;

    // IMessage
    std::size_t getParams() const override;
    MsgParamType getParamType(std::size_t index) const override;

    int getParamInt(std::size_t index) const override;
    float getParamFloat(std::size_t index) const override;
    const char *getParamString(std::size_t index) const override;

    void setParamInt(std::size_t index, int value) override;
    void setParamFloat(std::size_t index, float value) override;
    void setParamString(std::size_t index, const char *string) override;

    int getDest() const override;
    int getType() const override;
    const float *getOrigin() const override;
    IEdict *getEdict() const override;

    // Message
    void init(int dest, int type, const float *origin, edict_t *edict);

    void exec() const;

    void clearParams();

    void addParam(MsgParamType type, std::variant<int, float, std::string> &&data);

    void setParam(std::size_t index, std::variant<int, float, std::string> &&data);

    template<typename T>
    const T &getParam(std::size_t index) const
    {
        return std::get<T>(m_params[index].data);
    }

private:
    int m_dest;
    int m_type;
    float *m_origin;
    std::unique_ptr<Edict> m_edict;

    std::vector<MessageParam> m_params;
};

class MessageHooks
{
public:
    struct HookInfo
    {
        std::size_t m_index;
        bool m_post;
        MessageHandler m_handler;
        std::any m_cbData;
    };

    MessageHooks() {}
    ~MessageHooks() {}

    void addHook(std::size_t index, MessageHandler handler, std::any cbData, bool post);

    void removeHook(std::size_t index);

    IForward::ReturnValue exec(const std::unique_ptr<Message> &message, bool post) const;

    bool hasHooks() const;

    void clearHooks();

private:
    std::vector<HookInfo> m_handlers;
};

class MessageMngr : public IMessageMngr
{
public:
    MessageMngr();
    ~MessageMngr() = default;

    // IMessageMngr
    std::size_t registerHook(int msgType, MessageHandler handler, std::any cbData, bool post) override;
    void unregisterHook(int msgType, std::size_t index) override;

    BlockType getMessageBlock(int msgType) const override;
    void setMessageBlock(int msgType, BlockType blockType) override;

    IForward::ReturnValue execHandlers(bool post);

    IMessage *getMessage() const override
    {
        return m_message.get();
    }

    bool inHook() const override;

    void clearMessages();

    META_RES MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed);

    META_RES MessageEnd();

    template<typename T>
    META_RES WriteParam(MsgParamType type, T value)
    {
        if (m_inblock)
        {
            return MRES_SUPERCEDE;
        }
        else if (m_inhook)
        {
            m_message->addParam(type, value);
            return MRES_SUPERCEDE;
        }
        return MRES_IGNORED;
    }

private:
    std::unique_ptr<Message> m_message;
    std::array<MessageHooks, MAX_USER_MESSAGES> m_hooks;
    std::array<BlockType, MAX_USER_MESSAGES> m_blocks;

    bool m_inhook;
    bool m_inblock;
    int m_msgType;

    std::size_t m_id;
};
