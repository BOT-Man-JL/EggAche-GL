//
// Windows Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include <exception>
#include <string>
#include <unordered_map>
#include <fstream>

#include <Windows.h>
#include <windowsx.h>
#pragma comment (lib, "Msimg32.lib")

#include "EggAche_Impl.h"

namespace EggAche_Impl
{
	class WindowImpl_Windows : public WindowImpl
	{
	public:
		WindowImpl_Windows (size_t width, size_t height,
							const char *cap_string);
		~WindowImpl_Windows () override;

		void Draw (const GUIContext *context, size_t x, size_t y) override;

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

		bool SetBrush (bool isTransparent,
					   unsigned int r,
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

		bool DrawBmp (const char *fileName,
					  int x, int y,
					  int width = -1, int height = -1,
					  int r = -1,
					  int g = -1,
					  int b = -1) override;

		bool SaveAsBmp (const char *fileName) override;

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

namespace EggAche_Impl
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

	class HwndManager
	{
	private:
		static std::unordered_map<HWND, WindowImpl_Windows *> *_hwndMapper;
	protected:
		HwndManager () {}
	public:
		static bool isRegClass;

		static std::unordered_map<HWND, WindowImpl_Windows *> *Instance ()
		{
			if (_hwndMapper == nullptr)
				_hwndMapper = new std::unordered_map<HWND, WindowImpl_Windows *> ();
			return _hwndMapper;
		}

		static bool IsRefed ()
		{
			return _hwndMapper != nullptr && !_hwndMapper->empty ();
		}

		static void Delete ()
		{
			delete _hwndMapper;
			_hwndMapper = nullptr;
		}
	};

	std::unordered_map<HWND, WindowImpl_Windows *> *HwndManager::_hwndMapper = nullptr;
	bool HwndManager::isRegClass = false;

	WindowImpl_Windows::WindowImpl_Windows (size_t width, size_t height,
											const char *cap_string)
		: _cxCanvas (width), _cyCanvas (height), _cxClient (width), _cyClient (height),
		capStr (cap_string), _hwnd (NULL), _hEvent (NULL), _fFailed (false)
	{
		if (width < 240 || height < 120)
			throw std::runtime_error ("Err_Window_#1_Too_Small");

#ifdef _MSC_VER
		// Windows SDK only support Unicode version Window Class
		if (!HwndManager::isRegClass)
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
		if (!HwndManager::isRegClass)
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
		HwndManager::isRegClass = true;

		this->_hEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
		if (!this->_hEvent)
		{
			throw std::runtime_error ("Err_Window_#2_Event");
		}

		auto hThread = CreateThread (NULL, 0,
			(LPTHREAD_START_ROUTINE) _NewWindow_Thread,
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
		if (this->_hwnd != NULL)
			SendMessage (this->_hwnd, WM_CLOSE, 0, 0);

		if (!HwndManager::IsRefed ())
			HwndManager::Delete ();
	}

	void WindowImpl_Windows::Draw (const GUIContext *context,
								   size_t x, size_t y)
	{
		if (this->_hwnd == NULL)
			return;

		auto hdcWnd = GetDC (this->_hwnd);
		if (!hdcWnd) throw std::runtime_error ("Draw Failed at GetDC");

		// Assume that context is GUIContext_Windows
		auto _context = static_cast<const GUIContext_Windows *> (context);
		if (!TransparentBlt (hdcWnd, x, y, _context->_w, _context->_h,
							 _context->_hdc, 0, 0, _context->_w, _context->_h,
							 GUIContext_Windows::_colorMask))
		{
			ReleaseDC (this->_hwnd, hdcWnd);
			throw std::runtime_error ("Draw Failed at TransparentBlt");
		}

		ReleaseDC (this->_hwnd, hdcWnd);
	}

	std::pair<size_t, size_t> WindowImpl_Windows::GetSize ()
	{
		return std::make_pair (this->_cxClient, this->_cyClient);
	}

	bool WindowImpl_Windows::IsClosed () const
	{
		return this->_hwnd == NULL;
	}

	void WindowImpl_Windows::OnClick (std::function<void (int, int)> fn)
	{
		onClick = std::move (fn);
	}

	void WindowImpl_Windows::OnPress (std::function<void (char)> fn)
	{
		onPress = std::move (fn);
	}

	void WindowImpl_Windows::OnResized (std::function<void (int, int)> fn)
	{
		onResized = std::move (fn);
	}

	void WindowImpl_Windows::OnRefresh (std::function<void ()> fn)
	{
		onRefresh = std::move (fn);
	}

	void WindowImpl_Windows::_NewWindow_Thread (WindowImpl_Windows *pew)
	{
		RECT rect { 0 };
		rect.right = pew->_cxCanvas;
		rect.bottom = pew->_cyCanvas;
		if (!AdjustWindowRect (&rect, WS_OVERLAPPEDWINDOW, FALSE))
		{
			pew->_fFailed = true;
			SetEvent (pew->_hEvent);
		}

		pew->_hwnd = CreateWindowA ("LJN_WNDCLASSA", pew->capStr.c_str (),
									WS_OVERLAPPEDWINDOW,
									// & ~WS_THICKFRAME &~WS_MAXIMIZEBOX,
									CW_USEDEFAULT, CW_USEDEFAULT,
									rect.right - rect.left,
									rect.bottom - rect.top,
									NULL, NULL,
									(HINSTANCE) GetCurrentProcess (), NULL);
		if (!pew->_hwnd)
		{
			pew->_fFailed = true;
			SetEvent (pew->_hEvent);
		}

		(*HwndManager::Instance ())[pew->_hwnd] = pew;

		ShowWindow (pew->_hwnd, SW_NORMAL);
		UpdateWindow (pew->_hwnd);
		SetEvent (pew->_hEvent);

		MSG msg;
		while (GetMessage (&msg, NULL, 0, 0))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	LRESULT CALLBACK WindowImpl_Windows::_WndProc (
		HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!HwndManager::IsRefed ())
			goto tagRet;

		auto hwndMapper = HwndManager::Instance ();
		switch (message)
		{
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			if ((*hwndMapper)[hwnd]->onClick)
				(*hwndMapper)[hwnd]->onClick (GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam));
			return 0;

		case WM_CHAR:
			if ((*hwndMapper)[hwnd]->onPress)
				(*hwndMapper)[hwnd]->onPress ((char) wParam);
			return 0;

		case WM_SIZE:
			(*hwndMapper)[hwnd]->_cxClient = LOWORD (lParam);
			(*hwndMapper)[hwnd]->_cyClient = HIWORD (lParam);
			if ((*hwndMapper)[hwnd]->onResized)
				(*hwndMapper)[hwnd]->onResized ((*hwndMapper)[hwnd]->_cxClient, (*hwndMapper)[hwnd]->_cyClient);
			return 0;

		case WM_PAINT:
			BeginPaint (hwnd, NULL);
			if ((*hwndMapper)[hwnd]->onRefresh)
				(*hwndMapper)[hwnd]->onRefresh ();
			EndPaint (hwnd, NULL);
			return 0;

		case WM_DESTROY:
			(*hwndMapper)[hwnd]->_hwnd = NULL;
			hwndMapper->erase (hwnd);

			PostQuitMessage (0);
			return 0;
		}

	tagRet:
		return DefWindowProc (hwnd, message, wParam, lParam);
	}

	// Context

	const COLORREF GUIContext_Windows::_colorMask = RGB (0, 0, 201);
	const COLORREF GUIContext_Windows::_GetColor (int r, int g, int b)
	{
		const auto mMax =
			[] (const int &a, const int &b) { return a > b ? a : b; };
		const auto mMin =
			[] (const int &a, const int &b) { return a < b ? a : b; };

		r = mMax (0, mMin (255, r));
		g = mMax (0, mMin (255, g));
		b = mMax (0, mMin (255, b));

		if (RGB (r, b, b) != _colorMask)
			return RGB (r, g, b);
		else
			return RGB (r, g, b) + 1;
	}

	GUIContext_Windows::GUIContext_Windows (size_t width, size_t height)
		: _hdc (NULL), _hBitmap (NULL), _w (width), _h (height)
	{
		HBRUSH	hBrush;
		RECT	rect;
		HDC		hdcWnd;

		hdcWnd = GetDC (NULL);
		this->_hdc = CreateCompatibleDC (hdcWnd);
		this->_hBitmap = CreateCompatibleBitmap (hdcWnd, this->_w, this->_h);

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

		// Set default Font to Consolas :-)
		SetFont ();

		ReleaseDC (NULL, hdcWnd);
		DeleteObject (hBrush);
	}

	GUIContext_Windows::~GUIContext_Windows ()
	{
		HGDIOBJ hObj;

		hObj = SelectObject (this->_hdc,
			(HBRUSH) GetStockObject (NULL_BRUSH));
		if (hObj != GetStockObject (NULL_BRUSH))
			DeleteObject (hObj);

		hObj = SelectObject (this->_hdc,
			(HPEN) GetStockObject (BLACK_PEN));
		if (hObj != GetStockObject (BLACK_PEN) &&
			hObj != GetStockObject (NULL_PEN))
			DeleteObject (hObj);

		hObj = SelectObject (this->_hdc,
			(HPEN) GetStockObject (SYSTEM_FONT));
		if (hObj != GetStockObject (SYSTEM_FONT))
			DeleteObject (hObj);

		DeleteObject (this->_hBitmap);
		DeleteDC (this->_hdc);
	}

	bool GUIContext_Windows::SetPen (unsigned int width,
									 unsigned int r,
									 unsigned int g,
									 unsigned int b)
	{
		if (width == 0)
		{
			auto hObj = SelectObject (this->_hdc,
				(HPEN) GetStockObject (NULL_PEN));
			if (hObj != GetStockObject (BLACK_PEN) &&
				hObj != GetStockObject (NULL_PEN))
				DeleteObject (hObj);
			return true;
		}

		auto hPen = CreatePen (PS_SOLID, max (0, width),
							   _GetColor (r, g, b));
		if (!hPen) return false;

		auto hObj = SelectObject (this->_hdc, hPen);
		if (hObj != GetStockObject (BLACK_PEN) &&
			hObj != GetStockObject (NULL_PEN))
			DeleteObject (hObj);

		return true;
	}

	bool GUIContext_Windows::SetBrush (bool isTransparent,
									   unsigned int r,
									   unsigned int g,
									   unsigned int b)
	{
		if (isTransparent)
		{
			auto hObj = SelectObject (this->_hdc,
				(HPEN) GetStockObject (NULL_BRUSH));
			if (hObj != GetStockObject (NULL_BRUSH))
				DeleteObject (hObj);
			return true;
		}

		auto hBrush = CreateSolidBrush (_GetColor (r, g, b));
		if (!hBrush) return false;

		auto hObj = SelectObject (this->_hdc, hBrush);
		if (hObj != GetStockObject (NULL_BRUSH))
			DeleteObject (hObj);

		return true;
	}

	bool GUIContext_Windows::SetFont (unsigned int size,
									  const char *family,
									  unsigned int r,
									  unsigned int g,
									  unsigned int b)
	{
		SetTextColor (this->_hdc, _GetColor (r, g, b));

		auto hFont = CreateFontA (size, 0, 0, 0,
								  FW_DONTCARE, FALSE, FALSE, FALSE,
								  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								  CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY,
								  DEFAULT_PITCH, family);
		if (!hFont) return false;

		auto hObj = SelectObject (this->_hdc, hFont);
		if (hObj != GetStockObject (SYSTEM_FONT))
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

	bool GUIContext_Windows::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
									   int wElps, int hElps)
	{
		return !!RoundRect (this->_hdc, xBeg, yBeg, xEnd, yEnd, wElps, hElps);
	}

	bool GUIContext_Windows::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
									  int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Arc (this->_hdc, xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
										int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Chord (this->_hdc, xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
									  int xBeg, int yBeg, int xEnd, int yEnd)
	{
		return !!Pie (this->_hdc, xLeft, yTop, xRight, yBottom, xBeg, yBeg, xEnd, yEnd);
	}

	bool GUIContext_Windows::DrawTxt (int xBeg, int yBeg, const char * szText)
	{
		return !!TextOutA (this->_hdc, xBeg, yBeg, szText, (int) strlen (szText));
	}

	bool GUIContext_Windows::DrawBmp (const char *fileName, int x, int y,
									  int width, int height, int r, int g, int b)
	{
		HDC			hdcMemImag;
		HBITMAP		hBitmapImag;
		BITMAP		bitmap;
		COLORREF	colorMask;

		hBitmapImag = (HBITMAP) LoadImageA (NULL, fileName, IMAGE_BITMAP,
											0, 0, LR_LOADFROMFILE);
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
							 hdcMemImag, 0, 0,
							 bitmap.bmWidth, bitmap.bmHeight,
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
								 hdcMemImag, 0, 0,
								 bitmap.bmWidth, bitmap.bmHeight,
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

	bool GUIContext_Windows::SaveAsBmp (const char *fileName)
	{
		auto hdcMem = CreateCompatibleDC (this->_hdc);
		if (!hdcMem)
			return false;

		BITMAPINFO bmpInfo = { 0 };
		bmpInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = this->_w;
		bmpInfo.bmiHeader.biHeight = this->_h;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 24;
		BYTE *pData = NULL;

		auto hBmp = CreateDIBSection (hdcMem, &bmpInfo, DIB_RGB_COLORS,
			(VOID **) (&pData), NULL, 0);
		if (!hBmp)
		{
			DeleteDC (hdcMem);
			return false;
		}
		auto hBmpOld = SelectObject (hdcMem, hBmp);

		if (!BitBlt (hdcMem, 0, 0, this->_w, this->_h,
					 this->_hdc, 0, 0, SRCCOPY))
		{
			SelectObject (hdcMem, hBmpOld);
			DeleteObject (hBmp);
			DeleteDC (hdcMem);
			return false;
		}

		BITMAPINFOHEADER bmInfoHeader = { 0 };
		bmInfoHeader.biSize = sizeof (BITMAPINFOHEADER);
		bmInfoHeader.biWidth = this->_w;
		bmInfoHeader.biHeight = this->_h;
		bmInfoHeader.biPlanes = 1;
		bmInfoHeader.biBitCount = 24;

		auto bmpSize = (bmInfoHeader.biWidth * bmInfoHeader.biHeight) * (24 / 8);
		BITMAPFILEHEADER bmFileHeader = { 0 };
		bmFileHeader.bfType = 0x4d42;  // BMP
		bmFileHeader.bfOffBits = sizeof (BITMAPFILEHEADER) +
			sizeof (BITMAPINFOHEADER);
		bmFileHeader.bfSize = bmFileHeader.bfOffBits + bmpSize;
		
		std::ofstream ofs (fileName,
						   std::ios_base::out | std::ios_base::binary);
		if (!ofs.is_open ())
		{
			SelectObject (hdcMem, hBmpOld);
			DeleteObject (hBmp);
			DeleteDC (hdcMem);
			return false;
		}

		ofs.write ((const char *) &bmFileHeader, sizeof (BITMAPFILEHEADER));
		ofs.write ((const char *) &bmInfoHeader, sizeof (BITMAPINFOHEADER));
		ofs.write ((const char *) pData, bmpSize);

		SelectObject (hdcMem, hBmpOld);
		DeleteObject (hBmp);
		DeleteDC (hdcMem);
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

	void GUIContext_Windows::PaintOnContext (GUIContext *parentContext,
											 size_t x, size_t y) const
	{
		// Assume that context is GUIContext_Windows
		auto _context = static_cast<const GUIContext_Windows *> (parentContext);
		TransparentBlt (_context->_hdc, x, y, this->_w, this->_h,
						this->_hdc, 0, 0, this->_w, this->_h,
						GUIContext_Windows::_colorMask);
	}

	// MsgBox

	void MsgBox_Impl (const char * szTxt, const char * szCap)
	{
		MessageBoxA (NULL, szTxt, szCap, MB_OK);
	}
}
