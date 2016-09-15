//
// An Sample of EggAche Graphics Library
// By BOT Man, 2016
//

// Memory Leaks Tracer
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC  // For *alloc()
//#include <stdlib.h>  //Should be before including <crtdbg.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)  // For new
#endif  // _DEBUG

#include "../src/EggAche.h"
#include <thread>

int main (int argc, char *argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// If Traced "{69} normal block at ...", Then Call to Break
	//_CrtSetBreakAlloc (69);
#endif  // _DEBUG

	using namespace EggAche;

	Window window;                          // Create a new Window
	auto bgEgg = window.GetBackground ();   // Get Background Egg of this Window

	bgEgg->DrawTxt (0, 0, "Hello EggAche"); // Draw Text at (0, 0)
	bgEgg->DrawLine (0, 30, 100, 30);       // Draw Line From (0, 30) to (100, 30)
	bgEgg->DrawImg ("Egg.bmp", 20, 50);     // Draw Egg at (20, 50)

	window.Refresh ();                      // Refresh the Window to View Changes

	auto lineEgg = new Egg (1000, 750);     // Create a New Egg
	bgEgg->AddEgg (lineEgg);                // Associate this new Egg with Background Egg

	window.OnClick ([&] (int x, int y)      // Register OnClick Event
	{
		lineEgg->Clear ();                  // Clear Previous Content
		lineEgg->DrawLine (0, 0, x, y);     // Draw Line from (0, 0) to the Point you Clicked
		window.Refresh ();                  // Refresh the Window to View Changes

		bgEgg->SaveAsBmp ("Snapshot.bmp");  // Take a Snapshot :-)
	});

	auto aniEgg = new Egg (100, 100,        // Create a New Egg
						   100, 100);       // at (100, 100) initially
	bgEgg->AddEgg (aniEgg);                 // Associate this new Egg with Background Egg
	aniEgg->DrawImg ("Egg.bmp", 0, 0,       // Draw Bmp at (0, 0)
					 100, 100,              // of size 100 * 100
					 255, 255, 255);        // leave out White Color (FFFFFF)

	auto offset = 0;
	while (!window.IsClosed ())             // Rewrite this Part
	{
		if (offset < 10) offset++;          // Update offset
		else offset = -10;
		aniEgg->MoveTo (100 + offset * 10,  // Move aniEgg
						100 + offset * 10);
		window.Refresh ();                  // Refresh Window

		using namespace std::chrono_literals;
		std::this_thread::sleep_for (50ms); // Sleep just 50ms
	}

	delete aniEgg;                          // Remember to delete this Egg
	delete lineEgg;                         // Remember to delete this Egg
	return 0;
}
