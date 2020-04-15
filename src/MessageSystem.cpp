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

#include "spmod.hpp"

void Message::init(int dest, int type, const float *origin, edict_t *edict)
{
    m_dest = dest;
    m_type = type;
    m_origin = const_cast<float *>(origin);
    m_edict = std::make_unique<Edict>(edict);
}

void Message::exec() const
{
    MESSAGE_BEGIN(m_dest, m_type, m_origin, m_edict->getInternalEdict());

    for (auto param : m_params)
    {
        switch (param.type)
        {
            case MsgParamType::Byte:
                WRITE_BYTE(std::get<int>(param.data));
                break;
            case MsgParamType::Char:
                WRITE_CHAR(std::get<int>(param.data));
                break;
            case MsgParamType::Short:
                WRITE_SHORT(std::get<int>(param.data));
                break;
            case MsgParamType::Long:
                WRITE_LONG(std::get<int>(param.data));
                break;
            case MsgParamType::Angle:
                WRITE_ANGLE(std::get<float>(param.data));
                break;
            case MsgParamType::Coord:
                WRITE_COORD(std::get<float>(param.data));
                break;
            case MsgParamType::String:
                WRITE_STRING(std::get<std::string>(param.data).c_str());
                break;
            case MsgParamType::Entity:
                WRITE_ENTITY(std::get<int>(param.data));
                break;
        }
    }

    MESSAGE_END();
}

void Message::clearParams()
{
    m_params.clear();
}

int Message::getParamInt(std::size_t index) const
{
    return getParam<int>(index);
}
float Message::getParamFloat(std::size_t index) const
{
    return getParam<float>(index);
}
const char *Message::getParamString(std::size_t index) const
{
    return getParam<std::string>(index).data();
}

void Message::setParamInt(std::size_t index, int value)
{
    setParam(index, value);
}
void Message::setParamFloat(std::size_t index, float value)
{
    setParam(index, value);
}
void Message::setParamString(std::size_t index, const char *string)
{
    setParam(index, string);
}

void Message::addParam(MsgParamType type, std::variant<int, float, std::string> &&data)
{
    MessageParam param;
    param.type = type;
    param.data = data;

    m_params.push_back(param);
}

void Message::setParam(std::size_t index, std::variant<int, float, std::string> &&data)
{
    m_params[index].data = data;
}

std::size_t Message::getParams() const
{
    return m_params.size();
}

MsgParamType Message::getParamType(std::size_t index) const
{
    return m_params[index].type;
}

int Message::getDest() const
{
    return m_dest;
}

int Message::getType() const
{
    return m_type;
}

const float *Message::getOrigin() const
{
    return m_origin;
}

IEdict *Message::getEdict() const
{
    return m_edict.get();
}

void MessageHooks::addHook(std::size_t index, MessageHandler handler, std::any cbData, bool post)
{
    HookInfo hook;
    hook.m_index = index;
    hook.m_post = post;
    hook.m_handler = handler;
    hook.m_cbData = cbData;

    m_handlers.push_back(hook);
}
void MessageHooks::removeHook(std::size_t index)
{
    auto iter = m_handlers.begin();
    while (iter != m_handlers.end())
    {
        if (iter->m_index == index)
        {
            m_handlers.erase(iter);
            break;
        }
        ++iter;
    }
}
IForward::ReturnValue MessageHooks::exec(const std::unique_ptr<Message> &message, bool post) const
{
    IForward::ReturnValue result = IForward::ReturnValue::Ignored;

    for (const auto &hook : m_handlers)
    {
        if (hook.m_post != post)
        {
            continue;
        }

        IForward::ReturnValue ret;

        auto func = hook.m_handler;

        ret = func(message.get(), hook.m_cbData);

        if (ret == IForward::ReturnValue::Stop)
        {
            return IForward::ReturnValue::Stop;
        }
        else if (ret == IForward::ReturnValue::Handled)
        {
            result = IForward::ReturnValue::Handled;
        }
    }

    return result;
}

bool MessageHooks::hasHooks() const
{
    return m_handlers.size() ? true : false;
}

void MessageHooks::clearHooks()
{
    m_handlers.clear();
}

MessageMngr::MessageMngr() : m_message(std::make_unique<Message>())
{
    m_inblock = false;

    for (std::size_t i = 0; i < MAX_USER_MESSAGES; i++)
    {
        m_blocks[i] = BlockType::Not;
    }
}

std::size_t MessageMngr::registerHook(int msgType, MessageHandler handler, std::any cbData, bool post)
{
    m_hooks[msgType].addHook(m_id++, handler, cbData, post);

    return m_id - 1;
}

void MessageMngr::unregisterHook(int msgType, std::size_t index)
{
    m_hooks[msgType].removeHook(index);
}

BlockType MessageMngr::getMessageBlock(int msgType) const
{
    return m_blocks[msgType];
}

void MessageMngr::setMessageBlock(int msgType, BlockType blockType)
{
    m_blocks[msgType] = blockType;
}

IForward::ReturnValue MessageMngr::execHandlers(bool post)
{
    return m_hooks[m_message->getType()].exec(m_message, post);
}

bool MessageMngr::inHook() const
{
    return m_inhook;
}

void MessageMngr::clearMessages()
{
    for (unsigned int i = 0; i < MAX_USER_MESSAGES; i++)
    {
        m_hooks[i].clearHooks();
    }
    m_id = 0;
}

META_RES MessageMngr::MessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
    if (m_blocks[msg_type] != BlockType::Not)
    {
        m_inblock = true;
        m_msgType = msg_type;
        return MRES_SUPERCEDE;
    }

    m_inhook = m_hooks[msg_type].hasHooks();

    if (m_inhook)
    {
        m_message->init(msg_dest, msg_type, pOrigin, ed);
        return MRES_SUPERCEDE;
    }

    return MRES_IGNORED;
}
META_RES MessageMngr::MessageEnd()
{
    if (m_inblock)
    {
        m_inblock = false;
        if (m_blocks[m_msgType] == BlockType::Once)
        {
            m_blocks[m_msgType] = BlockType::Not;
        }
        return MRES_SUPERCEDE;
    }

    if (!m_inhook)
    {
        return MRES_IGNORED;
    }

    // exec pre hooks
    // add contidions for hooks like in amxx events

    IForward::ReturnValue ret = execHandlers(false);

    // hooks can change params
    // then exec message
    if (ret == IForward::ReturnValue::Ignored)
    {
        m_message->exec();

        // exec post hooks?
        execHandlers(true);
    }

    m_inhook = false;

    m_message->clearParams();

    return MRES_SUPERCEDE;
}
