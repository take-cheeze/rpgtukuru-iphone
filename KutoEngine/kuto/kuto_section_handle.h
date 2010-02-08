/**
 * @file
 * @brief Section Handle
 * @author project.kuto
 */
#pragma once

#include <string>
#include "kuto_task.h"


namespace kuto {

class SectionHandleBase
{
public:
	SectionHandleBase(const char* name) : name_(name) {}
	const std::string& getName() const { return name_; }
	
	virtual Task* start(Task* parent) = 0;

protected:
	std::string			name_;
};	// class SectionHandleBase

template<class T>
class SectionHandle : public SectionHandleBase
{
public:
	SectionHandle(const char* name) : SectionHandleBase(name) {}
	virtual Task* start(Task* parent) { return T::createTask(parent); }
};	// class SectionHandle

template<class T, class ParamType>
class SectionHandleParam1 : public SectionHandleBase
{
public:
	SectionHandleParam1(const char* name, const ParamType& param)
	: SectionHandleBase(name), param_(param)
	{}
	virtual Task* start(Task* parent) { return T::createTask(parent, param_); }

protected:
	ParamType			param_;
};	// class SectionHandleParam1

}	// namespace kuto
