/**
 * @file
 * @brief Touch Pad
 * @author project.kuto
 */
#pragma once

#include "kuto_math.h"
#include "kuto_singleton.h"
#include "kuto_virtual_pad.h"


namespace kuto {
	class KeyInfo
	{
	public:
		KeyInfo() { reset(); }
		void reset()
		{
			onFlag_ = false;
			pressFlag_ = false;
			releaseFlag_ = false;
			key = VirtualPad::KEY_MAX;
		}

	public:
		VirtualPad::KEYS key;
		struct {
			bool	onFlag_				: 1;
			bool	pressFlag_			: 1;
			bool	releaseFlag_		: 1;
		};
	};	// class KeyInfo


	class KeyPad : public Singleton<KeyPad>
	{
		friend class Singleton<KeyPad>;
	public:
		enum { MAX_KEY	= 4, };
	private:
		KeyPad();

	public:
		bool on(int index) const { return info_[index].onFlag_; }
		bool press(int index) const { return info_[index].pressFlag_; }
		bool release(int index) const { return info_[index].releaseFlag_; }
		VirtualPad::KEYS key(int index) const { return info_[index].key; }

		void update();
		void setKeys(const KeyInfo* info, int size);

	private:
		KeyInfo			info_[MAX_KEY];
		KeyInfo			nextInfo_[MAX_KEY];
		bool				nextInfoChanged_;
		int					nextInfoTouchSize_;
	};	// class KeyPad
}	// namespace kuto
