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
	PARRY = 1,		//��
	REBOUND = 2,	//����
	FROZEN = 3,		//����
	STUN = 4,		//��ѣ
	DAMAGE_TANK = 5,//̹��
	INVINCIBLE = 6,	//�޵�
	STUN_IMMUNE = 7, //������ѣ
	DEBUFF_IMMUNE = 8,	//���߼���Ч��
	CURE_RESISTANCE = 9, //����
};
struct Command
{
	int _skill_id;
	int _trigger_id;
};
//����

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

	float _hp;								//Ѫ��
	float _max_hp;							//���Ѫ��
	int _sp;								//spֵ
	AttributeValue _attack;					//����
	AttributeValue _phys_defense;			//�������
	AttributeValue _mana_defense;			//ħ������
	AttributeValue _phys_penetration;		//����͸
	AttributeValue _mana_penetration;		//ħ����͸
	AttributeValue _crit;					//������
	AttributeValue _crit_damage;			//�����˺�
	AttributeValue _hit;					//����
	AttributeValue _miss;					//����
	AttributeValue _attack_speed;			//����
	AttributeValue _absorb;					//��Ѫ
	AttributeValue _rebound;				//����
	AttributeValue _phys_damage_reduce;		//�������
	AttributeValue _mana_damage_reduce;		//ħ������

	AttributeValue* attributeValue(ATTRIBUTE_TYPE attr);
	float baseAttribute(ATTRIBUTE_TYPE attr) const;
	float attribute(ATTRIBUTE_TYPE attr) const;

	virtual void update(float delta);

	float hpPerc() const;
	bool dead() const;
};

//ս���еĻ�����λ
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
		WARRIOR = 1,		//��ʿ
		WARRIOR_COPY = 2,	//��ʿ����
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
	int _id;		//��̬���ɵ�nid
	ACTION_STATE _state;	//��ǰ״̬
	float _auto_attack;	//�չ���ʱ

	std::string _name;
	UNIT_TYPE _unit_type;
	ActionUnit* _father;

	BattleGroup* _group;
	std::deque<Command> _command;
	std::vector<int> _skill_list;
};

//��ʿ
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

	//���Ŀ�������Ƿ���ָ��buff nid��buff�� ���ڸ��������ڼ����ͷ����һ�������� �����Զ��۳�buff����(�в����Ļ�)
	bool existBuff(int nid);

	//���Ŀ�� �Ƿ���ָ�����͵�buff������/���棩
	bool existBuff(BUFF_TYPE type);

	bool existEffectBuff(BUFF_EFFECT effect);
protected:
	vector<ActionUnit*> _children;	// ��ʿ ���ܴ����� ս����λ������: ���� �ٻ��޵�
	std::vector<Buff*> _buff_vec;
};

//��ʿ����
class WarriorCopy :
	public Warrior
{
public:
	virtual void update(float delta) override;
	ActionUnit* _ower;	//�ٻ���λ
	float _alive_time;
};

//����
class BowArrow :
	public ActionUnit
{
public:

private:
	vector<ActionUnit*> _attacked;	//�ѹ������ĵ�λ���ϣ� ���ڸü����еĵ�λ�����ٴ��ܵ��Լ���Ч��
};

//�ӵ�
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
	float _alive_time;		//buff����ʱ��, _alive_time < -99, buff is forever, _alive_time <= 0, need been removed
	int _times;				//buff����, _times == -1, this buff don't have times limit, _times == 0, buff need been removed, _time > 0, normal
	bool _can_removed;		//�Ƿ�ɱ��Ƴ�
	float _effect_interval;	//Ч����Ч���
	float _cur_interval;	//��ǰ���
	ActionUnit* _appender;	//˭���� �ӵ�buff��
	vector<EffectCommand*> _effect_command;
};

#endif // battle_unit_h__
