/**
 * @file
 * @brief Section Manager
 * @author project.kuto
 */

#include "kuto_section_manager.h"


namespace kuto {

// SectionManager* SectionManager::instance_ = NULL;


SectionManager::~SectionManager()
{
	for (SectionHandleList::size_type i = 0; i < sectionHandles_.size(); i++) {
		delete sectionHandles_[i];
	}
	sectionHandles_.clear();
}

bool SectionManager::initialize(Task* rootTask)
{
	rootTask_ = rootTask;
	currentTask_ = NULL;
	return true;
}

SectionHandleBase* SectionManager::getSectionHandle(const char* name)
{
	for (SectionHandleList::size_type i = 0; i < sectionHandles_.size(); i++) {
		if (sectionHandles_[i]->getName() == name)
			return sectionHandles_[i];
	}
	return NULL;
}

bool SectionManager::beginSection(const char* name)
{
	SectionHandleBase* handle = getSectionHandle(name);
	if (!handle)
		return false;
	currentTask_ = handle->start(rootTask_);
	currentTask_->callbackSectionManager(true);
	return true;
}

}	// namespace kuto
