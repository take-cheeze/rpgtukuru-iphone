/**
 * @file
 * @brief Performance Infomation
 * @author project.kuto
 */

#include "kuto_performance_info.h"
#include "kuto_render_manager.h"
#include "kuto_graphics_device.h"
#include "kuto_graphics2d.h"

#include <rpg2k/Define.hpp>

#include <algorithm>
#include <cstdio>


namespace kuto {

PerformanceInfo::PerformanceInfo()
: IRender2D(kuto::Layer::DEBUG_2D, 0.f)
, constructTime_( Timer::time() ), countFromConstruct_(0), delayCount_(0)
, delayFlag_(false)
{
}

void PerformanceInfo::update()
{
}

void PerformanceInfo::calculate()
{
	static u64 const BASE_NANO = 1000000000LL / u64(rpg2k::FRAME_PER_SECOND);		// 60fps
// 描画用の値を計算
	fps_ = 1000000000.f / ( Timer::elapsedTimeInNanoseconds(constructTime_ , Timer::time()) / float(countFromConstruct_) );
	// fps_ = 1000000000.f / float(Timer::elapsedTimeInNanoseconds(total_));
	totalTime_ = float(Timer::elapsedTimeInNanoseconds(total_)) / float(BASE_NANO * 100.f);
	updateTime_ = float(Timer::elapsedTimeInNanoseconds(update_)) / float(BASE_NANO * 100.f);
	drawTime_ = float(Timer::elapsedTimeInNanoseconds(draw_)) / float(BASE_NANO * 100.f);
	renderTime_ = float(Timer::elapsedTimeInNanoseconds(render_)) / float(BASE_NANO * 100.f);

	if( float(Timer::elapsedTimeInNanoseconds(total_)) > BASE_NANO ) {
		delayCount_++;
		delayFlag_ = true;
	}
	countFromConstruct_++;

// clear
	total_ = update_ = draw_ = render_ = std::pair<u64, u64>(0LL, 0LL);
}

void PerformanceInfo::clearFpsState()
{
	constructTime_ = countFromConstruct_ = delayCount_ = 0;
}
bool PerformanceInfo::clearDelayFlag()
{
	bool ret = false;
	std::swap(delayFlag_, ret);
	return ret;
}

void PerformanceInfo::render(kuto::Graphics2D& g) const
{
	char str[64];
	GraphicsDevice& dev = GraphicsDevice::instance();
	Vector2 pos;
	float const middle = dev.width() * 0.5f;

	#define FONT_OPTIONS kuto::Color(1.f, 1.f, 1.f, 1.f), 12.f, kuto::Font::NORMAL

	pos.x = 0.f; pos.y = (dev.height() > int(rpg2k::SCREEN_SIZE[1]))? float(rpg2k::SCREEN_SIZE[1]) : 0.f;
	sprintf(str, "FPS   : %.2f", fps_); g.drawText(str, pos, FONT_OPTIONS);
	pos.x = middle;
	sprintf(str, "Delay : %5u", unsigned(delayCount_)); g.drawText(str, pos, FONT_OPTIONS);

	pos.x = 0.f; pos.y += 13.f;
	sprintf(str, "Total : %.2f%%", totalTime_); g.drawText(str, pos, FONT_OPTIONS);
	pos.x = middle;
	sprintf(str, "Update: %.2f%%", updateTime_); g.drawText(str, pos, FONT_OPTIONS);

	pos.x = 0.f; pos.y += 13.f;
	sprintf(str, "Draw  : %.2f%%", drawTime_); g.drawText(str, pos, FONT_OPTIONS);
	pos.x = middle;
	sprintf(str, "Render: %.2f%%", renderTime_); g.drawText(str, pos, FONT_OPTIONS);

	pos.x = 0.f; pos.y += 13.f;

	#undef FONT_OPTIONS
}

}	// namespace kuto
