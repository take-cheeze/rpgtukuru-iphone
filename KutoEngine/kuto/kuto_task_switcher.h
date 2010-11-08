#pragma once

#include "kuto_task.h"

#include <algorithm>


namespace kuto
{
	class TaskSwitcher : public kuto::Task
	{
	public:
		void switchTask(Task* target)
		{
			TaskList& list = kuto::Task::children();
			TaskList::iterator const it = std::find( list.begin(), list.end(), target );
			kuto_assert( it != list.end() );

			for(std::size_t i = 0; i < list.size(); i++) {
				list[i].freeze();
			}
			it->freeze(false);
		}

		template<class T>
		T* addChildBack(std::auto_ptr<T> child)
		{
			child->freeze();
			kuto::Task::addChildBack(child);
		}
		template<class T>
		T* addChild(std::auto_ptr<T> child) { return this->addChildBack(child); }
		template<class T>
		T* addChildFront(std::auto_ptr<T> child)
		{
			child->freeze();
			kuto::Task::addChildFront(child);
		}
	};
} // namespace kuto
