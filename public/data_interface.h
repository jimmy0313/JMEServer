#ifndef data_interface_h__
#define data_interface_h__

#include "base.h"

template<class K, class V>
class DataManagerInterface
{
public:
	V addData(K key, V v)
	{
		auto it = _data_map.find(key);
		if (it != _data_map.end())
			return it->second;
		auto res = _data_map.insert(make_pair(key, v));
		return res.second ? res.first->second : V();
	}
	V getData(K key)
	{
		auto it = _data_map.find(key);
		if (it != _data_map.end())
			return it->second;
		return V();
	}
protected:
	map<K, V> _data_map;
};
#endif // data_interface_h__
