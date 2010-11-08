#ifndef _INC__RPG2K__MODEL__ARRAY_2D_HPP
#define _INC__RPG2K__MODEL__ARRAY_2D_HPP

#include <boost/ptr_container/ptr_map.hpp>
#include "Descriptor.hpp"


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

		typedef boost::ptr_multimap<unsigned, Array1D> BaseOfArray2D;

		class Array2D : public BaseOfArray2D
		{
		private:
			ArrayDefine arrayDefine_;

			Element* const this_;
		protected:
			Array2D();
			void init(StreamReader& f);
			bool isInvalidArray2D(Binary const& b);
		public:
			typedef iterator Iterator;
			typedef reverse_iterator RIterator;
			typedef const_iterator ConstIterator;
			typedef const_reverse_iterator ConstRIterator;

			Array2D(Array2D const& array);

			Array2D(ArrayDefine info);
			Array2D(ArrayDefine info, StreamReader& s);
			Array2D(ArrayDefine info, Binary const& b);

			Array2D(Element& e);
			Array2D(Element& e, StreamReader& s);
			Array2D(Element& e, Binary const& b);

			Array2D const& operator =(Array2D const& src);

			Array1D& operator [](unsigned index);
			Array1D const& operator [](unsigned index) const;

			bool exists(unsigned index) const;
			bool exists(unsigned index1, unsigned index2) const;

			ArrayDefine arrayDefine() const { return arrayDefine_; }

			bool isElement() const { return this_ != NULL; }
			Element& toElement() const;

			unsigned count() const;
			unsigned serializedSize() const;
			void serialize(StreamWriter& s) const;
		}; // class Array2D
	} // namespace structure
} // namespace rpg2k

#endif
