/**
	@file
	@brief		多次元構造体
	@author		sue445
*/
// ヘッダの二度読み防止
#ifndef _INC_CDIMENSION
#define _INC_CDIMENSION

#include <stdarg.h>


namespace sueLib {

//=============================================================================
/**
	多次元構造体(n次元用)

	@param	DIMENSION		[in] 次元数
	@param	DimType			[in] 各次元の型(省略時：int)
*/
//=============================================================================
template< unsigned int DIMENSION, typename DimType=int >
class CDimension
{
protected:
	DimType m_Dimension[DIMENSION];	///< 各次元のデータを格納している配列

public:
	/// デフォルトコンストラクタ
	CDimension()
	{
		for(unsigned int i = 0; i < DIMENSION; i++){
			m_Dimension[i] = 0;
		}
	}

	/// コンストラクタ
	CDimension(DimType x, ...)
	{
		va_list args;
		va_start(args, x);
		m_Dimension[0] = x;
		for(unsigned int i = 1; i < DIMENSION; i++){
			m_Dimension[i] = va_arg(args, DimType);
		}
		va_end(args);
	}

	/// コピーコンストラクタ
	template< unsigned int DIMENSION2, typename DimType2 >
	CDimension(const CDimension< DIMENSION2, DimType2 >& obj){ *this = obj; }

	/// 代入演算子
	template< unsigned int DIMENSION2, typename DimType2 >
	CDimension& operator=(const CDimension< DIMENSION2, DimType2 >& obj)
	{
		unsigned int i;

		// こっちの方が大きい場合は端数を0で埋める
		if(DIMENSION > DIMENSION2){
			for(i = 0; i < DIMENSION2; i++){
				m_Dimension[i] = (DimType)obj.Get(i);
			}
			for(i = DIMENSION2; i < DIMENSION; i++){
				m_Dimension[i] = 0;
			}
		}
		else{
			for(i = 0; i < DIMENSION; i++){
				m_Dimension[i] = (DimType)obj.Get(i);
			}
		}
		return *this;
	}

	/// 比較演算子(mapやsetでのソート用)
	bool operator<(const CDimension& obj) const
	{
		DimType value1, value2;
		for(unsigned int i = 0; i < DIMENSION; i++){
			value1 = Get(i);
			value2 = obj.Get(i);
			if(value1 < value2)				return true;
			else if(value1 > value2)		return false;
		}
		return false;
	}
	bool operator==(const CDimension& obj) const
	{
		for(unsigned int i = 0; i < DIMENSION; i++){
			if(Get(i) != obj.Get(i))	return false;
		}
		return true;
	}
	bool operator!=(const CDimension& obj) const{ return !(*this==obj); }

	//=============================================================================
	/**
		任意の次元のデータを取り出す

		@param	i		[in] 取り出す次元

		@return			その次元のデータ
	*/
	//=============================================================================
	DimType	Get(unsigned int i) const { return (i < DIMENSION) ? m_Dimension[i] : 0; }
};

/// 1次元
typedef CDimension< 1, int > CDim1;

/// 2次元
class CDim2 : public CDimension< 2, int >
{
public:
	/// デフォルトコンストラクタ
	CDim2(int _x=0, int _y=0) : CDimension< 2, int >(_x, _y) {}

	int GetX() const { return Get(0); }		///< X座標を取得
	int GetY() const { return Get(1); }		///< Y座標を取得
};

/// 3次元
class CDim3 : public CDimension< 3, int >
{
public:
	/// デフォルトコンストラクタ
	CDim3(int _x=0, int _y=0, int _z=0) : CDimension< 3, int >(_x, _y, _z) {}

	int GetX() const { return Get(0); }		///< X座標を取得
	int GetY() const { return Get(1); }		///< Y座標を取得
	int GetZ() const { return Get(2); }		///< Z座標を取得
};

} // end of namespace sueLib

#endif
