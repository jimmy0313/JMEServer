#ifndef battle_unit_h__
#define battle_unit_h__

#include <vector>
#include <functional>
#include <deque>

#ifdef WIN32
#include "json/json.h"
#else
#include "json.h"
#endif // WIN32

using namespace std;

class Battle;
class BattleEvent;

#include "skill.h"

enum BUFF_TYPE
{
	DEBUFF = 0,
	BENEFIT = 1,
};

enum BUFF_EFFECT
{
	PARRY = 1,		//格挡
	REBOUND = 2,	//反伤
	FROZEN = 3,		//冰冻
	STUN = 4,		//晕眩
	DAMAGE_TANK = 5,//坦伤
	INVINCIBLE = 6,	//无敌
	STUN_IMMUNE = 7, //免疫晕眩
	DEBUFF_IMMUNE = 8,	//免疫减益效果
	CURE_RESISTANCE = 9, //禁疗
};
struct Command
{
	int _skill_id;
	int _trigger_id;
};
//属性

struct AttributeBuff
{
	int _buff_id;
	float _time;
	float _value;

	AttributeBuff();
};
struct AttributeValue
{
	float _value;
	vector<AttributeBuff> _val_buff;

	float value() const;
	void value(float val);
	AttributeValue();

	virtual void update(float delta);
	void pushBuff(const AttributeBuff& buff);
};

enum ATTRIBUTE_TYPE
{
	HP = 1,
	MAX_HP/* = 2*/,
	SP/* = 3*/,
	ATTACK/* = 4*/,
	PHYS_DEFENSE/* = 5*/,
	MANA_DEFENSE/* = 6*/,
	PHYS_PENETRATION/* = 7*/,
	MANA_PENETRATION/* = 8*/,
	CRIT/* = 9*/,
	CRIT_DAMAGE/* = 10*/,
	HIT/* = 11*/,
	MISS/* = 12*/,
	ATTACK_SPEED/* = 13*/,
	ABSORB/* = 14*/,
	DAMAGE_REBOUND/* = 15*/,
	PHYS_DAMAGE_REDUCE/* = 16*/,
	MANA_DAMAGE_REDUCE/* = 17*/,

};
ATTRIBUTE_TYPE parseAttribute(const std::string& attr);
struct Attribute
{
	Attribute();
	Attribute(const Json::Value& val);

	float _hp;								//血量
	float _max_hp;							//最大血量
	int _sp;								//sp值
	AttributeValue _attack;					//攻击
	AttributeValue _phys_defense;			//物理防御
	AttributeValue _mana_defense;			//魔法防御
	AttributeValue _phys_penetration;		//物理穿透
	AttributeValue _mana_penetration;		//魔法穿透
	AttributeValue _crit;					//暴击率
	AttributeValue _crit_damage;			//暴击伤害
	AttributeValue _hit;					//命中
	AttributeValue _miss;					//闪避
	AttributeValue _attack_speed;			//攻速
	AttributeValue _absorb;					//吸血
	AttributeValue _rebound;				//反弹
	AttributeValue _phys_damage_reduce;		//物理减伤
	AttributeValue _mana_damage_reduce;		//魔法减伤

	AttributeValue* attributeValue(ATTRIBUTE_TYPE attr);
	float baseAttribute(ATTRIBUTE_TYPE attr) const;
	float attribute(ATTRIBUTE_TYPE attr) const;

	virtual void update(float delta);

	float hpPerc() const;
	bool dead() const;
};

//战斗中的基础单位
class BattleGroup;
class Buff;
class Warrior;

enum ACTION_STATE
{
	NORMAL = 1,
	USE_SKILL/* = 2*/,
	DEAD/* = 3*/,
	NEED_DELETE/* = 4*/,
};
class ActionUnit
{
public:
	friend class TriggerManager;
	friend class BattleGroup;
	enum UNIT_TYPE
	{
		WARRIOR = 1,		//勇士
		WARRIOR_COPY = 2,	//勇士分身
	};
public:
	ActionUnit();
	ActionUnit(const Json::Value& val);

	virtual ~ActionUnit();

	virtual void init();
	virtual void update(float delta);

	void useSkill(int skill_id, int trigger_id);
	void dispel(int num);
	void attack();
	void parry();

	virtual void damage(float damage){};
	virtual void cure(float damage){};

	void skillComplete();
	void setGroup(BattleGroup* group);

