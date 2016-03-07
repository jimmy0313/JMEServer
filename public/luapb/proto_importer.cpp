
#include "proto_importer.h"
#include "log/JME_GLog.h"

class MyMultiFileErrorCollector : 
	public google::protobuf::compiler::MultiFileErrorCollector
{
	virtual void AddError(const std::string& filename, int line, int column, const std::string& message)
	{
		LOGE("%s:%d:%d:%s", filename, line, column, message);
	}
};

static MyMultiFileErrorCollector errorCollector;
static google::protobuf::compiler::DiskSourceTree sourceTree;

ProtoImporter::ProtoImporter():
	importer(&sourceTree, &errorCollector)
{
	char* protopath = getenv("PROTO_PATH");
	if (!protopath)
	{
		sourceTree.MapPath("", "");
	}
	else
	{
		sourceTree.MapPath("", protopath);
	}
}

bool ProtoImporter::import(const std::string& filename)
{
	const google::protobuf::FileDescriptor* filedescriptor = importer.Import(filename);
	if (!filedescriptor)
	{
		LOGE("import [ %s ] file descriptor error", filename);
		return false;
	}
	return true;
}

google::protobuf::Message* ProtoImporter::createDynamicMessage(const std::string& typeName)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor = importer.pool()->FindMessageTypeByName(typeName);
	if (descriptor)
	{
		const google::protobuf::Message* prototype = factory.GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return message;
}
