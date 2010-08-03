#include <SDL/SDL.h>
#include <zlib.h>

#include "Debug.hpp"
#include "Graphics2D.hpp"
#include "Main.hpp"
#include "Project.hpp"

#if RPG2K_IS_PSP || RPG2K_IS_IPHONE
	#define glOrtho glOrthof
#endif

extern "C"
{
	#include <SOIL.h>

	unsigned int
	SOIL_internal_create_OGL_texture
	(
		const unsigned char *const data,
		int width, int height, int channels,
		unsigned int reuse_texture_ID,
		unsigned int flags,
		unsigned int opengl_texture_type,
		unsigned int opengl_texture_target,
		unsigned int texture_check_size_enum
	);
	void SOIL_set_transparent(int trans);
	int SOIL_has_transparent();
}


namespace rpg2k
{
	Image::Image(GLuint n, uint w, uint h)
	: name_(n), size_(w, h)
	{
	}
	Image::Image(GLuint n, Size2D const& size)
	: name_(n), size_(size)
	{
	}
	Image::~Image()
	{
		glDeleteTextures(1, &name_);
	}

	uint Image::textureWidth () const
	{
		for(uint i = 2; true; i <<= 1) if( i >= width() ) return i;
	}
	uint Image::textureHeight() const
	{
		for(uint i = 2; true; i <<= 1) if( i >= height() ) return i;
	}