	bool isMe(ActionUnit* unit) const;
	bool isMe(int unit_id) const;
	bool isMates(ActionUnit* unit) const;
	bool isMates(int unit_id) const;
	bool isEnemy(ActionUnit* unit) const;
	bool isEnemy(int unit_id) const;

	bool isNeedDelete() const;

	ActionUnit* getEnemy(int nid);
	ActionUnit* getMate(int nid);
	void getEnemy(vector<ActionUnit*>& target_vec);
	void getUnit(vector<ActionUnit*>& target_vec);

	void clearCommand();
	void pushCommand(const Command& cmd);
	void exeCommand(const Command& cmd);
	
	virtual Warrior* father();
public:
	int _id;		//动态生成的nid
	ACTION_STATE _state;	//当前状态
	float _auto_attack;	//普攻计时

	std::string _name;
	UNIT_TYPE _unit_type;
	ActionUnit* _father;

	BattleGroup* _group;
	std::deque<Command> _command;
	std::vector<int> _skill_list;
};

//勇士
class Warrior :
	public Attribute,
	public ActionUnit
{
public:
	friend class BattleGroup;
public:
	Warrior(const Json::Value& val);
	virtual ~Warrior();

	virtual void update(float delta) override;
	virtual void damage(float damage) override;
	virtual void cure(float cure) override;

	void clearBuff();

	void pushBuff(Buff* buff);
	void removeTypeBuff(BUFF_TYPE type);
	void removeBuff(int nid);

	//检测目标身上是否有指定buff nid的buff， 由于该条件属于技能释放最后一个条件， 所以自动扣除buff层数(有层数的话)
	bool existBuff(int nid);

	//检测目标 是否有指定类型的buff（增益/减益）
	bool existBuff(BUFF_TYPE type);

	bool existEffectBuff(BUFF_EFFECT effect);
protected:
	vector<ActionUnit*> _children;	// 勇士 技能创建的 战斗单位，例如: 分身， 召唤兽等
	std::vector<Buff*> _buff_vec;
};

//勇士分身
class WarriorCopy :
	public Warrior
{
public:
	virtual void update(float delta) override;
	ActionUnit* _ower;	//召唤单位
	float _alive_time;
};

//弓箭
class BowArrow :
	public ActionUnit
{
public:

private:
	vector<ActionUnit*> _attacked;	//已攻击过的单位集合， 处于该集合中的单位不能再次受到自己的效果
};

//子弹
class GunArrow :
	public ActionUnit
{

};

class Totems :
	public ActionUnit
{

};

const static int BaseId_0 = 1;
const static int BaseId_1 = 10000000;
class BattleGroup
{
public:
	friend class ActionUnit;
	friend class TriggerManager;
public:
	BattleGroup(Battle* battle, int nid):
		_battle(battle),
		_base_id(nid)
	{

	}
	virtual ~BattleGroup();

	void update(float delta);

	void addUnit(ActionUnit* unit);

	ActionUnit* getUnit(int nid);
	ActionUnit* getEnemy(int nid);
	void getEnemy(vector<ActionUnit*>& target_vec);
	void getUnit(vector<ActionUnit*>& target_vec);

	ActionUnit* getHpLessMate();
	ActionUnit* getTankMate();

	void pushAddUnit(ActionUnit* unit);
	void delUnit(ActionUnit* unit);

	bool unitExist(ActionUnit* unit);
	bool unitExist(int nid);

	int createId();

	bool allDead();

	void print();
public:
	int _base_id;
	Battle* _battle;
	vector<ActionUnit*> _group_unit;
	vector<ActionUnit*> _add_unit;
	vector<ActionUnit*> _del_unit;
};


class Buff :
	public ActionUnit
{
public:
	friend struct AddBuffEffect;
	friend struct DelBuffEffect;
	friend struct Warrior;

	void update(float delta) override;

	Buff();
	Buff(int nid, const Json::Value& val);
	virtual ~Buff(){};

	void effectAction();

	void exec();
protected:
	int _id;
	BUFF_TYPE _type;
	BUFF_EFFECT _buff_effect;
	float _alive_time;		//buff持续时间, _alive_time < -99, buff is forever, _alive_time <= 0, need been removed
	int _times;				//buff层数, _times == -1, this buff don't have times limit, _times == 0, buff need been removed, _time > 0, normal
	bool _can_removed;		//是否可被移除
	float _effect_interval;	//效果生效间隔
	float _cur_interval;	//当前间隔
	ActionUnit* _appender;	//谁给我 加的buff？
	vector<EffectCommand*> _effect_command;
};

#endif // battle_unit_h__
