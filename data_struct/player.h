#ifndef player_h__
#define player_h__

#include "base.h"
#include "lua_helper.h"

#include "item.h"
#include "hero.h"
#include "weapon.h"
class Player :
	public LuaObj
{
	friend class CacheDataManager;
public:
	Player();
	~Player();

	typedef boost::shared_ptr<Player> Ptr;

	static Ptr loadSHM(int id);
	static Ptr loadDB(int id, int sid);	//根据id 加载玩家
	static Ptr create();	//创建新玩家

	int Id() const { return _id; }
	void Id(int val) { _id = val; }
	
	void Name(string name) { memcpy(_name, name.c_str(), name.length()); }
	string Name() const { return string(_name); }

	int Level() const { return _level; }
	void Level(int val) { _level = val; }

	int Exp() const { return _exp; }
	void Exp(int val) { _exp = val; }

	ItemBox* getItemBox() { return &_item_box;; }
	HeroBox* getHeroBox() { return &_hero_box; }
	WeaponBox* getWeaponBox() { return &_weapon_box; }
protected:
	bool dbLoad(int pid, int sid);
	bool dbWrite(int pid, int sid);

	bool __dbLoad(int pid, int sid);
	bool __dbWrite(int pid, int sid);

private:
	int _id;
	char _name[30];
	int _level;
	int _exp;

	ItemBox _item_box;
	HeroBox _hero_box;
	WeaponBox _weapon_box;
public:
	bool expire();
	void remove(int sid);
	void used();
protected:
	time_t __used_time;
	bool __offline;
protected:
	//luabind
	void __bindLua();
	auto_luabind_h(Player);
};

#endif // player_h__