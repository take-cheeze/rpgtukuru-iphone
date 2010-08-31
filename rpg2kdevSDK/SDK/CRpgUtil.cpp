/**
	@file
	@brief		ストリームを介せずにバッファから直接データを操作する
	@author		sue445
*/
#include "CRpgUtil.h"
#include "CRpgIOBase.h"
#include <kuto/kuto_utility.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_file.h>


//=============================================================================
/**
	バッファから直接データを取得

	@param	buf			[in] 入力元バッファ

	@return				取得したバッファ
*/
//=============================================================================
smart_buffer CRpgUtil::GetData(const smart_buffer& buf)
{
	CRpgIOBase st;

	return (st.OpenFromMemory(buf.GetPtr(), buf.GetSize())) ? st.ReadData() : smart_buffer();
}

//=============================================================================
/**
	バッファから文字列を取得

	@param	buf			[in] 入力元バッファ

	@return				取得した文字列
*/
//=============================================================================
std::string CRpgUtil::GetString(const smart_buffer& buf)
{
	CRpgIOBase st;

	return (st.OpenFromMemory(buf.GetPtr(), buf.GetSize())) ? st.ReadString() : "";
}

//=============================================================================
/**
	ツクール仕様の1次元配列を読み込む

	@param	buf		[in] 入力元バッファ

	@return			取得した1次元配列
*/
//=============================================================================
CRpgArray1 CRpgUtil::GetArray1(const smart_buffer& buf)
{
	CRpgArray1		array;

	// バッファが初期化されていない
	if (buf.GetSize() == 0)
		return array;

	// 読み込み用ストリームの初期化
	CRpgIOBase st;
	st.OpenFromMemory(buf.GetPtr(), buf.GetSize());

	// データを格納する
	while (!st.IsEof()) {
		int col = st.ReadBerNumber();
		if (col == 0)
			break;
		array.SetData(col, st.ReadData());
	}

	return array;
}

//=============================================================================
/**
	ツクール仕様の2次元配列を読み込む

	@param	buf		[in] 入力元バッファ

	@return			取得した2次元配列
*/
//=============================================================================
CRpgArray2 CRpgUtil::GetArray2(const smart_buffer& buf)
{
	CRpgArray2		array;

	// バッファが初期化されていない
	if (buf.GetSize() == 0)
		return array;

	// 読み込み用ストリームの初期化
	CRpgIOBase st;
	st.OpenFromMemory(buf.GetPtr(), buf.GetSize());

	// 一番最初には配列のサイズが格納されている
	/* int max = */ st.ReadBerNumber();

	// データを格納する
	while(!st.IsEof()){
		int row = st.ReadBerNumber();
		while (!st.IsEof()) {
			int col = st.ReadBerNumber();
			if (col == 0)
				break;
			array.SetData(row, col, st.ReadData());
		}
	}

	return array;
}


bool CRpgUtil::LoadImage(kuto::Texture& texture, const std::string& filename, bool useAlphaPalette, int hue)
{
	// search current directory
	std::string temp = filename + ".png";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = filename + ".bmp";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = filename + ".xyz";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	// search runtime directory
	std::string dirName = kuto::File::getFileName(kuto::File::getDirectoryName(filename));
#if RPG2K_IS_WINDOWS
	std::string rtpPath = "D:/ASCII/RPG2000/RTP/";
#else
	std::string rtpPath = "User/Media/Photos/RPG2000/RTP/";
#endif
	rtpPath.append(dirName).append("/").append( kuto::File::getFileName(filename) );
	temp = rtpPath + ".png";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	temp = rtpPath + ".bmp";
	if (kuto::File::exists(temp.c_str())) {
		texture.loadFromFile(temp.c_str(), useAlphaPalette, hue);
		return true;
	}
	return false;
}
