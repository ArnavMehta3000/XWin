#include "../XWin/include/XWindow.h"
#include <format>
#include <memory>
#include <ranges>
#include <vector>

namespace rn = std::ranges;
namespace vi = rn::views;
using namespace std::string_literals;
using namespace std::chrono_literals;


int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd)
{
	try
	{
		// Register one window class
		auto winClass = std::make_shared<XWin::XWindowClass>(hInstance);

		// Create a vector of 5 windows
		auto windowPtrs = vi::iota(0, 6) | 
			vi::transform([hInstance, winClass](auto i) 
				{ return std::make_shared<XWin::XWindow>(winClass, L"XWindow...?"); }) | 
			rn::to<std::vector>();


		// Keep application running as long as all windows are open
		int x = 0;
		while (!windowPtrs.empty())
		{
			std::erase_if(windowPtrs, [](auto& p) {return p->IsClosing(); });

			for (auto& p : windowPtrs)
			{
				p->SetTitle(std::format(L"XWin Demo Window [{:*<{}}]", L'*', x + 1));
			}

			x = (x + 1) % 20;
			std::this_thread::sleep_for(50ms);
		}
	}
	catch (const std::exception& e)
	{
		OutputDebugStringA(e.what());
	}

    return EXIT_SUCCESS;
}