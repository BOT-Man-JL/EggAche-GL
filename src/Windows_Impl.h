//
// Windows Implementation Interface of EggAche Graphics Library
// By BOT Man, 2016
//

#ifndef EGGACHE_GL_IMPL_WIN
#define EGGACHE_GL_IMPL_WIN

#include <string>
#include <unordered_map>
#include <Windows.h>

#include "EggAche_Impl.h"

namespace EggAche
{
	class WindowImpl_Windows : public WindowImpl
	{
	public:
		WindowImpl_Windows (size_t width, size_t height,
							const char *cap_string);
		~WindowImpl_Windows () override;

		void Draw (const GUIContext *context, size_t x, size_t y) override;
		void Clear () override;

		std::pair<size_t, size_t> GetSize () override;
		bool IsClosed () const override;

		void OnClick (std::function<void (int, int)> fn) override;
		void OnPress (std::function<void (char)> fn) override;
		void OnResized (std::function<void (int, int)> fn) override;
		void OnRefresh (std::function<void ()> fn) override;

	protected:
		int			_cxCanvas, _cyCanvas;
		int			_cxClient, _cyClient;
		std::string	capStr;

		HWND		_hwnd;
		HANDLE		_hEvent;
		bool		_fFailed;

		std::function<void (int, int)> onClick;
		std::function<void (char)> onPress;
		std::function<void (int, int)> onResized;
		std::function<void ()> onRefresh;

		static void WINAPI _NewWindow_Thread (WindowImpl_Windows *pew);
		static LRESULT CALLBACK _WndProc (HWND, UINT, WPARAM, LPARAM);

		WindowImpl_Windows (const WindowImpl_Windows &) = delete;		// Not allow to copy
		void operator= (const WindowImpl_Windows &) = delete;			// Not allow to copy
	};

	class GUIContext_Windows : public GUIContext
	{
	public:
		GUIContext_Windows (size_t width, size_t height);
		~GUIContext_Windows () override;

		bool SetPen (unsigned int width,
					 unsigned int r = 0,
					 unsigned int g = 0,
					 unsigned int b = 0) override;

		bool SetBrush (unsigned int r,
					   unsigned int g,
					   unsigned int b) override;

		bool SetFont (unsigned int size = 18,
					  const char *family = "Consolas",
					  unsigned int r = 0,
					  unsigned int g = 0,
					  unsigned int b = 0) override;

		bool DrawLine (int xBeg, int yBeg, int xEnd, int yEnd) override;

		bool DrawRect (int xBeg, int yBeg, int xEnd, int yEnd) override;

		bool DrawElps (int xBeg, int yBeg, int xEnd, int yEnd) override;

		bool DrawRdRt (int xBeg, int yBeg,
					   int xEnd, int yEnd, int wElps, int hElps) override;

		bool DrawArc (int xLeft, int yTop, int xRight, int yBottom,
					  int xBeg, int yBeg, int xEnd, int yEnd) override;

		bool DrawChord (int xLeft, int yTop, int xRight, int yBottom,
						int xBeg, int yBeg, int xEnd, int yEnd) override;

		bool DrawPie (int xLeft, int yTop, int xRight, int yBottom,
					  int xBeg, int yBeg, int xEnd, int yEnd) override;

		bool DrawTxt (int xBeg, int yBeg, const char *szText) override;

		bool DrawBmp (const char *szPath,
					  int x, int y,
					  int width = -1, int height = -1,
					  int r = -1,
					  int g = -1,
					  int b = -1) override;

		void Clear () override;

		void PaintOnContext (GUIContext *,
							 size_t x, size_t y) const override;

	protected:
		HDC _hdc;
		HBITMAP _hBitmap;
		size_t _w, _h;

		static const COLORREF _colorMask;
		static const COLORREF _GetColor (int r, int g, int b);

		friend void WindowImpl_Windows::Draw (const GUIContext *, size_t, size_t);

		GUIContext_Windows (const GUIContext_Windows &) = delete;		// Not allow to copy
		void operator= (const GUIContext_Windows &) = delete;			// Not allow to copy
	};
}

#endif