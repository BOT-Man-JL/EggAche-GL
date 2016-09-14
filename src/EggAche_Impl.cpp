//
// Common Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include "EggAche.h"
#include "EggAche_Impl.h"

namespace EggAche
{
	EggAche_Impl::GUIFactory *NewGUIFactory ()
	{
#ifdef EGGACHE_WINDOWS
		return new EggAche_Impl::GUIFactory_Windows ();
#endif
#ifdef EGGACHE_LINUX
		return new EggAche_Impl::GUIFactory_Linux ();
#endif
#ifdef EGGACHE_MAC
		return new EggAche_Impl::GUIFactory_Mac ();
#endif
	}

	Window::Window (size_t width,
					size_t height,
					const char *cap_string)
		: bgEgg (nullptr), windowImpl (nullptr)
	{
		bgEgg = new Egg (width, height);

		auto guiFactory = NewGUIFactory ();

		windowImpl = guiFactory->NewWindow (width, height, cap_string);
		windowImpl->OnRefresh (std::bind (&Window::Refresh, this));

		delete guiFactory;
	}

	Window::~Window ()
	{
		delete windowImpl;
		delete bgEgg;
	}

	Egg *Window::GetBackground ()
	{
		return bgEgg;
	}

	void Egg::DrawOnContext (EggAche_Impl::GUIContext *parentContext,
							 size_t x, size_t y) const
	{
		// Actual Position of this Egg
		this->context->PaintOnContext (parentContext, x, y);

		for (auto subEgg : this->subEggs)
			subEgg->DrawOnContext (parentContext,
								   x + subEgg->x, y + subEgg->y);
	}

	void Window::Refresh ()
	{
		if (IsClosed ())
			return;

		auto guiFactory = NewGUIFactory ();

		// Remarks:
		// Buffering the Drawing Content into a Context
		// to avoid flash Screen

		auto wndSize = windowImpl->GetSize ();
		auto context = 
			guiFactory->NewGUIContext (wndSize.first, wndSize.second);

		context->SetBrush (false, 255, 255, 255);
		context->DrawRect (-10, -10, wndSize.first + 10, wndSize.second + 10);

		this->bgEgg->DrawOnContext (context, 0, 0);
		windowImpl->Draw (context, 0, 0);

		delete context;
		delete guiFactory;
	}

	bool Window::IsClosed () const
	{
		return windowImpl->IsClosed ();
	}

	void Window::OnClick (std::function<void (int, int)> fn)
	{
		windowImpl->OnClick (std::move (fn));
	}

	void Window::OnPress (std::function<void (char)> fn)
	{
		windowImpl->OnPress (std::move (fn));
	}

	void Window::OnResized (std::function<void (int, int)> fn)
	{
		windowImpl->OnResized (std::move (fn));
	}

	Egg::Egg (size_t width, size_t height,
			  int pos_x, int pos_y)
		: context (nullptr), x (pos_x), y (pos_y)
	{
		auto guiFactory = NewGUIFactory ();
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

	void Egg::AddEgg (Egg *egg)
	{
		if (egg != this)
		{
			subEggs.push_back (egg);
			subEggs.unique ();
		}
	}

	void Egg::RemoveEgg (Egg *egg)
	{
		subEggs.remove (egg);
	}

	bool Egg::SetPen (unsigned int width,
					  unsigned int r, unsigned int g, unsigned int b)
	{
		return context->SetPen (width, r, g, b);
	}

	bool Egg::SetBrush (bool isTransparent,
						unsigned int r, unsigned int g, unsigned int b)
	{
		return context->SetBrush (isTransparent, r, g, b);
	}

	bool Egg::SetFont (unsigned int size, const char *family,
					   unsigned int r, unsigned int g, unsigned int b)
	{
		return context->SetFont (size, family, r, g, b);
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

	bool Egg::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
						int wElps, int hElps)
	{
		return context->DrawRdRt (xBeg, yBeg, xEnd, yEnd,
								  wElps, hElps);
	}

	bool Egg::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
					   int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawArc (xLeft, yTop, xRight, yBottom,
								 xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
						 int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawChord (xLeft, yTop, xLeft, yBottom,
								   xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
					   int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawPie (xLeft, yTop, xLeft, yBottom,
								 xBeg, yBeg, xEnd, yEnd);
	}

	bool Egg::DrawTxt (int xBeg, int yBeg, const char *szText)
	{
		return context->DrawTxt (xBeg, yBeg, szText);
	}

	bool Egg::DrawBmp (const char *szPath, int x, int y)
	{
		return context->DrawBmp (szPath, x, y);
	}

	bool Egg::DrawBmp (const char *szPath, int x, int y,
					   int width, int height, int r, int g, int b)
	{
		return context->DrawBmp (szPath, x, y, width, height, r, g, b);
	}

	void Egg::Clear ()
	{
		context->Clear ();
	}

	void MsgBox (const char *szTxt, const char *szCap)
	{
		EggAche_Impl::MsgBox_Impl (szTxt, szCap);
	}
}