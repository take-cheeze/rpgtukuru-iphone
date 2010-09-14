/**
 * @file
 * @brief Render Interface
 * @author project.kuto
 */
#pragma once

#include "kuto_layer.h"
#include "kuto_task.h"
#include "kuto_types.h"


namespace kuto
{
	class IRender : public Task
	{
	private:
		bool renderFlag_;
		virtual void draw();

		IRender(IRender const& src);

	protected:
		IRender(Layer::Type type, float priority);
		virtual ~IRender();

		void reset(Layer::Type type, float priority);
		void setRenderFlag(bool const val) { renderFlag_ = val; }

	public:
		virtual void render() const = 0;

		bool clearRenderFlag();
	}; // class IRender
	class Graphics2D;
	class IRender2D : public IRender
	{
	private:
		virtual void render(Graphics2D* g) const = 0;
		virtual void render() const;

	protected:
		IRender2D(Layer::Type type, float priority);
	}; // class IRender2D
	class Graphics3D;
	class IRender3D : public IRender
	{
	private:
		virtual void render3D(Graphics3D* g) const = 0;
		virtual void render() const;

	public:
		IRender3D(Layer::Type type, float priority);
	}; // class IRender3D

}	// namespace kuto

