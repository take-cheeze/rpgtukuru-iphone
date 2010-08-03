#ifndef _INC__RPG2K__MODEL__ARRAY_2D_HPP
#define _INC__RPG2K__MODEL__ARRAY_2D_HPP

#include "Map.hpp"
#include "Event.hpp"


namespace rpg2k
{
	class Binary;
	namespace structure
	{
		class Array1D;
		class Descriptor;
		class Element;
		class StreamReader;
		class StreamWriter;

		class Array2D
		{
		private:
			Map< uint, Array1D > data_;
			ArrayDefine arrayDefine_;

			Element* this_;
		protected:
			Array2D();
			void init(StreamReader& f);
			bool isInvalidArray2D(Binary const& b);
		public:
			typedef Map< uint, Array1D >::Iterator Iterator;
			typedef Map< uint, Array1D >::ReverseIterator ReverseIterator;

			Array2D(Array2D const& array);

			Array2D(ArrayDefine info);
			Array2D(ArrayDefine info, StreamReader& s);
			Array2D(ArrayDefine info, Binary const& b);

			Array2D(Element& e, Descriptor const& info);
			Array2D(Element& e, Descriptor const& info, StreamReader& s);
			Array2D(Element& e, Descriptor const& info, Binary const& b);

			virtual ~Array2D();

			Array2D& operator =(Array2D const& src);

			Array1D& operator [](uint index);
			Array1D const& operator [](uint index) const
			{
				return const_cast< Array2D& >(*this)[index];
			}

			bool exists(uint index) const;
			bool exists(uint index1, uint index2) const;

			void remove(uint index) { data_.remove(index); }
			void clear() { data_.clear(); }

			ArrayDefine getArrayDefine() const { return arrayDefine_; }

			bool isElement() const { return this_ != NULL; }
			Element& toElement() const;

			uint getExistence() const;
			uint serializedSize() const;
			void serialize(StreamWriter& s) const;

			Iterator begin() const { return data_.begin(); }
			Iterator end  () const { return data_.end  (); }
			ReverseIterator rbegin() const { return data_.rbegin(); }
			ReverseIterator rend  () const { return data_.rend  (); }
		}; // class Array2D
	} // namespace structure
} // namespace rpg2k

#endif
