
#include "EggAche.h"
#include <thread>

int main ()
{
	EggAche::Window wnd {1000, 750, "A"};
	//EggAche::Window wwnd {};

	EggAche::Egg e {100, 30};
	//EggAche::Egg ee {100, 100};

	e.DrawTxt (0, 0, "haha");

	wnd.AddEgg (e);
	//wwnd.AddEgg (ee);

	wnd.Refresh ();

	while (!wnd.IsClosed ())
		std::this_thread::sleep_for (std::chrono::seconds (1));
}