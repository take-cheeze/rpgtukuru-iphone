/**
 * @file
 * @brief Application main
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_task.h>


namespace kuto
{
	class VirtualPad;
	class SectionManager;
	class PerformanceInfo;
}

/// アプリメインクラス
class AppMain : public kuto::Task
{
public:
	AppMain();
	~AppMain();

public:
	bool initialize();
	void update();

	kuto::SectionManager& sectionManager() { return sectionManager_; }
	kuto::VirtualPad& virtualPad() { return virtualPad_; }

	static AppMain& instance();

private:
	kuto::VirtualPad& 		virtualPad_;
	kuto::SectionManager&	sectionManager_;
	kuto::PerformanceInfo&	performanceInfo_;
}; // class AppMain
