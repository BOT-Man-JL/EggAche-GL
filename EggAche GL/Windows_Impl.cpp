
#include <Windows.h>
#include <exception>
#include "Windows_Impl.h"
#pragma comment (lib, "Msimg32.lib")

namespace EggAche
{
	// Factory

	WindowImpl *GUIFactory_Windows::NewWindow (size_t width, size_t height,
											   const char *cap_string)
	{
		return new WindowImpl_Windows (width, height, cap_string);
	}

	GUIContext *GUIFactory_Windows::NewGUIContext (size_t width, size_t height)
	{
		return new GUIContext_Windows (width, height);
	}

	// Window

	std::unordered_map<HWND, WindowImpl_Windows *> WindowImpl_Windows::_mHwnd;

	WindowImpl_Windows::WindowImpl_Windows (size_t width, size_t height, const char *cap_string)
		: _cxCanvas (width), _cyCanvas (height), _szCap (nullptr),
		_hwnd (NULL), _hEvent (NULL), _fFailed (false), WindowImpl (width, height, cap_string)
	{
		_szCap = new char[strlen (cap_string) + 1];
		strcpy (_szCap, cap_string);

		if (width < 240 || height < 120)
			throw std::runtime_error ("Err_Window_#1_Too_Small");

#ifdef _MSC_VER
		// Windows SDK only support Unicode version Window Class
		if (_mHwnd.empty ())
		{
			WNDCLASSW wndclass;
			wndclass.style = CS_HREDRAW | CS_VREDRAW;
			wndclass.lpfnWndProc = _WndProc;
			wndclass.cbClsExtra = 0;
			wndclass.cbWndExtra = 0;
			wndclass.hInstance = (HINSTANCE) GetCurrentProcess ();
			wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
			wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
			wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
			wndclass.lpszMenuName = NULL;
			wndclass.lpszClassName = L"LJN_WNDCLASSA";

			if (!RegisterClassW (&wndclass))
				throw std::runtime_error ("Err_Window_#2_RegClass");
		}
#else
		// MinGW only support ANSI version Window Class
		if (_mHwnd.empty ())
		{
			WNDCLASSA wndclass;
			wndclass.style = CS_HREDRAW | CS_VREDRAW;
			wndclass.lpfnWndProc = _WndProc;
			wndclass.cbClsExtra = 0;
			wndclass.cbWndExtra = 0;
			wndclass.hInstance = (HINSTANCE) GetCurrentProcess ();
			wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
			wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
			wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
			wndclass.lpszMenuName = NULL;
			wndclass.lpszClassName = "LJN_WNDCLASSA";

			if (!RegisterClassA (&wndclass))
				throw std::runtime_error ("Err_Window_#2_RegClass");
		}
#endif

		this->_hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
		if (!this->_hEvent)
		{
			throw std::runtime_error ("Err_Window_#2_Event");
		}

		auto hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) _NewWindow_Thread,
			(LPVOID) this, 0, NULL);

		if (!hThread)
		{
			CloseHandle (this->_hEvent);
			throw std::runtime_error ("Err_Window_#2_Thread");
		}

		WaitForSingleObject (this->_hEvent, INFINITE);
		CloseHandle (hThread);
		CloseHandle (this->_hEvent);

		if (this->_fFailed)
			throw std::runtime_error ("Err_Window_#3_CreateWindow");
	}

	WindowImpl_Windows::~WindowImpl_Windows ()
	{
		delete[] _szCap;
		if (this->_hwnd != NULL)
			SendMessage (this->_hwnd, WM_CLOSE, 0, 0);
	}

	void WindowImpl_Windows::Draw (const GUIContext *context, size_t x, size_t y)
	{
		RECT	rect;
		HDC		hdcWnd;

		if (this->_hwnd == NULL)
			return;

		hdcWnd = GetDC (this->_hwnd);
		if (!hdcWnd) throw std::runtime_error ("Draw Failed at GetDC");

		// Assume that context is GUIContext_Windows
		auto _context = static_cast<const GUIContext_Windows *> (context);
		if (!TransparentBlt (hdcWnd, x, y, _context->_w, _context->_h,
							 _context->_hdc, 0, 0, _context->_w, _context->_h,
							 GUIContext_Windows::_colorMask))
		{
			ReleaseDC (this->_hwnd, hdcWnd);
			throw std::runtime_error ("Draw Failed at BitBlt");
		}

		ReleaseDC (this->_hwnd, hdcWnd);
	}

	bool WindowImpl_Windows::IsClosed () const
	{
		return this->_hwnd == NULL;
	}

	void WindowImpl_Windows::_NewWindow_Thread (WindowImpl_Windows *pew)
	{
		MSG msg;

		pew->_hwnd = CreateWindowA ("LJN_WNDCLASSA", pew->_szCap,
									WS_OVERLAPPEDWINDOW,  // & ~WS_THICKFRAME &~WS_MAXIMIZEBOX,
									CW_USEDEFAULT, CW_USEDEFAULT,  //CW_USEDEFAULT, CW_USEDEFAULT,
									pew->_cxCanvas, pew->_cyCanvas,
									NULL, NULL, (HINSTANCE) GetCurrentProcess (), NULL);
		if (!pew->_hwnd)
		{
			pew->_fFailed = true;
			SetEvent (pew->_hEvent);
		}

		_mHwnd[pew->_hwnd] = pew;

		ShowWindow (pew->_hwnd, SW_NORMAL);
		UpdateWindow (pew->_hwnd);
		SetEvent (pew->_hEvent);

		while (GetMessage (&msg, NULL, 0, 0))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	LRESULT CALLBACK WindowImpl_Windows::_WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			//case WM_LBUTTONUP:
			//case WM_RBUTTONUP:
			//	if (_mHwnd[hwnd]->_fnOnClick)
			//		_mHwnd[hwnd]->_fnOnClick (
			//			GET_X_LPARAM (lParam) * _mHwnd[hwnd]->_cxCanvas / _mHwnd[hwnd]->_cxClient,
			//			GET_Y_LPARAM (lParam) * _mHwnd[hwnd]->_cyCanvas / _mHwnd[hwnd]->_cyClient);
			//	return 0;

			//case WM_CHAR:
			//	if (_mHwnd[hwnd]->_fnOnPress)
			//		_mHwnd[hwnd]->_fnOnPress ((char) wParam);
			//	return 0;

		case WM_SIZE:
			//_mHwnd[hwnd]->_cxClient = LOWORD (lParam);
			//_mHwnd[hwnd]->_cyClient = HIWORD (lParam);
			return 0;

		case WM_PAINT:
			BeginPaint (hwnd, NULL);
			//_mHwnd[hwnd]->Draw (context);
			EndPaint (hwnd, NULL);
			return 0;

		case WM_DESTROY:
			_mHwnd[hwnd]->_hwnd = NULL;
			_mHwnd.erase (hwnd);
			_mHwnd[NULL] = nullptr;		// _mHwnd.empty == false

			PostQuitMessage (0);
			return 0;
		}

		return DefWindowProc (hwnd, message, wParam, lParam);
	}

	// Context

	const COLORREF GUIContext_Windows::_colorMask = RGB (0, 0, 201);
	const COLORREF GUIContext_Windows::_GetColor (int r, int g, int b)
	{
		const auto mMax = [] (const int &a, const int &b) { return a > b ? a : b; };
		const auto mMin = [] (const int &a, const int &b) { return a < b ? a : b; };

		r = mMax (0, mMin (255, r));
		g = mMax (0, mMin (255, g));
		b = mMax (0, mMin (255, b));

		if (RGB (r, b, b) != _colorMask)
			return RGB (r, g, b);
		else
			return RGB (r, g, b) + 1;
	}

	GUIContext_Windows::GUIContext_Windows (size_t width, size_t height)
		: _hdc (NULL), _hBitmap (NULL), _w (width), _h (height),
		GUIContext (width, height)
	{
		HBRUSH	hBrush;
		RECT	rect;
		HDC		hdcWnd;

		hdcWnd = GetDC (NULL);
		this->_hdc = CreateCompatibleDC (hdcWnd);

		auto cxBmp = (int) (this->_w);
		auto cyBmp = (int) (this->_h);

		_hBitmap = CreateCompatibleBitmap (hdcWnd, cxBmp, cyBmp);

		SetMapMode (this->_hdc, MM_ANISOTROPIC);
		SetWindowExtEx (this->_hdc, width, height, NULL);
		SetViewportExtEx (this->_hdc, cxBmp, cyBmp, NULL);

		if (!this->_hdc || !this->_hBitmap)
		{
			if (this->_hBitmap) DeleteObject (this->_hBitmap);
			if (this->_hdc) DeleteDC (this->_hdc);
			ReleaseDC (NULL, hdcWnd);
			throw std::runtime_error ("Err_DC_#0_Bitmap");
		}
		SelectObject (this->_hdc, this->_hBitmap);

		rect.left = rect.top = 0;
		rect.right = width;
		rect.bottom = height;
		hBrush = CreateSolidBrush (_colorMask);
		FillRect (this->_hdc, &rect, hBrush);

		SelectObject (this->_hdc, (HBRUSH) GetStockObject (NULL_BRUSH));
		SelectObject (this->_hdc, (HPEN) GetStockObject (BLACK_PEN));
		SetBkMode (this->_hdc, TRANSPARENT);

		ReleaseDC (NULL, hdcWnd);
		DeleteObject (hBrush);
	}

	GUIContext_Windows::~GUIContext_Windows ()
	{
		HGDIOBJ hObj;

		hObj = SelectObject (this->_hdc, (HBRUSH) GetStockObject (NULL_BRUSH));
		if (hObj != GetStockObject (NULL_BRUSH))
			DeleteObject (hObj);
		hObj = SelectObject (this->_hdc, (HPEN) GetStockObject (BLACK_PEN));
		if (hObj != GetStockObject (BLACK_PEN) && hObj != GetStockObject (NULL_PEN))
			DeleteObject (hObj);

		DeleteObject (this->_hBitmap);
		DeleteDC (this->_hdc);
	}

	bool GUIContext_Windows::SetPen (unsigned int width, unsigned int r, unsigned int g, unsigned int b)
	{
		HPEN		hPen;
		HGDIOBJ		hObj;
		COLORREF	color = _GetColor (r, g, b);

		if (r == -1 || g == -1 || b == -1 || width == 0)
		{
			hObj = SelectObject (this->_hdc, (HPEN) GetStockObject (NULL_PEN));
			if (hObj != GetStockObject (BLACK_PEN) && hObj != GetStockObject (NULL_PEN))
				DeleteObject (hObj);
			return true;
		}

		hPen = CreatePen (PS_SOLID, max (0, width), color);
		if (!hPen) return false;

		hObj = SelectObject (this->_hdc, hPen);
		if (hObj != GetStockObject (BLACK_PEN) && hObj != GetStockObject (NULL_PEN))
			DeleteObject (hObj);

		return true;
	}

	bool GUIContext_Windows::SetBrush (unsigned int r, unsigned int g, unsigned int b)
	{
		HBRUSH		hBrush;
		HGDIOBJ		hObj;
		COLORREF	color = _GetColor (r, g, b);

		if (r == -1 || g == -1 || b == -1)
		{
			hObj = SelectObject (this->_hdc, (HPEN) GetStockObject (NULL_BRUSH));
			if (hObj != GetStockObject (NULL_BRUSH))
				DeleteObject (hObj);
			return true;
		}

		hBrush = CreateSolidBrush (color);
		if (!hBrush) return false;

		hObj = SelectObject (this->_hdc, hBrush);
		if (hObj != GetStockObject (NULL_BRUSH))
			DeleteObject (hObj);

		return true;
	}

	bool GUIContext_Windows::DrawLine (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		if (!MoveToEx (this->_hdc, xBeg, yBeg, NULL))	return false;
		if (!LineTo (this->_hdc, xEnd, yEnd))			return false;
		return true;
	}

	bool GUIContext_Windows::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Rectangle (this->_hdc, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Ellipse (this->_hdc, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd, int wElps, int hElps)
	{
		return !!RoundRect (this->_hdc, xBeg, yBeg, xEnd, yEnd, wElps, hElps);
	}

	bool GUIContext_Windows::DrawArc (int xLeft, int yTop, int xRight, int yBottom, int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Arc (this->_hdc, xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawChord (int xLeft, int yTop, int xRight, int yBottom, int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Chord (this->_hdc, xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawPie (int xLeft, int yTop, int xRight, int yBottom, int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Pie (this->_hdc, xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawTxt (int xBeg, int yBeg, const char * szText,
									  size_t fontSize, const char *fontFamily)
	{
		HFONT hFont, hFontPre;
		hFont = CreateFontA (fontSize, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
							 CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, fontFamily);
		hFontPre = (HFONT) SelectObject (this->_hdc, hFont);

		return !!TextOutA (this->_hdc, xBeg, yBeg, szText, (int) strlen (szText));

		SelectObject (this->_hdc, hFontPre);
		DeleteObject (hFont);
	}

	bool GUIContext_Windows::DrawBmp (const char * szPath, int x, int y, int width, int height, int r, int g, int b)
	{
		HDC			hdcMemImag;
		HBITMAP		hBitmapImag;
		BITMAP		bitmap;
		COLORREF	colorMask;

		hBitmapImag = (HBITMAP) LoadImageA (NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (!hBitmapImag) return false;
		GetObject (hBitmapImag, sizeof (BITMAP), &bitmap);

		hdcMemImag = CreateCompatibleDC (this->_hdc);
		if (!hdcMemImag)
		{
			DeleteObject (hBitmapImag);
			return false;
		}
		SelectObject (hdcMemImag, hBitmapImag);

		if (width == -1) width = bitmap.bmWidth;
		if (height == -1) height = bitmap.bmHeight;

		if (r == -1 || g == -1 || b == -1)
		{
			if (!StretchBlt (this->_hdc, x, y, width, height,
							 hdcMemImag, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
							 SRCCOPY))
			{
				DeleteDC (hdcMemImag);
				DeleteObject (hBitmapImag);
				return false;
			}
		}
		else
		{
			colorMask = RGB (max (0, min (255, r)),
							 max (0, min (255, g)),
							 max (0, min (255, b)));
			if (!TransparentBlt (this->_hdc, x, y, width, height,
								 hdcMemImag, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
								 colorMask))
			{
				DeleteDC (hdcMemImag);
				DeleteObject (hBitmapImag);
				return false;
			}
		}

		DeleteDC (hdcMemImag);
		DeleteObject (hBitmapImag);
		return true;
	}

	void GUIContext_Windows::Clear ()
	{
		RECT rect;
		rect.left = rect.top = 0;
		rect.right = this->_w;
		rect.bottom = this->_h;

		auto hBrush = CreateSolidBrush (_colorMask);
		FillRect (this->_hdc, &rect, hBrush);
		DeleteObject (hBrush);
	}

	void MsgBox_Impl (const char * szTxt, const char * szCap)
	{
		MessageBoxA (NULL, szTxt, szCap, MB_OK);
	}
}
