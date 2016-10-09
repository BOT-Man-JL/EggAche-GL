//
// A Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <thread>
#include <string>
#include <mutex>
#include <memory>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	size_t width = 800, height = 600;
	Window window (width, height, "Little Typer");

	auto canvas = std::unique_ptr<Canvas> (new Canvas (width, height));
	window.SetBackground (canvas.get ());

	const auto fontHeight = 18;							// Default Font Height is 18
	const auto fontWidth = canvas->GetTxtWidth ("a");	// Default Font is Fixed-Width
	auto chPerLine = width / fontWidth;
	std::string bufStr;

	std::mutex mtx;
	auto drawScheme = [&] ()
	{
		canvas->Clear ();

		// Text
		auto rowCount = bufStr.size () / chPerLine;
		for (size_t i = 0; i < rowCount + 1; i++)
		{
			std::string strLine (bufStr, i * chPerLine, chPerLine);
			canvas->DrawTxt (0, (int) (i * fontHeight), strLine.c_str ());
		}

		// Cursor
		auto xPos = (bufStr.size () % chPerLine) * fontWidth;
		auto yPos = rowCount * fontHeight;
		canvas->DrawLine ((int) xPos, (int) yPos,
			(int) xPos, (int) (yPos + fontHeight));

		window.Refresh ();
	};
	drawScheme ();

	window.OnResized ([&] (Window *, unsigned x, unsigned y)
	{
		std::lock_guard<std::mutex> lg (mtx);

		width = x;
		height = y;
		chPerLine = width / fontWidth;

		canvas = std::unique_ptr<Canvas> (new Canvas (width, height));
		window.SetBackground (canvas.get ());
		drawScheme ();
	});

	window.OnPress ([&] (Window *, char ch)
	{
		std::lock_guard<std::mutex> lg (mtx);

		switch (ch)
		{
		case '\b':
			if (!bufStr.empty ())
				bufStr.pop_back ();
			break;

		case '\r':
		case '\n':
		case '\t':
		case '\v':
		case '\f':
			break;

		default:
			bufStr.push_back (ch);
			break;
		}
		drawScheme ();
	});

	while (!window.IsClosed ())
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for (50ms); // Sleep just 50ms
	}

	return 0;
}
