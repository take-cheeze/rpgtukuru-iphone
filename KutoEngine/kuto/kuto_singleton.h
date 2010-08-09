/**
 * @file
 * @brief Singleton
 * @author project.kuto
 */
#pragma once

#include "kuto_error.h"
#include <memory>


namespace kuto {

template<class T>
class Singleton
{
public:
/*
	static T* createInstance() { kuto_assert(!instance_); instance_ = new T(); return instance_; }
	static void destroyInstance() { kuto_assert(instance_); delete instance_; instance_ = NULL; }
	static T* instance() { kuto_assert(instance_); return instance_; }
 */
	static T* instance() { static T theInstance; return &theInstance; }


protected:
	Singleton() {}

private: // disable copy
	Singleton(Singleton const& src);
	Singleton& operator =(Singleton const& src);

protected:
	// static T*		instance_;
};

// template<class T> T* Singleton<T>::instance_ = NULL;

}	// namespace kuto
