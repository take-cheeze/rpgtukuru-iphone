#ifndef _INC__RPG2K__MODEL__ARRAY_1D_HPP
#define _INC__RPG2K__MODEL__ARRAY_1D_HPP

#include <map>
#if RPG2K_DEBUG
	#include <boost/ptr_container/ptr_map.hpp>
#else
	#include <boost/ptr_container/ptr_unordered_map.hpp>
#endif
#include "Descriptor.hpp"


namespace rpg2k
{
	namespace structure
	{
		class Array2D;
		class Element;
		class Music;
		class EventState;
		class Sound;
		class StreamReader;
		class StreamWriter;

		#if RPG2K_DEBUG
			typedef boost::ptr_map<unsigned, Element> BaseOfArray1D;
		#else
			typedef boost::ptr_unordered_map<unsigned, Element> BaseOfArray1D;
		#endif

		class Array1D : public BaseOfArray1D
		{
		private:
			std::map< unsigned, Binary > binBuf_;
			ArrayDefine arrayDefine_;

			Element* const this_;

			bool exists_;
			Array2D* const owner_;
			unsigned const index_;

			enum { ARRAY_1D_END = 0, };
		protected:
			Array1D();

			void init(StreamReader& s);
			bool createAt(unsigned pos);
		public:
			typedef iterator Iterator;
			typedef reverse_iterator RIterator;
			typedef const_iterator ConstIterator;
			typedef const_reverse_iterator ConstRIterator;

			Array1D(Array1D const& array);

			Array1D(ArrayDefine info);
			Array1D(ArrayDefine info, StreamReader& s);
			Array1D(ArrayDefine info, Binary const& b);

			Array1D(Element& e);
			Array1D(Element& e, StreamReader& s);
			Array1D(Element& e, Binary const& b);

			Array1D(Array2D& owner, unsigned index);
			Array1D(Array2D& owner, unsigned index, StreamReader& f);

			Array1D const& operator =(Array1D const& src);

			bool isArray2D() const { return owner_ != NULL; }
			unsigned const& index() const;
			Array2D& owner();
			Array2D const& owner() const;

			Element& operator [](unsigned index);
			Element const& operator [](unsigned index) const;

			bool exists() const;
			bool exists(unsigned index) const;

			unsigned count() const;
			unsigned serializedSize() const;
			void serialize(StreamWriter& s) const;

			void substantiate();

			ArrayDefine arrayDefine() const { return arrayDefine_; }

			bool isElement() const;
			Element& toElement() const;
		}; // class Array1D
	} // namespace structure
} // namespace rpg2k

#endif
