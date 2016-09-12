//
// Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include "EggAche.h"

// Using Windows Impl
#include "Windows_impl.h"

namespace EggAche
{
	Window::Window (size_t width,
					size_t height,
					const char *cap_string)
		: bgEgg (nullptr), windowImpl (nullptr)
	{
		bgEgg = new Egg (width, height);

		GUIFactory *guiFactory = nullptr;

#ifdef WIN32
		// Windows Impl
		guiFactory = new GUIFactory_Windows ();
#endif

		windowImpl = guiFactory->NewWindow (width, height, cap_string);
		windowImpl->OnRefresh (std::bind (&Window::Refresh, this));
		delete guiFactory;
	}

	Window::Window (Window &&origin)
		: bgEgg (origin.bgEgg), windowImpl (origin.windowImpl)
	{
		windowImpl->OnRefresh (std::bind (&Window::Refresh, this));
		origin.bgEgg = nullptr;
		origin.windowImpl = nullptr;
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

	void Window::DrawEgg (GUIContext *parentContext, const Egg *egg,
						  size_t xPre, size_t yPre)
	{
		// Actual Position of this Egg
		auto x = xPre + egg->x, y = yPre + egg->y;
		egg->context->PaintOnContext (parentContext, x, y);

		for (auto subEgg : egg->subEggs)
			DrawEgg (parentContext, subEgg, x, y);
	}

	void Window::Refresh ()
	{
		if (IsClosed ())
			return;

		GUIFactory *guiFactory = nullptr;

#ifdef WIN32
		// Windows Impl
		guiFactory = new GUIFactory_Windows ();
#endif

		// Remarks:
		// Buffering the Drawing Content into a Context
		// to avoid flash Screen

		auto wndSize = windowImpl->GetSize ();
		auto context = 
			guiFactory->NewGUIContext (wndSize.first, wndSize.second);

		context->SetBrush (255, 255, 255);
		context->DrawRect (-10, -10, wndSize.first + 10, wndSize.second + 10);

		DrawEgg (context, this->bgEgg, 0, 0);
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
		GUIFactory *guiFactory = nullptr;

#ifdef WIN32
		// Windows Impl
		guiFactory = new GUIFactory_Windows ();
#endif

		context = guiFactory->NewGUIContext (width, height);
		delete guiFactory;
	}

	Egg::Egg (Egg &&origin)
		: context (origin.context), x (origin.x), y (origin.y),
		subEggs (std::move (origin.subEggs))
	{
		origin.context = nullptr;
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

	bool Egg::DrawTxt (int xBeg, int yBeg, const char *szText,
					   size_t fontSize, const char *fontFamily)
	{
		return context->DrawTxt (xBeg, yBeg, szText,
								 fontSize, fontFamily);
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
		MsgBox_Impl (szTxt, szCap);
	}
}