//
// An Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <thread>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	Window window;                          // Create a new Window
	auto bgEgg = window.GetBackground ();   // Get Background Egg of this Window

	bgEgg->DrawTxt (0, 0, "Hello EggAche"); // Draw Text at (0, 0)
	bgEgg->DrawLine (0, 30, 100, 30);       // Draw Line From (0, 30) to (100, 30)
	bgEgg->DrawBmp ("Egg.bmp", 20, 50);     // Draw Egg at (20, 50)

	window.Refresh ();                      // Refresh the Window to View Changes

	auto lineEgg = new Egg (1000, 750);     // Create a New Egg
	bgEgg->AddEgg (lineEgg);                // Associate this new Egg with Background Egg

	window.OnClick ([&] (int x, int y)      // Register OnClick Event
	{
		lineEgg->Clear ();                  // Clear Previous Content
		lineEgg->DrawLine (0, 0, x, y);     // Draw Line from (0, 0) to the Point you Clicked
		window.Refresh ();                  // Refresh the Window to View Changes
	});

	auto aniEgg = new Egg (100, 100,        // Create a New Egg
						   100, 100);       // at (100, 100) initially
	bgEgg->AddEgg (aniEgg);                 // Associate this new Egg with Background Egg
	aniEgg->DrawBmp ("Egg.bmp", 0, 0,       // Draw Bmp at (0, 0)
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
