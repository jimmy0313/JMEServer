#include "hero.h"
#include "db_manager.h"

auto_luabind_cpp(Hero)
auto_luabind_cpp(HeroBox)
Hero::Hero():
BoxObj(),
_template_id(0),
_level(1),
_exp(0),
_star(1),
_train_level(0),
_train_exp(0),
_weapon(0)
{

}

void Hero::__bindLua()
{
	using namespace luabind;

	module(LUA_STATE)
	[
		class_<Hero>("Hero")
		.def_readwrite("id", &Hero::_id)
		.def_readwrite("template_id", &Hero::_template_id)
		.def_readwrite("level", &Hero::_level)
		.def_readwrite("exp", &Hero::_exp)
		.def_readwrite("star", &Hero::_star)
		.def_readwrite("train_level", &Hero::_train_level)
		.def_readwrite("train_exp", &Hero::_train_exp)
		.def_readwrite("occup_skill", &Hero::_occup_skill)
		.def_readwrite("weapon", &Hero::_weapon)
	];

	LOGT("Export class [Hero] to lua");
}

HeroBox::HeroBox():
Box<Hero, INIT_HERO_NUM, MAX_HERO_NUM>()
{

}

int HeroBox::createNewId()
{
	auto dbpool = DBManager::getInstance()->mongoPool("world");
	if (nullptr != dbpool)
	{
		__TRY__

		MongoConnHelper conn(dbpool);
		int id = conn->createAutoIncId("server", "hero");
		return id;

		__CATCH_DB_EXCEPTION_ERROR__

		__TRY_END__
	}
	return 0;
}

bool HeroBox::dbLoad(int pid, int sid)
{
	return true;
}

bool HeroBox::dbWrite(int pid, int sid)
{
	return true;
}

void HeroBox::__bindLua()
{
	using namespace luabind;
	Box<Hero, INIT_HERO_NUM ,MAX_HERO_NUM>::__bindLua();

	module(LUA_STATE)
	[
		class_<HeroBox, Box<Hero, INIT_HERO_NUM ,MAX_HERO_NUM> >("HeroBox")
		.enum_("constants")
		[
			value("INIT_HERO_NUM", INIT_HERO_NUM),
			value("MAX_HERO_NUM", MAX_HERO_NUM)
		]
	];
	module(LUA_STATE, "HeroBox")
	[
		def("create_new_id", HeroBox::createNewId)
	];
	LOGT("Export class [HeroBox] to lua");
}
