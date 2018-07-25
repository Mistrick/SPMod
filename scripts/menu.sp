#include <spmod>

public PluginInfo pluginInfo =
{
	name = "test",
    version = "0.0.0",
	author = "author",
	url = "https://github.com/Amaroq7/SPMod"
};

Menu m;

public void OnPluginInit()
{
	Command("say /m", MenuTest);

	m = Menu(TestHandler);
	m.SetTitle("Title");
	m.AddItem("one");
	m.AddItem("two");
	m.AddItem("three");
	m.AddItem("four");
	m.AddItem("5");
	m.AddItem("6");
	m.AddItem("7");
	m.AddItem("8");
	m.AddItem("9");
	m.AddItem("10");
	m.AddItem("11");
}

public PluginReturn MenuTest(int client, Command cid)
{
	m.Display(client);
}

public int TestHandler(Menu menu, MenuItem item, int player)
{
	PrintToServer("menu %d, item %d, player %d", menu, item, player);
	return 0;
}