	Graphics2D::Graphics2D(Main& m)
	: owner_(m)
	{
		int flagRet = 0;
		flagRet = SDL_GL_SetAttribute(SDL_GL_RED_SIZE  , 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE , 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE  , 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE , 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
		flagRet = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		rpg2k_assert(flagRet == 0);
		bool res = SDL_SetVideoMode(SCREEN_SIZE[0], SCREEN_SIZE[1], 32, SDL_OPENGL); rpg2k_assert(res);
	// set matrix mode
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	// set clearing
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearStencil(0);
	// set matrix
		glViewport(0, 0, SCREEN_SIZE[0], SCREEN_SIZE[1]);
		glOrtho(0.0, SCREEN_SIZE[0], SCREEN_SIZE[1], 0.0, -1.0, 1.0);
	// set alpha blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// set image extension
		#define PP_enumImageExt(name) imageExt_.push_back(#name);
		PP_allImageType(PP_enumImageExt)
		#undef PP_enumImageExt
	// set buffer of image pool
		#define PP_addPool(name) imagePool_.add(Material::name);
		PP_transMaterial(PP_addPool)
		PP_nonTransMaterial(PP_addPool)
		#undef PP_addPool
	// set buffer of picture pool
		picturePool_.add(false);
		picturePool_.add(true );
	}
	Graphics2D::~Graphics2D()
	{
	}

	Image const& Graphics2D::getPicture(RPG2kString const& name, bool trans)
	{
		if( !picturePool_[trans].exists(name) ) {
			picturePool_[trans].addPointer( name, loadImage(Material::Picture, name, trans) );
		}

		return picturePool_[trans][name];
	}
	Image const& Graphics2D::getImage(Material::Type type, RPG2kString const& name)
	{
		if( !imagePool_[type].exists(name) ) {
			bool trans;

			switch(type) {
				#define PP_case(type) case Material::type:
				PP_transMaterial   (PP_case) trans = true ; break;
				PP_nonTransMaterial(PP_case) trans = false; break;
				#undef PP_case
				default: rpg2k_assert(false);
			}

			imagePool_[type].addPointer( name, loadImage(type, name, trans) );
		}
		return imagePool_[type][name];
	}

	boost::shared_ptr< Image > Graphics2D::loadImage(Material::Type type, RPG2kString const& name, bool trans) const
	{
		switch(type) {
			#define PP_case(type) case Material::type:
			PP_imageMaterial(PP_case) break;
			#undef PP_case
			default: rpg2k_assert(false);
		}

		for(int i = 0; i < Material::RES_END; i++) {
			for(int j = 0; j < IMG_END; j++) {
				SystemString filename =
					getOwner().getMaterial().getFileName( (Material::Resource)i, type, name, imageExt_[j] );

				unsigned char* img = NULL;
				int width, height, channels;

				if( j == IMG_xyz ) {
					img = Image::xyz().load(filename, width, height, channels, trans);
				} else {
					SOIL_set_transparent(trans);
					img = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
				}
				if(img == NULL) continue;

				GLuint texID = SOIL_internal_create_OGL_texture(
					img, width, height, channels,
					SOIL_CREATE_NEW_ID, SOIL_FLAG_TEXTURE_REPEATS,
					GL_TEXTURE_2D, GL_TEXTURE_2D,
					GL_MAX_TEXTURE_SIZE
				);

				if( j == IMG_xyz ) Image::xyz().free(img);
				else SOIL_free_image_data(img);

				cout << "Load Image success. NAME = " <<  name.toSystem() << ";" << endl;
				return boost::shared_ptr< Image >( new Image(texID, width, height) );
			}
		}

		throw std::runtime_error(
			"Cannot load Image. Material = " + getOwner().getMaterial().getName(type)
			+ "; NAME = \"" + name.toSystem() + "\";"
		);
	}

	void Graphics2D::drawImage(
		Image const& img,
		Vector2D const& dstP, Size2D const& dstS,
		Vector2D const& srcP, Size2D const& srcS
	) {
		GLshort vertexGL[ /* Vector2D::dimension() */ 2 * 4 ];
		vertexGL[Vector2D::dimension() * 0 + 0] = dstP[0];
		vertexGL[Vector2D::dimension() * 0 + 1] = dstP[1];
		vertexGL[Vector2D::dimension() * 1 + 0] = dstP[0] + dstS[0];
		vertexGL[Vector2D::dimension() * 1 + 1] = dstP[1];
		vertexGL[Vector2D::dimension() * 2 + 0] = dstP[0] + dstS[0];
		vertexGL[Vector2D::dimension() * 2 + 1] = dstP[1] + dstS[1];
		vertexGL[Vector2D::dimension() * 3 + 0] = dstP[0];
		vertexGL[Vector2D::dimension() * 3 + 1] = dstP[1] + dstS[1];
		GLshort coordGL[ /* Vector2D::dimension() */ 2 * 4 ];
		coordGL[Vector2D::dimension() * 0 + 0] = srcP[0];
		coordGL[Vector2D::dimension() * 0 + 1] = srcP[1];
		coordGL[Vector2D::dimension() * 1 + 0] = srcP[0] + srcS[0];
		coordGL[Vector2D::dimension() * 1 + 1] = srcP[1];
		coordGL[Vector2D::dimension() * 2 + 0] = srcP[0] + srcS[0];
		coordGL[Vector2D::dimension() * 2 + 1] = srcP[1] + srcS[1];
		coordGL[Vector2D::dimension() * 3 + 0] = srcP[0];
		coordGL[Vector2D::dimension() * 3 + 1] = srcP[1] + srcS[1];

		GLubyte indicesGL[4];
		for(int i = 0; i < 4; i++) indicesGL[i] = i;

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glOrtho( 0.0, img.width()*2, 0.0, img.height()*2, -1.0, 1.0 );
			glMatrixMode(GL_MODELVIEW);

			glBindTexture( GL_TEXTURE_2D, img.name() );
			glVertexPointer( Vector2D::dimension(), GL_SHORT, 0, vertexGL );
			glTexCoordPointer( Vector2D::dimension(), GL_SHORT, 0, coordGL );
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, indicesGL);
		glPopMatrix();
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	void Graphics2D::drawImage(Image const& img, Vector2D const (&dstP)[4], ColorRGBA const& color)
	{
		GLshort vertexGL[2 * 4];
		GLfloat coordGL[2 * 4] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f,
		};
		GLubyte indicesGL[4];
		GLfloat colorGL[4 * 4];
		for(int i = 0; i < 4; i++) {
			indicesGL[i] = i;
			for(uint j = 0; j < Vector2D::dimension(); j++) {
				vertexGL[Vector2D::dimension()*i + j] = dstP[i][j];
			}
			for(uint j = 0; j < color.dimension(); j++) {
				colorGL[i*color.dimension() + j] = color[j];
			}
		}

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		{
			glBindTexture( GL_TEXTURE_2D, img.name() );
			glVertexPointer( Vector2D::dimension(), GL_SHORT, 0, vertexGL );
			glTexCoordPointer( Vector2D::dimension(), GL_FLOAT, 0, coordGL );
			glColorPointer( ColorRGBA::dimension(), GL_FLOAT, 0, colorGL);
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, indicesGL);
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	void Graphics2D::drawPixel(Image const& img, Vector2D const& dstP, Vector2D const& srcP)
	{
		GLshort vertexGL[2];
		vertexGL[0] = dstP[0]; vertexGL[1] = dstP[1];

		GLfloat coordGL[2];
		coordGL[0] = srcP[0]; coordGL[1] = srcP[1];

		GLubyte indicesGL[1];
		indicesGL[0] = 0;
		coordGL[0] /= img.width(); coordGL[1] /= img.height();

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_TEXTURE_2D);
		{
			glDisable(GL_POINT_SMOOTH);
			glBindTexture( GL_TEXTURE_2D, img.name() );
			glVertexPointer( Vector2D::dimension(), GL_SHORT, 0, vertexGL );
			glTexCoordPointer( Vector2D::dimension(), GL_FLOAT, 0, coordGL );
			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_BYTE, indicesGL);
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}
	void Graphics2D::drawPixel(Image const& img, Vector2D const& dstP, Size2D const& dstS, Vector2D const& srcP)
	{
		GLshort vertexGL[2 * 4];
		vertexGL[Vector2D::dimension() * 0 + 0] = dstP[0];
		vertexGL[Vector2D::dimension() * 0 + 1] = dstP[1];
		vertexGL[Vector2D::dimension() * 1 + 0] = dstP[0] + dstS[0];
		vertexGL[Vector2D::dimension() * 1 + 1] = dstP[1];
		vertexGL[Vector2D::dimension() * 2 + 0] = dstP[0] + dstS[0];
		vertexGL[Vector2D::dimension() * 2 + 1] = dstP[1] + dstS[1];
		vertexGL[Vector2D::dimension() * 3 + 0] = dstP[0];
		vertexGL[Vector2D::dimension() * 3 + 1] = dstP[1] + dstS[1];

		GLfloat coordGL[2 * 4];
		coordGL[Vector2D::dimension() * 0 + 0] = srcP[0];
		coordGL[Vector2D::dimension() * 0 + 1] = srcP[1];
		coordGL[Vector2D::dimension() * 1 + 0] = srcP[0] + 1;
		coordGL[Vector2D::dimension() * 1 + 1] = srcP[1];
		coordGL[Vector2D::dimension() * 2 + 0] = srcP[0] + 1;
		coordGL[Vector2D::dimension() * 2 + 1] = srcP[1] + 1;
		coordGL[Vector2D::dimension() * 3 + 0] = srcP[0];
		coordGL[Vector2D::dimension() * 3 + 1] = srcP[1] + 1;

		GLubyte indicesGL[4];
		for(int i = 0; i < 4; i++) {
			indicesGL[i] = i;
			coordGL[Vector2D::dimension()*i + 0] /= img.width();
			coordGL[Vector2D::dimension()*i + 1] /= img.height();
		}

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_TEXTURE_2D);
		{
			glBindTexture( GL_TEXTURE_2D, img.name() );
			glVertexPointer( Vector2D::dimension(), GL_SHORT, 0, vertexGL );
			glTexCoordPointer( Vector2D::dimension(), GL_FLOAT, 0, coordGL );
			glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, indicesGL);
		}
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	const Vector2D Graphics2D::SHADE_P(16, 32);
	const Size2D Graphics2D::FONT_COLOR_S(16, 16);

	uint Graphics2D::drawChar(RPG2kString::const_iterator& it, Vector2D const& p, uint color)
	{
		font::CharType t = font::Font::check(it);

		int width;
		font::font_t* data;
		switch(t) {
			case font::FULL:
				width = font::FULL_FONT_W;
				data = font::Font::getFull( it, getOwner().getProject().fontType() );
				break;
			case font::HALF:
				width = font::HALF_FONT_W;
				if( *it == ' ' ) { // skip space
					++it;
					return width;
				} else data = font::Font::getHalf(it);
				break;
			default:
				width = 0;
				data = NULL;
				break;
		}

		Image const& sys = getSystemGraphic();
		Vector2D fontP = Vector2D( color % FONT_COLOR_ROW, (color/FONT_COLOR_ROW + 3) ) * FONT_COLOR_S;
		fontP[0] += (FONT_COLOR_S[0] - width) / 2;
		fontP[1] += (FONT_COLOR_S[1] - font::FONT_H) / 2;
		Vector2D shadeP(SHADE_P);
		shadeP[0] += (FONT_COLOR_S[0] - width) / 2;
		shadeP[1] += (FONT_COLOR_S[1] - font::FONT_H) / 2;

		GLshort vertexGL[2 * font::FONT_H*font::FONT_H * 2];
		GLfloat coordGL[2 * font::FONT_H*font::FONT_H * 2];
		GLushort indicesGL[font::FONT_H*font::FONT_H * 2];

		int arraySize = 0;
		for(int x = 0; x < width; x++) {
			font::font_t mask = 0x01 << x;
			for(int y = 0; y < font::FONT_H; y++) {
				if( data[y] & mask ) {
					// shade
					vertexGL[Vector2D::dimension() * arraySize * 2 + 0] = p[0] + x + 1;
					vertexGL[Vector2D::dimension() * arraySize * 2 + 1] = p[1] + y + 1;
					coordGL[Vector2D::dimension() * arraySize * 2 + 0] = shadeP[0] + x + 1;
					coordGL[Vector2D::dimension() * arraySize * 2 + 1] = shadeP[1] + y + 1;
					// font
					vertexGL[Vector2D::dimension() * arraySize * 2 + 0 + 2] = p[0] + x;
					vertexGL[Vector2D::dimension() * arraySize * 2 + 1 + 2] = p[1] + y;
					coordGL[Vector2D::dimension() * arraySize * 2 + 0 + 2] = fontP[0] + x;
					coordGL[Vector2D::dimension() * arraySize * 2 + 1 + 2] = fontP[1] + y;

					arraySize++;
				}
			}
		}
		for(uint i = 0; i < Vector2D::dimension() * arraySize * 2; i++) indicesGL[i] = i;

		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glPushMatrix();
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glOrtho( 0.0, sys.width()*2, 0.0, sys.height()*2, -1.0, 1.0 );
			glMatrixMode(GL_MODELVIEW);

			glDisable(GL_POINT_SMOOTH);
			glBindTexture( GL_TEXTURE_2D, sys.name() );
			glVertexPointer( Vector2D::dimension(), GL_SHORT, 0, vertexGL );
			glTexCoordPointer( Vector2D::dimension(), GL_FLOAT, 0, coordGL );
			glDrawElements(GL_POINTS, arraySize * 2, GL_UNSIGNED_SHORT, indicesGL);
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		return width;
	}
	void Graphics2D::drawString(RPG2kString const& str, Vector2D const& p, uint color)
	{
		Vector2D cur = p;
		RPG2kString::const_iterator it = str.begin();

		while( it != str.end() ) cur[0] += drawChar(it, cur, color);
	}

	bool Graphics2D::interval() const
	{
		return ( getOwner().loopCount() % (CURSOR_INTERVAL*2) / CURSOR_INTERVAL ) != 0;
	}
	Image const& Graphics2D::getSystemGraphic()
	{
		return getImage( Material::System, getOwner().getProject().systemGraphic() );
	};

	void Graphics2D::drawWindowType(Vector2D const& dstP, Size2D const& dstS, WindowType type)
	{
		Vector2D frontP(32*type, 0), backP;
		Vector2D backS; // background image size

		Image const& sys = getSystemGraphic();

		switch(type) {
			case WINDOW:
				backP(32*0, 0);
				backS = 32;
				break;
			case CURSOR_0: case CURSOR_1:
				backP(frontP[0] + CORNER, frontP[1] + CORNER);
				backS = NON_FRAME;
				break;
			default:
				return;
		}
		// draw wallpaper and side frame
		switch( getOwner().getProject().wallpaperType() ) {
			case model::Wallpaper::TILE: {
			// draw wallpaper
				// draw wallpaper aligned space
				Size2D align = dstS / backS;
				Size2D remainS = dstS % backS;
				Vector2D remainP = align * backS;
				Size2D it;
				for(it[0] = 0; it[0] < align[0]; it[0]++) {
				for(it[1] = 0; it[1] < align[1]; it[1]++) {
					drawImage(sys, dstP + backS*it, backP, backS);
				}
				}
				// draw wallpaper unaligned space
				if(!remainS[0]) {
					for(it[1] = 0; it[1] < align[1]; it[1]++) {
						drawImage(sys,
							Vector2D(dstP[0] + remainP[0], dstP[1] + backS[1]*it[1]),
							backP, Size2D(remainS[0], backS[1])
						);
					}
				}
				if(!remainS[1]) {
					for(it[0] = 0; it[0] < align[0]; it[0]++) {
						drawImage(sys,
							Vector2D(dstP[0] + backS[0]*it[0], dstS[1] + remainP[1]),
							backP, Size2D(backS[0], remainS[1])
						);
					}
				}
				if( (!remainS[0]) && (!remainS[1]) ) {
					drawImage(sys, dstS + remainP, backP, remainS);
				}
			// draw side frame
				// draw top and bottom frame aligned
				align( (dstS[0]-FRAME_W*2) / FRAME_L, (dstS[1]-FRAME_W*2) / FRAME_L);
				for(it[0] = 0; it[0] < align[0]; it[0]++) {
					// top frame
					drawImage(sys,
						Vector2D(dstP[0] + FRAME_W + FRAME_L * it[0], dstP[1]),
						Vector2D(frontP[0]+FRAME_W, frontP[1]),
						Size2D(FRAME_L, FRAME_W)
					);
					// bottom frame
					drawImage(sys,
						Vector2D(dstP[0] + FRAME_W + FRAME_L * it[0], dstP[1] + dstS[1]-FRAME_W),
						Vector2D(frontP[0]+FRAME_W, frontP[1]+C_AND_N),
						Size2D(FRAME_L, FRAME_W)
					);
				}
				// draw left and bottom frame aligned
				for(it[1] = 0; it[1] < align[1]; it[1]++) {
					// draw left
					drawImage(sys,
						Vector2D(dstP[0], dstP[1]+FRAME_W + FRAME_L*it[1]),
						Vector2D(frontP[0], frontP[1]+FRAME_W),
						Size2D(FRAME_W, FRAME_L)
					);
					// draw right
					drawImage(sys,
						Vector2D(dstP[0] + dstS[0]-FRAME_W, dstP[1]+FRAME_W + FRAME_L*it[1]),
						Vector2D(frontP[0]+C_AND_N, frontP[1]+FRAME_W),
						Size2D(FRAME_W, FRAME_L)
					);
				}
				// draw frame unaligned
				remainS( (dstS[0]-FRAME_W*2) % FRAME_L, (dstS[1]-FRAME_W*2) % FRAME_L );
				remainP(align[0] * FRAME_L + FRAME_W, align[1] * FRAME_L + FRAME_W);
				// top frame
				drawImage(sys,
					Vector2D(dstP[0]+remainP[0], dstP[1]),
					Vector2D(frontP[0]+FRAME_W, frontP[1]),
					Size2D(remainS[0], FRAME_W)
				);
				// bottom frame
				drawImage(sys,
					Vector2D(dstP[0]+remainP[0], dstP[1] + dstS[1]-FRAME_W),
					Vector2D(frontP[0]+FRAME_W, frontP[1]+C_AND_N),
					Size2D(remainS[0], FRAME_W)
				);
				// left frame
				drawImage(sys,
					Vector2D(dstP[0], dstP[1]+remainP[1]),
					Vector2D(frontP[0], frontP[1]+FRAME_W),
					Size2D(FRAME_W, remainS[1])
				);
				// right frame
				drawImage(sys,
					Vector2D(dstP[0] + dstS[0]-FRAME_W, dstP[1]+remainP[1]),
					Vector2D(frontP[0]+C_AND_N, frontP[1]+FRAME_W),
					Size2D(FRAME_W, remainS[1])
				);
			} break;
			case model::Wallpaper::ZOOM: {
				// draw wallpaper
				drawImage(sys, dstP, dstS, backP, backS);
				// draw top frame
				drawImage(sys,
					Vector2D(dstP[0]+FRAME_W, dstP[1]),
					Size2D(dstS[0]-FRAME_W, FRAME_W),
					Vector2D(frontP[0]+FRAME_W, frontP[1]),
					Size2D(FRAME_L, FRAME_W)
				);
				// draw left frame
				drawImage(sys,
					Vector2D(dstP[0], dstP[1]+FRAME_W),
					Size2D(FRAME_W, dstS[1]-FRAME_W),
					Vector2D(frontP[0], frontP[1]+FRAME_W),
					Size2D(FRAME_W, FRAME_L)
				);
				// draw right frame
				drawImage(sys,
					Vector2D(dstP[0] + dstS[0]-FRAME_W, dstP[1]+FRAME_W),
					Size2D(FRAME_W, dstS[1]-FRAME_W),
					Vector2D(frontP[0]+C_AND_N, frontP[1]+FRAME_W),
					Size2D(FRAME_W, FRAME_L)
				);
				// draw bottom frame
				drawImage(sys,
					Vector2D(dstP[0]+FRAME_W, dstP[1] + dstS[1]-FRAME_W),
					Size2D(dstS[0]-FRAME_W, FRAME_W),
					Vector2D(frontP[0]+FRAME_W, frontP[1]+C_AND_N),
					Size2D(FRAME_L, FRAME_W)
				);
			} break;
			default: rpg2k_assert(false);
		}

		Vector2D cornerSize(CORNER, CORNER);
		// draw corner frame
		drawImage(sys, dstP, frontP, cornerSize);
		drawImage(sys,
			Vector2D(dstP[0] + dstS[0] - CORNER, dstP[1]),
			Vector2D(frontP[0] + C_AND_N, frontP[1]), cornerSize
		);
		drawImage(sys,
			Vector2D(dstP[0], dstP[1] + dstS[1] - CORNER),
			Vector2D(frontP[0], frontP[1] + C_AND_N), cornerSize
		);
		drawImage(sys,
			Vector2D(dstP[0] + dstS[0] - CORNER, dstP[1] + dstS[1] -CORNER),
			Vector2D(frontP[0] + C_AND_N, frontP[1] + C_AND_N), cornerSize
		);
	}

	void Graphics2D::drawWallpaper(Vector2D const& dstP, Size2D const& dstS)
	{
		Image const& sys = getSystemGraphic();
		Vector2D backP = Vector2D(0, 32) + 1;
		Size2D backS = Size2D(16, 16) - 1;

		switch( getOwner().getProject().wallpaperType() ) {
			case model::Wallpaper::TILE: {
				Size2D align = dstS / backS;
				Size2D remainS = dstS % backS;
				Vector2D remainP = align * backS;
				Size2D it;
				for(it[0] = 0; it[0] < align[0]; it[0]++) {
				for(it[1] = 0; it[1] < align[1]; it[1]++) {
					drawImage(sys, dstP + backS*it, backP, backS);
				}
				}
				// draw wallpaper unaligned space
				if(!remainS[0]) {
					for(it[1] = 0; it[1] < align[1]; it[1]++) {
						drawImage(sys,
							Vector2D(dstP[0] + remainP[0], dstP[1] + backS[1]*it[1]),
							backP, Size2D(remainS[0], backS[1])
						);
					}
				}
				if(!remainS[1]) {
					for(it[0] = 0; it[0] < align[0]; it[0]++) {
						drawImage(sys,
							Vector2D(dstP[0] + backS[0]*it[0], dstS[1] + remainP[1]),
							backP, Size2D(backS[0], remainS[1])
						);
					}
				}
				if( (!remainS[0]) && (!remainS[1]) ) {
					drawImage(sys, dstS + remainP, backP, remainS);
				}
			} break;
			case model::Wallpaper::ZOOM:
				drawImage(sys, dstP, dstS, backP, backS);
				break;
			default:
				break;
		}
	}

	void Graphics2D::drawMoneyWindow(Vector2D const& p)
	{
		model::Project& proj = getOwner().getProject();

		drawWindow(p, MONEY_WINDOW_SIZE);
		std::ostringstream ss;
		ss << std::setw(8) << proj.getLSD().getMoney();
		drawString( ss.str(), p + Vector2D(16, 10), font::FNT_NORMAL);
		drawString( proj.getLDB()[21].getArray1D()[95], p + Vector2D(16 + font::HALF_FONT_W*8, 10), font::FNT_MONEY);
	}

	void Graphics2D::drawChipSet(Vector2D const& dstP, int chipID)
	{
		int interval = getOwner().loopCount() % (3*4);

		model::Project& proj = getOwner().getProject();
		Image const& src = getImage(
			Material::ChipSet,
			proj.getLDB()[20].getArray2D()[ proj.chipSetID() ][2]
		);
		model::SaveData& lsd = getOwner().getProject().getLSD();

		if( chipID == 10000 ) { return; // skip unvisible upper chip
		} else if( rpg2k::within(chipID, 3000) ) {
			rpg2k_assert( rpg2k::within( chipID / 1000, 3) );
			rpg2k_assert( rpg2k::within( chipID % 1000 / 50, 0x10 ) );
			rpg2k_assert( rpg2k::within( chipID % 1000 % 50, 0x2f ) );

			drawBlockA_B(src, dstP, chipID / 1000, chipID % 1000 / 50,  chipID % 1000 / 50, interval / 3);
		} else if( rpg2k::within(3000, chipID, 4000) ) {
			rpg2k_assert( rpg2k::within(interval, 4) );
			rpg2k_assert( ( (chipID-3000)%50 ) == 28 );

			drawImage(src, dstP, Vector2D( (chipID-3000)/50+3, interval % 4 ) * CHIP_SIZE, CHIP_SIZE);
		} else if( rpg2k::within(4000, chipID, 5000) ) {
			rpg2k_assert( rpg2k::within( (chipID-4000) / 50, 12 ) );
			rpg2k_assert( rpg2k::within( (chipID-4000) % 50, 0x2f ) );

			drawBlockD( src, dstP, (chipID-4000) / 50, (chipID-4000) % 50 );
		} else if( rpg2k::within(5000, chipID, 5144) ) {
			int num = lsd.getReplace(ChipSet::LOWER, chipID -  5000);
			Vector2D srcP(
				(num%6) + (num<96 ? 0 :  6) + 12,
				(num/6) - (num<96 ? 0 : 16)
			);
			drawImage(src, dstP, srcP * CHIP_SIZE, CHIP_SIZE);
		} else if( rpg2k::within(10000, chipID, 10144) ) {
			int num = lsd.getReplace(ChipSet::UPPER, chipID - 10000);
			Vector2D srcP(
				(num%6) + (num<48 ? 0 : 6) + (12+6),
				(num/6) + (num<48 ? 8 : -8)
			);
			drawImage(src, dstP, srcP * CHIP_SIZE, CHIP_SIZE);
		}
	}
	// SEG[0] = (0,0), SEG[1] = (8,0),
	// SEG[3] = (0,8), SEG[2] = (8,8)
	static const Vector2D SEG_P[4] = {
		Vector2D(0, 0),
		Vector2D(8, 0),
		Vector2D(8, 8),
		Vector2D(0, 8),
	};
	void Graphics2D::drawBlockA_B(Image const& src, Vector2D const& dstP, int x, int y, int z, int anime)
	{
		// OCN_SEG[0] = (0,0), OCN_SEG[1] = (8,0),
		// OCN_SEG[2] = (0,8), OCN_SEG[3] = (8,8)
		static const Vector2D OCN_SEG_P[4] = {
			Vector2D(0, 0),
			Vector2D(8, 0),
			Vector2D(0, 8),
			Vector2D(8, 8),
		};

		enum Pattern { A, B, C, D, N };
		Vector2D PAT_P[4];
		for(int i = 0; i < 4; i++) PAT_P[i](0, 16*i);
	// Ocean
		Vector2D oceanP( CHIP_SIZE[0]*anime, CHIP_SIZE[1]*4 );
		Pattern ocean[4];
		Pattern ON = (x == 2) ? D : A, OFF = (x == 2) ? C : B;

		for(int i = 0; i < 4; i++) ocean[i] = y&(0x01<<i) ? ON : OFF;
	// Coast
		Vector2D coastP( CHIP_SIZE[0]*(anime + (x==1 ? 3 : 0)), 0 );
		Pattern coast[4];
		if(z<0x10) {
			for(int i = 0; i < 4; i++) coast[i] = z&(0x01<<i) ? D : N;
		} else if(z<0x14) {
			coast[0] = B; coast[1] = z&0x01 ? D : N;
			coast[3] = B; coast[2] = z&0x02 ? D : N;
		} else if(z<0x18) {
			coast[0] = C; coast[1] = C;
			coast[3] = z&0x02 ? D : N; coast[2] = z&0x01 ? D : N;
		} else if(z<0x1c) {
			coast[0] = z&0x02 ? D : N; coast[1] = B;
			coast[3] = z&0x01 ? D : N; coast[2] = B;
		} else if(z<0x20) {
			coast[0] = z&0x01 ? D : N; coast[1] = z&0x02 ? D : N;
			coast[3] = C; coast[2] = C;
		} else if(z==0x21) {
			coast[0] = coast[1] = coast[3] = coast[2] = B;
		} else if(z==0x22) {
			coast[0] = coast[1] = coast[3] = coast[2] = C;
		} else if(z<0x24) {
			coast[0] = A; coast[1] = C;
			coast[3] = B; coast[2] = z&0x01 ? D : N;
		} else if(z<0x26) {
			coast[0] = C; coast[1] = A;
			coast[3] = z&0x01 ? D : N; coast[2] = B;
		} else if(z<0x28) {
			coast[0] = z&0x01 ? D : N; coast[1] = B;
			coast[3] = C; coast[2] = A;
		} else if(z<0x2a) {
			coast[0] = B; coast[1] = z&0x01 ? D : N;
			coast[3] = A; coast[2] = C;
		} else if(z==0x2a) {
			coast[0] = A; coast[1] = A;
			coast[3] = B; coast[2] = B;
		} else if(z==0x2b) {
			coast[0] = A; coast[1] = C;
			coast[3] = A; coast[2] = C;
		} else if(z==0x2c) {
			coast[0] = B; coast[1] = B;
			coast[3] = A; coast[2] = A;
		} else if(z==0x2d) {
			coast[0] = C; coast[1] = A;
			coast[3] = C; coast[2] = A;
		} else if(z==0x2e) {
			coast[0] = coast[1] =
				coast[3] = coast[2] = A;
		} else return;
	// Draw
		for(int i = 0; i < 4; i++) {
			if( ocean[i] != N ) drawImage(
				src, dstP + OCN_SEG_P[i],
				oceanP + PAT_P[ ocean[i] ] + OCN_SEG_P[i],
				CHIP_SIZE / 2
			);
		}
		for(int i = 0; i < 4; i++) {
			drawImage(
				src, dstP + SEG_P[i],
				coastP + PAT_P[ coast[i] ] + SEG_P[i],
				CHIP_SIZE / 2
			);
		}
	}
	void Graphics2D::drawBlockD(Image const& src, Vector2D const& dstP, int x, int y)
	{
		enum Pattern {
			A , B , C ,
			D7, D8, D9,
			D4, D5, D6,
			D1, D2, D3,
		};
		Vector2D PAT_P[12];
		for(int i = 0; i < 4; i++) for(int j = 0; j < 3; j++) PAT_P[3*i + j](j, i) *= 16;

		Vector2D baseP(
			CHIP_SIZE[0]*(3*(x%2) + (x<4 ? 0 : 6)),
			CHIP_SIZE[1]*(4*(x/2) - (x<4 ? 0 : 8))
		);
		Pattern pat[4] = { D5, D5, D5, D5 };

		if(y < 0x10) {
			for(int i = 0; i < 4; i++) pat[i] = y&(0x01<<i) ? C : D5;
		} else if(y < 0x14) {
			pat[0] = D4; pat[1] = y&0x01 ? C : D5;
			pat[3] = D4; pat[2] = y&0x02 ? C : D5;
		} else if(y < 0x18) {
			pat[0] = D8; pat[1] = D8;
			pat[3] = y&0x02 ? C : D5; pat[2] = y&0x01 ? C : D5;
		} else if(y < 0x1C) {
			pat[0] = y&0x01 ? C : D5; pat[1] = D6;
			pat[3] = y&0x02 ? C : D5; pat[2] = D6;
		} else if(y < 0x20) {
			pat[0] = y&0x01 ? C : D5; pat[1] = y&0x02 ? C : D5;
			pat[3] = D2; pat[2] = D2;
		} else if(y == 0x20) {
			pat[0] = D4; pat[1] = D6;
			pat[3] = D4; pat[2] = D6;
		} else if(y == 0x21) {
			pat[0] = D8; pat[1] = D8;
			pat[3] = D2; pat[2] = D2;
		} else if(y < 0x24) {
			pat[0] = D7; pat[1] = D7;
			pat[3] = D7; pat[2] = y&0x01 ? C : D7;
		} else if(y < 0x26) {
			pat[0] = D9; pat[1] = D9;
			pat[3] = y&0x01 ? C : D9; pat[2] = D9;
		} else if(y < 0x28) {
			pat[0] = y&0x01 ? C : D3; pat[1] = D3;
			pat[3] = D3; pat[2] = D3;
		} else if(y < 0x2a) {
			pat[0] = D1; pat[1] = y&0x01 ? C : D1;
			pat[3] = D1; pat[2] = D1;
		} else if(y == 0x2a) {
			pat[0] = D7; pat[1] = D9;
			pat[3] = D7; pat[2] = D9;
		} else if(y == 0x2b) {
			pat[0] = D7; pat[1] = D7;
			pat[3] = D1; pat[2] = D1;
		} else if(y == 0x2c) {
			pat[0] = D1; pat[1] = D3;
			pat[3] = D1; pat[2] = D3;
		} else if(y == 0x2d) {
			pat[0] = D9; pat[1] = D9;
			pat[3] = D3; pat[2] = D3;
		} else if(y == 0x2e) {
			pat[0] = D7; pat[1] = D9;
			pat[3] = D1; pat[2] = D3;
		} else return;

		for(int i = 0; i < 4; i++) {
			drawImage(src, dstP+SEG_P[i], baseP+PAT_P[pat[i]]+SEG_P[i], CHIP_SIZE/2);
		}
	}

	void Graphics2D::clip(Vector2D const& coord, Size2D const& size)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor( coord[0], SCREEN_SIZE[1] - coord[1] - size[1], size[0], size[1] );
	}
	void Graphics2D::unclip()
	{
		glDisable(GL_SCISSOR_TEST);
	}

	void Graphics2D::clear()
	{
		glClear( GL_COLOR_BUFFER_BIT /* | GL_STENCIL_BUFFER_BIT */ );
	}

	void Graphics2D::gotoTitle()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void Graphics2D::swapBuffers()
	{
		glFlush();
		SDL_GL_SwapBuffers();
	}

	void Graphics2D::drawCharSet(structure::EventState& state, Vector2D const& dstP)
	{
		Vector2D srcP(
			state.charSetPos() % 4 * CharSet::Pat::END + state.charSetPat(),
			state.charSetPos() / 4 * CharSet::Dir::END + state.charSetDir()
		);
		drawImage(
			getImage( Material::CharSet, state.charSet() ),
			dstP, srcP * CHAR_SIZE, CHAR_SIZE
		);
	}
	void Graphics2D::drawFaceSet(RPG2kString const& faceSet, uint faceSetPos, Vector2D const& dstP)
	{
		drawImage(
			getImage(Material::FaceSet, faceSet),
			dstP,
			Vector2D(faceSetPos % 4, faceSetPos / 4) * FACE_SIZE,
			FACE_SIZE
		);
	}

	void Image::xyz::free(uint8_t* data)
	{
		delete [] data;
	}
	uint8_t* Image::xyz::load(SystemString const& filename, int& width, int& height, int& channels, bool trans)
	{
		static uint const  SIGN_SIZE = 4;
		static uint8_t const* SIGN = reinterpret_cast< uint8_t const* >("XYZ1");
		static uint const BUFF_SIZE = 1024;
		static uint const COLOR_NUM = 256, COLOR_SIZE = 3, PALETTE_SIZE = COLOR_SIZE * COLOR_NUM;

		uint8_t sign[SIGN_SIZE];
		#pragma pack(push, 1) // no alignment
			struct Palette { uint8_t r, g, b; };
		#pragma pack(pop)
	// Open File
		std::FILE* fp = std::fopen(filename.c_str(), "rb");
		if(fp == NULL) return NULL;
	// read header
		if(
			( std::fread(sign, 1, sizeof(sign), fp) != sizeof(sign) ) ||
			( std::memcmp(sign, SIGN, SIGN_SIZE) != 0 )
		) {
			std::fclose(fp);
			return NULL;
		}

		width  = std::fgetc(fp) | (std::fgetc(fp) << CHAR_BIT); // read uint16_t little endian
		height = std::fgetc(fp) | (std::fgetc(fp) << CHAR_BIT);
		channels = trans ? 4 : 3;
	// init zlib
		::z_stream z;
		z.zalloc   = Z_NULL;
		z.zfree    = Z_NULL;
		z.opaque   = Z_NULL;
		z.next_in  = Z_NULL;
		z.avail_in = 0;
		if(::inflateInit(&z) != Z_OK) {
			fclose(fp);
			return NULL;
		}
	// decode palette and image data
		uint8_t  inbuff[BUFF_SIZE];
		boost::scoped_array< uint8_t > loadBuff( new uint8_t[PALETTE_SIZE + width*height] );
		int status = Z_OK;
		z.next_out = reinterpret_cast< ::Bytef* >( loadBuff.get() );
		z.avail_out = PALETTE_SIZE + width*height;
		while(true) {
			if(z.avail_in == 0) {
				z.next_in  = reinterpret_cast< ::Bytef* >(inbuff);
				z.avail_in = fread(inbuff, 1, BUFF_SIZE, fp);
			}
			status = ::inflate(&z, Z_NO_FLUSH);
			if( (z.avail_out == 0) && (status == Z_STREAM_END) ) {
				std::fclose(fp);
				break;
			}

			if( (status != Z_OK) || (status == Z_STREAM_END) ) {
				std::fclose(fp);
				return NULL;
			}
		}
	// convert
		Palette* palette = reinterpret_cast< Palette* >( loadBuff.get() );
		uint8_t* data = loadBuff.get() + PALETTE_SIZE;
		uint8_t* ret = new uint8_t[channels * width * height];

		for(int y = 0; y < height; y++) {
			for(int x = 0; x < width; x++) {
				uint8_t index = data[(x + y*width)];
				ret[channels * (x + y * width) + 0] = palette[index].r;
				ret[channels * (x + y * width) + 1] = palette[index].g;
				ret[channels * (x + y * width) + 2] = palette[index].b;
				if(trans) ret[channels * (x + y * width) + 3] = (index == TRANS_COLOR) ? 0x00 : 0xff;
			}
		}

		return ret;
	} // uint8_t* Image::xyz::load
} // namespace rpg2k
