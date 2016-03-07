#JMEServer
开发环境选择<br>
windows: vs2012以上(vs2012为支持可变模板参数,编译器需要使用vc_compilerCTPNov2012,external提供下载)<br>
linux: gcc 4.8.1以上<br>


注意:<br>
如果使用auto_luabind宏 “自动”调用__bindLua函数绑定class到lua, 请使用third_party\lua\luabind替换自己的luabind头文件<br>

third_party 中的第三方编译环境均为vs2012<br>

服务器架构， 数据流向请参考external中的.vsd文件<br>

debug 请修改各种project下面Debug/bin/config/conf.json 配置正确的服务器端口 与 数据库信息<br>

服务器玩家数据全部在共享内存中， 避免了多服务器间的数据拷贝，同时也限制了服务器必须部署在同一物理机上


