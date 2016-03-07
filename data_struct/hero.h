#ifndef hero_h__
#define hero_h__

#include "base.h"
#include "lua_helper.h"
#include "box.h"

struct Hero :
	public LuaObj,
	public BoxObj
{
	int _template_id;	//模板id
	int _level;
	int _exp;
	int _star;	//星级
	int _train_level;	//训练等级
	int _train_exp;		//训练经验
	int _occup_skill;	//职业技能

	int _weapon;		//武器id

	Hero();
protected:
	void __bindLua();
	auto_luabind_h(Hero)
};

enum{INIT_HERO_NUM = 10, MAX_HERO_NUM = 200,};
class HeroBox :
	public Box<Hero, INIT_HERO_NUM, MAX_HERO_NUM>
{
	friend class Player;
public:
	HeroBox();

	static int createNewId();
protected:
	bool dbLoad(int pid, int sid);
	bool dbWrite(int pid, int sid);
protected:
	void __bindLua();
	auto_luabind_h(HeroBox)
};
#endif // hero_h__
