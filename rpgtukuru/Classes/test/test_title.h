/**
 * @file
 * @brief Title Test
 * @author project.kuto
 */

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <kuto/kuto_math.h>
#include <rpg2k/DataBase.hpp>


class TestTitle : public kuto::IRender2D, public kuto::TaskCreator<TestTitle>
{
	friend class kuto::TaskCreator<TestTitle>;
private:
	TestTitle();

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;

private:
	rpg2k::model::DataBase				rpgLdb_;
	int					animationCounter_;
	kuto::Vector2		screenOffset_;
	kuto::Vector2		screenScale_;
	kuto::Texture		titleTex_;
	kuto::Texture		gameoverTex_;
	kuto::Texture		systemTex_;
	bool				drawTitle_;
	int					cursor_;
};
