/**
 * @file
 * @brief Singleton Task Template
 * @author takuto
 */
#pragma once

#include "kuto_error.h"
#include "kuto_task.h"


namespace kuto {

template<class T>
class TaskSingleton : public Task
{
public:
	static T& instance() { static T instance_; return instance_; }

protected:
	TaskSingleton() {}
};	// class TaskSingleton

}	// namespace kuto
