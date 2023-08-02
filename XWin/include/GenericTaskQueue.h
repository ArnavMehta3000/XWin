#pragma once
#include <deque>
#include <future>
#include <functional>


namespace XWin
{
	class GenericTaskQueue
	{
		using Task = std::move_only_function<void()>;

	public:
		template<std::invocable F>
		auto Push(F&& function)
		{
			using T = std::invoke_result_t<F>;
			std::packaged_task<T()> pkg{ std::forward<F>(function) };
			auto future = pkg.get_future();
			PushInternal([pkg = std::move(pkg)]() mutable { pkg(); });
			return future;
		}
		
		void PopExecute();

	private:
		void PushInternal(Task task);

		std::mutex m_mutex;
		std::deque<Task> m_tasks;
	};
}