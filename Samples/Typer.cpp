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
#include <string>

int main (int argc, char *argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// If Traced "{69} normal block at ...", Then Call to Break
	//_CrtSetBreakAlloc (69);
#endif  // _DEBUG

	using namespace EggAche;

	size_t width = 800, height = 600;
	Window window (width, height, "Typer");
	auto bgEgg = window.GetBackground ();

	const auto fontWidth = 8;
	auto chPerLine = width / fontWidth;
	std::string bufStr;

	auto drawScheme = [&] ()
	{
		bgEgg->Clear ();
		for (size_t i = 0; i < bufStr.size () / chPerLine + 1; i++)
		{
			std::string strLine (bufStr, i * chPerLine, chPerLine);
			bgEgg->DrawTxt (0, i * 18, strLine.c_str ());
		}
		// Todo: Cursor
		window.Refresh ();
	};

	window.OnResized ([&] (Window *, int w, int h)
	{
		width = w;
		height = h;
		chPerLine = width / fontWidth;

		drawScheme ();
	});

	window.OnPress ([&] (Window *, char ch)
	{
		switch (ch)
		{
		case '\b':
			if (!bufStr.empty ())
				bufStr.pop_back ();
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
