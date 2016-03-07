#ifndef JME_SharedMemory_h__
#define JME_SharedMemory_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	通过共享内存创建对象， 仅支持固定大小的对象创建, 带有 虚函数/虚基类 的类无法使用
	warning:	一块共享内存 在 一个进程中仅能有一个映射， 具体 对象限制参见boost::interprocess
*********************************************************************/


#include <map>
#include <string>
#include "boost/interprocess/shared_memory_object.hpp"
#include "boost/interprocess/mapped_region.hpp"
#include "boost/thread/recursive_mutex.hpp"

#include "log/JME_GLog.h"

using namespace std;
namespace JMEngine
{
	namespace game
	{
		template<class T>
		class SharedMemory
		{
		public:
			typedef map<string, boost::interprocess::mapped_region*> MappedRegion;

			//主要用于cache, 用于负责cache的进程 创建共享内存 设置正确数据(如 从内存读取)
			static boost::shared_ptr<T> create(boost::interprocess::create_only_t, const string& name, boost::interprocess::mode_t mode);

			//主要用于logic，用于logic进程， 对数据进行修改
			static boost::shared_ptr<T> create(boost::interprocess::open_only_t, const string& name, boost::interprocess::mode_t mode);

			static bool existMappedRegion(const char* name);
		private:
			static void destory(string name, bool remove_memory);
			static void saveMappedRegion(const char* name, boost::interprocess::mapped_region* mmap);
			static void deleteMappedRegion(const char* name);
		private:
			static boost::recursive_mutex _mutex;
			static MappedRegion _mapped_region;
		};

		template<class T>
		typename SharedMemory<T>::MappedRegion SharedMemory<T>::_mapped_region;
		template<class T>
		typename boost::recursive_mutex SharedMemory<T>::_mutex;

		template<class T>
		bool JMEngine::game::SharedMemory<T>::existMappedRegion(const char* name)
		{
			boost::recursive_mutex::scoped_lock lock(_mutex);

			auto it = _mapped_region.find(name);
			if (it != _mapped_region.end())
				return true;
			return false;
		}

		template<class T>
		void JMEngine::game::SharedMemory<T>::deleteMappedRegion(const char* name)
		{
			boost::recursive_mutex::scoped_lock lock(_mutex);

			auto it = _mapped_region.find(name);
			if (it != _mapped_region.end())
			{
				auto mmap = it->second;
				delete mmap;

				_mapped_region.erase(name);
			}
		}

		template<class T>
		void JMEngine::game::SharedMemory<T>::saveMappedRegion(const char* name, boost::interprocess::mapped_region* mmap)
		{
			boost::recursive_mutex::scoped_lock lock(_mutex);

			auto result = _mapped_region.insert(make_pair(name, mmap));
		}

		template<class T>
		void JMEngine::game::SharedMemory<T>::destory(string name, bool remove_memory)
		{
			JMEngine::game::SharedMemory<T>::deleteMappedRegion(name.c_str());
			if (remove_memory)
			{
				boost::interprocess::shared_memory_object::remove(name.c_str());
			}
		}

		template<class T>
		boost::shared_ptr<T> JMEngine::game::SharedMemory<T>::create(boost::interprocess::create_only_t, const string& name, boost::interprocess::mode_t mode)
		{
			try
			{
				//移除可能存在老共享内存
				JMEngine::game::SharedMemory<T>::deleteMappedRegion(name.c_str());
				boost::interprocess::shared_memory_object::remove(name.c_str());

				//创建新的共享内存, 并分配大小
				boost::interprocess::shared_memory_object shm(boost::interprocess::create_only, name.c_str(), mode);
				shm.truncate(sizeof(T));

				//创建内存映射
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//保存内存映射信息
				JMEngine::game::SharedMemory<T>::saveMappedRegion(name.c_str(), mmap);

				//通过共享内存 创建shared_ptr<T>对象
				//共享内存， 谁创建， 谁销毁
				auto ptr = boost::shared_ptr<T>(new(mmap->get_address()) T, boost::bind(SharedMemory<T>::destory, name, true));
				return ptr;
			}
			catch(boost::interprocess::interprocess_exception& e)
			{
				LOGE("Create shared memory with name [ %s ] error ==> [ %s ]", name, e.what());

				return boost::shared_ptr<T>();
			}
		}

		template<class T>
		boost::shared_ptr<T> JMEngine::game::SharedMemory<T>::create(boost::interprocess::open_only_t, const string& name, boost::interprocess::mode_t mode)
		{
			try
			{
				//检测当前进程是否已经打开共享内存
				if (JMEngine::game::SharedMemory<T>::existMappedRegion(name.c_str()))
				{
					LOGE("The shared memory with name [ %s ] had been opened", name);
					return boost::shared_ptr<T>();
				}
				//打开共享内存
				boost::interprocess::shared_memory_object shm(boost::interprocess::open_only, name.c_str(), mode);

				//创建内存映射
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//如何共享内存分配内存大小 与 对象大小不一致， 对象被修改（大小一致 并不代表 对象没有被修改）
				if (sizeof(T) != mmap->get_size())
				{
					//移除内存映射
					delete mmap;
					return boost::shared_ptr<T>();
				}
				//保存内存映射
				JMEngine::game::SharedMemory<T>::saveMappedRegion(name.c_str(), mmap);
				
#ifdef EXPORT_TO_LUA
				//如果对象需要导入到lua中， 采用下面的写法，会导致错误, access violation no rtti data, 所以采用此种写法
				static T* temp_memory = new T;
				*temp_memory = *(T*)(mmap->get_address());
				auto ptr = boost::shared_ptr<T>(new(mmap->get_address()) T, boost::bind(SharedMemory<T>::destory, name, false));
				*ptr = *temp_memory;
#else
				//直接将共享内存 转为对象， 不调用构造函数， 避免构造函数修改内存数据
				auto ptr = boost::shared_ptr<T>((T*)mmap->get_address(), boost::bind(SharedMemory<T>::destory, name, false));
#endif // EXPORT_LUA

				return ptr;
			}
			catch(boost::interprocess::interprocess_exception& e)
			{
				LOGE("Open shared memory with name [ %s ] error ==> [ %s ]", name, e.what());

				return boost::shared_ptr<T>();
			}
		}

	}
}
#endif // JME_SharedMemory_h__
