#ifndef _INC__RPG2k__STRUCTURE_HPP
#define _INC__RPG2k__STRUCTURE_HPP

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include <limits.h>
#include <string.h>
#include <stdint.h>

#include "Defines.hpp"

namespace rpg2kLib
{
	namespace structure
	{

		using namespace std;

		class StreamReader;

		static const uint
			BER_BIT  = (CHAR_BIT-1),
			BER_SIGN = 0x01 << BER_BIT,
			BER_MASK = BER_SIGN - 1;
		extern uint getBERSize(uint32_t num);

		template< typename T >
		class Stack : public vector< T >
		{
		public:
			Stack< T >() : vector< T >() {}

			uint length() { return vector< T >::size(); }

			T& top() { return vector< T >::back(); }
			T  pop()
			{
				T ret = top();
				vector< T >::pop_back();
				return ret;
			}
			void push(T data) { vector< T >::push_back(data); }
		};

	// class that saves "*new"ed value and deletes that at deconstructor
		template< typename Key, typename T >
		class Map
		{
		private:
			map< Key, T* > DATA;
		protected:
			typedef typename map< Key, T* >::value_type val_type;
			typedef typename map< Key, T* >::const_iterator iterator;
		public:
			class Iterator
			{
			private:
				friend class Map< Key, T >;

			protected:
				const Map< Key, T >& OWNER;
				iterator ITERATOR;

				Iterator();
				Iterator(const Map< Key, T >& owner, const iterator it)
					: OWNER(owner), ITERATOR(it)
				{
				}
			public:
				bool operator !=(const Iterator& it) const
				{
					return this->ITERATOR != it.ITERATOR;
				}
				bool operator ==(const Iterator& it) const
				{
					return this->ITERATOR == it.ITERATOR;
				}

				Key first() const { return ITERATOR->first; }
				T& second() const { return *(ITERATOR->second); }

				Iterator& operator ++() { ++ITERATOR; return *this; }
				Iterator& operator --() { --ITERATOR; return *this; }
			};

			Map() : DATA() {}
			Map(const Map< Key, T >& src) : DATA()
			{
				for(iterator it = src.DATA.begin(); it != src.DATA.end(); ++it)
					DATA.insert( val_type( it->first, new T( *(it->second) ) ) );
			}

			Map< Key, T >& operator =(const Map< Key, T >& src)
			{
				clear();

				for(iterator it = src.DATA.begin(); it != src.DATA.end(); ++it)
					DATA.insert( val_type( it->first, new T( *(it->second) ) ) );

				return *this;
			}

			void clear()
			{
				for(iterator it = DATA.begin(); it != DATA.end(); ++it) {
					// clog << dec << it->first << " " << hex << it->second << endl;
					delete it->second;
				}
				DATA.clear();
			}

			virtual ~Map() { clear(); }

			uint size() const { return DATA.size(); }

			bool exists(const Key& key) const { return DATA.find(key) != DATA.end(); }

			T& get(const Key& key) const
			{
				iterator it = DATA.find(key);
				if( it != DATA.end() ) return *(it->second);
				else throw invalid_argument("Key doesn't exist in Map.");
			}
			T& operator [](const Key& key) const
			{
				return *( DATA.find(key)->second );
			}

			// skips remove if doesn't exist
			void remove(const Key& key)
			{
				if( exists(key) ) {
					delete DATA[key];
					DATA.erase(key);
				}
			}

			bool empty() const { return DATA.empty(); }

		// param "data" must be a "*new"ed value
			void add(const Key& key, T& data)
			{
				if( exists(key) ) remove(key); // throw "Item already exists.";

				DATA.insert( val_type(key, &data) );
			}

			Iterator begin() const { return Iterator( *this, DATA.begin() ); }
			Iterator end  () const { return Iterator( *this, DATA.end  () ); }
		};

		class Binary;

		template< typename T >
		class Array
		{
		private:
			uint SIZE;
			T* DATA;
		public:
			Array() : SIZE(0), DATA(NULL) {}
			Array(uint size) : SIZE(size), DATA(new T[SIZE]) {}
			Array(const Array& array) : SIZE(array.SIZE), DATA(new T[SIZE])
			{
				memcpy(DATA, array.DATA, sizeof(T)*SIZE);
			}
			virtual ~Array()
			{
				if(DATA != NULL) {
					delete [] DATA;
					DATA = NULL;
				}
			}

			bool empty() { return SIZE == 0; }
			uint length() const { return SIZE; }

			const Binary& operator =(const Binary& bin);
			const Array< T >& operator =(const Array< T >& a)
			{
				this->SIZE = a.SIZE;

				if(DATA != NULL) delete [] DATA;
				this->DATA = new T[SIZE];
				memcpy(this->DATA, a.DATA, sizeof(T)*SIZE);

				return a;
			}

