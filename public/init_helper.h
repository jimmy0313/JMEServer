#ifndef init_helper_h__
#define init_helper_h__

#include <type_traits>
#include "boost/shared_ptr.hpp"

class AutoInitLogicInterface
{
private:
	virtual void __init() = 0;
};

template<class T>
struct auto_init_helper
{
	static_assert(std::is_convertible<T*, AutoInitLogicInterface*>::value, "class T must inherit from abstract AutoInitLogicInterface");
	auto_init_helper(){ _obj = boost::shared_ptr<T>(new T); _obj->__init(); }

	boost::shared_ptr<T> _obj;
};

#define auto_init_h(T) \
	friend struct auto_init_helper<T>; \
	static auto_init_helper<T> __init_helper;

#define auto_init_cpp(T) \
struct auto_init_helper<T> T::__init_helper;

#endif // init_helper_h__
