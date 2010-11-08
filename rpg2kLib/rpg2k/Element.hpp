#ifndef _INC__RPG2K__MODEL__ELEMENT_HPP
#define _INC__RPG2K__MODEL__ELEMENT_HPP

#include <vector>

#include "Descriptor.hpp"
#include "Structure.hpp"


namespace rpg2k
{
	namespace structure
	{
		class StreamReader;
		class StreamWriter;

		class Element
		{
		private:
			Descriptor const* const descriptor_;

			bool exists_;

			Element* const owner_;
			unsigned const index1_;
			unsigned const index2_;

			union {
				#define PP_types(TYPE) TYPE* TYPE##_;
				PP_basicTypes(PP_types)
				PP_rpg2kTypes(PP_types)
				#undef PP_types
			} impl_;

			Binary binData_;

			void init();
			void init(Binary const& b);
			void init(StreamReader& s);
		public:
			Element();
			Element(Element const& e);
			Element(Descriptor const& info);
			Element(Descriptor const& info, Binary const& b);
			Element(Descriptor const& info, StreamReader& s);

			Element(Array1D const& owner, unsigned index);
			Element(Array1D const& owner, unsigned index , Binary const& b);
			Element(Array2D const& owner, unsigned index1, unsigned index2);
			Element(Array2D const& owner, unsigned index1, unsigned index2, Binary const& b);

			bool isDefined() const { return descriptor_ != NULL; }
			bool hasOwner() const { return owner_ != NULL; }
			bool exists() const { return exists_; }

			~Element();

			Descriptor const& descriptor() const;

			unsigned serializedSize() const;
			void serialize(StreamWriter& s) const;
			Binary serialize() const;

			#define PP_castOperator(TYPE) \
				operator TYPE const&() const; \
				operator TYPE&(); \
				TYPE& to_##TYPE() { return to<TYPE>(); } \
				TYPE const& to_##TYPE() const { return to<TYPE>(); }
			PP_basicTypes(PP_castOperator)
			#undef PP_castOperator

			#define PP_castOperator(TYPE) \
				operator TYPE const&() const; \
				operator TYPE&(); \
				TYPE& to##TYPE() { return to<TYPE>(); } \
				TYPE const& to##TYPE() const { return to<TYPE>(); }
			PP_rpg2kTypes(PP_castOperator)
			#undef PP_castOperator

			#define PP_castOperator(type) \
				operator type const&() const { return reinterpret_cast<type const&>(toArray1D()); } \
				operator type&() { return reinterpret_cast<type&>(toArray1D()); } \
				type& to##type() { return to<type>(); } \
				type const& to##type() const { return to<type>(); }
			PP_castOperator(Music)
			PP_castOperator(Sound)
			PP_castOperator(EventState)
			#undef PP_castOperator

			operator unsigned&() { return reinterpret_cast<unsigned&>( to<int>() ); }
			operator unsigned const&() const { return reinterpret_cast<unsigned const&>( to<int>() ); }

			void substantiate();

			template<typename T>
			T& to() { return static_cast<T&>(*this); }
			template<typename T>
			T const& to() const { return static_cast<T const&>(*this); }
			template<typename T>
			void assign(T const& src) 
			{
				static_cast<T&>(*this) = src;
				this->substantiate();
			}
			template<typename T>
			T const& operator =(T const& src) { this->assign(src); return static_cast<T&>(*this); }

			Element const& operator =(Element const& src);

			Element& owner();
			Element const& owner() const;

			unsigned index1() const;
			unsigned index2() const;
		}; // class Element

		template< > inline
		unsigned const& Element::operator =(unsigned const& num) { (*this) = int(num); return *this; }
		template< > inline
		unsigned const& Element::to<unsigned>() const { return reinterpret_cast< unsigned const& >( to<int>() ); }
		template< > inline
		unsigned& Element::to<unsigned>() { return reinterpret_cast< unsigned& >( to<int>() ); }

		class BerEnum : public std::vector<unsigned>
		{
		protected:
			void init(StreamReader& s);
		public:
			BerEnum() {}
			BerEnum(StreamReader& s);
			BerEnum(Binary const& b);

			unsigned serializedSize() const;
			void serialize(StreamWriter& s) const;
		}; // class BerEnum
	} // namespace structure
} // namespace rpg2k

#endif
