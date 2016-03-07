#ifndef JME_MongoConnection_h__
#define JME_MongoConnection_h__

#include "client/dbclient.h"
#include "json/json.h"

#include <string>

using namespace std;
namespace JMEngine
{
	namespace db
	{
		class MongoConnection
		{
		public:

			//************************************
			// Method:    connect
			// FullName:  JMEngine::db::MongoConnection::connect
			// Access:    public 
			// Returns:   bool, succeed ==> true, failed ==> false
			// Qualifier:
			// Parameter: const string & addr
			// Parameter: string * err
			//************************************
			bool connect(const string& addr, string* err);

			//************************************
			// Method:    auth
			// FullName:  JMEngine::db::MongoConnection::auth
			// Access:    public 
			// Returns:   bool, succeed ==> true, failed ==> false
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const string & userName
			// Parameter: const string & pwd
			// Parameter: string * err
			// Parameter: bool digestPassword
			//************************************
			bool auth(const string& dbName, const string& userName, const string& pwd, string* err, bool digestPassword = true);

			//************************************
			// Method:    selectJson
			// FullName:  JMEngine::db::MongoConnection::selectJson
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const mongo::BSONObj & key
			// Parameter: Json::Value * res
			// Parameter: const mongo::BSONObj * fileds
			//************************************
			void selectJson(const string& dbName, const mongo::BSONObj& key, Json::Value* res, const mongo::BSONObj* fileds = NULL);

			//************************************
			// Method:    selectMoreJson
			// FullName:  JMEngine::db::MongoConnection::selectMoreJson
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const mongo::BSONObj & key
			// Parameter: Json::Value * res
			//************************************
			void selectMoreJson(const string& dbName, const mongo::BSONObj& key, Json::Value* res);

			//************************************
			// Method:    insertJsonObj
			// FullName:  JMEngine::db::MongoConnection::insertJsonObj
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const Json::Value & val
			//************************************
			void insertJsonObj(const string& dbName, const Json::Value& val);

			//************************************
			// Method:    insertBsonObj
			// FullName:  JMEngine::db::MongoConnection::insertBsonObj
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const mongo::BSONObj & obj
			//************************************
			void insertBsonObj(const string& dbName, const mongo::BSONObj& obj);

			//************************************
			// Method:    updateJsonVal
			// FullName:  JMEngine::db::MongoConnection::updateJsonVal
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const mongo::BSONObj & key
			// Parameter: const Json::Value & val
			// Parameter: bool upsert
			// Parameter: bool multi
			//************************************
			void updateJsonVal(const string& dbName, const mongo::BSONObj& key, const Json::Value& val, bool upsert = true, bool multi = false);

			//************************************
			// Method:    updateBsonVal
			// FullName:  JMEngine::db::MongoConnection::updateBsonVal
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const mongo::BSONObj & key
			// Parameter: const mongo::BSONObj & val
			// Parameter: bool upsert
			// Parameter: bool multi
			//************************************
			void updateBsonVal(const string& dbName, const mongo::BSONObj& key, const mongo::BSONObj& val, bool upsert = true, bool multi = false);
			
			//************************************
			// Method:    removeBson
			// FullName:  JMEngine::db::MongoConnection::removeBson
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const mongo::BSONObj & key
			// Parameter: bool justOne
			//************************************
			void removeBson(const string& dbName, const mongo::BSONObj& key, bool justOne = true);
			
			//************************************
			// Method:    createAutoIncId
			// FullName:  JMEngine::db::MongoConnection::createAutoIncId
			// Access:    public 
			// Returns:   int, 0 ==> error
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const string & key
			// Parameter: int init
			//************************************
			int createAutoIncId(const string& dbName, const string& key, int init = 1);

			//************************************
			// Method:    findAndModify
			// FullName:  JMEngine::db::MongoConnection::findAndModify
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & dbName
			// Parameter: const string & table
			// Parameter: const mongo::BSONObj & key
			// Parameter: const mongo::BSONObj & query
			// Parameter: bool upsert
			//************************************
			void findAndModify(const string& dbName, const string& table, const mongo::BSONObj& key, const mongo::BSONObj& query, bool upsert = true);

			//************************************
			// Method:    dropCollection
			// FullName:  JMEngine::db::MongoConnection::dropCollection
			// Access:    public 
			// Returns:   void
			// Qualifier:
			// Parameter: const string & collection
			//************************************
			void dropCollection(const string& collection);
		private:
			mongo::DBClientConnection _conn;
		};
	}
}
#endif // JME_MongoConnection_h__
