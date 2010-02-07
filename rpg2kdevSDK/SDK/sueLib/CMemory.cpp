/**
	@file
	@brief		メモリ簡易ユーティリティ
	@author		sue445
*/
#include "CMemory.h"

namespace sueLib {

/// CMemoryから呼び出すテンプレートクラス
template< typename T >
class CMem{
public:
	//=============================================================================
	/**
		メモリ転送

		@param	pDst		[out] 転送先ポインタ
		@param	pSrc		[in] 転送元ポインタ
		@param	nSize		[in] 転送サイズ
	*/
	//=============================================================================
	static void Copy(void* pDst, const void* pSrc, unsigned int nSize)
	{
		T* lpDst		= reinterpret_cast< T* >( pDst );
		const T* lpSrc	= reinterpret_cast< const T* >( pSrc );

		while(nSize>=sizeof(T)){
			*lpDst++ = *lpSrc++;
			nSize-=sizeof(T);
		}

		// 端数はしょうがないから1バイトで転送
		unsigned char* pDst1		= reinterpret_cast< unsigned char* >( lpDst );
		const unsigned char* pSrc1	= reinterpret_cast< const unsigned char* >( lpSrc );
		while(nSize){
			*pDst1++ = *pSrc1++;
			nSize--;
		}
	}

	//=============================================================================
	/**
		32ビット単位でFillMemory

		@param	pDst		[out] 転送先ポインタ
		@param	nSize		[in] 転送サイズ
		@param	data1		[in] 書き出すデータ(1バイト版)
		@param	data2		[in] 書き出すデータ(data1をnバイトに拡張したやつ)
	*/
	//=============================================================================
	static void Fill(void* pDst, unsigned int nSize, unsigned char data1, T data2)
	{
		T* lpDst		= reinterpret_cast< T* >( pDst );

		while(nSize>=sizeof(T)){
			*lpDst++ = data2;
			nSize-=sizeof(T);
		}

		// 端数はしょうがないから1バイトで転送
		unsigned char* pDst1		= reinterpret_cast< unsigned char* >( lpDst );
		while(nSize){
			*pDst1++ = data1;
			nSize--;
		}
	}
};

//=============================================================================
/**
	32ビット単位でメモリ転送

	@param	pDst		[out] 転送先ポインタ
	@param	pSrc		[in] 転送元ポインタ
	@param	nSize		[in] 転送サイズ
*/
//=============================================================================
void CMemory::Copy(void* pDst, const void* pSrc, unsigned int nSize)
{
	CMem< unsigned long >::Copy(pDst, pSrc, nSize);
}

//=============================================================================
/**
	32ビット単位でFillMemory

	@param	pDst		[out] 転送先ポインタ
	@param	nSize		[in] 転送サイズ
	@param	cData		[in] 書き出すデータ
*/
//=============================================================================
void CMemory::Fill(void* pDst, unsigned int nSize, unsigned char cData)
{
	CMem< unsigned long >::Fill(pDst, nSize, cData,
		(cData<<24) | (cData<<16) | (cData<<8) | cData);
}

} // end of namespace sueLib
