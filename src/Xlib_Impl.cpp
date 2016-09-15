//
// X Window System Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include <exception>
#include <thread>
#include <X11/Xlib.h>

#include "EggAche_Impl.h"

namespace EggAche_Impl
{
	class WindowImpl_Xlib : public WindowImpl
	{
	public:
		WindowImpl_Xlib (size_t width, size_t height,
						 const char *cap_string);
		~WindowImpl_Xlib () override;

		void Draw (const GUIContext *context, size_t x, size_t y) override;

		std::pair<size_t, size_t> GetSize () override;
		bool IsClosed () const override;

		void OnClick (std::function<void (int, int)> fn) override;
		void OnPress (std::function<void (char)> fn) override;
		void OnResized (std::function<void (int, int)> fn) override;
		void OnRefresh (std::function<void ()> fn) override;

		static void EventHandler ();

	protected:
		int			_cxCanvas, _cyCanvas;
		int			_cxClient, _cyClient;

		Window		_window;
		bool		_isClosed;

		std::function<void (int, int)> onClick;
		std::function<void (char)> onPress;
		std::function<void (int, int)> onResized;
		std::function<void ()> onRefresh;

		WindowImpl_Xlib (const WindowImpl_Xlib &) = delete;			// Not allow to copy
		void operator= (const WindowImpl_Xlib &) = delete;			// Not allow to copy
	};

	class GUIContext_Xlib : public GUIContext
	{
	public:
		GUIContext_Xlib (size_t width, size_t height);
		~GUIContext_Xlib () override;

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

		bool DrawImg (const char *fileName,
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
		size_t _w, _h;

		GUIContext_Xlib (const GUIContext_Xlib &) = delete;			// Not allow to copy
		void operator= (const GUIContext_Xlib &) = delete;			// Not allow to copy
	};
}

namespace EggAche_Impl
{
	// Factory

	WindowImpl *GUIFactory_Xlib::NewWindow (size_t width, size_t height,
											const char *cap_string)
	{
		return new WindowImpl_Xlib (width, height, cap_string);
	}

	GUIContext *GUIFactory_Xlib::NewGUIContext (size_t width, size_t height)
	{
		return new GUIContext_Xlib (width, height);
	}

	// Window

	class WindowManager
	{
	private:
		static Display *display;
	protected:
		WindowManager () {}
	public:
		static size_t refCount;

		static Display *Instance ()
		{
			std::thread eventHandler (WindowImpl_Xlib::EventHandler);
			eventHandler.detach ();

			if (display == nullptr)
			{
				/* open connection with the server */
				display = XOpenDisplay (NULL);
				if (display == NULL)
					throw std::runtime_error ("Failed at XOpenDisplay");
			}
			return display;
		}

		static void Delete ()
		{
			/* close connection to server */
			XCloseDisplay (display);
		}
	};

	Display *WindowManager::display = nullptr;
	size_t WindowManager::refCount = 0;

	void WindowImpl_Xlib::EventHandler ()
	{
		auto display = WindowManager::Instance ();
		auto screen = DefaultScreen (display);

		XEvent event;

		while (!wnd->_isClosed)
		{
			XNextEvent (display, &event);
			switch (event.type)
			{
			case ButtonPress:
			case KeyPress:
			case ResizeRequest:

			case Expose:
				XFillRectangle (display, wnd->_window, DefaultGC (display, screen), 20, 20, 10, 10);
				XDrawString (display, wnd->_window, DefaultGC (display, screen), 50, 50,
							 "Hello, World!", strlen ("Hello, World!"));
				break;

			case DestroyNotify:
				wnd->_isClosed = true;
				break;

			default:
				break;
			}
		}
	}

	WindowImpl_Xlib::WindowImpl_Xlib (size_t width, size_t height,
									  const char *cap_string)
		: _cxCanvas (width), _cyCanvas (height), _cxClient (width), _cyClient (height),
		_isClosed (false)
	{
		auto display = WindowManager::Instance ();
		auto screen = DefaultScreen (display);

		auto initX = 10, initY = 10, initBorder = 1;
		this->_window = XCreateSimpleWindow (display, RootWindow (display, screen),
											 initX, initY, width, height, initBorder,
											 BlackPixel (display, screen),
											 WhitePixel (display, screen));

		/* select kind of events we are interested in */
		XSelectInput (display, this->_window,
					  ExposureMask | KeyPressMask | ButtonPressMask |
					  ResizeRedirectMask | SubstructureNotifyMask);

		/* map (show) the window */
		XMapWindow (display, this->_window);

		WindowManager::refCount++;
	}

	WindowImpl_Xlib::~WindowImpl_Xlib ()
	{
		auto display = WindowManager::Instance ();
		XEvent event;
		event.type = DestroyNotify;
		event.window = this->_window;

		XSendEvent (display, this->_window, false,
					SubstructureNotifyMask, &event);

		WindowManager::refCount--;
	}

	// Context

	// MsgBox
/*
	void MsgBox_Impl (const char * szTxt, const char * szCap)
	{

	}*/
}


int main (int argc, char *argv[])
{
	using namespace EggAche_Impl;
	WindowImpl_Xlib wnd (200, 200);
	
	getchar ();
	return 0;
}
