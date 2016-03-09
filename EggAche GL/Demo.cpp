
#include "EggAche.h"
#include <thread>

int main ()
{
	EggAche::Window wnd {1000, 750, "A"};
	EggAche::Window wwnd {};
	wwnd = wnd;

	EggAche::Egg e {10, 30};
	EggAche::Egg ee {100, 100};

	e.DrawTxt (0, 0, "haha");
	ee = e;
	ee.DrawTxt (10, 0, "hh");

	wnd.AddEgg (e);
	wwnd.AddEgg (ee);

	wnd.Refresh ();

	while (!wnd.IsClosed () || !wwnd.IsClosed ())
		std::this_thread::sleep_for (std::chrono::seconds (1));
}