#ifndef bag_h__
#define bag_h__

#include "base.h"
#include "lua_helper.h"
#include "box.h"

struct Item :
	public LuaObj,
	public BoxObj
{
	int _num;
	Item();
protected:
	void __bindLua();
	auto_luabind_h(Item);
};

enum{INIT_ITEM_NUM = 50, MAX_ITEM_NUM = 100};
class ItemBox :
	public Box<Item, INIT_ITEM_NUM, MAX_ITEM_NUM>
{
	friend class Player;
public:
	ItemBox();
protected:
	bool dbLoad(int pid, int sid);
	bool dbWrite(int pid, int sid);

protected:
	void __bindLua();
	auto_luabind_h(ItemBox);
};
#endif // bag_h__
