/**
	@file
	@brief		オブジェクト解放時に自動的にバッファを解放するクラス
	@author		sue445
*/
#ifndef _INC_SMART_BUFFER
#define _INC_SMART_BUFFER

#include "smart_array.h"
#include "CMemory.h"
//#include <memory.h>
#include <string.h>

namespace sueLib {

/// オブジェクト解放時に自動的にバッファを解放するクラス
class smart_buffer : public smart_array<char>
{
public:
	smart_buffer() {}										///< コンストラクタ
	smart_buffer(unsigned int nSize) { New(nSize); }		///< コンストラクタ
	smart_buffer(const char* str) { SetString(str); }		///< コンストラクタ(文字列で初期化)

	operator char*() { return GetPtr(); } 					///< バッファのポインタを直接取得する
	operator const char*() const { return GetPtr(); } 		///< バッファのポインタを直接取得する

	template< typename T >
	char* operator+(T nOffset){ return GetPtr()+nOffset; }	///< バッファ先頭からのオフセット

	template< typename T >
	const char* operator+(T nOffset) const { return GetPtr()+nOffset; }	///< バッファ先頭からのオフセット

	//=============================================================================
	/**
		バッファを作成する

		@note				作成されたバッファは自動的に0で初期化される
		@param	nSize		[in] バッファのサイズ

		@return				バッファのポインタ
	*/
	//=============================================================================
	char* New(unsigned int nSize=0)
	{
		Close();
		if(nSize){
			Resize(nSize);
//			memset(GetPtr(), 0, GetSize());
			CMemory::Fill(GetPtr(), GetSize(), 0);
		}
		return GetPtr();
	}	///< バッファを作成

	//=============================================================================
	/**
		文字列で初期化する

		@param	str			[in] 文字列

		@return				文字列のポインタ
	*/
	//=============================================================================
	const char* SetString(const char* str)
	{
		int length = (int)strlen(str);
		New(length+1);
		if(length>0)	strcpy(GetPtr(), str);
		return GetPtr();
	}

	void Close() { Release(); }	///< バッファを閉じる

	//=============================================================================
	/**
		データの中身を比較する(データポインタを比較するわけではないので比較演算子とは別実装)

		@param	bufSrc		[in] 比較元データ

		@retval	true		一致
		@retval	false		不一致
	*/
	//=============================================================================
	bool Compare(const smart_buffer& bufSrc) const
	{
		if(GetSize()==bufSrc.GetSize() && memcmp(GetPtr(),bufSrc.GetPtr(),GetSize())==0){
			return true;
		}

		return false;
	}


	//=============================================================================
	/**
		メモリデータからsmart_bufferを作成

		@param	pSrc		[in] バッファのポインタ
		@param	nSrcSize	[in] バッファサイズ

		@retval	true		バッファの作成に成功
		@retval	false		バッファの作成に失敗
	*/
	//=============================================================================
	bool OpenFromData(const char* pSrc, unsigned int nSrcSize)
	{
		if(pSrc && nSrcSize && New(nSrcSize)){
			memcpy(GetPtr(), pSrc, GetSize());
			return true;
		}
		return false;
	}	///< データから作成

	//=============================================================================
	/**
		メモリデータからsmart_bufferを作成

		@note				別のsmart_bufferの内容をコピーする
		@param	buf			[in] コピー元のsmart_bufferオブジェクト

		@retval	true		バッファの作成に成功
		@retval	false		バッファの作成に失敗
	*/
	//=============================================================================
	bool OpenFromData(const smart_buffer& buf)
	{
		if(buf.GetPtr() && buf.GetSize()){
			return OpenFromData(buf.GetPtr(), buf.GetSize());
		}
		return false;
	}

// stdio.hインクルード時のみ使用
#ifdef _INC_STDIO
	//=============================================================================
	/**
		ファイルから作成

		@param	szFile		[in] ファイル名
		@param	nOffset		[in] 読み込み開始オフセット
		@param	nSize		[in] 読み込みサイズ(省略時はnOffsetからファイル終端まで読み込む)
		@param	bTxtMode	[in] テキスト用に終端にNULLを付加する

		@note		stdio.hインクルード時のみ使用可能

		@retval	true		読み込み成功
		@retval	false		読み込み失敗
	*/
	//=============================================================================
	bool OpenFromFile(const char* szFile, unsigned int nOffset=0, unsigned int nSize=0, bool bTxtMode=false)
	{
		FILE* fp = fopen(szFile, "rb");
		if(fp){
			fseek(fp, 0, SEEK_END);
			unsigned int filesize = (unsigned int)ftell(fp);

			if(nSize==0 || nSize+nOffset>filesize)		nSize = filesize - nOffset;
			New(nSize+bTxtMode);
			fseek(fp, nOffset, SEEK_SET);
			fread(GetPtr(), nSize, 1, fp);
			fclose(fp);
			return true;
		}
		else if(bTxtMode){
			New(1);
		}
		return false;
	}	///< ファイルから作成

	//=============================================================================
	/**
		ファイルに保存

		@param	szFile		[in] ファイル名
		@param	bTxtSave	[in] テキストとして保存するなら true / バイナリとして保存するなら false
		@param	nOffset		[in] 書き出し開始オフセット
		@param	nSize		[in] 書き出しサイズ(省略時はnOffsetからファイル終端まで書き出す)

		@retval	true		書き出し成功
		@retval	false		書き出し失敗
	*/
	//=============================================================================
	bool SaveToFile(const char* szFile, bool bTxtSave=false, unsigned int nOffset=0, unsigned int nSize=0) const
	{
		if(GetPtr() && GetSize()>0){
			// バイナリ保存
			if(!bTxtSave){
				FILE* fp = fopen(szFile, "wb");
				if(fp){
					if(nSize==0 || nSize+nOffset>GetSize())		nSize = GetSize() - nOffset;
					fwrite(GetPtr()+nOffset, nSize, 1, fp);
					fclose(fp);
					return true;
				}
			}
			// テキスト保存
			else{
				FILE* fp = fopen(szFile, "wt");
				if(fp){
					fwrite(GetPtr()+nOffset, strlen(GetPtr()+nOffset), 1, fp);
					fclose(fp);
					return true;
				}
			}
		}
		return false;
	}	///< ファイルに保存
#endif

};

} // end of namespace sueLib


#endif
