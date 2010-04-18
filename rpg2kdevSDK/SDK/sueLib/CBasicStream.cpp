/**
	@file
	@brief		ファイル入力とメモリ入力を同じように扱うための基本ストリーム
	@author		sue445
*/
#include "CBasicStream.h"
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <kuto/kuto_error.h>
#include <kuto/kuto_file.h>


//=============================================================================
/**
	メモリデータからストリームを開く

	@param	pData		[in] メモリデータへのポインタ
	@param	nLength		[in] メモリデータのサイズ
	@param	bWrite		[in] 書き込みモードかどうか(省略時:false)
	@note				メモリ中のストリームでなおかつ書き出しモードの場合、nLengthで指定した\n
						サイズ以上を書き出すことはできません

	@retval	true		ストリームの初期化に成功
	@retval	false		ストリームの初期化に失敗
*/
//=============================================================================
bool CBasicStream::OpenFromMemory(const void* pData, unsigned int nLength, bool bWrite)
{
	if(m_bOpened || pData==NULL || nLength==0)		return false;

	m_pData		= pData;
	m_nLength	= nLength;
	m_nPosition	= 0;
	m_bFile		= false;
	m_bOpened	= true;
	m_bWrite	= bWrite;

	return true;
}

//=============================================================================
/**
	ファイルからストリームを開く

	@param	szFile		[in] ファイル名
	@param	bWrite		[in] 書き込みモードかどうか(省略時:false)

	@retval	true		ストリームの初期化に成功
	@retval	false		ストリームの初期化に失敗
*/
//=============================================================================
bool CBasicStream::OpenFromFile(const char* szFile, bool bWrite)
{
	if(m_bOpened)		return false;

	if(bWrite){
		// no support!
		kuto_assert(0);
	}
	else{
		u32 fileSize = 0;
		m_FileBuf = kuto::File::readBytes(szFile, fileSize);
		if (!m_FileBuf)
			return false;
		return OpenFromMemory(m_FileBuf, fileSize, bWrite);
	}
	return true;
}

//=============================================================================
/**
	ストリームを解放する

	@note		デストラクタで実行されるので明示的に行う必要はありません
*/
//=============================================================================
void CBasicStream::Close()
{
	if(!m_bOpened)		return;

	if (m_FileBuf) {
		kuto::File::freeBytes(m_FileBuf);
		m_FileBuf = NULL;
	}
	m_pData = NULL;
	m_nLength = m_nPosition = 0;
	m_bOpened = m_bWrite = false;
}

//=============================================================================
/**
	ストリームの読み出し位置をセットする

	@param	nPos		[in] 移動量
	@param	nOrigin		[in] 基準位置(SEEK_SET:先頭, SEEK_END:終端, SEEK_CUR:現在位置)
*/
//=============================================================================
void CBasicStream::SetSeek(int nPos, int nOrigin)
{
	if(!m_bOpened)		return;

	switch(nOrigin){
	case SEEK_CUR:
		m_nPosition += nPos;
		break;
	case SEEK_END:
		m_nPosition = m_nLength - nPos;
		break;
	case SEEK_SET:
		m_nPosition = nPos;
		break;
	default:
		return;
	}

	if(m_nPosition > m_nLength)		m_nPosition = m_nLength;
}

//=============================================================================
/**
	ストリームから指定サイズ読み込む

	@param	pData	[out] 読み込むデータのポインタ
	@param	nSize	[in] 読み込むデータのサイズ

	@return			実際に読み込んだサイズ
*/
//=============================================================================
unsigned int CBasicStream::Read(void* pData, unsigned int nSize)
{
	if(!m_bOpened || m_bWrite || IsEof() || pData==NULL || nSize==0)	return 0;

	const unsigned char* pSrc = reinterpret_cast< const unsigned char* >( m_pData );
	pSrc += m_nPosition;

	if(nSize > m_nLength-m_nPosition)	nSize = m_nLength - m_nPosition;

	if(m_bFile){
		// no support
		kuto_assert(0);
	}
	else{
//		memcpy(pData, pSrc, nSize);
		MemoryCopy(pData, pSrc, nSize);
	}
	m_nPosition += nSize;
	return nSize;
}

//=============================================================================
/**
	ストリームに指定サイズ書き出す

	@param	pData	[in] 書き出すデータのポインタ
	@param	nSize	[in] 書き出すデータのサイズ

	@return			実際に書き出したサイズ
*/
//=============================================================================
unsigned int CBasicStream::Write(const void* pData, unsigned int nSize)
{
#if 0
	if(!m_bOpened || !m_bWrite || pData==NULL || nSize==0)		return 0;

	unsigned char* pDst = (unsigned char*)reinterpret_cast< const unsigned char* >( m_pData );
	pDst += m_nPosition;

	if(!m_bFile && nSize > m_nLength-m_nPosition)	nSize = m_nLength - m_nPosition;

	if(m_bFile){
		_write(m_nFile, pData, nSize);
	}
	else{
//		memcpy(pDst, pData, nSize);
		MemoryCopy(pDst, pData, nSize);
	}
	m_nPosition += nSize;
#endif
	return nSize;
}

//=============================================================================
/**
	メモリ転送(4バイト単位で転送するので(゜д゜)ウマーなはず)

	@param	pDst		[out] 転送先ポインタ
	@param	pSrc		[in] 転送元ポインタ
	@param	nSize		[in] 転送サイズ
*/
//=============================================================================
void CBasicStream::MemoryCopy(void* pDst, const void* pSrc, unsigned int nSize)
{
#if defined(RPG2K_IS_PSP)
	memcpy(pDst, pSrc, nSize);
#else
	unsigned int* lpDst			= reinterpret_cast< unsigned int* >( pDst );
	const unsigned int* lpSrc	= reinterpret_cast< const unsigned int* >( pSrc );

	while(nSize>=4){
		*lpDst++ = *lpSrc++;
		nSize-=4;
	}

	unsigned char* pDst1		= reinterpret_cast< unsigned char* >( lpDst );
	const unsigned char* pSrc1	= reinterpret_cast< const unsigned char* >( lpSrc );

	// 端数はしょうがないから1バイトで転送
	while(nSize){
		*pDst1++ = *pSrc1++;
		nSize--;
	}
#endif
}
