#ifndef JME_SharedMemory_h__
#define JME_SharedMemory_h__

/********************************************************************
	created:	2015/06/19
	author:		huangzhi
	
	purpose:	ͨ�������ڴ洴������ ��֧�̶ֹ���С�Ķ��󴴽�, ���� �麯��/����� �����޷�ʹ��
	warning:	һ�鹲���ڴ� �� һ�������н�����һ��ӳ�䣬 ���� �������Ʋμ�boost::interprocess
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

			//��Ҫ����cache, ���ڸ���cache�Ľ��� ���������ڴ� ������ȷ����(�� ���ڴ��ȡ)
			static boost::shared_ptr<T> create(boost::interprocess::create_only_t, const string& name, boost::interprocess::mode_t mode);

			//��Ҫ����logic������logic���̣� �����ݽ����޸�
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
				//�Ƴ����ܴ����Ϲ����ڴ�
				JMEngine::game::SharedMemory<T>::deleteMappedRegion(name.c_str());
				boost::interprocess::shared_memory_object::remove(name.c_str());

				//�����µĹ����ڴ�, �������С
				boost::interprocess::shared_memory_object shm(boost::interprocess::create_only, name.c_str(), mode);
				shm.truncate(sizeof(T));

				//�����ڴ�ӳ��
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//�����ڴ�ӳ����Ϣ
				JMEngine::game::SharedMemory<T>::saveMappedRegion(name.c_str(), mmap);

				//ͨ�������ڴ� ����shared_ptr<T>����
				//�����ڴ棬 ˭������ ˭����
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
				//��⵱ǰ�����Ƿ��Ѿ��򿪹����ڴ�
				if (JMEngine::game::SharedMemory<T>::existMappedRegion(name.c_str()))
				{
					LOGE("The shared memory with name [ %s ] had been opened", name);
					return boost::shared_ptr<T>();
				}
				//�򿪹����ڴ�
				boost::interprocess::shared_memory_object shm(boost::interprocess::open_only, name.c_str(), mode);

				//�����ڴ�ӳ��
				auto mmap = new boost::interprocess::mapped_region(shm, mode);

				//��ι����ڴ�����ڴ��С �� �����С��һ�£� �����޸ģ���Сһ�� �������� ����û�б��޸ģ�
				if (sizeof(T) != mmap->get_size())
				{
					//�Ƴ��ڴ�ӳ��
					delete mmap;
					return boost::shared_ptr<T>();
				}
				//�����ڴ�ӳ��
				JMEngine::game::SharedMemory<T>::saveMappedRegion(name.c_str(), mmap);
				
#ifdef EXPORT_TO_LUA
				//���������Ҫ���뵽lua�У� ���������д�����ᵼ�´���, access violation no rtti data, ���Բ��ô���д��
				static T* temp_memory = new T;
				*temp_memory = *(T*)(mmap->get_address());
				auto ptr = boost::shared_ptr<T>(new(mmap->get_address()) T, boost::bind(SharedMemory<T>::destory, name, false));
				*ptr = *temp_memory;
#else
				//ֱ�ӽ������ڴ� תΪ���� �����ù��캯���� ���⹹�캯���޸��ڴ�����
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
