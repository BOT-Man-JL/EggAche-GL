//
// A Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <mutex>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	// 0 - Up, 1 - Down, 2 - Left, 3 - Right
	const auto xWnd = 640, yWnd = 480;
	const auto cDir = 4;
	const auto cDraPerDir = 8;
	const std::vector<std::pair<size_t, size_t>> draSize
	{ { 66, 94 }, { 68, 82 }, { 95, 99 }, { 95, 99 } };

	// Init Window
	Window window (xWnd, yWnd);
	auto &bgEgg = window.GetBackground ();
	bgEgg.DrawImg ("Assets/bg.bmp", 0, 0, xWnd, yWnd);

	// Init Eggs
	std::vector<std::vector<std::unique_ptr<Egg>>> draEggs (cDir);
	for (size_t i = 0; i < cDir; i++)
		for (size_t j = 0; j < cDraPerDir; j++)
		{
			auto fileName = std::string ("Assets/dra") +
				std::to_string (i) + ".bmp";
			std::unique_ptr<Egg> egg (
				new Egg (draSize[i].first, draSize[i].second));

			egg->DrawImgMask (fileName.c_str (), fileName.c_str (),
							  (int) draSize[i].first, (int) draSize[i].second,
							  0, 0,
							  (int) (j * draSize[i].first), 0,
							  (int) (j * draSize[i].first), (int) (draSize[i].second));
			draEggs[i].emplace_back (std::move (egg));
		}

	// Shared Data
	auto isPaused = false;
	auto iDir = 0;
	auto pos_x = 0, pos_y = 0;
	std::mutex mtx;

	// Handle KeyPress
	window.OnPress ([&] (Window*, char ch)
	{
		std::lock_guard<std::mutex> lg (mtx);
		switch (ch)
		{
		case 'W':
		case 'w':
			iDir = 0;
			pos_y -= 10;
			if (pos_y < 0) pos_y = 0;
			break;
		case 'S':
		case 's':
			iDir = 1;
			pos_y += 10;
			if (pos_y > yWnd) pos_y = yWnd;
			break;
		case 'A':
		case 'a':
			iDir = 2;
			pos_x -= 10;
			if (pos_x < 0) pos_x = 0;
			break;
		case 'D':
		case 'd':
			iDir = 3;
			pos_x += 10;
			if (pos_x > xWnd) pos_x = xWnd;
			break;
		case ' ':
			isPaused = !isPaused;
			break;
		default:
			break;
		}
	});

	// Game Loop
	for (auto iFrame = 0; !window.IsClosed ();)
	{
		{
			std::lock_guard<std::mutex> lg (mtx);
			if (!isPaused)
			{
				// Control Frame
				++iFrame;
				if (iFrame == cDraPerDir) iFrame = 0;

				// Draw
				draEggs[iDir][iFrame].get ()->MoveTo (pos_x, pos_y);
				bgEgg.AddEgg (draEggs[iDir][iFrame].get ());
				window.Refresh ();
				bgEgg.RemoveEgg (draEggs[iDir][iFrame].get ());
			}
		}

		std::this_thread::sleep_for (std::chrono::milliseconds { 50 });
	}

	return 0;
}
