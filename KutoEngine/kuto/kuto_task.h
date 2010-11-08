/**
 * @file
 * @brief Task Class
 * @author project.kuto
 */
#pragma once

#include <deque>
#include <iostream>
#include <memory>

#include <boost/noncopyable.hpp>


namespace kuto {

/// Taskクラス
class Task : boost::noncopyable
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

	bool isInitialized() const { return flag_.initialized; }
	bool isPauseUpdate() const { return flag_.pauseUpdate; }
	bool isPauseDraw  () const { return flag_.pauseDraw  ; }
	bool isReleased   () const { return flag_.released   ; }
	bool isFreeze     () const { return flag_.freeze     ; }
	bool isCallbackSectionManager() const { return flag_.callbackSectionManager; }

	void pauseUpdate(bool flag = true) { flag_.pauseUpdate = flag; }
	void   pauseDraw(bool flag = true) { flag_.pauseDraw   = flag; }
	void     release(bool flag = true) { flag_.released    = flag; }
	void      freeze(bool flag = true) { flag_.freeze      = flag; }
	void callbackSectionManager(bool flag) { flag_.callbackSectionManager = flag; }

	bool isInitializedChildren() const;

	void updateChildren(bool parentPaused = false);
	void drawChildren(bool parentPaused = false);
	void deleteReleasedChildren();

	Task* parent() { return parent_; }
	Task const* parent() const { return parent_; }

	template<class T>
	T* addChildBack(std::auto_ptr<T> child)
	{
		child->parent_ = this;
		T* ret = child.release();
		addChildBackImpl( std::auto_ptr<Task>( static_cast<Task*>(ret) ) );
		return ret;
	}
	template<class T>
	T* addChild(std::auto_ptr<T> child) { return addChildBack(child); }
	template<class T>
	T* addChildFront(std::auto_ptr<T> child)
	{
		child->parent_ = this;
		T* ret = child.release();
		addChildFrontImpl( std::auto_ptr<Task>( static_cast<Task*>(ret) ) );
		return ret;
	}

private:
	void addChildBackImpl(std::auto_ptr<Task> child);
	void addChildFrontImpl(std::auto_ptr<Task> child);
	void removeChild(Task* child);
	void updateImpl(bool parentPaused);
	void drawImpl(bool parentPaused);
	void deleteChildrenImpl(bool parentDeleted);

protected:
	typedef std::deque<Task*> TaskList;
	TaskList& children() { return children_; }

private:
	Task*		parent_;			///< 親タスク
	TaskList	children_;

	struct Flag {
		bool		initialized				: 1;		///< 初期化完了フラグ
		bool		pauseUpdate				: 1;		///< updateをコールしないフラグ（initializeはコールする）
		bool		pauseDraw				: 1;		///< drawをコールしないフラグ
		bool		released				: 1;		///< 削除フラグ
		bool		updated					: 1;		///< 一度でもupdateがコールされたフラグ
		bool		callbackSectionManager	: 1;		///< 削除時にSectionManagerにコールバックを返す
		bool		freeze					: 1;		///< freezeされてるフラグ（initializeもupdateもdrawもやらない）

		Flag();
	} flag_;
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
