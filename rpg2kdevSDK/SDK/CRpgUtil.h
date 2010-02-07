/**
	@file
	@brief		ストリームを介せずにバッファから直接データを操作する
	@author		sue445
*/
#ifndef _INC_CRPGUTIL
#define _INC_CRPGUTIL

#include <vector>
#include <string>
#include "CBer.h"
#include "CRpgArray.h"

namespace kuto {
	class Texture;
}	// namespace kuto

/// ストリームを介せずにバッファから直接データを操作する
class CRpgUtil : public CBer
{
public:
	static	smart_buffer	GetData(  const sueLib::smart_buffer& buf);
	static	std::string		GetString(const sueLib::smart_buffer& buf);
	static	CRpgArray1		GetArray1(const sueLib::smart_buffer& buf);
	static	CRpgArray2		GetArray2(const sueLib::smart_buffer& buf);
	static bool LoadImage(kuto::Texture& texture, const std::string& filename, bool useAlphaPalette, int hue = 0);

	template<class T>
	static void AnalyzeDataArray(std::vector< T >& dest, CRpgArray2& array2, int row, int column); 
	template<class T>
	static void AnalyzeDataArray(std::vector< T >& dest, CRpgArray2& array2, int row, int column, int size); 
	template<class T>
	static void AnalyzeDataArray(std::vector< T >& dest, CRpgArray1& array1, int row); 
	template<class T>
	static void AnalyzeDataArray(std::vector< T >& dest, CRpgArray1& array2, int row, int size); 
};



template<class T>
void CRpgUtil::AnalyzeDataArray(std::vector< T >& dest, CRpgArray2& array2, int row, int column)
{
	int size = array2.GetNumber(row, column);
	if (size > 0) {
		AnalyzeDataArray(dest, array2, row, column+1, size);
	}
}

template<class T>
void CRpgUtil::AnalyzeDataArray(std::vector< T >& dest, CRpgArray2& array2, int row, int column, int size)
{
	const T* temp = (const T*)array2.GetData(row, column).GetPtr();
	if (!temp)
		return;
	dest.resize(size);
	for (int i = 0; i < size; i++) {
		dest[i] = temp[i];
	}
}

template<class T>
void CRpgUtil::AnalyzeDataArray(std::vector< T >& dest, CRpgArray1& array1, int row)
{
	int size = array1.GetNumber(row);
	if (size > 0) {
		AnalyzeDataArray(dest, array1, row+1, size);
	}
}

template<class T>
void CRpgUtil::AnalyzeDataArray(std::vector< T >& dest, CRpgArray1& array1, int row, int size)
{
	const T* temp = (const T*)array1.GetData(row).GetPtr();
	if (!temp)
		return;
	dest.resize(size);
	for (int i = 0; i < size; i++) {
		dest[i] = temp[i];
	}
}

#endif
