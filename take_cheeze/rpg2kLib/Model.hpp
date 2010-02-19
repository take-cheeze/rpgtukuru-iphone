#ifndef _INC__RPG2K__MODEL_HPP
#define _INC__RPG2K__MODEL_HPP

#include <fstream>
#include <list>

#include "Element.hpp"

namespace rpg2kLib
{
	namespace model
	{
		using namespace structure;

		class BerEnum : public vector< uint >
		{
		private:
			Binary BIN_DATA;
		protected:
			void init(StreamReader& s);
		public:
			BerEnum(Element& e, const Descriptor& info) {}
			BerEnum(Element& e, const Descriptor& info, StreamReader& s);
			BerEnum(Element& e, const Descriptor& info, Binary& b);

			virtual ~BerEnum() {}

			const Binary& toBinary();
		};

		class Sound : protected Array1D
		{
		public:
			Sound(Element& e, const Descriptor& info) : Array1D(e, info) {}
			Sound(Element& e, const Descriptor& info, StreamReader& s) : Array1D(e, info, s) {}
			Sound(Element& e, const Descriptor& info, Binary& b) : Array1D(e, info, b) {}

			virtual ~Sound() {}

			using Array1D::operator =;

			using Array1D::toBinary;
			using Array1D::operator [];

			using Array1D::substantiate;
			using Array1D::exists;

			string fileName() {return (*this)[1]; }
			int volume () { return (*this)[3]; }
			int tempo  () { return (*this)[4]; }
			int balance() { return (*this)[5]; }

			operator string() { return fileName(); }
		};

		class Music : protected Array1D
		{
		public:
			Music(Element& e, const Descriptor& info) : Array1D(e, info) {}
			Music(Element& e, const Descriptor& info, StreamReader& s) : Array1D(e, info, s) {}
			Music(Element& e, const Descriptor& info, Binary& b) : Array1D(e, info, b) {}

			virtual ~Music() {}

			using Array1D::operator =;

			using Array1D::toBinary;
			using Array1D::operator [];

			using Array1D::substantiate;
			using Array1D::exists;

			string fileName() { return (*this)[1]; }
			int fadeInTime() { return (*this)[2]; }
			int volume () { return (*this)[3]; }
			int tempo  () { return (*this)[4]; }
			int balance() { return (*this)[5]; }

			operator string() { return fileName(); }
		};

		class EventState : protected Array1D
		{
		public:
			EventState(Element& e, const Descriptor& info) : Array1D(e, info) {}
			EventState(Element& e, const Descriptor& info, StreamReader& s) : Array1D(e, info, s) {}
			EventState(Element& e, const Descriptor& info, Binary& b) : Array1D(e, info, b) {}

			virtual ~EventState() {}

			using Array1D::operator =;

			using Array1D::toBinary;
			using Array1D::operator [];

			using Array1D::substantiate;
			using Array1D::exists;

			int mapID() { return (*this)[11]; }
			int x() { return (*this)[12]; }
			int y() { return (*this)[13]; }

			string charSet() { return (*this)[73]; }
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

			string FILE_DIR, FILE_NAME;
			Map< uint, Element > DATA;
		protected:
			void setFileName(string name) { FILE_NAME = name; }
			Map< uint, Element >& getData() { return DATA; }
			const Map< uint, Element >& getData() const { return DATA; }

			void checkExists();

			virtual void init() = 0;

			virtual string getHeader() const = 0;
			virtual string defaultName() const { return ""; }

			void load();

			string getFileName() const { return FILE_DIR + PATH_SEPR + FILE_NAME; }
			string getDirectory() const { return FILE_DIR; }

			Map< uint, Descriptor >& getDescriptor() const;
		public:
			Base(string dir);
			Base(string dir, string name);
			virtual ~Base();

			bool exists() { return EXISTS; }

			virtual void save();

			Element& operator [](uint index);
			const Element& operator [](uint index) const;
		};

		class DefineLoader
		{
		private:
			static const string DEFINE_DIR;

			Map< string, Map< uint, Descriptor > > DEFINE_BUFF;
			map< string, bool > IS_ARRAY;
		protected:
			enum TokenType {
				O_INDEX = 0, INDEX, C_INDEX1, C_INDEX2,
				TYPE, NAME, EQUALS, DEFAULT,
				O_STRUCT, C_STRUCT,
				EXP_END,
			};
			void parse(Map< uint, Descriptor >& res, const list< string >& token);

			Map< uint, Descriptor >& load(string name);

			DefineLoader();
			DefineLoader(const DefineLoader& dl);

			~DefineLoader();
		public:
			static DefineLoader& getInstance();

			Map< uint, Descriptor >& get(string name);
			ArrayDefine getArrayDefine(string name);

			bool isArray(string typeName) const
			{
				return IS_ARRAY.find(typeName) != IS_ARRAY.end();
			}

			static int toNumber(string str);
			static bool toBool(string str);

			static void toToken(list< string >& token, ifstream& stream);
		};

	}; // namespace model
}; // namespace rpg2kLib

#endif
