//
// A Sample of EggAche Graphics Library
// By BOT Man, 2016
//

#include "../src/EggAche.h"
#include <vector>
#include <tuple>

int main (int argc, char *argv[])
{
	using namespace EggAche;

	const auto wndSize = 300;
	Window window (wndSize, wndSize, "Rainbow Animation");
	auto bgEgg = window.GetBackground ();

	std::vector<std::tuple<unsigned, unsigned, unsigned>> colors
	{
		{255, 0, 0},
		{255, 165, 0},
		{255, 255, 0},
		{0, 255, 0},
		{0, 127, 255},
		{0, 0, 255},
		{139, 0, 255}
	};
	auto cColor = colors.size ();
	double anglePerSeg = 360.0 / cColor;

	Egg rainbowEgg (wndSize, wndSize);
	bgEgg->AddEgg (&rainbowEgg);
	rainbowEgg.SetPen (0, 0, 0, 0);
	auto DrawRainbow = [&] (double angleBeg,
							unsigned r,
							unsigned g,
							unsigned b)
	{
		rainbowEgg.SetBrush (false, r, g, b);
		rainbowEgg.DrawPie (0, 0, wndSize, wndSize,
							angleBeg, anglePerSeg);
	};

	bool isStop = false;
	window.OnClick ([&] (Window *, int, int)
	{
		isStop = !isStop;
	});

	auto offset = 0;
	const auto offsetCount = 200;
	while (!window.IsClosed ())
		if (!isStop)
		{
			if (offset < offsetCount) offset++;
			else offset = 0;

			rainbowEgg.Clear ();
			for (size_t i = 0; i < cColor; i++)
				DrawRainbow (anglePerSeg * i +
							 offset * 360.0 / offsetCount,
							 std::get<0> (colors[i]),
							 std::get<1> (colors[i]),
							 std::get<2> (colors[i]));
			window.Refresh ();
		}

	return 0;
}
