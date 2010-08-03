/**
 * @file
 * @brief Task Class
 * @author project.kuto
 */
#pragma once

#include <iostream>

namespace kuto {

/// Taskクラス
class Task
{
public:
	Task(Task* parent);

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
	virtual void update() {}

	/**
	 * 描画処理 (主にRenderManagerへの描画登録)
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
	void deleteChildren();

	Task* getParent() { return parent_; }

private:
	void addChild(Task* child);
	void removeChild(Task* child);
	void updateImpl(bool parentPaused);
	void drawImpl(bool parentPaused);
	void deleteChildrenImpl(bool parentDeleted);

private:
	Task*		parent_;			///< 親タスク
	Task*		sibling_;			///< 弟タスク
	Task*		child_;				///< 子タスク

	bool		initializedFlag_		;		///< 初期化完了フラグ
	bool		pauseUpdateFlag_		;		///< updateをコールしないフラグ（initializeはコールする）
	bool		pauseDrawFlag_			;		///< drawをコールしないフラグ
	bool		releasedFlag_			;		///< 削除フラグ
	bool		updatedFlag_			;		///< 一度でもupdateがコールされたフラグ
	bool		callbackSectionManager_	;		///< 削除時にSectionManagerにコールバックを返す
	bool		freezeFlag_				;		///< freezeされてるフラグ（initializeもupdateもdrawもやらない）
/*
	struct {
		bool		initializedFlag_		: 1;		///< 初期化完了フラグ
		bool		pauseUpdateFlag_		: 1;		///< updateをコールしないフラグ（initializeはコールする）
		bool		pauseDrawFlag_			: 1;		///< drawをコールしないフラグ
		bool		releasedFlag_			: 1;		///< 削除フラグ
		bool		updatedFlag_			: 1;		///< 一度でもupdateがコールされたフラグ
		bool		callbackSectionManager_	: 1;		///< 削除時にSectionManagerにコールバックを返す
		bool		freezeFlag_				: 1;		///< freezeされてるフラグ（initializeもupdateもdrawもやらない）
	};
 */
};	// class Task

}	// namespace kuto
