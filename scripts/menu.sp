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
}

public PluginReturn MenuTest(int client, Command cid)
{
	m.Display(client);
}

public int TestHandler(Menu menu, MenuItem item, int player)
{
	return 0;
}