#ifndef proto_importer_h__
#define proto_importer_h__

#include "boost/thread/detail/singleton.hpp"

#include <string>
#include <map>

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

class ProtoImporter
{
public:
	ProtoImporter();
public:
	bool import(const std::string& filename);
	google::protobuf::Message* createDynamicMessage(const std::string& typeName);
public:
	google::protobuf::compiler::Importer importer;
	google::protobuf::DynamicMessageFactory factory;
};

static std::map<google::protobuf::Message*, bool> newMap;

#define Proto boost::detail::thread::singleton<ProtoImporter>::instance()

#endif // proto_importer_h__
