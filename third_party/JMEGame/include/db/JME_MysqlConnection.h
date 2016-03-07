#define _CRT_SECURE_NO_WARNINGS

#ifndef JME_MysqlConnection_h__
#define JME_MysqlConnection_h__

#include <string>
#include "db/JME_DBPool.h"

#ifdef WIN32
	#include "winsock2.h"
	#pragma comment(lib,"ws2_32")
#endif

#ifdef WIN32
	#include "mysql.h"
	#include "errmsg.h"
#else
	#include <mysql.h>
	#include <errmsg.h>
#endif

using namespace std;

namespace JMEngine
{
	namespace db
	{
		class MysqlConnector
		{
		public:

		public:
			MysqlConnector();
			~MysqlConnector();

			bool connect();
			bool lockTable( const char* table, const char* priority );
			bool unlockTable();
			bool query( const char *sql );
			bool isResEmpty();
			bool connected();

			MYSQL_RES* genRes();
			MYSQL* get_mysql();
			MYSQL_RES* getRes() { return m_queryRes; }

		public:
			void setConfig(const string& ip, const string& user, const string& pwd, const string& db, const int port);

		private:
			MYSQL m_mysql;
			MYSQL_ROW m_row;
			MYSQL_RES *m_queryRes;
			MYSQL_FIELD *m_field;

			string _host;
			string _user;
			string _passwd;
			string _db;
			unsigned short _port;
		};
	}
}
#endif // JME_MysqlConnection_h__