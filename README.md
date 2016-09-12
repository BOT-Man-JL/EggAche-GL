# EggAche-GL

EggAche is a **Lightweight**, **Cross-Platform** C++ Graphics Library

![Egg](Test_Windows/Egg.bmp)

## Requirement

- Currently only **Windows** (MSVC / MinGW are both supported)
- **C++ 11** Support

## Get Started

1. Download this Project's **_[zip](https://github.com/BOT-Man-JL/EggAche-GL/archive/master.zip)_**
2. Add the **5 Files** in **_[src path](https://github.com/BOT-Man-JL/EggAche-GL/tree/master/src)_** to your project
3. **#include "EggAche.h"** where you want to use EggAche Library
4. Using **#define** in **EggAche.h** to specify your target Platform

``` c++
// Enable Windows Version
#define EGGACHE_WINDOWS

//#define EGGACHE_LINUX
//#define EGGACHE_MAC
```

And...

Enjoy it :smile:

## Sample

### Basic

This Sample Shows how to

- Create Window
- Draw on Background Egg
- Refresh Window
- Wait for User Closing the Window

``` c++
#include <thread>    // For this_thread::sleep_for
#include "EggAche.h"

int main ()
{
    using namespace EggAche;

    Window window;                          // Create a new Window
    auto bgEgg = window.GetBackground ();   // Get Background Egg of this Window

    bgEgg->DrawTxt (0, 0, "Hello EggAche"); // Draw Text at (0, 0)
    bgEgg->DrawLine (0, 30, 100, 30);       // Draw Line From (0, 30) to (100, 30)
    bgEgg->DrawBmp ("Egg.bmp", 20, 50);     // Draw Egg at (20, 50)

    window.Refresh ();                      // Refresh the Window to View Changes

    while (!window.IsClosed ())             // Not Quit until the Window is closed
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for (500ms);
    }
    return 0;
}
```

![Basic](Demo/Basic.png)

### Click Event Handler

This Sample Shows how to

- Bind Event Handler to a Window
- Associate an Egg to another one

``` c++
    // ...
    //window.Refresh ();
    
    auto lineEgg = new Egg (1000, 750);  // Create a New Egg
    bgEgg->AddEgg (lineEgg);             // Associate this new Egg with Background Egg

    window.OnClick ([&] (int x, int y)   // Register OnClick Event
    {
        lineEgg->Clear ();               // Clear Previous Content
        lineEgg->DrawLine (0, 0, x, y);  // Draw Line from (0, 0) to the Point you Clicked
        window.Refresh ();               // Refresh the Window to View Changes
    });

    //while (!window.IsClosed ())
    //...
    delete lineEgg;                      // Remember to delete this Egg
    //return 0;
```

![Click](Demo/Click.png)

### Simple Animation

[Todo] :sweat_smile:

## Update History

- [v1.0](https://github.com/BOT-Man-JL/EggAche-GL/raw/master/EggAche_C.zip)
  - This version is written in **C**
  - There's **No Classes and Objects**
  - There's only **GDI Wrappers**...
- [v2.0](https://github.com/BOT-Man-JL/EggAche-GL/releases/tag/v2.0)
  - This version is written in **C++**
  - Everything is encapsulated in **Classes**
- [v3.0](https://github.com/BOT-Man-JL/EggAche-GL/archive/master.zip)
  - Refactoring with **Design Patterns**
  - Fully using **C++ 11 Features**