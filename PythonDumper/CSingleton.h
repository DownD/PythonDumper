#pragma once

template<typename T>
class CSingleton {
public:
	static T& instance();
	CSingleton(const CSingleton&) = delete;
	CSingleton& operator= (const CSingleton) = delete;

protected:
	virtual ~CSingleton();
	static T* object;
};

template <typename T> T * CSingleton <T>::object = 0;