//
// Test on EggAche Graphics Library
// By BOT Man, 2016
//

// Memory Leaks Tracer
#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC  // For *alloc()
#include <stdlib.h>  //Should be before including <crtdbg.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)  // For new
#endif  // _DEBUG
#endif  // _MSC_VER

#include "EggAche.h"
#include <thread>
#include <string>

int main (int argc, char *argv[])
{
#ifdef _MSC_VER
#ifdef _DEBUG
	_CrtSetDbgFlag (_CrtSetDbgFlag (_CRTDBG_REPORT_FLAG) | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	// If Traced "{69} normal block at ...", Then Call to Break
	//_CrtSetBreakAlloc (191);
#endif  // _DEBUG
#endif  // _MSC_VER

	using namespace EggAche;
	auto wnd = new Window ();

	auto egg = wnd->GetBackground ();

	egg->DrawTxt (0, 0, "Welcome", 30, "Arial");
	egg->DrawLine (0, 0, 1000, 750);
	egg->DrawBmp ("Egg.bmp", 100, 200);

	auto egge = new Egg (1000, 750);
	egg->AddEgg (egge);

	auto eggee = new Egg (200, 300, 200, 300);
	eggee->DrawLine (0, 0, 200, 300);
	eggee->DrawTxt (0, 0, "Detecting Key Press ~");
	egg->AddEgg (eggee);

	auto eggeee = new Egg (200, 300);
	eggeee->DrawTxt (0, 18, "Detecting Mouse Click");
	eggeee->DrawLine (200, 0, 0, 300);
	eggee->AddEgg (eggeee);

	wnd->OnClick ([&] (int x, int y)
	{
		egge->Clear ();
		egge->DrawLine (0, 0, x, y);
		wnd->Refresh ();
	});

	auto wwwnd = Window ();
	auto bgEgg = wwwnd.GetBackground ();
	//auto wwnd = Window (std::move (wwwnd));

	// Todo: Where is haha?
	bgEgg->DrawTxt (0, 0, "haha", 200);
	//bgEgg->DrawLine (0, 0, 100, 100);
	bgEgg->AddEgg (eggee);
	egg->AddEgg (bgEgg);

	auto isQuit = false;
	wnd->OnPress ([&] (char ch)
	{
		if (ch == 'Q' || ch == 'q')
			isQuit = true;
		else
		{
			std::string promptStr ("You inputted: ");
			promptStr += ch;
			MsgBox (promptStr.c_str ());
		}
	});

	//wnd->Refresh ();
	//wwnd.Refresh ();
	while (!wnd->IsClosed () && !isQuit)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for (500ms);
	}

	delete egge;
	delete eggee;
	delete eggeee;
	delete wnd;
	return 0;
}
