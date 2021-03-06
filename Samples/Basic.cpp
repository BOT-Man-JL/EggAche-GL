﻿//
// A Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <thread>
#include <mutex>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	std::mutex mtx;                         // Mutex for Draw Lock

	Window window (640, 480);               // Create a new Window
	Canvas bgCanvas (640, 480);             // Create a new Canvas
	window.SetBackground (&bgCanvas);       // Set Background Canvas of this Window

	bgCanvas.DrawTxt (0, 0, "Hello EggAche");    // Draw Text at (0, 0)
	bgCanvas.DrawLine (0, 30, 100, 30);          // Draw Line From (0, 30) to (100, 30)
	bgCanvas.DrawImg ("Assets/Egg.bmp", 20, 50); // Draw Canvas at (20, 50)

	window.Refresh ();                      // Refresh the Window to View Changes

	Canvas lineCanvas (640, 480);           // Create a New Canvas
	bgCanvas += &lineCanvas;                // Associate this new Canvas with Background Canvas

	window.OnClick ([&]
	(Window *, int x, int y)                // Register OnClick Event
	{
		std::lock_guard<std::mutex> lg (mtx);

		lineCanvas.Clear ();                // Clear Previous Content
		lineCanvas.DrawLine (0, 0, x, y);   // Draw Line from (0, 0) to the Point you Clicked
		window.Refresh ();                  // Refresh the Window to View Changes

		bgCanvas.SaveAsBmp ("Snapshot.bmp");// Take a Snapshot :-)
	});

	Canvas aniCanvas (73, 75,               // Create a New Canvas
					  100, 100);            // at (100, 100) initially
	bgCanvas += &aniCanvas;                 // Associate this new Canvas with Background Canvas
	aniCanvas.DrawImgMask ("Assets/EggMask.bmp",
						   "Assets/EggMask.bmp",
						   73, 75, 0, 0,
						   0, 0, 73, 0);
	// Draw Image EggMask.bmp with a Mask in EggMask.bmp, of size 73 * 75, at (0, 0)
	// And the left-top of Src Image is (0, 0), left-top of Mask Image is (74, 0)

	auto offset = 0;
	while (!window.IsClosed ())             // Rewrite this Part
	{
		{
			std::lock_guard<std::mutex> lg (mtx);

			if (offset < 10) offset++;          // Update offset
			else offset = -10;
			aniCanvas.MoveTo (100 + offset * 10,// Move aniCanvas
							  100 + offset * 10);
			window.Refresh ();                  // Refresh Window
		}

		std::this_thread::sleep_for (
			std::chrono::milliseconds (50));    // Sleep just 50ms
	}

	return 0;
}
