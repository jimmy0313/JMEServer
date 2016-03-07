#include "account_handler.h"
#include "db_manager.h"
#include "util/JME_String.h"
#include "account_server.h"
#include "account.pb.h"
#include "server_rpc.pb.h"
#include "trace.h"
#include "boost/date_time/posix_time/ptime.hpp"
#include "game/JME_RPCManager.h"

auto_init_cpp(AccountHandler);
// std::map<int, LoginClient> AccountHandler::_logined_client_map;

const static int Msg_RequestLogin = 10002;

void AccountHandler::__init()
{
	JMEngine::rpc::RpcHandlerInterface::regRpcHandler("postMessage", boost::bind(&AccountHandler::postMessage, this, _1));

	AccountMessageDispatcher::regMessageHandler(Msg_RequestLogin, boost::bind(&AccountHandler::msg_10001, this, _1, _2));
}

void AccountHandler::msg_10001(google::protobuf::Message* rpc, const string& msg)
{
	rpc_post_message* p = static_cast<rpc_post_message*>(rpc);

	int account_id = 0;

	server_login_response response;
	response.set_state(server_login_response_LoginState_Unknown);

	__TRY__
		do
		{	
			client_request_login proto;
			proto.ParseFromString(msg);

			auto name = proto.name();
			auto pwd = proto.passwd();
			int server_id = proto.server_id();

			MysqlConnHelper mysqlconn(DBManager::getInstance()->mysqlPool("account"));

			auto sql = JMEngine::tools::createString("select account_id, password from account_db where username='%s'", name);
			if (!mysqlconn->query(sql.c_str()))
				break;

			if( mysqlconn->isResEmpty() ) // No username found
			{
				response.set_state(server_login_response_LoginState_NotFound);
				break;
			}
			MYSQL_RES* res = mysqlconn->getRes();
			MYSQL_ROW row = mysql_fetch_row( res );

			if( !row )
				break;

			string user_pwd = row[1];
			if ( user_pwd.compare(pwd) )
			{
				response.set_state(server_login_response_LoginState_PasswdError);
				break;
			}
			response.set_state(server_login_response_LoginState_Succeed);

			account_id = boost::lexical_cast<int>(row[0]);

			{
				//repeated login check
				auto logined_client = lastLoginClient(account_id, server_id);
				if (nullptr != logined_client)
				{
					//repeated login
					kickoutClient(logined_client);
				}
				clientLogin(account_id, p->gate(), p->net_id(), server_id);
			}
			{
				MongoConnHelper mongoconn(DBManager::getInstance()->mongoPool("world"));

				static string dbname = "server.account_role";

				auto createAuthKey = []()
				{
					boost::system_time curTime = get_system_time();
					boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970,1,1)); 
					boost::posix_time::time_duration diff = curTime - time_t_epoch;

					auto time_string = boost::lexical_cast<string>(diff.total_milliseconds());
					int len = time_string.length() * 2;

					string key;
					key.resize(len);

					auto seed = diff.total_milliseconds();
					srand(seed);
					for( int i = 0; i < len; i += 2 )
					{
						seed = rand();
						*((char*)key.data() + i) = 'A'+ rand() % 26;
						*((char*)key.data() + i + 1) = time_string[i/2];
						srand(seed);
					}

					return key;
				};

				auto key = createAuthKey();
				response.set_auth_key(key);

				mongoconn->updateBsonVal(dbname, BSON("account_id"<<account_id<<"server_id"<<server_id), BSON("account_id"<<account_id<<"server_id"<<server_id<<"auto_key"<<key));
			}
		}while(0);

		__CATCH_DB_EXCEPTION_ERROR__
		__CATCH_STD_EXCEPTION_ERROR__
		__TRY_END__

		__TRY__

		rpc_send_client res;
		res.set_msg_id(Msg_RequestLogin);
		res.set_net_id(p->net_id());
		res.set_params(response.SerializeAsString());

		GETRPC(p->gate())->callRpcMethod("sendClient", &res);

		__CATCH_RPC_EXCEPTION_ERROR__
		__TRY_END__
}

google::protobuf::Message* AccountHandler::postMessage(const string& params)
{
	__TRY__

	rpc_post_message proto;
	proto.ParseFromString(params);

	DEFAULTMSGLOG

	AccountMessageDispatcher::execMessageHandler(proto.msg_id(), &proto, proto.params());

	__CATCH_STD_EXCEPTION_ERROR__

	__TRY_END__

	return nullptr;
}

LoginClient* AccountHandler::lastLoginClient(int account_id, int server_id)
{
	LoginClientKey key;
	key._account_id = account_id;
	key._server_id = server_id;
	auto it = _logined_client_map.find(key);
	if (it == _logined_client_map.end())
		return nullptr;
	return &it->second;
}

void AccountHandler::clientLogin(int account_id, const string& gate, int net_id, int server_id)
{
	LoginClientKey key;
	key._account_id = account_id;
	key._server_id = server_id;

	LoginClient client(account_id, gate, net_id, server_id);
	_logined_client_map[key] = client;
}

void AccountHandler::kickoutClient(const LoginClient* client)
{
	rpc_kickout_client rpc;
	rpc.set_net_id(client->_net_id);
	rpc.set_reason(rpc_kickout_client_Reason_RepeatedLogin);

	GETRPC(client->_gate)->callRpcMethod("kickout", &rpc);
}

bool LoginClientKey::operator<(const LoginClientKey& key) const
{
	if (_account_id < key._account_id)
		return true;
	if (_account_id > key._account_id)
		return false;

	if (_server_id < key._server_id)
		return true;
	if (_server_id > key._server_id)
		return false;

	return false;
}
