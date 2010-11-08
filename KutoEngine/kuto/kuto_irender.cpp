#include "kuto_irender.h"
#include "kuto_render_manager.h"

#include <algorithm>


namespace kuto
{
	IRender::IRender(Layer::Type const type, float const priority)
	: renderFlag_(false)
	{
		RenderManager::instance().addRender(this, type, priority);
	}
	IRender::~IRender()
	{
		RenderManager::instance().removeRender(this);
	}

	void IRender::reset(Layer::Type const type, float const priority)
	{
		RenderManager::instance().removeRender(this);
		RenderManager::instance().addRender(this, type, priority);
	}
	void IRender::draw()
	{
		renderFlag_ = true;
	}
	bool IRender::clearRenderFlag()
	{
		bool ret = false;
		std::swap(ret, renderFlag_);
		return ret;
	}

	IRender2D::IRender2D(Layer::Type const type, float const priority)
	: IRender(type, priority)
	{
	}
	void IRender2D::render() const
	{
		render( *RenderManager::instance().graphics2D() );
	}
} // namespace kuto
