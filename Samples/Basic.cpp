//
// A Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <thread>
#include <mutex>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	Window window (640, 480);               // Create a new Window
	Canvas bgCanvas (640, 480);             // Create a new Canvas
	window.SetBackground (&bgCanvas);       // Set Background Canvas of this Window

	bgCanvas.DrawTxt (0, 0, "Hello EggAche");  // Draw Text at (0, 0)
	bgCanvas.DrawLine (0, 30, 100, 30);        // Draw Line From (0, 30) to (100, 30)
	bgCanvas.DrawImg ("Egg.bmp", 20, 50);      // Draw Egg at (20, 50)

	window.Refresh ();                      // Refresh the Window to View Changes

	Canvas lineEgg (640, 480);              // Create a New Egg
	bgCanvas += &lineEgg;                   // Associate this new Egg with Background Egg

	std::mutex mtx;                         // Mutex for Draw Lock

	window.OnClick ([&]
	(Window *, int x, int y)                // Register OnClick Event
	{
		std::lock_guard<std::mutex> lg (mtx);

		lineEgg.Clear ();                   // Clear Previous Content
		lineEgg.DrawLine (0, 0, x, y);      // Draw Line from (0, 0) to the Point you Clicked
		window.Refresh ();                  // Refresh the Window to View Changes

		bgCanvas.SaveAsBmp ("Snapshot.bmp");   // Take a Snapshot :-)
	});

	Canvas aniEgg (100, 100,                // Create a New Egg
				   100, 100);               // at (100, 100) initially
	bgCanvas += &aniEgg;                    // Associate this new Egg with Background Egg
	aniEgg.DrawImg ("Egg.bmp", 0, 0,        // Draw Bmp at (0, 0)
					100, 100,               // of size 100 * 100
					255, 255, 255);         // leave out White Color (FFFFFF)

	auto offset = 0;
	while (!window.IsClosed ())             // Rewrite this Part
	{
		{
			std::lock_guard<std::mutex> lg (mtx);

			if (offset < 10) offset++;          // Update offset
			else offset = -10;
			aniEgg.MoveTo (100 + offset * 10,   // Move aniEgg
						   100 + offset * 10);
			window.Refresh ();                  // Refresh Window
		}

		using namespace std::chrono_literals;
		std::this_thread::sleep_for (50ms); // Sleep just 50ms
	}

	return 0;
}
