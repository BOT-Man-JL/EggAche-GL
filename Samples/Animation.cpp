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
#include <ctime>
#include <iostream>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	// 0 - Up, 1 - Down, 2 - Left, 3 - Right
	const auto xWnd = 640, yWnd = 480;
	const auto cDir = 4;
	const auto cDraPerDir = 8;
	const auto stepLen = 10;
	const std::vector<std::pair<size_t, size_t>> draSize
	{ { 66, 94 }, { 68, 82 }, { 95, 99 }, { 95, 99 } };

	// Init Window
	Window window (xWnd, yWnd, "Little Dragon");
	Canvas bgCanvas (xWnd, yWnd);
	window.SetBackground (&bgCanvas);
	bgCanvas.DrawImg ("Assets/bg.bmp", 0, 0, xWnd, yWnd);

	// Init Canvases
	std::vector<std::vector<std::unique_ptr<Canvas>>> dras (cDir);
	for (size_t i = 0; i < cDir; i++)
		for (size_t j = 0; j < cDraPerDir; j++)
		{
			auto fileName = std::string ("Assets/dra") +
				std::to_string (i) + ".bmp";
			std::unique_ptr<Canvas> egg (
				new Canvas (draSize[i].first, draSize[i].second));

			egg->DrawImgMask (fileName.c_str (), fileName.c_str (),
				(int) draSize[i].first, (int) draSize[i].second,
							  0, 0,
							  (int) (j * draSize[i].first), 0,
							  (int) (j * draSize[i].first), (int) (draSize[i].second));
			dras[i].emplace_back (std::move (egg));
		}

	// Shared Data
	auto iDir = 0;
	bool isMouseDown = false;
	std::vector<bool> isKeyDown (4, false);
	auto pos_x = 0, pos_y = 0;
	auto mouse_x = 0, mouse_y = 0;
	std::mutex mtx;

	// Fix Pos
	auto fixPos = [&] ()
	{
		if (pos_x < 0)
			pos_x = 0;
		if (pos_x > xWnd - draSize[iDir].first)
			pos_x = xWnd - draSize[iDir].first;

		if (pos_y < 0)
			pos_y = 0;
		if (pos_y > yWnd - draSize[iDir].second)
			pos_y = yWnd - draSize[iDir].second;
	};

	// Handle Mouse
	window.OnLButtonDown ([&] (Window*, unsigned x, unsigned y)
	{
		std::lock_guard<std::mutex> lg (mtx);
		isMouseDown = true;
	});
	window.OnLButtonUp ([&] (Window*, unsigned x, unsigned y)
	{
		std::lock_guard<std::mutex> lg (mtx);
		isMouseDown = false;
	});
	window.OnMouseMove ([&] (Window*, unsigned x, unsigned y)
	{
		std::lock_guard<std::mutex> lg (mtx);
		mouse_x = x - draSize[iDir].first / 2;
		mouse_y = y - draSize[iDir].second / 2;
	});

	// Handle Key
	window.OnKeyDown ([&] (Window*, char ch)
	{
		std::lock_guard<std::mutex> lg (mtx);
		auto isValid = false;
		switch (ch)
		{
		case 'W':
			iDir = 0;
			isValid = true;
			break;
		case 'S':
			iDir = 1;
			isValid = true;
			break;
		case 'A':
			iDir = 2;
			isValid = true;
			break;
		case 'D':
			iDir = 3;
			isValid = true;
			break;
		default:
			break;
		}
		if (isValid)
			isKeyDown[iDir] = true;
	});

	window.OnKeyUp ([&] (Window*, char ch)
	{
		std::lock_guard<std::mutex> lg (mtx);
		auto iDirKey = -1;
		switch (ch)
		{
		case 'W':
			iDirKey = 0;
			break;
		case 'S':
			iDirKey = 1;
			break;
		case 'A':
			iDirKey = 2;
			break;
		case 'D':
			iDirKey = 3;
			break;
		default:
			break;
		}
		if (iDirKey != -1)
			isKeyDown[iDirKey] = false;
	});

	// For fps
	const auto sampleCount = 100;
	auto cc = 0;
	auto tt = clock ();

	// Game Loop
	Canvas *pDra = nullptr;
	for (auto iFrame = 0; !window.IsClosed ();)
	{
		auto tBeg = clock ();
		{
			std::lock_guard<std::mutex> lg (mtx);

			// Draw Previous Frame
			window.Refresh ();

			// Update pos
			if (isKeyDown[0])
				pos_y -= stepLen;
			if (isKeyDown[1])
				pos_y += stepLen;
			if (isKeyDown[2])
				pos_x -= stepLen;
			if (isKeyDown[3])
				pos_x += stepLen;
			if (isMouseDown)
			{
				if (mouse_x - pos_x > stepLen)
				{
					pos_x += stepLen;
					iDir = 3;
				}
				else if (pos_x - mouse_x > stepLen)
				{
					pos_x -= stepLen;
					iDir = 2;
				}
				if (mouse_y - pos_y > stepLen)
				{
					pos_y += stepLen;
					iDir = 1;
				}
				else if (pos_y - mouse_y > stepLen)
				{
					pos_y -= stepLen;
					iDir = 0;
				}
			}
			fixPos ();

			// Control Frame
			++iFrame;
			if (iFrame == cDraPerDir) iFrame = 0;

			// Compute what to Draw
			if (pDra != nullptr)
				bgCanvas -= pDra;
			pDra = dras[iDir][iFrame].get ();
			pDra->MoveTo (pos_x, pos_y);
			bgCanvas += pDra;

			// Buffering for next Refresh
			bgCanvas.Buffering ();
		}

		// Count fps
		if (cc > sampleCount)
		{
			cc = 0;
			tt = clock ();
		}
		std::cout << "fps: " << cc++ * 1000.0 / (clock () - tt) << std::endl;

		// Sleep
		auto tElapse = (clock () - tBeg);
		if (tElapse < 50)
			std::this_thread::sleep_for (std::chrono::milliseconds { 50 - tElapse });
	}

	return 0;
}
