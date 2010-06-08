/**
	@file
	@brief		バッファからイベントを読むためのストリーム
	@author		sue445
*/
#include "CRpgEventStream.h"
#include <kuto/kuto_utility.h>


//=============================================================================
/**
	ストリームからイベント1つ分を取得

	@param	event			[out] 取得したイベント

	@retval	true			イベントの取得に成功
	@retval	false			イベントの取得に失敗
*/
//=============================================================================
bool CRpgEventStream::ReadEvent(CRpgEvent& event)
{
	if(IsEof())		return false;

	// イベントコード
	event.eventCode_ = ReadBerNumber();
	if(event.eventCode_ == 0)	return false;

	// ネスト数
	event.nest_ = ReadBerNumber();

	// 文字列引数
	event.stringParam_ = kuto::sjis2utf8(ReadString());

	// 引数の数
	event.intParams_.resize(ReadBerNumber());
	for (std::vector< CRpgEvent::ExtraIntParam >::size_type i = 0; i < event.intParams_.size(); i++) {
		if (IsEof()) {
			if (i < event.intParams_.size()) {
				event.intParams_.erase(event.intParams_.begin() + i, event.intParams_.end());
			}
			break;
		}
		event.intParams_[i] = ReadBerNumber();
#if 0
		if (event.eventCode_ == CODE_CHARA_MOVE && i >= 4) {
			switch (event.intParams_[i]) {
			case 32:	// スイッチON
			case 33:	// スイッチOFF
				{
					CRpgEvent::ExtraIntParam ip;
					ip.index = i - 4;
					ip.subIndex = 0;
					ip.value = ReadBerNumber();
					event.extraIntParam_.push_back(ip);
					event.intParams_.pop_back();
				}
				break;
			case 34:	// グラフィック変更 
				{
					CRpgEvent::ExtraStringParam sp;
					sp.index = i - 4;
					sp.subIndex = 0;
					sp.value = ReadString();
					event.extraStringParam_.push_back(sp);
					CRpgEvent::ExtraIntParam ip;
					ip.index = i - 4;
					ip.subIndex = 0;
					ip.value = ReadBerNumber();
					event.extraIntParam_.push_back(ip);
					event.intParams_.pop_back();
					event.intParams_.pop_back();
				}
				break;
			case 35:	// 効果音の演奏 
				{
					CRpgEvent::ExtraStringParam sp;
					sp.index = i - 4;
					sp.subIndex = 0;
					sp.value = ReadString();
					event.extraStringParam_.push_back(sp);
					CRpgEvent::ExtraIntParam ip;
					ip.index = i - 4;
					ip.subIndex = 0;
					ip.value = ReadBerNumber();
					event.extraIntParam_.push_back(ip);
					ip.subIndex = 1;
					ip.value = ReadBerNumber();
					event.extraIntParam_.push_back(ip);
					ip.subIndex = 2;
					ip.value = ReadBerNumber();
					event.extraIntParam_.push_back(ip);
				}
				break;
			}
		}
#endif
	}

	return true;
}
