#ifndef _INC__RPG2K__MODEL_HPP
#define _INC__RPG2K__MODEL_HPP

#include <deque>
#include <fstream>
#include <map>
#include <set>

#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Element.hpp"
#include "Map.hpp"


namespace rpg2k
{
	namespace model
	{
		bool fileExists(SystemString const& fileName);

		typedef boost::shared_ptr< structure::Descriptor > DescriptorPointer;
		typedef boost::shared_ptr< structure::Element > ElementPointer;

		class Base
		{
		private:
			bool exists_;

			SystemString fileDir_, fileName_;
			std::deque< ElementPointer > data_;
		protected:
			void setFileName(SystemString const& name) { fileName_ = name; }
			std::deque< ElementPointer >& getData() { return data_; }
			std::deque< ElementPointer > const& getData() const { return data_; }

			void checkExists();

			virtual void load();

			virtual char const* getHeader() const = 0;
			virtual char const* defaultName() const { return ""; }

			std::deque< DescriptorPointer > const& getDescriptor() const;

			Base(SystemString const& dir);
			Base(SystemString const& dir, SystemString const& name);

			Base(Base const& src);
			Base& operator =(Base const& src);
		public:
			virtual ~Base();

			bool exists() const { return exists_; }

			void reset();

			virtual void save();

			structure::Element& operator [](uint index);
			structure::Element const& operator [](uint index) const;

			SystemString const& fileName() const { return fileName_; }
			SystemString const& directory() const { return fileDir_; }
			SystemString fullPath() const { return (fileDir_ + PATH_SEPR + fileName_); } // not absolute
		}; // class Base

		class DefineLoader
		{
		private:
			typedef structure::Map< uint, structure::Descriptor > ArrayDefineIntern;

			std::map< RPG2kString, std::deque< DescriptorPointer > > defineBuff_;
			std::map< RPG2kString, const char* > defineText_;
			std::set< RPG2kString > isArray_;
		protected:
			std::deque< DescriptorPointer > parse(std::deque< RPG2kString > const& token);
			std::deque< DescriptorPointer > load(RPG2kString const& name);

			DefineLoader();
			DefineLoader(DefineLoader const& dl);

			~DefineLoader();
		public:
			static DefineLoader& instance();

			std::deque< DescriptorPointer > const& get(RPG2kString const& name);
			structure::ArrayDefine getArrayDefine(RPG2kString const& name);
			structure::ArrayDefinePointer getArrayDefinePointer(RPG2kString const& name);

			bool isArray(RPG2kString const& typeName) const
			{
				return isArray_.find(typeName) != isArray_.end();
			}

			static void toToken(std::deque< RPG2kString >& token, std::istream& stream);
		}; // class DefineLoader
	} // namespace model
} // namespace rpg2k

#endif
