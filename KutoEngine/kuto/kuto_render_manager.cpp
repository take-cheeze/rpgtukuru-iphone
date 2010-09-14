/**
 * @file
 * @brief Render Manager
 * @author project.kuto
 */

#include "kuto_render_manager.h"
#include "kuto_font.h"
#include "kuto_graphics_device.h"
#include "kuto_graphics2d.h"


namespace kuto {

/**
 * コンストラクタ
 */
RenderManager::RenderManager()
: graphics2D_( new Graphics2D() )
, currentLayer_(Layer::TYPE_END)
{
	layers_[Layer::OBJECT_2D] = new Layer2D();
	layers_[Layer::DEBUG_2D] = new Layer2D();
}

/**
 * デストラクタ
 */
RenderManager::~RenderManager()
{
	for (u32 layerIndex = 0; layerIndex < Layer::TYPE_END; layerIndex++)
		delete layers_[layerIndex];
}

/**
 * 描画登録
 * @param render		登録するクラス
 * @param layer			登録するレイヤー
 * @param priority		描画プライオリティ（大きい値ほど先に描画される）
 */
void RenderManager::addRender(IRender* render, Layer::Type layer, float priority)
{
	layers_[layer]->addRender(priority, render);
}
void RenderManager::removeRender(IRender* render)
{
	for(std::size_t i = 0; i < layers_.size(); i++) layers_[i]->removeRender(render);
}

/**
 * 登録してあるものを描画
 */
void RenderManager::render()
{
	GraphicsDevice::instance()->beginRender();
	for (u32 layerIndex = 0; layerIndex < layers_.size(); layerIndex++) {
		currentLayer_ = Layer::Type(layerIndex);
		layers_[layerIndex]->render();
	}
	GraphicsDevice::instance()->endRender();
}


}	// namespace kuto
