#ifndef _INC__RPG2K__MODEL_HPP
#define _INC__RPG2K__MODEL_HPP

#include <fstream>
#include <list>

#include "Element.hpp"

namespace rpg2kLib
{
	namespace model
	{

		class BerEnum : public std::vector< uint >
		{
		private:
			structure::Binary BIN_DATA;
		protected:
			void init(structure::StreamReader& s);
		public:
			BerEnum(structure::Element& e, const structure::Descriptor& info) {}
			BerEnum(structure::Element& e, const structure::Descriptor& info, structure::StreamReader& s);
			BerEnum(structure::Element& e, const structure::Descriptor& info, structure::Binary& b);

			virtual ~BerEnum() {}

			const structure::Binary& toBinary();
		};

		class Sound : protected structure::Array1D
		{
		public:
			Sound(structure::Element& e, const structure::Descriptor& info) : Array1D(e, info) {}
			Sound(structure::Element& e, const structure::Descriptor& info, structure::StreamReader& s) : Array1D(e, info, s) {}
			Sound(structure::Element& e, const structure::Descriptor& info, structure::Binary& b) : Array1D(e, info, b) {}

			virtual ~Sound() {}

			using Array1D::operator =;

			using Array1D::toBinary;
			using Array1D::operator [];

			using Array1D::substantiate;
			using Array1D::exists;

			std::string fileName() { return (*this)[1]; }
			int volume () { return (*this)[3]; }
			int tempo  () { return (*this)[4]; }
			int balance() { return (*this)[5]; }

			operator std::string() { return fileName(); }
		};

		class Music : protected structure::Array1D
		{
		public:
			Music(structure::Element& e, const structure::Descriptor& info) : Array1D(e, info) {}
			Music(structure::Element& e, const structure::Descriptor& info, structure::StreamReader& s) : Array1D(e, info, s) {}
			Music(structure::Element& e, const structure::Descriptor& info, structure::Binary& b) : Array1D(e, info, b) {}

			virtual ~Music() {}

			using Array1D::operator =;

			using Array1D::toBinary;
			using Array1D::operator [];

			using Array1D::substantiate;
			using Array1D::exists;

			std::string fileName() { return (*this)[1]; }
			int fadeInTime() { return (*this)[2]; }
			int volume () { return (*this)[3]; }
			int tempo  () { return (*this)[4]; }
			int balance() { return (*this)[5]; }

			operator std::string() { return fileName(); }
		};

		class EventState : protected structure::Array1D
		{
		public:
			EventState(structure::Element& e, const structure::Descriptor& info) : Array1D(e, info) {}
			EventState(structure::Element& e, const structure::Descriptor& info, structure::StreamReader& s) : Array1D(e, info, s) {}
			EventState(structure::Element& e, const structure::Descriptor& info, structure::Binary& b) : Array1D(e, info, b) {}

			virtual ~EventState() {}

			using Array1D::operator =;

			using Array1D::toBinary;
			using Array1D::operator [];

			using Array1D::substantiate;
			using Array1D::exists;

			int mapID() { return (*this)[11]; }
			int x() { return (*this)[12]; }
			int y() { return (*this)[13]; }

			std::string charSet() { return (*this)[73]; }
			int charSetPos() { return (*this)[74]; }
			int charSetPat() { return (*this)[75]; }

			EventDir eventDir() { return (EventDir) (int)(*this)[21]; }
			EventDir  talkDir() { return (EventDir) (int)(*this)[22]; }

			CharSetDir charSetDir();
		};

		class Base
		{
		private:
			bool EXISTS;

			std::string FILE_DIR, FILE_NAME;
			structure::Map< uint, structure::Element > DATA;
		protected:
			void setFileName(std::string name) { FILE_NAME = name; }
			structure::Map< uint, structure::Element >& getData() { return DATA; }
			const structure::Map< uint, structure::Element >& getData() const { return DATA; }

			void checkExists();

			virtual void init() = 0;

			virtual std::string getHeader() const = 0;
			virtual std::string defaultName() const { return ""; }

			void load();

			std::string getFileName() const { return FILE_DIR + PATH_SEPR + FILE_NAME; }
			std::string getDirectory() const { return FILE_DIR; }

			structure::Map< uint, structure::Descriptor >& getDescriptor() const;
		public:
			Base(std::string dir);
			Base(std::string dir, std::string name);
			virtual ~Base();

			bool exists() { return EXISTS; }

			virtual void save();

			structure::Element& operator [](uint index);
			const structure::Element& operator [](uint index) const;
		};

		class DefineLoader
		{
		private:
			static const std::string DEFINE_DIR;

			structure::Map< std::string, structure::Map< uint, structure::Descriptor > > DEFINE_BUFF;
			std::map< std::string, bool > IS_ARRAY;
		protected:
			enum TokenType {
				O_INDEX = 0, INDEX, C_INDEX1, C_INDEX2,
				TYPE, NAME, EQUALS, DEFAULT,
				O_STRUCT, C_STRUCT,
				EXP_END,
			};
			void parse(structure::Map< uint, structure::Descriptor >& res, const std::list< std::string >& token);

			structure::Map< uint, structure::Descriptor >& load(std::string name);

			DefineLoader();
			DefineLoader(const DefineLoader& dl);

			~DefineLoader();
		public:
			static DefineLoader& getInstance();

			structure::Map< uint, structure::Descriptor >& get(std::string name);
			structure::ArrayDefine getArrayDefine(std::string name);

			bool isArray(std::string typeName) const
			{
				return IS_ARRAY.find(typeName) != IS_ARRAY.end();
			}

			static int toNumber(std::string str);
			static bool toBool(std::string str);

			static void toToken(std::list< std::string >& token, std::ifstream& stream);
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
