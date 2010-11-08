/**
 * @file
 * @brief Section Manager
 * @author project.kuto
 */
#pragma once

#include "kuto_error.h"
#include "kuto_task.h"
#include "kuto_section_handle.h"

#include <memory>
#include <vector>


class AppMain;

namespace kuto {

class SectionManager : public Task
{
	friend class ::AppMain;
public:
	static SectionManager& instance();

	~SectionManager();
protected:
	SectionManager() : Task() {}

public:
	typedef std::vector<SectionHandleBase*> SectionHandleList;

	void addSectionHandle(std::auto_ptr<SectionHandleBase> handle) { sectionHandles_.push_back(handle.release()); }
	SectionHandleBase* sectionHandle(const char* name);
	bool beginSection(const char* name);
	const SectionHandleList& sectionHandles() const { return sectionHandles_; }
	Task* currentTask() { return currentTask_; }
	void callbackTaskDelete(Task* task) { if (currentTask_ == task) currentTask_ = NULL; }

private:
	virtual void update();

private:
	Task*						currentTask_;
	SectionHandleList			sectionHandles_;
};	// class SectionManager

}	// namespace kuto
