/**
 * @file
 * @brief Section Handle
 * @author project.kuto
 */
#pragma once

#include <memory>
#include <string>
#include "kuto_task.h"


namespace kuto {


class SectionHandleBase
{
public:
	SectionHandleBase(const char* name) : name_(name) {}
	virtual ~SectionHandleBase() {}
	const std::string& getName() const { return name_; }

	virtual std::auto_ptr<Task> start() = 0;

protected:
	std::string			name_;
};	// class SectionHandleBase

template<class T>
class SectionHandle : public SectionHandleBase
{
public:
	SectionHandle(const char* name) : SectionHandleBase(name) {}

private:
	virtual std::auto_ptr<Task> start() { return std::auto_ptr<Task>(T::createTask().release()); }
};	// class SectionHandle

template<class T, class ParamType>
class SectionHandleParam1 : public SectionHandleBase
{
public:
	SectionHandleParam1(const char* name, const ParamType& param)
	: SectionHandleBase(name), param_(param)
	{}

private:
	virtual std::auto_ptr<Task> start() { return std::auto_ptr<Task>(T::createTask(param_).release()); }

	ParamType			param_;
};	// class SectionHandleParam1

}	// namespace kuto
