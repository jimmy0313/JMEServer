#ifndef box_h__
#define box_h__

#include "base.h"
#include "lua_helper.h"

struct BoxObj
{
	int _id;	//Ψһid
	bool enable(){ return 0 != _id; }
};

template<class T, int init_size, int max_size>
class Box :
	public LuaObj
{
	static_assert(std::is_convertible<T*, BoxObj*>::value, "class T must inherit from abstract BoxObj");
public:
	Box():
	_size(init_size)
	{};

	T* get(int index)
	{
		if (index < 0 || index >= _size)
			return nullptr;
		return &_box[index];
	}
	T* getNew()
	{
		for (int i = 0; i < _size && i < max_size; i++)
		{
			if (_box[i].enable())
				continue;
			return &_box[i];
		}
		return nullptr;
	}
protected:
	int _size;
	T _box[max_size];

	void __bindLua()
	{
		using namespace luabind;
		module(LUA_STATE)
		[
			class_<Box<T, init_size, max_size> >(typeid(Box<T, init_size, max_size>).name())
			.def("get", &Box<T, init_size, max_size>::get)
			.def("get_new", &Box<T, init_size, max_size>::getNew)
			.def_readwrite("size", &Box<T, init_size, max_size>::_size)
		];
	}
};
#endif // box_h__
