#include "GenericTaskQueue.h"

namespace XWin
{
	void GenericTaskQueue::PopExecute()
	{
		Task task;
		{
			std::lock_guard lck{ m_mutex };
			task = std::move(m_tasks.front());
			m_tasks.pop_front();
		}
		task();
	}

	void GenericTaskQueue::PushInternal(Task task)
	{
		std::lock_guard lock{ m_mutex };
		m_tasks.push_back(std::move(task));
	}
}