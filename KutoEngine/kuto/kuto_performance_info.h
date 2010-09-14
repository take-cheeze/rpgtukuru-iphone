/**
 * @file
 * @brief Performance Infomation
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_timer.h"
#include "kuto_irender.h"

#include <utility>


namespace kuto {

/// パフォーマンス計測
class PerformanceInfo : public IRender2D
{
public:
	PerformanceInfo();

	void start() { total_.first = kuto::Timer::getTime(); }
	void startUpdate() { update_.first = kuto::Timer::getTime(); }
	void startDraw() { draw_.first = kuto::Timer::getTime(); }
	void startRender() { render_.first = kuto::Timer::getTime(); }
	void end() { total_.second = kuto::Timer::getTime(); }
	void endUpdate() { update_.second = kuto::Timer::getTime(); }
	void endDraw() { draw_.second = kuto::Timer::getTime(); }
	void endRender() { render_.second = kuto::Timer::getTime(); }

	virtual void render(kuto::Graphics2D* g) const;

	void calculate();

	void clearFpsState();
	bool clearDelayFlag();

private:
	virtual void update();

private:
	std::pair<u64, u64> total_, update_, draw_, render_;
	float fps_, totalTime_, updateTime_, drawTime_, renderTime_;

	u64 constructTime_, countFromConstruct_, delayCount_;
	bool delayFlag_;
};	// class Viewport

}	// namespace kuto
