#include "battle_data.h"
#include <fstream>

DataManager* DataManager::getInstance()
{
	static auto _pInstance = new DataManager;
	return _pInstance;
}

void DataManager::init()
{
	auto readJson = [](const char* path) -> Json::Value
	{
		string json,line;
		ifstream ifs(path);
		while(getline(ifs,line))
		{
			json.append(line);
		}
		Json::Value val;
		Json::Reader reader;
		if(false ==reader.parse(json, val))
		{
			return Json::Value::null;
		}
		return val;
	};
	{
		auto v = readJson("skill.txt");
		for (auto it = v.begin(); it != v.end(); ++it)
		{
			int skill_id = atoi(it.key().asCString());
			Skill skill(skill_id, *it);

			_skill_map[skill_id] = skill;
		}
	}
	{
		auto v = readJson("buff.txt");
		for (auto it = v.begin(); it != v.end(); ++it)
		{
			int buff_id = atoi(it.key().asCString());
			Buff buff(buff_id, *it);

			_buff_map[buff_id] = buff;
		}
	}
}

const Buff* DataManager::getBuff(int buff_id)
{
	auto it = _buff_map.find(buff_id);
	if (it == _buff_map.end())
		return nullptr;
	return &it->second;
}

Buff* DataManager::createBuff(int buff_id)
{
	auto it = _buff_map.find(buff_id);
	if (it == _buff_map.end())
		return nullptr;

	return new Buff(it->second);
}

Skill* DataManager::getSkill(int skill_id)
{
	auto it = _skill_map.find(skill_id);
	if (it == _skill_map.end())
		return nullptr;
	return &it->second;
}
