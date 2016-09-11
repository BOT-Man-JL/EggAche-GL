//
// Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include "EggAche.h"
#include "Windows_impl.h"

namespace EggAche
{
	Window::Window (size_t width,
					size_t height,
					const char *cap_string)
		: bgEgg (new Egg (width, height)), windowImpl (nullptr)
	{
		GUIFactory *guiFactory = nullptr;

#ifdef WIN32
		// Windows Impl
		guiFactory = new GUIFactory_Windows ();
#endif

		windowImpl = guiFactory->NewWindow (width, height, cap_string);
		delete guiFactory;
	}

	Window::~Window ()
	{
		delete windowImpl;
	}

	Egg *Window::GetEgg ()
	{
		return bgEgg;
	}

	void Window::DrawEgg (const Egg *egg)
	{
		windowImpl->Draw (egg->context, egg->x, egg->y);
		for (auto subEgg : egg->subEggs)
			DrawEgg (subEgg);
	}

	void Window::Refresh ()
	{
		//if (!IsClosed ())
			DrawEgg (bgEgg);
	}

	bool Window::IsClosed () const
	{
		return windowImpl->IsClosed ();
	}

	template<typename ONCLICK>
	inline void Window::OnClick (ONCLICK fn)
	{
		windowImpl->OnClick (fn);
	}

	template<typename ONPRESS>
	inline void Window::OnPress (ONPRESS fn)
	{
		windowImpl->OnPress (fn);
	}

	Egg::Egg (unsigned int width, unsigned int height, int pos_x, int pos_y)
		: context (nullptr), x (pos_x), y (pos_y)
	{
		GUIFactory *guiFactory = nullptr;

#ifdef WIN32
		// Windows Impl
		guiFactory = new GUIFactory_Windows ();
#endif

		context = guiFactory->NewGUIContext (width, height);
		delete guiFactory;
	}

	Egg::~Egg ()
	{
		delete context;
	}

	void Egg::Move (int scale_x, int scale_y)
	{
		x = x + scale_x;
		y = y + scale_y;
	}

	void Egg::MoveTo (int pos_x, int pos_y)
	{
		x = pos_x;
		y = pos_y;
	}

	int Egg::GetX () const
	{
		return x;
	}

	int Egg::GetY () const
	{
		return y;
	}

	void Egg::AddEgg (const Egg &egg)
	{
		subEggs.push_back (&egg);
		subEggs.unique ();
	}

	void Egg::RemoveEgg (const Egg &egg)
	{
		subEggs.remove (&egg);
	}

	bool Egg::SetPen (unsigned int width, unsigned int r, unsigned int g, unsigned int b)
	{
		return context->SetPen (width, r, g, b);
	}

	bool Egg::SetBrush (unsigned int r, unsigned int g, unsigned int b)
	{
		return context->SetBrush (r, g, b);
	}

	bool Egg::DrawLine (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawLine (xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawRect (xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawElps (xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd, int wElps, int hElps)
	{
		return context->DrawRdRt (xBeg, yBeg, xEnd, yEnd, wElps, hElps);
	}

	bool Egg::DrawArc (int xLeft, int yTop, int xRight, int yBottom, int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawArc (xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawChord (int xLeft, int yTop, int xRight, int yBottom, int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawChord (xLeft, yTop, xLeft, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawPie (int xLeft, int yTop, int xRight, int yBottom, int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawPie (xLeft, yTop, xLeft, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawTxt (int xBeg, int yBeg, const char *szText)
	{
		return context->DrawTxt (xBeg, yBeg, szText);
	}

	bool Egg::DrawBmp (const char * szPath)
	{
		return context->DrawBmp (szPath);
	}

	bool Egg::DrawBmp (const char * szPath, int x, int y, int width, int height, int r, int g, int b)
	{
		return context->DrawBmp (szPath, x, y, width, height, r, g, b);
	}

	void Egg::Clear ()
	{
		context->Clear ();
	}

	void MsgBox (const char *szTxt, const char *szCap)
	{
		MsgBox_Impl (szTxt, szCap);
	}
}