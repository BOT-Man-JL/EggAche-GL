
#ifndef EGGACHE_GL_IMPL_WIN
#define EGGACHE_GL_IMPL_WIN

#include <unordered_map>
#include <string>
#include <Windows.h>
#include "EggAche_impl.h"

namespace EggAche
{
	class WindowImpl_Windows : public WindowImpl
	{
	public:
		WindowImpl_Windows (size_t width, size_t height,
							const char *cap_string);
		~WindowImpl_Windows ();

		void Draw (const GUIContext *context, size_t x, size_t y) override;
		bool IsClosed () const override;

		WindowImpl_Windows (const WindowImpl_Windows &) = delete;		// Not allow to copy
		void operator= (const WindowImpl_Windows &) = delete;			// Not allow to copy
	protected:
		int			_cxCanvas, _cyCanvas;
		std::string	_szCap;

		HWND		_hwnd;
		HANDLE		_hEvent;
		bool		_fFailed;

		static std::unordered_map<HWND, WindowImpl_Windows *> _mHwnd;
		static void WINAPI _NewWindow_Thread (WindowImpl_Windows *pew);
		static LRESULT CALLBACK _WndProc (HWND, UINT, WPARAM, LPARAM);
	};

	class GUIContext_Windows : public GUIContext
	{
	public:
		GUIContext_Windows (size_t width, size_t height);
		~GUIContext_Windows ();

		bool SetPen (unsigned int width,
					 unsigned int r = 0,
					 unsigned int g = 0,
					 unsigned int b = 0) override;

		bool SetBrush (unsigned int r,
					   unsigned int g,
					   unsigned int b) override;

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
					  int x = 0, int y = 0,
					  int width = -1, int height = -1,
					  int r = -1,
					  int g = -1,
					  int b = -1) override;

		void Clear () override;

		friend class WindowImpl_Windows;
		GUIContext_Windows (const GUIContext_Windows &) = delete;		// Not allow to copy
		void operator= (const GUIContext_Windows &) = delete;			// Not allow to copy
	protected:
		HDC _hdc;
		HBITMAP _hBitmap;
		size_t _w, _h;

		static const COLORREF _colorMask;
		static const COLORREF _GetColor (int r, int g, int b);
	};

	class GUIFactory_Windows : public GUIFactory
	{
	public:
		WindowImpl *NewWindow (size_t width, size_t height,
							   const char *cap_string) override;
		GUIContext *NewGUIContext (size_t width, size_t height) override;
	};
}

#endif