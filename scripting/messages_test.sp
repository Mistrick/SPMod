#include <spmod>

public PluginInfo pluginInfo =
{
	name = "Messages Test",
	version = "0.0.0",
	author = "author",
	url = "https://github.com/Amaroq7/SPMod"
};

public void OnPluginInit()
{
	Command("^say /mblock$", MsgTest);
	HookMessage(GetUserMsgId("SayText"), MessageHandler, false);
	HookMessage(GetUserMsgId("SayText"), MessageHandler, true);
}
public PluginReturn MsgTest(int client, Command cid)
{
	SetMsgBlock(76, MessageBlock_Once);
}
public PluginReturn MessageHandler(MessageDest dest, int type, int receiver)
{
	PrintToServer("MsgHook: dest %d, %d type, receiver %d", dest, type, receiver);
	
	int params = GetMsgArgs();
	
	for(int i = 0; i < params; i++)
	{
		switch(GetMsgArgType(i))
		{
			case MsgArg_Byte, MsgArg_Char, MsgArg_Short, MsgArg_Long, MsgArg_Entity:
			{
				PrintToServer("param %d - %d", i, GetMsgArgInt(i));
			}
			case MsgArg_Angle, MsgArg_Coord:
			{
				PrintToServer("param %d - %f", i, GetMsgArgFloat(i));
			}
			case MsgArg_String:
			{
				char buffer[256];
				GetMsgArgString(i, buffer, sizeof buffer);
				PrintToServer("param %d - %s", i, buffer);
				if( i == 3) SetMsgArgString(i, "changed");
			}
		}
	}
}
