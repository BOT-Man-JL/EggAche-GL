//
// A Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <thread>
#include <string>
#include <mutex>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	const size_t width = 800, height = 600;
	Window window (width, height, "Typer");
	auto &bgEgg = window.GetBackground ();

	const auto fontHeight = 18;							// Default Font Height is 18
	const auto fontWidth = bgEgg.GetTxtWidth ("a");	// Default Font is Fixed-Width
	const auto chPerLine = width / fontWidth;
	std::string bufStr;

	std::mutex mtx;
	auto drawScheme = [&] ()
	{
		std::lock_guard<std::mutex> lg (mtx);
		bgEgg.Clear ();

		// Text
		auto rowCount = bufStr.size () / chPerLine;
		for (size_t i = 0; i < rowCount + 1; i++)
		{
			std::string strLine (bufStr, i * chPerLine, chPerLine);
			bgEgg.DrawTxt (0, i * fontHeight, strLine.c_str ());
		}

		// Cursor
		auto xPos = (bufStr.size () % chPerLine) * fontWidth;
		auto yPos = rowCount * fontHeight;
		bgEgg.DrawLine (xPos, yPos, xPos, yPos + fontHeight);

		window.Refresh ();
	};
	drawScheme ();

	window.OnPress ([&] (Window *, char ch)
	{
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
