/**
 * @file
 * @brief Load Manager
 * @author project.kuto
 */

#include "kuto_load_manager.h"
#include "kuto_load_core.h"
#include "kuto_utility.h"


namespace kuto {

/**
 * コンストラクタ
 * @param parent		親タスク
 */
LoadManager::LoadManager()
: eraseCoreFlag_(false)
{
}

/**
 * デストラクタ
 */
LoadManager::~LoadManager()
{
	for (u32 i = 0; i < coreList_.size(); i++) {
		delete coreList_[i];
	}
}

/**
 * 実行
 */
void LoadManager::update()
{
	// clear not referenced cache
	while (eraseCoreFlag_) {
		eraseCoreFlag_ = false;
		for (LoadCoreList::iterator it = coreList_.begin(); it != coreList_.end();) {
			LoadCore* core = *it;
			if (core->getRefCount() == 0) {
				it = coreList_.erase(it);
				delete core;
				eraseCoreFlag_ = true;
			} else {
				++it;
			}
		}
	}
}

/**
 * 同じ名前のファイルをすでに読み込んでいないか検索
 * @param filename		ファイル名
 * @param subname		サブ名（pngで色調変えたりする場合用）
 * @return				キャッシュ内のファイル（なければNULL）
 */
LoadCore* LoadManager::searchLoadCore(const std::string& filename, const char* subname)
{
	std::string crcname(filename);
	if (subname)
		crcname += subname;
	u32 crc = crc32(crcname);
	for (u32 i = 0; i < coreList_.size(); i++) {
		if (coreList_[i]->getCrc() == crc) {
			coreList_[i]->incRefCount();
			return coreList_[i];
		}
	}
	return NULL;
}

/**
 * キャッシュに追加
 * @param core		追加するファイル
 */
void LoadManager::addLoadCore(LoadCore* core)
{
	coreList_.push_back(core);
}

/**
 * 解放
 * @param core		解放するファイル
 */
void LoadManager::releaseLoadCore(LoadCore* core)
{
	core->decRefCount();
	if (core->getRefCount() == 0)	// 参照カウンタが0になったらerase
		eraseCoreFlag_ = true;
}



}	// namespace kuto
