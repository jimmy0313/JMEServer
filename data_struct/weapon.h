#ifndef weapon_h__
#define weapon_h__

#include "base.h"
#include "lua_helper.h"
#include "box.h"

struct WeaponAttribute :
	public LuaObj
{
	enum ADDITION_TYPE
	{
		ADDITION_NULL = 0,
		ADDITION_A = 1,
		ADDITION_D = 2,
	};
	enum ATTR_TYPE
	{
		ATTR_ATTACk = 1,
		ATTR_PHYS_PENETRATION = 2,
		ATTR_MANA_PENETRATION = 3,
		ATTR_CRIT = 4,
		ATTR_CRIT_DAMAGE = 5,
		ATTR_ATTACK_SPEED = 6,

		ATTR_HP = 7,
		ATTR_PHYS_DEFENSE = 8,
		ATTR_MANA_DEFENSE = 9,
		ATTR_DAMAGE_REDUCE = 10,
		ATTR_ABSORB = 11,
	};
	ADDITION_TYPE _addition_type;
	ATTR_TYPE _attr_type;
	int _index;
	bool _is_max;

	WeaponAttribute();
protected:
	void __bindLua();
	auto_luabind_h(WeaponAttribute)
};
struct Weapon :
	public LuaObj,
	public BoxObj
{
	enum{MAX_ATTR_NUM = 3,};

	int _template_id;
	int _attr_size;
	WeaponAttribute _attribute[MAX_ATTR_NUM];

	Weapon();

	WeaponAttribute* getAttribute(int index);
protected:
	void __bindLua();
	auto_luabind_h(Weapon)
};

enum{INIT_WEAPON_NUM = 10, MAX_WEAPON_NUM = 200,};
class WeaponBox :
	public Box<Weapon, INIT_WEAPON_NUM, MAX_WEAPON_NUM>
{
	friend class Player;
public:
	WeaponBox();

	static int createNewId();
protected:
	bool dbLoad(int pid, int sid);
	bool dbWrite(int pid, int sid);
protected:
	void __bindLua();
	auto_luabind_h(WeaponBox)
};

#endif // weapon_h__
