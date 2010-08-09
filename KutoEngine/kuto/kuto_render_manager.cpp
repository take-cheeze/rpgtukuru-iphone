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

// RenderManager* RenderManager::instance_ = NULL;		///< シングルトンポインタ

/**
 * コンストラクタ
 */
RenderManager::RenderManager()
: graphics2D_(new Graphics2D())
, currentLayer_(LAYER_MAX)
{
	// Font::createInstance();		// create font library

	layers_[LAYER_2D_OBJECT] = new Layer2D();
	layers_[LAYER_2D_DEBUG] = new Layer2D();
}

/**
 * デストラクタ
 */
RenderManager::~RenderManager()
{
	for (u32 layerIndex = 0; layerIndex < LAYER_MAX; layerIndex++)
		delete layers_[layerIndex];
}

/**
 * 描画登録
 * @param render		登録するクラス
 * @param layer			登録するレイヤー
 * @param priority		描画プライオリティ（大きい値ほど先に描画される）
 */
void RenderManager::addRender(IRender* render, LAYER_TYPE layer, float priority)
{
	layers_[layer]->addRenderObject(RenderObject(render, priority));
}

/**
 * 登録してあるものを描画
 */
void RenderManager::render()
{
	GraphicsDevice::instance()->beginRender();
	for (u32 layerIndex = 0; layerIndex < layers_.size(); layerIndex++) {
		currentLayer_ = (LAYER_TYPE)layerIndex;
		layers_[layerIndex]->preRender();
		layers_[layerIndex]->render();
		layers_[layerIndex]->postRender();
	}
	GraphicsDevice::instance()->endRender();
}


}	// namespace kuto

