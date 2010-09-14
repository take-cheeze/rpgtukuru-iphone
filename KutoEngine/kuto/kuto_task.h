/**
 * @file
 * @brief Task Class
 * @author project.kuto
 */
#pragma once

#include <deque>
#include <iostream>
#include <memory>


namespace kuto {

/// Taskクラス
class Task
{
	friend class std::auto_ptr<Task>;
public:
	// Task(Task* parent);
	Task();

protected:
	virtual ~Task();

public:
	/**
	 * 初期化処理 (読み込み待ちとか)
	 * @retval true		完了
	 * @retval false	未完了
	 */
	virtual bool initialize() { return true; }

	/**
	 * 実行処理 (操作や計算など)
	 */
	virtual void update() = 0;

	/**
	 * 描画処理 (主にRenderManagerへの描画登録などの命令発行)
	 */
	virtual void draw() {}

	bool isInitialized() const { return initializedFlag_; }
	void pauseUpdate(bool pauseFlag) { pauseUpdateFlag_ = pauseFlag; }
	bool isPauseUpdate() const { return pauseUpdateFlag_; }
	void pauseDraw(bool pauseFlag) { pauseDrawFlag_ = pauseFlag; }
	bool isPauseDraw() const { return pauseDrawFlag_; }
	void release() { releasedFlag_ = true; }
	bool isReleased() const { return releasedFlag_; }
	void callbackSectionManager(bool flag) { callbackSectionManager_ = flag; }
	bool isCallbackSectionManager() const { return callbackSectionManager_; }
	void freeze(bool freezeFlag) { freezeFlag_ = freezeFlag; }
	bool isFreeze() const { return freezeFlag_; }

	bool isInitializedChildren() const;

	void updateChildren(bool parentPaused = false);
	void drawChildren(bool parentPaused = false);
	void deleteReleasedChildren();

	Task* getParent() { return parent_; }
	Task const* getParent() const { return parent_; }

	template<class T>
	T* addChild(std::auto_ptr<T> child)
	{
		child->parent_ = this;
		T* ret = child.release();
		addChildImpl( std::auto_ptr<Task>( static_cast<Task*>(ret) ) );
		return ret;
	}

private:
	void addChildImpl(std::auto_ptr<Task> child);
	void removeChild(Task* child);
	void updateImpl(bool parentPaused);
	void drawImpl(bool parentPaused);
	void deleteChildrenImpl(bool parentDeleted);

private:
	Task*		parent_;			///< 親タスク
	std::deque<Task*> children_;

	struct {
		bool		initializedFlag_		: 1;		///< 初期化完了フラグ
		bool		pauseUpdateFlag_		: 1;		///< updateをコールしないフラグ（initializeはコールする）
		bool		pauseDrawFlag_			: 1;		///< drawをコールしないフラグ
		bool		releasedFlag_			: 1;		///< 削除フラグ
		bool		updatedFlag_			: 1;		///< 一度でもupdateがコールされたフラグ
		bool		callbackSectionManager_	: 1;		///< 削除時にSectionManagerにコールバックを返す
		bool		freezeFlag_				: 1;		///< freezeされてるフラグ（initializeもupdateもdrawもやらない）
	};
};	// class Task

template<class T>
struct TaskCreator
{
	static std::auto_ptr<T> createTask() { return std::auto_ptr<T>(new T()); }
}; // TaskCreator
template<class T, class Param1>
struct TaskCreatorParam1
{
	static std::auto_ptr<T> createTask(Param1 param1) { return std::auto_ptr<T>(new T(param1)); }
}; // TaskCreatorParam1
template<class T, class Param1, class Param2>
struct TaskCreatorParam2
{
	static std::auto_ptr<T> createTask(Param1 param1, Param2 param2) { return std::auto_ptr<T>(new T(param1, param2)); }
}; // TaskCreatorParam2
template<class T, class Param1, class Param2, class Param3>
struct TaskCreatorParam3
{
	static std::auto_ptr<T> createTask(Param1 param1, Param2 param2, Param3 param3) { return std::auto_ptr<T>(new T(param1, param2, param3)); }
}; // TaskCreatorParam3
template<class T, class Param1, class Param2, class Param3, class Param4>
struct TaskCreatorParam4
{
	static std::auto_ptr<T> createTask(Param1 param1, Param2 param2, Param3 param3, Param4 param4) { return std::auto_ptr<T>(new T(param1, param2, param3, param4)); }
}; // TaskCreatorParam4

}	// namespace kuto
