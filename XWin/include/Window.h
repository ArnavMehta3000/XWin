#pragma once
#include "WindowClass.h"
#include "GenericTaskQueue.h"
#include <thread>
#include <semaphore>
#include <atomic>

namespace XWin
{
	class XWindow : public IWindow
	{
	public:
		XWindow(std::shared_ptr<IWindowClass> windowClass, const wchar_t* windowTitle,
			unsigned int width = 1280, unsigned int height = 720,
			unsigned int posX = CW_USEDEFAULT, unsigned int posY = CW_USEDEFAULT);
		~XWindow() override;
		HWND GetHandle() const override;
		bool IsClosing() const override;
		std::future<void> SetTitle(std::wstring title) override;

	private:
		static constexpr unsigned int CustomTaskMessageId = WM_USER + 0;

		void MessagePump() noexcept;
		LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept override;
		
		template<std::invocable F>
		auto Dispatch_(F&& f) const
		{
			auto future = m_tasks.Push(std::forward<F>(f));
			NotifyTaskDispatch();
			return future;
		}
		void NotifyTaskDispatch() const;

		std::shared_ptr<IWindowClass> m_windowClass;
		mutable GenericTaskQueue      m_tasks;
		std::binary_semaphore         m_startSignal{ 0 };
		std::jthread                  m_msgPumpThread;
		HWND                          m_hWnd = nullptr;
		std::atomic<bool>             m_closing = false;
	};
}