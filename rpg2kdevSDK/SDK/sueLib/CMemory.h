/**
	@file
	@brief		簡易メモリユーティリティ
	@author		sue445
*/
#ifndef _INC_CMEMORY
#define _INC_CMEMORY

namespace sueLib {

/// 簡易メモリユーティリティ
class CMemory{
public:
	static void Copy(void* pDst, const void* pSrc, unsigned int nSize);		///< メモリ転送
	static void Fill(void* pDst, unsigned int nSize, unsigned char cData);	///< FillMemory
	static void Zero(void* pDst, unsigned int nSize){ Fill(pDst,nSize,0); }	///< ZeroMemory
};

} // end of namespace sueLib

#endif
