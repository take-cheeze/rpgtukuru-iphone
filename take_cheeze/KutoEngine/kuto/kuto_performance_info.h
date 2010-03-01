/**
 * @file
 * @brief Performance Infomation
 * @author project.kuto
 */
#pragma once

#include "kuto_types.h"
#include "kuto_timer.h"
#include "kuto_irender.h"

namespace kuto {

/// パフォーマンス計測
class PerformanceInfo : public IRender
{
public:
	PerformanceInfo() : startTime_(0), updateEndTime_(0), drawEndTime_(0), renderEndTime_(0) {}
	
	void start() { startTime_ = kuto::Timer::getTime(); }			///< スタート
	void endUpdate() { updateEndTime_ = kuto::Timer::getTime(); }	///< Update終了
	void endDraw() { drawEndTime_ = kuto::Timer::getTime(); }		///< Draw終了
	void endRender() { renderEndTime_ = kuto::Timer::getTime(); }	///< 描画終了
	
	void draw();
	virtual void render(Graphics2D& g);
	// virtual void render(Graphics& g);
	
public:
	u64		startTime_;
	u64		updateEndTime_;
	u64		drawEndTime_;
	u64		renderEndTime_;
	float 	fps_;
	float 	totalTime_;
	float 	updateTime_;
	float 	drawTime_;
	float 	renderTime_;
};	// class Viewport

}	// namespace kuto
