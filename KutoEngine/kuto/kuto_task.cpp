/**
 * @file
 * @brief Task Class
 * @author project.kuto
 */

#include "kuto_task.h"
#include "kuto_section_manager.h"


namespace kuto {

/**
 * コンストラクタ
 * @param parent		親タスク
 */
Task::Task(Task* parent)
: parent_(parent), sibling_(NULL), child_(NULL)
, initializedFlag_(false), pauseUpdateFlag_(false), pauseDrawFlag_(false), releasedFlag_(false), updatedFlag_(false)
, callbackSectionManager_(false), freezeFlag_(false)
{
	if (parent_)
		parent_->addChild(this);
}

/**
 * デストラクタ
 */
Task::~Task()
{
}

/**
 * 自分の子供に追加
 * @param child		追加する子タスク
 */
void Task::addChild(Task* child)
{
	if (child_ == NULL)
		child_ = child;
	else {
		Task* top = child_;
		while (top->sibling_ != NULL) {
			top = top->sibling_;
		}
		top->sibling_ = child;
	}
}

/**
 * 自分の子供から削除
 * @param child		削除する子タスク
 */
void Task::removeChild(Task* child)
{
	if (child_ == child)
		child_ = child_->sibling_;
	else {
		Task* top = child_;
		while (top->sibling_ != child) {
			top = top->sibling_;
		}
		top->sibling_ = child->sibling_;	
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
	for (Task* top = child_; top != NULL; top = top->sibling_) {
		if (!top->freezeFlag_) {
			top->updateImpl(parentPaused);
			if (top->child_ != NULL)
				top->updateChildren(pauseUpdateFlag_ || parentPaused);
		}
	}
}

/**
 * 子供を順次描画
 * @param parentPause		親タスクがPause状態
 */
void Task::drawChildren(bool parentPaused)
{
	for (Task* top = child_; top != NULL; top = top->sibling_) {
		if (!top->freezeFlag_) {
			top->drawImpl(parentPaused);
			if (top->child_ != NULL)
				top->drawChildren(parentPaused || pauseDrawFlag_);
		}
	}
}

/**
 * 全子供削除
 */
void Task::deleteChildren()
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
	for (Task* top = child_; top != NULL;) {
		if (top->child_ != NULL)
			top->deleteChildrenImpl(parentDeleted);
		Task* sibling = top->sibling_;
		if (parentDeleted || top->isReleased()) {
			if (!parentDeleted)
				this->removeChild(top);
			if (top->isCallbackSectionManager() && SectionManager::instance())
				SectionManager::instance()->callbackTaskDelete(top);
			delete top;
		}
		top = sibling;
	}
}

/**
 * 全子タスクが初期化完了？
 * @retval true			完了
 * @retval false		未完了
 */
bool Task::isInitializedChildren() const
{
	for (Task* top = child_; top != NULL; top = top->sibling_) {
		if (!top->isInitialized())
			return false;
		if (top->child_ != NULL) {
			if (!top->isInitializedChildren())
				return false;
		}
	}
	return true;
}

}	// namespace kuto
