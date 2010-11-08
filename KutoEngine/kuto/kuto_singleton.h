/**
 * @file
 * @brief Singleton
 * @author project.kuto
 */
#pragma once

#include "kuto_error.h"

#include <boost/noncopyable.hpp>


namespace kuto {

template<class T>
class Singleton : boost::noncopyable 
{
public:
	static T& instance() { static T instance_; return instance_; }

protected:
	Singleton() {}
};

}	// namespace kuto
