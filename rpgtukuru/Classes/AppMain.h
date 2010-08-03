/**
 * @file
 * @brief Application main
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_performance_info.h>

class MainTask;


/// アプリメインクラス
class AppMain
{
public:
	AppMain();
	~AppMain();

	void initialize();
	void update();

private:
	MainTask*				mainTask_;
	kuto::PerformanceInfo	performanceInfo_;
};

