
#ifndef __LUA_CJSON_EXTRA_H_
#define __LUA_CJSON_EXTRA_H_

#if defined(_USRDLL)
    #define LUA_CJSON_EXTENSIONS_DLL     __declspec(dllexport)
#else         /* use a DLL library */
    #define LUA_CJSON_EXTENSIONS_DLL
#endif

#if __cplusplus
extern "C" {
#endif

#include "lauxlib.h"

void LUA_CJSON_EXTENSIONS_DLL luaopen_lua_cjson_extensions(lua_State *L);
    
#if __cplusplus
}
#endif

#endif /* __LUA_CJSON_EXTRA_H_ */
