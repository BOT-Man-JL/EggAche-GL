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
		: bgCanvas (nullptr), windowImpl (nullptr)
	{
		std::unique_ptr<EggAche_Impl::GUIFactory> guiFactory (
			NewGUIFactory ());

		windowImpl = guiFactory->NewWindow (width, height, cap_string);
		windowImpl->OnRefresh (std::bind (&Window::Refresh, this));
	}

	Window::~Window ()
	{
		delete windowImpl;
	}

	void Window::SetBackground (Canvas *canvas)
	{
		bgCanvas = canvas;
	}

	void Window::ClearBackground ()
	{
		bgCanvas = nullptr;
	}

	void Canvas::RecursiveDraw (EggAche_Impl::GUIContext *parentContext,
								size_t x, size_t y) const
	{
		// Actual Position of this Canvas
		this->context->PaintOnContext (parentContext, x, y);

		for (auto subCanvas : this->subCanvases)
			subCanvas->RecursiveDraw (parentContext,
									  x + subCanvas->x, y + subCanvas->y);
	}

	void Window::Refresh ()
	{
		if (IsClosed ())
			return;

		if (!this->bgCanvas)
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

		this->bgCanvas->RecursiveDraw (context.get (), 0, 0);
		windowImpl->Draw (context.get (), 0, 0);
	}

	bool Canvas::SaveAsImg (std::function<bool (EggAche_Impl::GUIContext *context)> fn) const
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

	bool Canvas::SaveAsJpg (const char * fileName) const
	{
		auto fn = [&] (EggAche_Impl::GUIContext *context)
		{
			return context->SaveAsJpg (fileName);
		};
		return SaveAsImg (fn);
	}

	bool Canvas::SaveAsPng (const char * fileName) const
	{
		auto fn = [&] (EggAche_Impl::GUIContext *context)
		{
			return context->SaveAsPng (fileName);
		};
		return SaveAsImg (fn);
	}

	bool Canvas::SaveAsBmp (const char *fileName) const
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

	void Window::OnResized (std::function<void (Window *, unsigned, unsigned)> fn)
	{
		windowImpl->OnResized ([=] (unsigned x, unsigned y)
		{
			fn (this, x, y);
		});
	}

	Canvas::Canvas (size_t width, size_t height,
					int pos_x, int pos_y)
		: context (nullptr), x (pos_x), y (pos_y), w (width), h (height)
	{
		std::unique_ptr<EggAche_Impl::GUIFactory> guiFactory (
			NewGUIFactory ());
		context = guiFactory->NewGUIContext (width, height);
	}

	Canvas::~Canvas ()
	{
		delete context;
	}

	void Canvas::Buffering ()
	{
		// Todo:
	}

	void Canvas::operator+= (Canvas *canvas)
	{
		if (canvas != this)
		{
			subCanvases.push_back (canvas);
			subCanvases.unique ();
		}
	}

	void Canvas::operator-= (Canvas *canvas)
	{
		subCanvases.remove (canvas);
	}

	void Canvas::Move (int scale_x, int scale_y)
	{
		x = x + scale_x;
		y = y + scale_y;
	}

	void Canvas::MoveTo (int pos_x, int pos_y)
	{
		x = pos_x;
		y = pos_y;
	}

	bool Canvas::SetPen (unsigned width,
						 unsigned r, unsigned g, unsigned b)
	{
		return context->SetPen (width, r, g, b);
	}

	bool Canvas::SetBrush (bool isTransparent,
						   unsigned r, unsigned g, unsigned b)
	{
		return context->SetBrush (isTransparent, r, g, b);
	}

	bool Canvas::SetFont (unsigned size, const char *family,
						  unsigned r, unsigned g, unsigned b)
	{
		return context->SetFont (size, family, r, g, b);
	}

	bool Canvas::DrawLine (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawLine (xBeg, yBeg, xEnd, yEnd);
	}

	bool Canvas::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawRect (xBeg, yBeg, xEnd, yEnd);
	}

	bool Canvas::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return context->DrawElps (xBeg, yBeg, xEnd, yEnd);
	}

	bool Canvas::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
						   int wElps, int hElps)
	{
		return context->DrawRdRt (xBeg, yBeg, xEnd, yEnd,
								  wElps, hElps);
	}

	bool Canvas::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
						  double angleBeg, double cAngle)
	{
		return context->DrawArc (xLeft, yTop, xRight, yBottom,
								 angleBeg, cAngle);
	}

	bool Canvas::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
							double angleBeg, double cAngle)
	{
		return context->DrawChord (xLeft, yTop, xRight, yBottom,
								   angleBeg, cAngle);
	}

	bool Canvas::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
						  double angleBeg, double cAngle)
	{
		return context->DrawPie (xLeft, yTop, xRight, yBottom,
								 angleBeg, cAngle);
	}

	bool Canvas::DrawTxt (int xBeg, int yBeg, const char *szText)
	{
		return context->DrawTxt (xBeg, yBeg, szText);
	}

	size_t Canvas::GetTxtWidth (const char *szText)
	{
		return context->GetTxtWidth (szText);
	}

	bool Canvas::DrawImg (const char *fileName, int x, int y)
	{
		return context->DrawImg (fileName, x, y);
	}

	bool Canvas::DrawImg (const char *fileName, int x, int y,
						  unsigned width, unsigned height)
	{
		return context->DrawImg (fileName, x, y, width, height);
	}

	bool Canvas::DrawImg (const char *fileName, int x, int y,
						  unsigned width, unsigned height,
						  unsigned r, unsigned g, unsigned b)
	{
		return context->DrawImg (fileName, x, y, width, height, r, g, b);
	}

	bool Canvas::DrawImgMask (const char *srcFile,
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

	void Canvas::Clear ()
	{
		context->Clear ();
	}

	void MsgBox (const char *szTxt, const char *szCap)
	{
		EggAche_Impl::MsgBox_Impl (szTxt, szCap);
	}
}