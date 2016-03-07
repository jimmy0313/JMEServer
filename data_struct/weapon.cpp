#include "weapon.h"
#include "db_manager.h"

auto_luabind_cpp(WeaponAttribute)
auto_luabind_cpp(Weapon)
auto_luabind_cpp(WeaponBox)

WeaponAttribute::WeaponAttribute():
_addition_type(ADDITION_NULL),
_attr_type(ATTR_ATTACk),
_index(0),
_is_max(false)
{

}

void WeaponAttribute::__bindLua()
{
	using namespace luabind;

	module(LUA_STATE)
	[
		class_<WeaponAttribute>("WeaponAttribute")
		.def_readwrite("addition_type", &WeaponAttribute::_addition_type)
		.def_readwrite("attr_type", &WeaponAttribute::_attr_type)
		.def_readwrite("index", &WeaponAttribute::_index)
		.def_readwrite("is_max", &WeaponAttribute::_is_max)
		.enum_("constants")
		[
			value("ADDITION_NULL", ADDITION_NULL),
			value("ADDITION_A", ADDITION_A),
			value("ADDITION_D", ADDITION_D),

			value("ATTR_ATTACk", ATTR_ATTACk),
			value("ATTR_PHYS_PENETRATION", ATTR_PHYS_PENETRATION),
			value("ATTR_MANA_PENETRATION", ATTR_MANA_PENETRATION),
			value("ATTR_CRIT", ATTR_CRIT),
			value("ATTR_CRIT_DAMAGE", ATTR_CRIT_DAMAGE),
			value("ATTR_ATTACK_SPEED", ATTR_ATTACK_SPEED),
			value("ATTR_HP", ATTR_HP),
			value("ATTR_PHYS_DEFENSE", ATTR_PHYS_DEFENSE),
			value("ATTR_MANA_DEFENSE", ATTR_MANA_DEFENSE),
			value("ATTR_DAMAGE_REDUCE", ATTR_DAMAGE_REDUCE),
			value("ATTR_ABSORB", ATTR_ABSORB)
		]
	];

	LOGT("Export class [WeaponAttribute] to lua");
}

Weapon::Weapon():
BoxObj(),
_template_id(0),
_attr_size(1)
{

}

WeaponAttribute* Weapon::getAttribute(int index)
{
	if (index < 0 || index >= _attr_size)
		return nullptr;
	return &_attribute[index];
}

void Weapon::__bindLua()
{
	using namespace luabind;

	module(LUA_STATE)
	[
		class_<Weapon>("Weapon")
		.def_readwrite("id", &Weapon::_id)
		.def_readwrite("template_id", &Weapon::_template_id)
		.def_readwrite("attr_size", &Weapon::_attr_size)
		.def("get_attribute", &Weapon::getAttribute)
	];

	LOGT("Export class [Weapon] to lua");
}

WeaponBox::WeaponBox():
Box<Weapon, INIT_WEAPON_NUM, MAX_WEAPON_NUM>()
{

}

int WeaponBox::createNewId()
{
	auto dbpool = DBManager::getInstance()->mongoPool("world");
	if (nullptr != dbpool)
	{
		__TRY__

		MongoConnHelper conn(dbpool);
		int id = conn->createAutoIncId("server", "weapon");
		return id;

		__CATCH_DB_EXCEPTION_ERROR__

		__TRY_END__
	}
	return 0;
}

bool WeaponBox::dbLoad(int pid, int sid)
{
	return true;
}

bool WeaponBox::dbWrite(int pid, int sid)
{
	return true;
}

void WeaponBox::__bindLua()
{
	using namespace luabind;
	Box<Weapon, INIT_WEAPON_NUM, MAX_WEAPON_NUM>::__bindLua();

	module(LUA_STATE)
	[
		class_<WeaponBox, Box<Weapon, INIT_WEAPON_NUM, MAX_WEAPON_NUM> >("WeaponBox")
		.enum_("constants")
		[
			value("INIT_WEAPON_NUM", INIT_WEAPON_NUM),
			value("MAX_WEAPON_NUM", MAX_WEAPON_NUM)
		]
	];

	module(LUA_STATE, "WeaponBox")
	[
		def("create_new_id", &WeaponBox::createNewId)
	];

	LOGT("Export class [WeaponBox] to lua");
}
