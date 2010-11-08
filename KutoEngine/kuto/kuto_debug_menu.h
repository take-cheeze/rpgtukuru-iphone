/**
 * @file
 * @brief Debug Menu
 * @author project.kuto
 */
#pragma once

#include "kuto_task.h"
#include "kuto_irender.h"


namespace kuto {

class DebugMenu : public IRender2D, public TaskCreator<DebugMenu>
{
	friend class kuto::TaskCreator<DebugMenu>;
public:
	enum {
		kMaxRowSize = 10,
	};

public:
	int cursor() const { return cursor_; }

private:
	DebugMenu();

	virtual bool initialize();
	virtual void update();

	virtual void render(kuto::Graphics2D& g) const;

private:
	int			cursor_;
	int			scroll_;
};

}	// namespace kuto

