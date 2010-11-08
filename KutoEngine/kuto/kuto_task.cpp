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
Task::Flag::Flag()
: initialized(false)
, pauseUpdate(false), pauseDraw(false)
, released(false), updated(false)
, callbackSectionManager(false), freeze(false)
{
}
Task::Task()
: parent_(NULL)
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
void Task::addChildBackImpl(std::auto_ptr<Task> child)
{
	kuto_assert( child.get() );

	children_.push_back(child.release());
}
void Task::addChildFrontImpl(std::auto_ptr<Task> child)
{
	kuto_assert( child.get() );

	children_.push_front(child.release());
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
	if (!flag_.initialized)
		flag_.initialized = this->initialize();
	else {
		if (!flag_.pauseUpdate && !parentPaused) {
			this->update();
			flag_.updated = true;
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
	if (flag_.initialized && flag_.updated && !flag_.pauseDraw && !parentPaused) {
		this->draw();
	}
}

/**
 * 子供を順次実行
 * @param parentPause		親タスクがPause状態
 */
void Task::updateChildren(bool parentPaused)
{
	for(std::deque<Task*>::iterator it = children_.begin(); it < children_.end(); ++it) {
		if (!(*it)->flag_.freeze) {
			(*it)->updateImpl(parentPaused);
			(*it)->updateChildren((*it)->flag_.pauseUpdate || parentPaused);
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
		if (!(*it)->flag_.freeze) {
			(*it)->drawImpl(parentPaused);
			(*it)->drawChildren(parentPaused || (*it)->flag_.pauseDraw);
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
	parentDeleted |= flag_.released;
	std::deque<Task*> eraseList;
	for(std::deque<Task*>::iterator it = children_.begin(); it < children_.end(); ++it) {
		(*it)->deleteChildrenImpl(parentDeleted);
		if (parentDeleted || (*it)->isReleased()) {
			if (!parentDeleted)
				eraseList.push_back(*it);
			if ((*it)->isCallbackSectionManager() /* && SectionManager::instance() */)
				SectionManager::instance().callbackTaskDelete(*it);
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