			T* getPtr() const { return DATA; }
			T* getPtr(uint index) const
			{
				if( index <= length() ) return &(DATA[index]);
				else throw out_of_range("Array out of range.");
			}

			void reset(uint size = 0)
			{
				if(DATA != NULL) delete [] DATA;
				SIZE = size;
				DATA = new T[SIZE];
			}
			void reset(uint size, T defVal)
			{
				reset(size);
				for(uint i = 0; i < SIZE; i++) DATA[i] = defVal;
			}
			void resize(uint size)
			{
				T* dst = new T[SIZE];
				if(DATA != NULL) {
					memcpy(dst, DATA, sizeof(T)*SIZE);
					delete [] DATA;
					DATA = NULL;
				}
				SIZE = size;
				DATA = dst;
			}
			void resize(uint size, T defVal)
			{
				uint preSize = length();
				resize(size);
				if(size > preSize)
					for(uint i = preSize; i < size; i++) DATA[i] = defVal;
			}

			void set(uint index, T data)
			{
				if( index >= length() ) throw "Invalid index.";
				else DATA[index] = data;
			}
			void add(T data)
			{
				resize(SIZE);
				set(SIZE-1, data);
			}
			T& operator [](uint index) const
			{
				if( index >= length() ) throw "Getting a reference out of bound.";
			#if defined __BIG_ENDIAN__
				uint8_t* p = getPtr(index);
				T ret = 0;
				for(int i = 0; i < sizeof(T); i++) ret |= p[i]<<(8*i);
				return ret;
			#else
				return DATA[index];
			#endif
			}

			bool contains(T val)
			{
				for(uint i = 0; i < SIZE; i++) if( (*this)[i] == val) return true;
				return false;
			}
		};

		class Element;
		class Descriptor;

		class Binary : public Array< uint8_t >
		{
		public:
			Binary(Element& e, const Descriptor& info);
			Binary(Element& e, const Descriptor& info, StreamReader& f);
			Binary(Element& e, const Descriptor& info, Binary& b);

			Binary() : Array< uint8_t >() {}
			Binary(uint size) : Array< uint8_t >(size) {}
			Binary(uint8_t* data, uint size) : Array< uint8_t >(size)
			{
				reset(size);
				memcpy(getPtr(), data, size);
			}
			Binary(const Binary& b) : Array< uint8_t >(b) {}
			Binary(string str) { setString(str); }

			bool isNumber() const;
			// bool is(Type t);
		// setter
			void setString(string str);
			void setNumber(int    num);
			void setBool  (bool     b);
			void setDouble(double   d);
		// converter
			string toString() const { return string( (char*)getPtr(), length() ); }
			int    toNumber() const;
			bool   toBool  () const;
			double toDouble() const;
		// operator wrap of converter
			operator string() const { return toString(); }
			operator int   () const { return toNumber(); }
			operator bool  () const { return toBool  (); }
			operator double() const { return toDouble(); }
		// operator wrap of setter
			string operator =(string src) { setString(src); return src; }
			int    operator =(int    src) { setNumber(src); return src; }
			bool   operator =(bool   src) { setBool  (src); return src; }
			double operator =(double src) { setDouble(src); return src; }

			const Binary& toBinary() { return *this; }

/*
			template< typename T >
			const Array< T >& operator =(const Array< T >& array)
			{
				reset( sizeof(T)*array.length() );
				memcpy( this->getPtr(), array.getPtr(), sizeof(T)*array.length() );
				return array;
			}
 */
			template< typename T >
			void operator =(const vector< T >& v)
			{
				reset( sizeof(T)*v.size() );
#if defined __BIG_ENDIAN__
	#error ENDIAN CONVERT UNIMPLEMENTED
//#elif defined __LITTLE_ENDIAN__
#else
				memcpy( getPtr(), &(v[0]), sizeof(T)*v.size() );
/*
#else
	#error UNSUPPORTED ENDIAN
 */
#endif
			}
			template< typename T >
			operator vector< T >()
			{
				vector< T > v;

				if( ( length() % sizeof(T) ) != 0 ) throw "Convert failed.";
				v.resize( length() / sizeof(T) );
#if defined __BIG_ENDIAN__
	#error ENDIAN CONVERT UNIMPLEMENTED
//#elif defined __LITTLE_ENDIAN__
#else
				memcpy( &(v[0]), getPtr(), length() );
/*
#else
	#error UNSUPPORTED ENDIAN
 */
#endif
				return v;
			}
		};

		template< typename T >
		const Binary& Array< T >::operator =(const Binary& bin)
		{
			if( ( sizeof(T)*length() ) == bin.length() ) {
				memcpy( this->getPtr(), bin.getPtr(), sizeof(T)*bin.length() );
			} else reset(0);

			return bin;
		}

	}; // namespace structure
}; // namespace rpg2kLib

#endif
