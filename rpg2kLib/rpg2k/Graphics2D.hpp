#ifndef _INC_RPG2K__VIEW__GRAPHICS_2D__HPP
#define _INC_RPG2K__VIEW__GRAPHICS_2D__HPP

#include "Font.hpp"
#include "Map.hpp"
#include "Material.hpp"

#if RPG2K_IS_IPHONE
	#include <OpenGLES/ES1/gl.h>
#elif RPG2K_IS_PSP
	#include <GLES/gl.h>
#else
	#include <SDL/SDL_opengl.h>
#endif


namespace rpg2k
{
	namespace structure
	{
		class EventState;
	}

	/*
	 * trying to use texture like image
	 */
	class Image
	{
	private:
		GLuint const name_;
		Size2D const size_;
	protected:
		Image(Image const& src);
		Image const& operator =(Image const& src);
	public:
		Image(GLuint n, uint w, uint h);
		Image(GLuint n, Size2D const& size);
		~Image();

		GLuint name() const { return name_; }
		Size2D const& size() const { return size_; }
		uint  width() const { return size_[0]; }
		uint height() const { return size_[1]; }

		uint textureWidth () const;
		uint textureHeight() const;

		class Loader
		{
		protected:
			static uint const TRANS_COLOR = 0;
		public:
			virtual uint8_t* load(SystemString const& filename, int& width, int& height, int& channels, bool trans) = 0;
			virtual void free(uint8_t* data) = 0;
		}; // class Loader
		class xyz : public Loader
		{
		public:
			uint8_t* load(SystemString const& filename, int& width, int& height, int& channels, bool trans);
			void free(uint8_t* data);
		}; // class xyz
	}; // class Image

	class Main;

	#define PP_allImageType(func) func(bmp) func(png) func(xyz) func(jpg) func(dds) func(tga)
	class Graphics2D
	{
	public:
		#define PP_enumImageType(name) IMG_##name,
		enum ImageType { PP_allImageType(PP_enumImageType) IMG_END, };
		#undef PP_enumImageType
	private:
		friend class ::rpg2k::Main;

		static const Vector2D SHADE_P;
		static const Size2D FONT_COLOR_S;
		static const int FONT_COLOR_ROW = 10;

		static const uint CURSOR_INTERVAL = 15;

		static const int CORNER = 8, NON_FRAME = 16, FRAME_W = 8, FRAME_L = 16;
		static const int C_AND_N = CORNER + NON_FRAME;
		enum WindowType { WINDOW = 1, CURSOR_0, CURSOR_1, };
		void drawWindowType(Vector2D const& p, Size2D const& size, WindowType type);

		Main& owner_;

		structure::Map< Material::Type, structure::Map< RPG2kString, Image > > imagePool_;
		structure::Map< bool, structure::Map< RPG2kString, Image > > picturePool_;

		std::vector< SystemString > imageExt_;
	protected:
		// method for getting back to title
		void gotoTitle();
		void swapBuffers();

		std::auto_ptr< Image > loadImage(Material::Type type, RPG2kString const& name, bool trans) const;

		Image const& getSystemGraphic();

		void drawBlockA_B(Image const& src, Vector2D const& dstP, int x, int y, int z, int anime);
		void drawBlockD(Image const& src, Vector2D const& dstP, int x, int y);

		bool interval() const;

		Main& getOwner() const { return owner_; }
	public:
		Graphics2D(Main& m);
		~Graphics2D();

		void drawImage(Image const& img, Vector2D const& dstP, Size2D const& dstS, Vector2D const& srcP, Size2D const& srcS);
		void drawImage(Image const& img, Vector2D const& dstP, Vector2D const& srcP, Size2D const& srcS)
		{
			drawImage(img, dstP, srcS, srcP, srcS);
		}
		void drawImage(Image const& img, Vector2D const& dstP)
		{
			drawImage( img, dstP, img.size(), Vector2D(0, 0), img.size() );
		}

		void drawImage(Image const& img, Vector2D const (&dstP)[4], ColorRGBA const& color);

		void drawPixel(Image const& img, Vector2D const& dstP, Size2D const& dstS, Vector2D const& srcP);
		void drawPixel(Image const& img, Vector2D const& dstP, Vector2D const& srcP);

		// returns width of drawn font
		uint drawChar(RPG2kString::const_iterator& it, Vector2D const& p, uint color);

		void drawString(RPG2kString const& str, Vector2D const& p, uint color = font::FNT_NORMAL);
		void drawWindow(Vector2D const& p, Size2D const& size) { drawWindowType(p, size, WINDOW); }
		void drawCursor(Vector2D const& p, Size2D const& size) { drawWindowType( p, size, interval() ? CURSOR_0 : CURSOR_1 ); }
		void drawCursorPassive(Vector2D const& p, Size2D const& size) { drawWindowType(p, size, CURSOR_0); }
		void drawCursorActive (Vector2D const& p, Size2D const& size) { drawWindowType(p, size, CURSOR_1); }
		void drawMoneyWindow(Vector2D const& p);

		Image const& getImage(Material::Type type, RPG2kString const& name);
		Image const& getPicture(RPG2kString const& name, bool trans);

		void clear();

		void drawWallpaper(Vector2D const& dstP, Size2D const& dstS);

		void drawScrollUp(Vector2D const& dstP)
		{
			if( interval() )
				drawImage( getSystemGraphic(), dstP, Vector2D(40, 16), SCROLL_SIZE );
		}
		void drawScrollDw(Vector2D const& dstP)
		{
			if( interval() )
				drawImage( getSystemGraphic(), dstP, Vector2D(40,  8), SCROLL_SIZE );
		}

		void drawChipSet(Vector2D const& dstP, int chipID);
		void drawCharSet(structure::EventState& state, Vector2D const& dstP);
		void drawFaceSet(RPG2kString const& faceSet, uint faceSetPos, Vector2D const& dstP);

		void clip(Vector2D const& coord, Size2D const& size);
		void unclip();
	}; // class Graphics2D
} // namespace rpg2k

#endif
