#include <spmod>

public PluginInfo pluginInfo =
{
	name = "test",
    version = "0.0.0",
	author = "author",
	url = "https://github.com/Amaroq7/SPMod"
};

Menu m, m2, m3;
bool hide = false;

public void OnPluginInit()
{
	Command("^say /m", MenuTest);
	Command("^say /m2", MenuTest2);
	Command("^say /m3", MenuTest5);
	Command("^say /cm", MenuTest3);
	Command("^say /h", MenuTest4);

	m = Menu(TestHandler, true);
	m.SetTitle("Title");
	m.AddItem("one");
	m.AddItem("two");
	m.AddItem("three");
	m.AddItem("four", 0, ItemHandler);
	m.AddItem("5");
	m.AddItem("6");
	m.AddItem("7");
	m.AddItem("8");
	m.AddItem("9", 0, ItemHandler);
	m.AddItem("10");
	m.AddItem("11");
	
	m2 = Menu(TestHandler, true);
	
	m2.SetTitle("Test 2");
	m2.AddItem("one", 1, ItemHandler3);
	m2.AddItem("two", 2, ItemHandler3);
	m2.AddItem("three", 3, ItemHandler3);
	m2.AddItem("four", 4);
	m2.AddItem("5", 5, ItemHandler3);
	m2.AddItem("6", 6, ItemHandler3);
	m2.AddItem("7", 7, ItemHandler3);
	m2.AddItem("8", 9);
	m2.AddItem("9", 10);

	m2.SetProp(MProp_NumberFormat, "\\r[#num]");

	m3 = Menu(TestHandler, true);
	
	m3.SetTitle("Test 3");
	m3.AddItem("one");
	m3.AddItem("two");
	m3.AddItem("three");
	m3.AddItem("four");
	m3.AddItem("5", 0, ItemHandler3);
	m3.AddItem("6", 3, ItemHandler3);
	m3.AddItem("7");

	m3.AddStaticItem(0, "static1", 23, ItemHandler2);
	m3.AddStaticItem(5, "static2", 23, ItemHandler2);

	m3.ItemsPerPage = 5;
} 

public ItemStatus ItemHandler(Menu menu, MenuItem item, int player)
{
	PrintToServer("Item callback: menu %d, item %d, player %d", menu, item, player);
	item.SetName("changed");
	return ItemDisabled;
}

public ItemStatus ItemHandler2(Menu menu, MenuItem item, int player)
{
	char name[64];
	item.GetName(name, sizeof name);
	PrintToServer("Item callback2: menu %d, item %s[%d], player %d", menu, name, item, player);
	
	return ItemEnabled;
}
public ItemStatus ItemHandler3(Menu menu, MenuItem item, int player)
{
	PrintToServer("Item callback3: menu %d, item %d, player %d", menu, item, player);
	return hide ? ItemHide : ItemEnabled;
}

public PluginReturn MenuTest(int client, Command cid)
{
	m.Display(client, 0, 10);
}
public PluginReturn MenuTest2(int client, Command cid)
{
	m2.Display(client);
}
public PluginReturn MenuTest5(int client, Command cid)
{
	m3.Display(client);
}

public PluginReturn MenuTest3(int client, Command cid)
{
	CloseMenu(client);
}

public PluginReturn MenuTest4(int client, Command cid)
{
	hide = !hide;
}

public int TestHandler(Menu menu, MenuItem item, int player)
{
	PrintToServer("menu %d, item %d, item data %d, player %d", menu, item, item.GetData(), player);
	return 0;
}