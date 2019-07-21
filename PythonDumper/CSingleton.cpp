#include "CSingleton.h"


template<typename T>
T& CSingleton<T>::instance()
{
	if (!object)
		object = new T();
	return *object;
}


template<typename T>
CSingleton<T>::~CSingleton()
{
	if(object)
		delete object;

	object = 0;
}
