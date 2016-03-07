#ifndef lua_helper_h__
#define lua_helper_h__

#include <type_traits>

#define EXPORT_TO_LUA
class LuaObj
{
private:
	virtual void __bindLua() = 0;
};

template<class T>
struct lua_helper
{
	static_assert(std::is_convertible<T*, LuaObj*>::value, "class T must inherit from abstract LuaObj");
	lua_helper(){ T* obj = new T; obj->__bindLua(); delete obj; }
};

#define auto_luabind_h(T) \
	friend struct lua_helper<T>; \
	static lua_helper<T> __lua_helper;

#define auto_luabind_cpp(T) \
struct lua_helper<T> T::__lua_helper;


extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#ifdef malloc
#undef malloc
#endif
#include "luabind/luabind.hpp"

#define LUA_STATE lua_helper<lua_State>::getInstance()->_luaState

template<>
struct lua_helper<lua_State>
{
public:
	lua_State* _luaState;

#ifdef WIN32
	friend struct lua_helper<lua_State>;

#endif // WIN32

	static lua_helper<lua_State>* getInstance() 
	{
		static auto _pInstance = new lua_helper<lua_State>;
		return _pInstance;
	}
private:
	lua_helper(){ init(); }
	virtual ~lua_helper(){ lua_close(_luaState); } 
	lua_helper(const lua_helper&){} 
	void operator = (const lua_helper&){}
	void init()
	{
		_luaState = luaL_newstate();
		luaL_openlibs(_luaState);
		luabind::open(_luaState);
	}
};

#endif // lua_helper_h__