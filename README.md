# What is XWin?

XWin is a window creation creation and management library I extracted out and customized from [ChiliTomatoNoodle's Game Engine Series](https://www.youtube.com/playlist?list=PLqCJpWy5FoheHDzaP3R1eDFDOOff5TtBA) 
for personal use.

# Requirements

XWin needs **C++ 23** to work. Since it uses `std::move_only_function` for window task management.

# Building the project

- Clone the repository

```
https://github.com/ArnavMehta3000/XWin.git
```

- Build the project using Visual Studio 2022
	- By default the project builds into a static library into the `<SolutionDir>\Build\x64-<Build or Release>` directory
- Copy the `XWin.lib` file and the `<SolutionDir>\XWin\include\` into your project directory
	- Copy the source files if not compiling the project into a library

# Creating a window

All classes are in the *`XWin`* namespace
</br>
*Take a look at [XWinTests](https://github.com/ArnavMehta3000/XWin/blob/main/XWinTests/Main.cpp)* to see more details

## Step 1: Create a WindowClass Object

**NOTE: Only create one pointer of a certain class name. Attempts to create multiple window class pointers with the same name will throw and exception**

- Create a window class as a shared pointer by passing in the application `HINSTANCE` value

```cpp
std::shared_ptr<XWin::XWindowClass> windowClass = std::make_shared<XWin::XWindowClass>(hInstance);
```

- Optionally the class name may also be given as *wide character*.

 ```cpp
std::shared_ptr<XWin::XWindowClass> windowClass = std::make_shared<XWin::XWindowClass>(hInstance, L"My Window Class Name");
```


## Step 2: Create a Window Object

**NOTE: Windows are shown and destroyed when the constructors and destructors of `XWin::XWindow` are called respectively**

- Create a window shared pointer by providing the window class and window title.
```cpp
std::shared_ptr<XWin::XWindow> window = std::make_shared<XWin::XWindow>(windowClass, L"My Window Title");
```

- Optionally pass in the window dimensions and position.
```cpp
const unsigned int width = 800;
const unsigned int height = 800;
const unsigned int posX = 200;
const unsigned int posY = 500;

// Create a window with title "My Window Title" of size (800 x 800) at screen position (200 x 500)
std::shared_ptr<XWin::XWindow> window =  std::make_shared<XWin::XWindow>(windowClass, L"My Window Title", width, height, posX, posY);
```


## Step 3: Keeping the Window Open

- Each window runs its message pump on its own thread
- To keep a window open use a while loop and use the `IsClosing()` function to check for whether the window wants to close

```cpp
// Keep looping as long as window is open
while(!window->IsClosing())
{
	// Do window stuff
	// Update and Render to window...?
}
```

- Destroying the window pointer will destroy/close the window and release all resources and threads

# Window Functions

- Get the `HWND` using the following line of code
```cpp
HWND hWnd = window->GetHandle();
```

- Set the window title asynchronously using the following line of code
```cpp

window->SetTitle(L"New Title");
```

- Set title function returns a `std::future<void>` so it can be used to block the current thread to wait until the title of the window is set (synchronization)
```cpp
// Blocking function
window->SetTitle(L"New Title").get();
```

# Exception Handling

It is recommended to execute all XWin functions (window class and window creation) are executed in a *try-catch* block.
Since any failure throws a `XWin::XWinException` which can be caught and investigated as follows.

```cpp
#include "XWin/include/XWinException.h"
// You can use std::exception in the catch block as well (allowing you to ignore the above include)
// Since XWin::XWinException inherits from std::exception

try
{
	// WindowClass creation code
	// Window creation code
	// ...
	// ...
	// Other XWin code
}
catch (const XWin::XWinException& e)
{
	std::cout << e.what() << std::endl;
}
```

# Roadmap

[x] Basic window creation
[x] Separate window message thread
[ ] Hardware input polling (getting keyboard and mouse events from window)
[ ] Additional helper functions for creating a window
