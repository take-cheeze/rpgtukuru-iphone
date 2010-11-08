#ifndef _INC__RPG2K__MODEL_HPP
#define _INC__RPG2K__MODEL_HPP

#include <algorithm>
#include <deque>
#include <fstream>
#include <map>
#include <set>

#include "Array1D.hpp"
#include "Array2D.hpp"
#include "Element.hpp"

#include <boost/ptr_container/ptr_vector.hpp>


namespace rpg2k
{
	namespace structure { class StreamWriter; }

	namespace model
	{
		bool fileExists(SystemString const& fileName);

		class Base
		{
		private:
			bool exists_;

			SystemString fileDir_, fileName_;
			boost::ptr_vector<structure::Element> data_;

			virtual void loadImpl() = 0;
			virtual void saveImpl() = 0;

			virtual char const* header() const = 0;
			virtual char const* defaultName() const = 0;
		protected:
			void setFileName(SystemString const& name) { fileName_ = name; }
			boost::ptr_vector<structure::Element>& data() { return data_; }
			boost::ptr_vector<structure::Element> const& data() const { return data_; }

			void checkExists();

			boost::ptr_vector<structure::Descriptor> const& descriptor() const;

			Base(SystemString const& dir);
			Base(SystemString const& dir, SystemString const& name);

			Base(Base const& src);
			Base const& operator =(Base const& src);

			void load();
		public:
			virtual ~Base() {}

			bool exists() const { return exists_; }

			void reset();

			structure::Element& operator [](unsigned index);
			structure::Element const& operator [](unsigned index) const;

			SystemString const& fileName() const { return fileName_; }
			SystemString const& directory() const { return fileDir_; }
			SystemString fullPath() const
			{
				return SystemString(fileDir_).append(PATH_SEPR).append(fileName_);
			} // not absolute

			void saveAs(SystemString const& filename);
			void save() { saveAs( fullPath() ); }

			void serialize(structure::StreamWriter& s);
		}; // class Base

		class DefineLoader
		{
		private:
			typedef std::map< RPG2kString, boost::ptr_vector<structure::Descriptor> > DefineBuffer;
			DefineBuffer defineBuff_;
			typedef std::map<RPG2kString, const char*> DefineText;
			DefineText defineText_;
			std::set<RPG2kString> isArray_;
		protected:
			void parse(boost::ptr_vector<structure::Descriptor>& dst, std::deque<RPG2kString> const& token);
			void load(boost::ptr_vector<structure::Descriptor>& dst, RPG2kString const& name);

			DefineLoader();
			DefineLoader(DefineLoader const& dl);
		public:
			static DefineLoader& instance();

			boost::ptr_vector<structure::Descriptor> const& get(RPG2kString const& name);
			structure::ArrayDefine arrayDefine(RPG2kString const& name);

			bool isArray(RPG2kString const& typeName) const
			{
				return isArray_.find(typeName) != isArray_.end();
			}

			static void toToken(std::deque<RPG2kString>& token, std::istream& stream);
		}; // class DefineLoader
	} // namespace model
} // namespace rpg2k

#endif
