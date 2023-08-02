#pragma once
#include "IWindow.h"
#include <string>

namespace XWin
{
	class XWindowClass : public IWindowClass
	{
	public:
		XWindowClass(HINSTANCE hInst, const std::wstring& className = L"XWinDefaultWindowClass");
		ATOM GetAtom() const override;
		HINSTANCE GetInstance() const override;
		~XWindowClass() override;
	private:
		// functions 
		static LRESULT CALLBACK MessageSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		// data 
		HINSTANCE m_hInstance;
		ATOM m_atom;
	};
}