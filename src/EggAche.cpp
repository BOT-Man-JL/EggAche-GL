//
// Common Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include <memory>

#include "EggAche.h"
#include "EggAche_Impl.h"

namespace EggAche
{
	EggAche_Impl::GUIFactory *NewGUIFactory ()
	{
#ifdef EGGACHE_WINDOWS
		return new EggAche_Impl::GUIFactory_Windows ();
#endif
#ifdef EGGACHE_XWINDOW
		return new EggAche_Impl::GUIFactory_XWindow ();
#endif
	}

	Window::Window (size_t width,
					size_t height,
					const char *cap_string)
		: bgEgg (new Egg (width, height)), windowImpl (nullptr)
	{
		try
		{
			std::unique_ptr<EggAche_Impl::GUIFactory> guiFactory (
				NewGUIFactory ());

			windowImpl = guiFactory->NewWindow (width, height, cap_string);
			windowImpl->OnRefresh (std::bind (&Window::Refresh, this));
		}
		catch (const std::exception&)
		{
			delete bgEgg;
		}
	}

	Window::~Window ()
	{
		delete windowImpl;
		delete bgEgg;
	}

	Egg &Window::GetBackground ()
	{
		return *bgEgg;
	}

	void Egg::RecursiveDraw (EggAche_Impl::GUIContext *parentContext,
							 size_t x, size_t y) const
	{
		// Actual Position of this Egg
		this->context->PaintOnContext (parentContext, x, y);

		for (auto subEgg : this->subEggs)
			subEgg->RecursiveDraw (parentContext,
								   x + subEgg->x, y + subEgg->y);
	}

	void Window::Refresh ()
	{
		if (IsClosed ())
			return;

		std::unique_ptr<EggAche_Impl::GUIFactory> guiFactory (
			NewGUIFactory ());

		// Remarks:
		// Buffering the Drawing Content into a Context
		// to avoid flash Screen

		auto wndSize = windowImpl->GetSize ();
		std::unique_ptr<EggAche_Impl::GUIContext> context (
			guiFactory->NewGUIContext (wndSize.first, wndSize.second));

		context->SetBrush (false, 255, 255, 255);
		context->DrawRect (-10, -10,
			(int) (wndSize.first + 10), (int) (wndSize.second + 10));

		this->bgEgg->RecursiveDraw (context.get (), 0, 0);
		windowImpl->Draw (context.get (), 0, 0);
	}

	bool Egg::SaveAsImg (std::function<bool (EggAche_Impl::GUIContext *context)> fn) const
	{
		std::unique_ptr<EggAche_Impl::GUIFactory> guiFactory (
			NewGUIFactory ());
		std::unique_ptr<EggAche_Impl::GUIContext> context (
			guiFactory->NewGUIContext (this->w, this->h));

		context->SetBrush (false, 255, 255, 255);
		context->DrawRect (-10, -10, (int) (this->w + 10), (int) (this->h + 10));

		this->RecursiveDraw (context.get (), 0, 0);
		auto ret = fn (context.get ());

		return ret;
	}

	bool Egg::SaveAsJpg (const char * fileName) const
	{
		auto fn = [&] (EggAche_Impl::GUIContext *context)
		{
			return context->SaveAsJpg (fileName);
		};
		return SaveAsImg (fn);
	}

	bool Egg::SaveAsPng (const char * fileName) const
	{
		auto fn = [&] (EggAche_Impl::GUIContext *context)
		{
			return context->SaveAsPng (fileName);
		};
		return SaveAsImg (fn);
	}

	bool Egg::SaveAsBmp (const char *fileName) const
	{
		auto fn = [&] (EggAche_Impl::GUIContext *context)
		{
			return context->SaveAsBmp (fileName);
		};
		return SaveAsImg (fn);
	}

	bool Window::IsClosed () const
	{
		return windowImpl->IsClosed ();
	}

	void Window::OnClick (std::function<void (Window *, unsigned, unsigned)> fn)
	{
		windowImpl->OnClick ([=] (unsigned x, unsigned y)
		{
			fn (this, x, y);
		});
	}

	void Window::OnPress (std::function<void (Window *, char)> fn)
	{
		windowImpl->OnPress ([=] (char ch)
		{
			fn (this, ch);
		});
	}

	Egg::Egg (size_t width, size_t height,
			  int pos_x, int pos_y)
		: context (nullptr), x (pos_x), y (pos_y), w (width), h (height)
	{
		std::unique_ptr<EggAche_Impl::GUIFactory> guiFactory (
			NewGUIFactory ());
		context = guiFactory->NewGUIContext (width, height);
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

	bool Egg::SetPen (unsigned width,
					  unsigned r, unsigned g, unsigned b)
	{
		return context->SetPen (width, r, g, b);
	}

	bool Egg::SetBrush (bool isTransparent,
						unsigned r, unsigned g, unsigned b)
	{
		return context->SetBrush (isTransparent, r, g, b);
	}

	bool Egg::SetFont (unsigned size, const char *family,
					   unsigned r, unsigned g, unsigned b)
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
					   double angleBeg, double cAngle)
	{
		return context->DrawArc (xLeft, yTop, xRight, yBottom,
								 angleBeg, cAngle);
	}

	bool Egg::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
						 double angleBeg, double cAngle)
	{
		return context->DrawChord (xLeft, yTop, xRight, yBottom,
								   angleBeg, cAngle);
	}

	bool Egg::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
					   double angleBeg, double cAngle)
	{
		return context->DrawPie (xLeft, yTop, xRight, yBottom,
								 angleBeg, cAngle);
	}

	bool Egg::DrawTxt (int xBeg, int yBeg, const char *szText)
	{
		return context->DrawTxt (xBeg, yBeg, szText);
	}

	size_t Egg::GetTxtWidth (const char *szText)
	{
		return context->GetTxtWidth (szText);
	}

	bool Egg::DrawImg (const char *fileName, int x, int y)
	{
		return context->DrawImg (fileName, x, y);
	}

	bool Egg::DrawImg (const char *fileName, int x, int y,
					   unsigned width, unsigned height)
	{
		return context->DrawImg (fileName, x, y, width, height);
	}

	bool Egg::DrawImg (const char *fileName, int x, int y,
					   unsigned width, unsigned height,
					   unsigned r, unsigned g, unsigned b)
	{
		return context->DrawImg (fileName, x, y, width, height, r, g, b);
	}

	bool Egg::DrawImgMask (const char *srcFile,
						   const char *maskFile,
						   unsigned width, unsigned height,
						   int x_pos, int y_pos,
						   unsigned x_src, unsigned y_src,
						   unsigned x_msk, unsigned y_msk)
	{
		return context->DrawImgMask (srcFile, maskFile,
									 width, height,
									 x_pos, y_pos,
									 x_src, y_src,
									 x_msk, y_msk);
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