#pragma once
#include "WindowsWrapper.h"
#include <future>
#include <string>
#include <optional>
#include <memory>

namespace XWin
{
	// Base interface for win32 window class 
	class IWindowClass
	{
	public:
		virtual ~IWindowClass() = default;
		virtual ATOM GetAtom() const = 0;
		virtual HINSTANCE GetInstance() const = 0;
	protected:
		// allows descendents of IWindowClass access to the IWindow virtual message handler 
		static LRESULT ForwardMessage(class IWindow* pWnd, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	};


	// Base interface for windows (gui)
	class IWindow
	{
		// Allow WindowClasses access to the message handling function 
		friend class IWindowClass;
	public:
		virtual ~IWindow() = default;
		virtual HWND GetHandle() const = 0;
		virtual bool IsClosing() const = 0;
		virtual std::future<void> SetTitle(const std::wstring& title) = 0;
	
	protected:
		virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept = 0;
	};
}