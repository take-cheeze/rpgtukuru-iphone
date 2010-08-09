/**
 * @file
 * @brief Section Manager
 * @author project.kuto
 */
#pragma once

#include "kuto_error.h"
#include "kuto_task.h"
#include <vector>
#include "kuto_section_handle.h"
#include "kuto_singleton.h"


namespace kuto {

class SectionManager : public Singleton<SectionManager>
{
	friend class Singleton<SectionManager>;
public:
	typedef std::vector<SectionHandleBase*> SectionHandleList;
/*
	static SectionManager* createInstance() { kuto_assert(!instance_); instance_ = new SectionManager(); return instance_; }
	static void destroyInstance() { kuto_assert(instance_); delete instance_; instance_ = NULL; }
	static SectionManager* instance() { kuto_assert(instance_); return instance_; }
 */
private:
	SectionManager() : rootTask_(NULL) {}
	~SectionManager();

public:
	bool initialize(Task* rootTask);
	void addSectionHandle(SectionHandleBase* handle) { sectionHandles_.push_back(handle); }
	SectionHandleBase* getSectionHandle(const char* name);
	bool beginSection(const char* name);
	const SectionHandleList& getSectionHandles() const { return sectionHandles_; }
	Task* getCurrentTask() { return currentTask_; }
	void callbackTaskDelete(Task* task) { if (currentTask_ == task) currentTask_ = NULL; }

private:
	// static SectionManager*		instance_;
	Task*						rootTask_;
	Task*						currentTask_;
	SectionHandleList			sectionHandles_;
};	// class SectionManager

}	// namespace kuto
