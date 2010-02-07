/**
 * @file
 * @brief Singleton Task Template
 * @author takuto
 */
#pragma once

#include "kuto_task.h"
#include "kuto_error.h"


namespace kuto {

template<class T>
class TaskSingleton : public Task
{
public:
	static T* createTask(Task* parent) { if (!instance_) instance_ = new T(parent); return instance_; }
	static T* instance() { kuto_assert(instance_); return instance_; }

protected:
	TaskSingleton(Task* parent) : Task(parent) {}
	virtual ~TaskSingleton() { instance_ = NULL; }

protected:
	static T*			instance_;
};	// class TaskSingleton

template<class T> T* TaskSingleton<T>::instance_ = NULL;

}	// namespace kuto
