/**
 * @file
 * @brief Task Class
 * @author project.kuto
 */

#include "kuto_task.h"
#include "kuto_task_singleton.h"
#include "kuto_section_manager.h"

#include <algorithm>


namespace kuto {

/**
 * コンストラクタ
 * @param parent		親タスク
 */
/*
Task::Task(Task* parent)
: parent_(parent), sibling_(NULL), child_(NULL)
, initializedFlag_(false), pauseUpdateFlag_(false), pauseDrawFlag_(false)
, releasedFlag_(false), updatedFlag_(false)
, callbackSectionManager_(false), freezeFlag_(false)
{
	if (parent_)
		parent_->addChild(this);
}
 */
Task::Task()
: parent_(NULL)
, initializedFlag_(false), pauseUpdateFlag_(false), pauseDrawFlag_(false)
, releasedFlag_(false), updatedFlag_(false)
, callbackSectionManager_(false), freezeFlag_(false)
{
}

/**
 * デストラクタ
 */
Task::~Task()
{
	for(std::deque<Task*>::iterator it = children_.begin(); it < children_.end(); ++it) {
		delete (*it);
	}
}

/**
 * 自分の子供に追加
 * @param child		追加する子タスク
 */
void Task::addChildImpl(std::auto_ptr<Task> child)
{
	kuto_assert( child.get() );

	children_.push_back(child.release());
}

/**
 * 自分の子供から削除
 * @param child		削除する子タスク
 */
void Task::removeChild(Task* child)
{
	std::deque<Task*>::iterator it = std::find( children_.begin(), children_.end(), child );
	if( it != children_.end() ) {
		delete (*it);
		children_.erase(it);
	}
}

/**
 * 実行内部処理\n
 * 初期化中はinitialize()を呼び、初期化完了後はupdate()を呼ぶ。
 * @param parentPaused		親タスクがPause状態
 */
void Task::updateImpl(bool parentPaused)
{
	if (!initializedFlag_)
		initializedFlag_ = this->initialize();
	else {
		if (!pauseUpdateFlag_ && !parentPaused) {
			this->update();
			updatedFlag_ = true;
		}
	}
}

/**
 * 描画内部処理\n
 * draw()を呼ぶ。一度でもupdate()が呼ばれないと実行されない。
 * @param parentPause		親タスクがPause状態
 */
void Task::drawImpl(bool parentPaused)
{
	if (initializedFlag_ && updatedFlag_ && !pauseDrawFlag_ && !parentPaused)
		this->draw();
}

/**
 * 子供を順次実行
 * @param parentPause		親タスクがPause状態
 */
void Task::updateChildren(bool parentPaused)
{
	for(std::deque<Task*>::iterator it = children_.begin(); it < children_.end(); ++it) {
		if (!(*it)->freezeFlag_) {
			(*it)->updateImpl(parentPaused);
			(*it)->updateChildren((*it)->pauseUpdateFlag_ || parentPaused);
		}
	}
}

/**
 * 子供を順次描画
 * @param parentPause		親タスクがPause状態
 */
void Task::drawChildren(bool parentPaused)
{
	for(std::deque<Task*>::iterator it = children_.begin(); it < children_.end(); ++it) {
		if (!(*it)->freezeFlag_) {
			(*it)->drawImpl(parentPaused);
			(*it)->drawChildren(parentPaused || (*it)->pauseDrawFlag_);
		}
	}
}

/**
 * 全子供削除
 */
void Task::deleteReleasedChildren()
{
	deleteChildrenImpl(false);
}

/**
 * 子供削除内部処理
 * @param parentDeleted		親タスクが削除された
 */
void Task::deleteChildrenImpl(bool parentDeleted)
{
	parentDeleted |= releasedFlag_;
	std::deque<Task*> eraseList;
	for(std::deque<Task*>::iterator it = children_.begin(); it < children_.end(); ++it) {
		(*it)->deleteChildrenImpl(parentDeleted);
		if (parentDeleted || (*it)->isReleased()) {
			if (!parentDeleted)
				eraseList.push_back(*it);
			if ((*it)->isCallbackSectionManager() /* && SectionManager::instance() */)
				SectionManager::instance()->callbackTaskDelete(*it);
		}
	}
	for(std::size_t i = 0; i < eraseList.size(); i++) removeChild(eraseList[i]);
}

/**
 * 全子タスクが初期化完了？
 * @retval true			完了
 * @retval false		未完了
 */
bool Task::isInitializedChildren() const
{
	for(std::deque<Task*>::const_iterator it = children_.begin(); it < children_.end(); ++it) {
		if ( !(*it)->isInitialized() || !(*it)->isInitializedChildren() ) {
			return false;
		}
	}
	return true;
}

}	// namespace kuto
