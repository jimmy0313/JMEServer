#ifndef battle_event_h__
#define battle_event_h__

#include <vector>
#include <string>

using namespace std;
class BattleEvent
{
public:
public:
	BattleEvent(const string& evt, int tar);

	bool operator < (const BattleEvent& evt) const;
	void push(){}

	template<typename T1, typename... T2>
	void push(T1 t1, T2... t2);

	int get(unsigned char index) const;
public:
	string _event;
	int _target_id;

	vector<int> _params;
};

template<typename T1, typename... T2>
void BattleEvent::push(T1 t1, T2... t2)
{
	_params.push_back(t1);
	push(t2...);
}

class ActionUnit;
struct Counter
{
	string _cmd;	//²Ù×÷
	ActionUnit* _father;	//¹éÊô
	float _counter;	//´ÎÊı

	void reset();
	void add(float v);
	void del(float v);
};

class CounterManager
{
public:
	Counter* get(ActionUnit* unit, const string& cmd);
	virtual ~CounterManager();
	void update(float delta);

	void removeUnitCounter(ActionUnit* unit);
private:
	std::vector<Counter*> _counter;
};

#endif // battle_event_h__
