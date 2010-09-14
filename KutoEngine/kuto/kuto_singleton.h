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
	static T* instance() { static T instance_; return &instance_; }

protected:
	Singleton() {}

private: // disable copy
	Singleton(Singleton const& src);
	Singleton& operator =(Singleton const& src);
};

}	// namespace kuto
