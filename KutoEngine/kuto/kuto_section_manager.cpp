/**
 * @file
 * @brief Section Manager
 * @author project.kuto
 */

#include "kuto_section_manager.h"

#include "AppMain.h"


namespace kuto {

SectionManager& SectionManager::instance()
{
	return AppMain::instance().sectionManager();
}

SectionManager::~SectionManager()
{
	for (SectionHandleList::size_type i = 0; i < sectionHandles_.size(); i++) {
		delete sectionHandles_[i];
	}
	sectionHandles_.clear();
}

void SectionManager::update()
{
}

SectionHandleBase* SectionManager::sectionHandle(const char* name)
{
	for (SectionHandleList::size_type i = 0; i < sectionHandles_.size(); i++) {
		if (sectionHandles_[i]->name() == name)
			return sectionHandles_[i];
	}
	return NULL;
}

bool SectionManager::beginSection(const char* name)
{
	SectionHandleBase* handle = sectionHandle(name);
	if (!handle)
		return false;
	currentTask_ = /* GetAppMain()-> */ addChild(handle->start());
	currentTask_->callbackSectionManager(true);
	return true;
}

}	// namespace kuto
