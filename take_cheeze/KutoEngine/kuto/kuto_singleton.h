/**
 * @file
 * @brief Singleton
 * @author project.kuto
 */
#pragma once

#include "kuto_error.h"


namespace kuto {

template<class T>
class Singleton
{
public:
	static T* createInstance() { kuto_assert(!instance_); instance_ = new T(); return instance_; }
	static void destroyInstance() { kuto_assert(instance_); delete instance_; instance_ = NULL; }
	static T* instance() { kuto_assert(instance_); return instance_; }
	
protected:
	Singleton() {}

protected:
	static T*		instance_;
};

template<class T> T* Singleton<T>::instance_ = NULL;

}	// namespace kuto
