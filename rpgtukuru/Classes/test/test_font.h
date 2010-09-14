/**
 * @file
 * @brief Font Draw Test
 * @author project.kuto
 */
#pragma once

#include <kuto/kuto_irender.h>
#include <kuto/kuto_texture.h>
#include <rpg2k/DataBase.hpp>
#include <rpg2k/MapUnit.hpp>


class TestFont : public kuto::IRender2D, public kuto::TaskCreator<TestFont>
{
	friend class kuto::TaskCreator<TestFont>;
private:
	TestFont();

	virtual bool initialize();
	virtual void update();

public:
	virtual void render(kuto::Graphics2D* g) const;

private:
	kuto::Texture		texture_;
};

