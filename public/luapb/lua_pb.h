#ifndef lua_pb_h__
#define lua_pb_h__

#include "lua.hpp"
#include <google/protobuf/message.h>

typedef struct tagluamsg
{
	google::protobuf::Message* msg;
	bool isDelete; //是否要释放掉这个msg;
}lua_pbmsg;

typedef struct tagluarepeatedmsg
{
	google::protobuf::Message* msg;
	google::protobuf::FieldDescriptor* field;
}lua_repeated_msg;

#define PB_MESSAGE "pb"
#define PB_MESSAGE_META "pb_meta"

#define PB_REPEATED_MESSAGE_META "pb_repeated_meta"

extern "C" int luaopen_luapb(lua_State* L);

#endif // lua_pb_h__
