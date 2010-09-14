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
	static T* instance() { static T instance_; return &instance_; }

protected:
	TaskSingleton() {}

private: // disable copy
	TaskSingleton(TaskSingleton const& src);
	TaskSingleton& operator =(TaskSingleton const& src);
};	// class TaskSingleton

}	// namespace kuto
