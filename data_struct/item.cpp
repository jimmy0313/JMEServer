#include "item.h"
#include "json/json.h"
#include "db_manager.h"

auto_luabind_cpp(Item)
auto_luabind_cpp(ItemBox)

Item::Item() :
BoxObj(),
_num(0)
{

}

void Item::__bindLua()
{
	using namespace luabind;

	module(LUA_STATE)
	[
		class_<Item>("Item")
		.def_readwrite("id", &Item::_id)
		.def_readwrite("num", &Item::_num)
	];

	LOGT("Export class [Item] to lua");
}

ItemBox::ItemBox():
Box<Item, INIT_ITEM_NUM, MAX_ITEM_NUM>()
{

}

bool ItemBox::dbLoad(int pid, int sid)
{
	auto server = boost::lexical_cast<string>(sid);
	auto conn_server = string("server_") + server;
	auto dbpool = DBManager::getInstance()->mongoPool(conn_server.c_str());
	Json::Value res;
	if (nullptr == dbpool)
		return false;

	__TRY__

	MongoConnHelper conn(dbpool);
	auto dbname = conn_server + ".bag";
	conn->selectJson(dbname, BSON("_id"<<pid), &res);

	__CATCH_DB_EXCEPTION_ERROR__

	return false;

	__TRY_END__

	if (Json::Value::null == res)
		return true;

	_size = res["bag_size"].asInt();

	int item_num = res["item_list"].size();
	auto item_list_json = res["item_list"];
	for (int i = 0; i < item_num && i < MAX_ITEM_NUM; i++)
	{
		_box[i]._id = item_list_json[i]["id"].asInt();
		_box[i]._num = item_list_json[i]["num"].asInt();
	}
	return true;
}

bool ItemBox::dbWrite(int pid, int sid)
{
	//packet bag json
	Json::Value box_json;
	box_json["_id"] = pid;
	box_json["bag_size"] = _size;

	Json::Value item_json;
	for (int i = 0; i < _size && i < MAX_ITEM_NUM; i++)
	{
		auto& item = _box[i];
		if (0 == item._id)
			continue;

		item_json["id"] = item._id;
		item_json["num"] = item._num;
		box_json["item_list"].append(item_json);
	}
	
	//write to db
	{
		auto server = boost::lexical_cast<string>(sid);
		auto conn_server = string("server_") + server;
		auto dbpool = DBManager::getInstance()->mongoPool(conn_server.c_str());
		if (nullptr != dbpool)
		{
			__TRY__
			MongoConnHelper conn(dbpool);
			auto dbname = conn_server + ".bag";
			conn->updateJsonVal(dbname, BSON("_id"<<pid), item_json);

			__CATCH_DB_EXCEPTION_ERROR__

			__TRY_END__
		}
	}
	return true;
}

void ItemBox::__bindLua()
{
	using namespace luabind;
	Box<Item, INIT_ITEM_NUM, MAX_ITEM_NUM>::__bindLua();

	module(LUA_STATE)
	[
		class_<ItemBox, Box<Item, INIT_ITEM_NUM, MAX_ITEM_NUM> >("ItemBox")
		.enum_("constants")
		[
			value("INIT_ITEM_NUM", INIT_ITEM_NUM),
			value("MAX_ITEM_NUM", MAX_ITEM_NUM)
		]
	];

	LOGT("Export class [ItemBox] to lua");
}
