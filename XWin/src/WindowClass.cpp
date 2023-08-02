#include "WindowClass.h"
#include "IWindow.h"
#include "XWinException.h"
#include <format>

namespace XWin
{
	LRESULT IWindowClass::ForwardMessage(IWindow* window, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		return window->WndProc(hWnd, msg, wParam, lParam);
	}

	XWindowClass::XWindowClass(HINSTANCE hInst, const std::wstring& className)
		:
		m_hInstance(hInst)
	{
		const WNDCLASSEXW wc{
			.cbSize        = sizeof(wc),
			.style         = CS_OWNDC,
			.lpfnWndProc   = &XWindowClass::MessageSetup,
			.cbClsExtra    = 0,
			.cbWndExtra    = 0,
			.hInstance     = m_hInstance,
			.hIcon         = nullptr,
			.hCursor       = nullptr,
			.hbrBackground = nullptr,
			.lpszMenuName  = nullptr,
			.lpszClassName = className.c_str(),
			.hIconSm       = nullptr,
		};
		m_atom = RegisterClassExW(&wc);
		if (!m_atom) 			
			throw XWinException(std::format("Failed to register window! (Error {})", GetLastError()));
	}
	ATOM XWindowClass::GetAtom() const
	{
		return m_atom;
	}
	HINSTANCE XWindowClass::GetInstance() const
	{
		return m_hInstance;
	}
	XWindowClass::~XWindowClass()
	{
		UnregisterClass(MAKEINTATOM(m_atom), m_hInstance);
	}
	LRESULT XWindowClass::MessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// use create parameter passed in from CreateWindowExW() to store window class pointer at WinAPI side 
		if (msg == WM_NCCREATE)
		{
			// extract ptr to window class from creation data 
			const CREATESTRUCTW* const create = reinterpret_cast<CREATESTRUCTW*>(lParam);
			auto* const window = static_cast<IWindow*>(create->lpCreateParams);
			
			if (!window)
				throw XWinException("Failed to extract window interface");

			// set WinAPI-managed user data to store ptr to window instance 
			SetLastError(0);
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			if (auto hr = GetLastError())
				throw XWinException(std::format("HRESULT ERROR: {}", hr));

			// set message proc to normal (non-setup) handler now that setup is finished 
			SetLastError(0);
			SetWindowLongPtrW(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&XWindowClass::MessageRouter));
			if (auto hr = GetLastError())
				throw XWinException(std::format("HRESULT ERROR: {}", hr));

			// forward message to window instance handler 
			return ForwardMessage(window, hWnd, msg, wParam, lParam); 
		}
		// if we get a message before the WM_NCCREATE message, handle with default handler 
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	LRESULT XWindowClass::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// retrieve ptr to window instance 
		auto* const window = reinterpret_cast<IWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

		// forward message to window instance handler 
		return ForwardMessage(window, hWnd, msg, wParam, lParam);
	}
}