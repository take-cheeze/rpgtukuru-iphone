/**
 * @file
 * @brief Fade Effect
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>


class GameFadeEffect : public kuto::IRender2D, public kuto::TaskCreator<GameFadeEffect>
{
	friend class kuto::TaskCreator<GameFadeEffect>;
public:
	enum FadeType {
		kTypeFade,					///< 0:フェードアウト
    	kTypeRandomBlock,			///< 1:全体にランダムブロック
    	kTypeRandomBlockTopDown,	///< 2:上からランダムブロック
    	kTypeRandomBlockBottomUp,	///< 3:下からランダムブロック
    	kTypeBlind,					///< 4:ブラインドクローズ
    	kTypeStripeVertical,		///< 5:上下からストライプ
    	kTypeStripeHorizontal,		///< 6:左右からストライプ
    	kTypeHoleShrink,			///< 7:外側から中心へ
    	kTypeHoleExpand,			///< 8:中心から外側へ
    	kTypeScrollUp,				///< 9:上にスクロール
    	kTypeScrollDown,			///< 10:下にスクロール
    	kTypeScrollLeft,			///< 11:左にスクロール
    	kTypeScrollRight,			///< 12:右にスクロール
    	kTypeSeparateUD,			///< 13:上下に分割
    	kTypeSeparateLR,			///< 14:左右に分割
    	kTypeSeparateUDLR,			///< 15:上下左右に分割
    	kTypeZoom,					///< 16:ズームイン
    	kTypeMosaic,				///< 17:モザイク
    	kTypeRasterScroll,			///< 18:ラスタスクロール
    	kTypeImmediate,				///< 19:瞬間消去
    	kTypeNothing,				///< 20:消去しない
	};
	enum State {
		kStateNone,
		kStateEncountFlash,
		kStateFadeOut,
		kStateFadeOutEnd,
		kStateFadeIn,
		kStateFadeInEnd,
	};

private:
	GameFadeEffect();

	virtual void update();
	virtual void render(kuto::Graphics2D& g) const;

	void renderBattleFlash(kuto::Graphics2D& g) const;
	void renderFade(kuto::Graphics2D& g, float ratio) const;
	void renderRandomBlock(kuto::Graphics2D& g, float ratio) const;
	void renderBlind(kuto::Graphics2D& g, float ratio) const;
	void renderStripeVertical(kuto::Graphics2D& g, float ratio) const;
	void renderStripeHorizontal(kuto::Graphics2D& g, float ratio) const;
	void renderHoleShrink(kuto::Graphics2D& g, float ratio) const;
	void renderHoleExpand(kuto::Graphics2D& g, float ratio) const;

public:
	void start(FadeType type, State state);
	State state() const { return state_; }
	void setState(State value) { state_ = value; counter_ = 0; }

private:
	FadeType		type_;
	State			state_;
	int				counter_;
	kuto::Array<int, 40 * 30>		blocks_;
};	// class GameFadeEffect
