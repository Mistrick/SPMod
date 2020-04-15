/*
 *  Copyright (C) 2018-2020 SPMod Development Team
 *
 *  This file is part of SPMod.
 *
 *  SPMod is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SPMod is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SPMod.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ExtMain.hpp"

bool ignoreHooks;

// int GetUserMsgId(const char[] msgname);
static cell_t GetUserMsgId(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_msgname = 1
    };

    char *string;
    ctx->LocalToString(params[arg_msgname], &string);

    return gSPGlobal->getMetaFuncs()->getUsrMsgId(string);
}

// int GetUserMsgName(int msgid, char[] str, int len) = 3 params
static cell_t GetUserMsgName(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_msg = 1,
        arg_str,
        arg_len
    };

    std::string_view string = gSPGlobal->getMetaFuncs()->getUsrMsgName(params[arg_msg]);
    if (!string.empty())
    {
        ctx->StringToLocal(params[arg_str], params[arg_len], string.data());
        return string.length();
    }

    return 0;
}

// native Message HookMessage(int msgtype, MessageHandler handler, bool post = false);
static cell_t HookMessage(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_msg_type = 1,
        arg_handler,
        arg_post
    };

    SourcePawn::IPluginFunction *func = ctx->GetFunctionById(params[arg_handler]);

    if (params[arg_msg_type] < 0 || static_cast<unsigned int>(params[arg_msg_type]) >= SPMod::MAX_USER_MESSAGES)
    {
        ctx->ReportError("Invalid message id (%d).", params[arg_msg_type]);
        return 0;
    }

    // TODO: rework register
    return gSPGlobal->getMessageManager()->registerHook(params[arg_msg_type], SPExt::Listener::MessageHookCallback,
                                                        func, params[arg_post]);
}

// native void UnhookMessage(int msgtype, Message msghook);
static cell_t UnhookMessage(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_msg_type = 1,
        arg_hook_index
    };

    if (params[arg_msg_type] < 0 || static_cast<unsigned int>(params[arg_msg_type]) >= SPMod::MAX_USER_MESSAGES)
    {
        ctx->ReportError("Invalid message id (%d).", params[arg_msg_type]);
        return 0;
    }

    // TODO: rework unregister
    gSPGlobal->getMessageManager()->unregisterHook(params[arg_msg_type],
                                                   static_cast<std::size_t>(params[arg_hook_index]));

    return 1;
}

// native BlockType GetMsgBlock(int msgtype);
static cell_t GetMsgBlock(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_msg_type = 1
    };

    if (params[arg_msg_type] < 0 || static_cast<unsigned int>(params[arg_msg_type]) >= SPMod::MAX_USER_MESSAGES)
    {
        ctx->ReportError("Invalid message id (%d).", params[arg_msg_type]);
        return 0;
    }

    return static_cast<cell_t>(gSPGlobal->getMessageManager()->getMessageBlock(params[arg_msg_type]));
}

// native void SetMsgBlock(int msgtype, BlockType type);
static cell_t SetMsgBlock(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_msg_type = 1,
        arg_block_type
    };

    if (params[arg_msg_type] < 0 || static_cast<unsigned int>(params[arg_msg_type]) >= SPMod::MAX_USER_MESSAGES)
    {
        ctx->ReportError("Invalid message id (%d).", params[arg_msg_type]);
        return 0;
    }

    gSPGlobal->getMessageManager()->setMessageBlock(params[arg_msg_type],
                                                    static_cast<SPMod::BlockType>(params[arg_block_type]));

    return 1;
}

// native int GetMsgArgs();
static cell_t GetMsgArgs(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params [[maybe_unused]])
{
    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    return message->getParams();
}

// native MsgArgType GetMsgArgType(int param);
static cell_t GetMsgArgType(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_index = 1
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    return static_cast<cell_t>(message->getParamType(params[arg_index]));
}

// native int GetMsgArgInt(int param);
static cell_t GetMsgArgInt(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_index = 1
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    return static_cast<cell_t>(message->getParamInt(params[arg_index]));
}

// native float GetMsgArgFloat(int param);
static cell_t GetMsgArgFloat(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_index = 1
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    return static_cast<cell_t>(message->getParamFloat(params[arg_index]));
}

// native int GetMsgArgString(int param, char[] buffer, int size);
static cell_t GetMsgArgString(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_index = 1,
        arg_buffer,
        arg_size
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    char *buffer;
    ctx->LocalToString(params[arg_buffer], &buffer);

    return gSPGlobal->getUtils()->strCopy(buffer, params[arg_size], message->getParamString(params[arg_index]));
}

// native void SetMsgArgInt(int param, int value);
static cell_t SetMsgArgInt(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_index = 1,
        arg_value
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    message->setParamInt(params[arg_index], static_cast<int>(params[arg_value]));

    return 1;
}

// native void SetMsgArgFloat(int param, float value);
static cell_t SetMsgArgFloat(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_index = 1,
        arg_value
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    message->setParamFloat(params[arg_index], sp_ctof(params[arg_value]));

    return 1;
}

// native void SetMsgArgString(int param, char[] string);
static cell_t SetMsgArgString(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_index = 1,
        arg_string
    };

    SPMod::IMessageMngr *messageMngr = gSPGlobal->getMessageManager();

    if (!messageMngr->inHook())
    {
        ctx->ReportError("Invalid function call. Use this native in message hook handler.");
        return 0;
    }

    SPMod::IMessage *message = messageMngr->getMessage();

    if (params[arg_index] < 0 || static_cast<std::size_t>(params[arg_index]) >= message->getParams())
    {
        ctx->ReportError("Invalid argument number(%d).", params[arg_index]);
        return 0;
    }

    char *string;
    ctx->LocalToString(params[arg_string], &string);

    // TODO: check this
    message->setParamString(params[arg_index], string);

    return 1;
}

// native void MessageBegin(int dest, int msgid, const int origin[3] = {0,0,0}, int player = 0);
static cell_t MessageBegin(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_dest = 1,
        arg_msg_type,
        arg_origin,
        arg_player,
        arg_ignore_hooks
    };

    int numparam = *params;
    float vecOrigin[3];
    cell_t *cpOrigin;

    if (params[arg_msg_type] < 1 || ((params[arg_msg_type] > 63) // maximal number of engine messages
                                     && gSPGlobal->getMetaFuncs()->getUsrMsgName(params[2]).empty()))
    {
        ctx->ReportError("Plugin called MessageBegin with an invalid message id (%d).", params[arg_msg_type]);
        return 0;
    }

    ignoreHooks = static_cast<bool>(params[arg_ignore_hooks]);
    auto messageDest = static_cast<SPMod::MessageDest>(params[arg_dest]);

    switch (messageDest)
    {
        case SPMod::MessageDest::BROADCAST:
        case SPMod::MessageDest::ALL:
        case SPMod::MessageDest::SPEC:
        case SPMod::MessageDest::INIT:
            if (ignoreHooks)
                gSPGlobal->getEngineFuncs()->messageBegin(messageDest, params[arg_msg_type], nullptr);
            else
                gSPGlobal->getEngineHookedFuncs()->messageBegin(messageDest, params[arg_msg_type], nullptr);
            break;
        case SPMod::MessageDest::PVS:
        case SPMod::MessageDest::PAS:
        case SPMod::MessageDest::PVS_R:
        case SPMod::MessageDest::PAS_R:
            if (numparam < 3)
            {
                ctx->ReportError("Invalid number of parameters passed");
                return 0;
            }

            ctx->LocalToPhysAddr(params[arg_origin], &cpOrigin);

            vecOrigin[0] = sp_ctof(*cpOrigin);
            vecOrigin[1] = sp_ctof(*(cpOrigin + 1));
            vecOrigin[2] = sp_ctof(*(cpOrigin + 2));

            if (ignoreHooks)
                gSPGlobal->getEngineFuncs()->messageBegin(messageDest, params[arg_msg_type], vecOrigin);
            else
                gSPGlobal->getEngineHookedFuncs()->messageBegin(messageDest, params[arg_msg_type], vecOrigin);

            break;
        case SPMod::MessageDest::ONE_UNRELIABLE:
        case SPMod::MessageDest::ONE:
            if (numparam < 4)
            {
                ctx->ReportError("Invalid number of parameters passed");
                return 0;
            }

            SPMod::IPlayer *player = gSPGlobal->getPlayerManager()->getPlayer(params[arg_player]);

            if (ignoreHooks)
                gSPGlobal->getEngineFuncs()->messageBegin(messageDest, params[arg_msg_type], nullptr, player);
            else
                gSPGlobal->getEngineHookedFuncs()->messageBegin(messageDest, params[arg_msg_type], nullptr, player);

            break;
    }

    return 1;
}

// native void MessageEnd();
static cell_t MessageEnd(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params [[maybe_unused]])
{
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->messageEnd();
    else
        gSPGlobal->getEngineHookedFuncs()->messageEnd();

    return 1;
}

// native void WriteByte(int value);
static cell_t WriteByte(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeByte(params[arg_value]);
    else
        gSPGlobal->getEngineHookedFuncs()->writeByte(params[arg_value]);

    return 1;
}
// native void WriteChar(char value);
static cell_t WriteChar(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeChar(params[arg_value]);
    else
        gSPGlobal->getEngineHookedFuncs()->writeChar(params[arg_value]);

    return 1;
}
// native void WriteShort(int value);
static cell_t WriteShort(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeShort(params[arg_value]);
    else
        gSPGlobal->getEngineHookedFuncs()->writeShort(params[arg_value]);

    return 1;
}
// native void WriteLong(int value);
static cell_t WriteLong(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeLong(params[arg_value]);
    else
        gSPGlobal->getEngineHookedFuncs()->writeLong(params[arg_value]);

    return 1;
}
// native void WriteEntity(int value);
static cell_t WriteEntity(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeEntity(params[arg_value]);
    else
        gSPGlobal->getEngineHookedFuncs()->writeEntity(params[arg_value]);
    return 1;
}
// native void WriteAngle(float value);
static cell_t WriteAngle(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeAngle(sp_ctof(params[arg_value]));
    else
        gSPGlobal->getEngineHookedFuncs()->writeAngle(sp_ctof(params[arg_value]));
    return 1;
}
// native void WriteCoord(float value);
static cell_t WriteCoord(SourcePawn::IPluginContext *ctx [[maybe_unused]], const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeCoord(sp_ctof(params[arg_value]));
    else
        gSPGlobal->getEngineHookedFuncs()->writeCoord(sp_ctof(params[arg_value]));

    return 1;
}
// native void WriteString(char[] value);
static cell_t WriteString(SourcePawn::IPluginContext *ctx, const cell_t *params)
{
    enum
    {
        arg_value = 1
    };
    char *string;
    ctx->LocalToString(params[arg_value], &string);

    if (ignoreHooks)
        gSPGlobal->getEngineFuncs()->writeString(string);
    else
        gSPGlobal->getEngineHookedFuncs()->writeString(string);

    return 1;
}

sp_nativeinfo_t gMessageNatives[] = {{"GetUserMsgId", GetUserMsgId},
                                     {"GetUserMsgName", GetUserMsgName},

                                     {"HookMessage", HookMessage},
                                     {"UnhookMessage", UnhookMessage},

                                     {"GetMsgBlock", GetMsgBlock},
                                     {"SetMsgBlock", SetMsgBlock},

                                     {"GetMsgArgs", GetMsgArgs},
                                     {"GetMsgArgType", GetMsgArgType},
                                     {"GetMsgArgInt", GetMsgArgInt},
                                     {"GetMsgArgFloat", GetMsgArgFloat},
                                     {"GetMsgArgString", GetMsgArgString},
                                     {"SetMsgArgInt", SetMsgArgInt},
                                     {"SetMsgArgFloat", SetMsgArgFloat},
                                     {"SetMsgArgString", SetMsgArgString},

                                     {"MessageBegin", MessageBegin},
                                     {"MessageEnd", MessageEnd},
                                     {"WriteByte", WriteByte},
                                     {"WriteChar", WriteChar},
                                     {"WriteShort", WriteShort},
                                     {"WriteLong", WriteLong},
                                     {"WriteEntity", WriteEntity},
                                     {"WriteAngle", WriteAngle},
                                     {"WriteCoord", WriteCoord},
                                     {"WriteString", WriteString},

                                     {nullptr, nullptr}};
