#include "Window.h"
#include "XWinException.h"


namespace XWin
{
	XWindow::XWindow(std::shared_ptr<IWindowClass> windowClass, const wchar_t* windowTitle, unsigned int width, unsigned int height, unsigned int posX, unsigned int posY)
		:
		m_windowClass(std::move(windowClass)),
		m_msgPumpThread(&XWindow::MessagePump, this)
	{
		std::future<void> future = m_tasks.Push([=, this] ()
			{
				const DWORD styles = WS_THICKFRAME | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
				const DWORD exStyles = 0;

				RECT rect
				{
					.left   = static_cast<LONG>(posX),
					.top    = static_cast<LONG>(posY),
					.right  = static_cast<LONG>(posX + width),
					.bottom = static_cast<LONG>(posY + height)
				};

				if (AdjustWindowRect(&rect, styles, FALSE) == FALSE)
					throw XWinException("Failed to adjust window rect");

				const auto windowWidth = rect.right - rect.left;
				const auto windowHeight = rect.bottom - rect.top;

				const auto hModule = GetModuleHandleW(nullptr);


				if (!hModule) 
					throw XWinException("Failed to get module handle");

				m_hWnd = CreateWindowExW(
					exStyles,
					MAKEINTATOM(m_windowClass->GetAtom()),
					windowTitle,
					styles,
					posX, posY,
					windowWidth, windowHeight,
					nullptr, nullptr, hModule,
					this
				);

				if (!m_hWnd) 
					throw XWinException("Failed creating window");
			});


		m_startSignal.release();
		future.get();
	}

	HWND XWindow::GetHandle() const { return m_hWnd; }

	XWindow::~XWindow()
	{
		Dispatch_([this] ()
			{
				if (!DestroyWindow(m_hWnd))
					OutputDebugStringA("Failed destroying window");
			});

		m_msgPumpThread.join();
	}

	void XWindow::MessagePump() noexcept
	{
		m_startSignal.acquire();
		m_tasks.PopExecute();


		MSG msg{};
		while (GetMessageW(&msg, m_hWnd, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	bool XWindow::IsClosing() const
	{
		return m_closing;
	}

	std::future<void> XWindow::SetTitle(std::wstring title)
	{
		return Dispatch_(
			[=, this] ()
			{
				if (!SetWindowTextW(m_hWnd, title.c_str()))
				OutputDebugStringA("Failed setting window title");
			});
	}

	// Called from message router from base interface
	LRESULT XWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		try
		{
			switch (msg)
			{
			case WM_DESTROY:
				m_hWnd = nullptr;
				PostQuitMessage(0);
				return 0;
			case WM_CLOSE:
				m_closing = true;
				return 0;
			case CustomTaskMessageId:
				m_tasks.PopExecute();
				return 0;
			}
		}
		catch (const std::exception& e)
		{
			std::string errMsg = "Uncaught exception in window procedure\n  >>>";
			errMsg.append(e.what());

			OutputDebugStringA(errMsg.c_str());
		}
		catch (...)
		{
			OutputDebugStringA("Uncaught anonymous exception in window procedure");
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);

	}
	void XWin::XWindow::NotifyTaskDispatch() const
	{
		if (!PostMessageW(m_hWnd, CustomTaskMessageId, 0, 0)) 
			throw XWinException("Failed to post task notification message");
	}
}