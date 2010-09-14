/**
 * @file
 * @brief Render Layer
 * @author project.kuto
 */
#pragma once

#include <functional>
#include <map>


namespace kuto {

class IRender;
/// Layer Base Class
class Layer
{
public:
	/// Layer Types
	enum Type {
		OBJECT_2D,
		DEBUG_2D,
		TYPE_END,
	};

public:
	virtual ~Layer() {}
	void render() const;
	void addRender(float priority, IRender* render);
	void removeRender(IRender* render);

private:
	virtual void renderBegin() const;
	virtual void renderEnd  () const;

private:
	std::multimap< float, IRender*, std::greater<float> > renderObjects_;
};	// class Layer

/// 2D Layer
class Layer2D : public Layer
{
private:
	virtual void renderBegin() const;
};	// class Layer2D

}	// namespace kuto
