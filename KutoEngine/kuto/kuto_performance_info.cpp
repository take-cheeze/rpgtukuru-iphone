/**
 * @file
 * @brief Performance Infomation
 * @author project.kuto
 */

#include "kuto_performance_info.h"
#include "kuto_render_manager.h"
#include "kuto_graphics2d.h"
#include <cstdio>


namespace kuto {

void PerformanceInfo::draw()
{
	static const u64 BASE_NANO = 1000000000 / 30;		// 30fps
	// 描画用の値を計算
	fps_ = 1000000000.f / (float)kuto::Timer::getElapsedTimeInNanoseconds(startTime_, renderEndTime_);
	totalTime_ = (float)kuto::Timer::getElapsedTimeInNanoseconds(startTime_, renderEndTime_) / (float)BASE_NANO * 100.f;
	updateTime_ = (float)kuto::Timer::getElapsedTimeInNanoseconds(startTime_, updateEndTime_) / (float)BASE_NANO * 100.f;
	drawTime_ = (float)kuto::Timer::getElapsedTimeInNanoseconds(updateEndTime_, drawEndTime_) / (float)BASE_NANO * 100.f;
	renderTime_ = (float)kuto::Timer::getElapsedTimeInNanoseconds(drawEndTime_, renderEndTime_) / (float)BASE_NANO * 100.f;
	kuto::RenderManager::instance()->addRender(this, kuto::LAYER_2D_DEBUG, 0.f);	// 描画登録
}

void PerformanceInfo::render()
{
	Graphics2D* g = RenderManager::instance()->getGraphics2D();
	char str[64];
	float y = 0.f;
	sprintf(str, "FPS %.2f", fps_);
	g->drawText(str, kuto::Vector2(0.f, y), kuto::Color(1.f, 1.f, 1.f, 1.f), 12.f, kuto::Font::NORMAL);
	y += 13.f;
	sprintf(str, "Total %.2f%%  Update %.2f%%", totalTime_, updateTime_);
	g->drawText(str, kuto::Vector2(0.f, y), kuto::Color(1.f, 1.f, 1.f, 1.f), 12.f, kuto::Font::NORMAL);
	y += 13.f;
	sprintf(str, "Draw %.2f%%  Render %.2f%%", drawTime_, renderTime_);
	g->drawText(str, kuto::Vector2(0.f, y), kuto::Color(1.f, 1.f, 1.f, 1.f), 12.f, kuto::Font::NORMAL);
	y += 13.f;
}

}	// namespace kuto
