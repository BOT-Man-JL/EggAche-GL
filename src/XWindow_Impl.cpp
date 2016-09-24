//
// X Window System Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include <exception>
#include <thread>
#include <chrono>
#include <cstring>
#include <unordered_map>
#include <cmath>
#include <X11/Xlib.h>

#include "EggAche_Impl.h"

namespace EggAche_Impl
{
    //Display Manager

    class DisplayManager
    {
    private:
        static Display *_display;
        static size_t refCount;
    public:
        DisplayManager ()
        {
            if (refCount == 0)
            {
                XInitThreads ();
                _display = XOpenDisplay (NULL);
                if (_display == NULL)
                    throw std::runtime_error ("Failed at XOpenDisplay");
            }
            refCount++;
        }

        ~DisplayManager ()
        {
            refCount--;
            if (refCount == 0 & _display != nullptr)
            {
                XCloseDisplay (_display);
                printf ("close display\n");
            }
        }

        class DisplayHelper
        {
        public:
            static int refCount;
            DisplayHelper (Display *display)
                : _display (display)
            {
                refCount++;
                printf ("lock     %d  \n", refCount);
                XLockDisplay (_display);
            }

            ~DisplayHelper ()
            {
                refCount--;
                printf ("unlock     %d\n", refCount);
                XUnlockDisplay (_display);
            }

            operator Display *()
            {
                return _display;
            }

        private:
            Display *_display;
        };

        static DisplayHelper display ()
        {
            return DisplayHelper (_display);
        }
    };

    int DisplayManager::DisplayHelper::refCount = 0;
    size_t DisplayManager::refCount = 0;
    Display *DisplayManager::_display = nullptr;

    // Window Manager

    class WindowImpl_XWindow;  // Pre-define

    class WindowManager
    {
    private:
        static std::unordered_map<Window, WindowImpl_XWindow *> *_hwndMapper;
        static size_t refCount;

        static void EventHandler ();
    public:
        WindowManager ()
        {
            if (refCount == 0 && _hwndMapper == nullptr)
            {
                _hwndMapper = new std::unordered_map<Window, WindowImpl_XWindow *> ();

                // Create a new thread
                std::thread thread (EventHandler);
                thread.detach ();
            }
            refCount++;
        }

        ~WindowManager ()
        {
            refCount--;
            if (refCount == 0)
            {
                delete _hwndMapper;
                _hwndMapper = nullptr;
            }
        }

        static std::unordered_map<Window, WindowImpl_XWindow *> *hwndMapper ()
        {
            return _hwndMapper;
        }
    };

    size_t WindowManager::refCount = 0;
    std::unordered_map<Window, WindowImpl_XWindow *> *WindowManager::_hwndMapper = nullptr;

    // Window

    class WindowImpl_XWindow : public WindowImpl
    {
    public:
        WindowImpl_XWindow (size_t width, size_t height,
                            const char *cap_string);
        ~WindowImpl_XWindow () override;

        void Draw (const GUIContext *context, size_t x, size_t y) override;

        std::pair<size_t, size_t> GetSize () override;
        bool IsClosed () const override;

        void OnClick (std::function<void (int, int)> fn) override;
        void OnPress (std::function<void (char)> fn) override;
        void OnResized (std::function<void (int, int)> fn) override;
        void OnRefresh (std::function<void ()> fn) override;

    protected:
        int           _cxCanvas, _cyCanvas;
        int           _cxClient, _cyClient;
        Window        _window;

        DisplayManager _displayManager;
        WindowManager _windowManager;

        std::function<void (int, int)> onClick;
        std::function<void (char)> onPress;
        std::function<void (int, int)> onResized;
        std::function<void ()> onRefresh;

        friend class WindowManager;

        WindowImpl_XWindow (const WindowImpl_XWindow &) = delete;        // Not allow to copy
        void operator= (const WindowImpl_XWindow &) = delete;            // Not allow to copy
    };

    // Context

    class GUIContext_XWindow : public GUIContext
    {
    public:
        GUIContext_XWindow (size_t width, size_t height);
        ~GUIContext_XWindow () override;

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
        size_t GetTxtWidth (const char *szText) override;

        bool DrawImg (const char *fileName,
                      int x, int y,
                      int width = -1, int height = -1,
                      int r = -1,
                      int g = -1,
                      int b = -1) override;

        bool SaveAsBmp (const char *fileName) const override;
        bool SaveAsJpg (const char *fileName) const override;
        bool SaveAsPng (const char *fileName) const override;
        void Clear () override;

        void PaintOnContext (GUIContext *,
                             size_t x, size_t y) const override;

    protected:
        size_t _w, _h;
        Pixmap _pixmap;
        GC _penGC, _brushGC, _fontGC;
        bool isPenTransparent;
        bool isBrushTransparent;

        DisplayManager _displayManager;

        friend void WindowImpl_XWindow::Draw (const GUIContext *context,
                                              size_t x, size_t y);
        GUIContext_XWindow (const GUIContext_XWindow &) = delete;        // Not allow to copy
        void operator= (const GUIContext_XWindow &) = delete;            // Not allow to copy
    };
}

namespace EggAche_Impl
{
    // Factory

    WindowImpl *GUIFactory_XWindow::NewWindow (size_t width, size_t height,
                                               const char *cap_string)
    {
        return new WindowImpl_XWindow (width, height, cap_string);
    }

    GUIContext *GUIFactory_XWindow::NewGUIContext (size_t width, size_t height)
    {
        return new GUIContext_XWindow (width, height);
    }

    // Window Manager

    void WindowManager::EventHandler ()
    {
        // Add refCount of DisplayManager
        DisplayManager ();

        XEvent event;
        while (auto wndMapper = WindowManager::hwndMapper ())
        {
            auto display = DisplayManager::display ();
            Atom wmDeleteMessage = XInternAtom (display, "WM_DELETE_WINDOW", False);

            XNextEvent (display, &event);
            switch (event.type)
            {
            case ButtonPress:
                if ((*wndMapper)[event.xbutton.window]->onClick)
                    (*wndMapper)[event.xbutton.window]->onClick (event.xbutton.x, event.xbutton.y);
                break;

            case KeyPress:
                if ((*wndMapper)[event.xkey.window]->onPress)
                    (*wndMapper)[event.xkey.window]->onPress (event.xkey.keycode);
                break;

            case ResizeRequest:
                if ((*wndMapper)[event.xresizerequest.window]->onResized)
                    (*wndMapper)[event.xresizerequest.window]->onResized (event.xresizerequest.width, event.xresizerequest.height);
                break;

            case Expose:
                if ((*wndMapper)[event.xexpose.window]->onRefresh)
                    (*wndMapper)[event.xexpose.window]->onRefresh ();
                break;

            case ClientMessage:
                if (event.xclient.data.l[0] == wmDeleteMessage)
                {
                    XDestroyWindow (display, event.xany.window);

                    (*wndMapper)[event.xany.window]->_window = 0;
                    wndMapper->erase (event.xany.window);

                    if (wndMapper->empty ())
                        return;
                }
                break;
            }
        }
    }

    // Window

    WindowImpl_XWindow::WindowImpl_XWindow (size_t width, size_t height,
                                            const char *cap_string)
        : _cxCanvas (width), _cyCanvas (height), _cxClient (width), _cyClient (height),
        _window (0)
    {
        auto display = DisplayManager::display ();
        auto screen = DefaultScreen ((Display *) display);

        auto initX = 10, initY = 10, initBorder = 1;
        _window = XCreateSimpleWindow (display, RootWindow ((Display *) display, screen),
                                       initX, initY, width, height, initBorder,
                                       BlackPixel ((Display *) display, screen),
                                       WhitePixel ((Display *) display, screen));

        XStoreName (display, _window, cap_string);

        Atom wmDeleteMessage = XInternAtom (display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols (display, _window, &wmDeleteMessage, 1);

        /* select kind of events we are interested in */
        XSelectInput (display, _window,
                      ExposureMask | KeyPressMask | ButtonPressMask | ResizeRedirectMask);

        /* map (show) the window */
        XMapWindow (display, _window);

        (*WindowManager::hwndMapper ())[_window] = this;
    }

    WindowImpl_XWindow::~WindowImpl_XWindow ()
    {
        auto display = DisplayManager::display ();
        if (_window)
            XDestroyWindow (display, _window);
    }

    void WindowImpl_XWindow::Draw (const GUIContext *context,
                                   size_t x, size_t y)
    {
        auto display = DisplayManager::display ();
        auto screen = DefaultScreen ((Display *) display);

        auto _context = static_cast<const GUIContext_XWindow *> (context);
        XCopyArea (display, _context->_pixmap, _window, DefaultGC ((Display *) display, screen),
                   0, 0, _context->_w, _context->_h, x, y);
    }

    std::pair<size_t, size_t> WindowImpl_XWindow::GetSize ()
    {
        return std::make_pair (this->_cxClient, this->_cyClient);
    }

    bool WindowImpl_XWindow::IsClosed () const
    {
        return _window == 0;
    }

    void WindowImpl_XWindow::OnClick (std::function<void (int, int)> fn)
    {
        onClick = std::move (fn);
    }

    void WindowImpl_XWindow::OnPress (std::function<void (char)> fn)
    {
        onPress = std::move (fn);
    }

    void WindowImpl_XWindow::OnResized (std::function<void (int, int)> fn)
    {
        onResized = std::move (fn);
    }

    void WindowImpl_XWindow::OnRefresh (std::function<void ()> fn)
    {
        onRefresh = std::move (fn);
    }

    // Context

    GUIContext_XWindow::GUIContext_XWindow (size_t width, size_t height)
        : _w (width), _h (height), _pixmap (0), _penGC (0), _brushGC (0),
        _fontGC (0), isPenTransparent (false), isBrushTransparent (true)
    {
        auto display = DisplayManager::display ();
        auto screen = DefaultScreen ((Display *) display);

        _pixmap = XCreatePixmap (display, RootWindow ((Display *) display, screen),
                                 width, height,
                                 DefaultDepth ((Display *) display, screen));
        _penGC = XCreateGC (display, _pixmap, 0, NULL);
        _brushGC = XCreateGC (display, _pixmap, 0, NULL);
        _fontGC = XCreateGC (display, _pixmap, 0, NULL);
    }

    GUIContext_XWindow::~GUIContext_XWindow ()
    {
        auto display = DisplayManager::display ();
        XFreeGC (display, _penGC);
        XFreeGC (display, _brushGC);
        XFreeGC (display, _fontGC);
        XFreePixmap (display, _pixmap);
    }

    bool GUIContext_XWindow::SetPen (unsigned int width,
                                     unsigned int r,
                                     unsigned int g,
                                     unsigned int b)
    {
        if (width == 0)
        {
            isPenTransparent = true;
            return true;
        }
        isPenTransparent = false;

        auto display = DisplayManager::display ();
        auto screen = DefaultScreen ((Display *) display);

        XSetLineAttributes (display, _penGC, width, LineSolid, CapRound, JoinBevel);

        Colormap cmap = XCreateColormap (display, DefaultRootWindow ((Display *) display),
                                         DefaultVisual ((Display *) display, screen), AllocNone);
        XColor xcolor;
        xcolor.red = r * 257;
        xcolor.green = g * 257;
        xcolor.blue = b * 257;
        xcolor.flags = DoRed | DoGreen | DoBlue;
        XAllocColor (display, cmap, &xcolor);

        XSetForeground (display, _penGC, xcolor.pixel);

        XFreeColors (display, cmap, &xcolor.pixel, 1, 0);
        XFreeColormap (display, cmap);

        return true;
    }

    bool GUIContext_XWindow::SetBrush (bool isTransparent,
                                       unsigned int r,
                                       unsigned int g,
                                       unsigned int b)
    {
        if (isTransparent)
        {
            isBrushTransparent = true;
            return true;
        }
        isBrushTransparent = false;

        auto display = DisplayManager::display ();
        auto screen = DefaultScreen ((Display *) display);

        Colormap cmap = XCreateColormap (display, DefaultRootWindow ((Display *) display),
                                         DefaultVisual ((Display *) display, screen), AllocNone);
        XColor xcolor;
        xcolor.red = r * 257;
        xcolor.green = g * 257;
        xcolor.blue = b * 257;
        xcolor.flags = DoRed | DoGreen | DoBlue;
        XAllocColor (display, cmap, &xcolor);

        XSetForeground (display, _brushGC, xcolor.pixel);

        XFreeColors (display, cmap, &xcolor.pixel, 1, 0);
        XFreeColormap (display, cmap);

        return true;
    }

    bool GUIContext_XWindow::SetFont (unsigned int size,
                                      const char *family,
                                      unsigned int r,
                                      unsigned int g,
                                      unsigned int b)
    {
        // Font
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawLine (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        if (!isPenTransparent)
            XDrawLine (display, _pixmap, _penGC, xBeg, yBeg, xEnd, yEnd);
        return true;
    }

    bool GUIContext_XWindow::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        if (!isPenTransparent)
            XDrawRectangle (display, _pixmap, _penGC, xBeg, yBeg, xEnd, yEnd);
        if (!isBrushTransparent)
            XFillRectangle (display, _pixmap, _brushGC, xBeg, yBeg, xEnd, yEnd);
        return true;
    }

    bool GUIContext_XWindow::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        const int fullAngle = 360 * 64;
        if (!isPenTransparent)
            XDrawArc (display, _pixmap, _penGC, xBeg, yBeg, xEnd - xBeg, yEnd - yBeg, fullAngle, fullAngle);
        if (!isBrushTransparent)
            XFillArc (display, _pixmap, _brushGC, xBeg, yBeg, xEnd - xBeg, yEnd - yBeg, fullAngle, fullAngle);
        return true;
    }

    // Round-Corner Rectangle
    bool GUIContext_XWindow::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
                                       int wElps, int hElps)
    {
        auto display = DisplayManager::display ();
        const int fullAngle = 360 * 64;
        int rdWid = xEnd - xBeg - 2 * wElps;
        int rdHei = yEnd - yBeg - 2 * hElps;

        if (!isPenTransparent)
        {
            XDrawLine (display, _pixmap, _penGC, xBeg + wElps, yBeg, xEnd - wElps, yBeg);
            XDrawLine (display, _pixmap, _penGC, xBeg, yBeg + hElps, xBeg, yEnd - hElps);
            XDrawLine (display, _pixmap, _penGC, xBeg + wElps, yEnd, xEnd - wElps, yEnd);
            XDrawLine (display, _pixmap, _penGC, xEnd, yBeg + hElps, xEnd, yEnd - hElps);

            // left up
            XDrawArc (display, _pixmap, _penGC, xBeg, yBeg, wElps * 2, hElps * 2, fullAngle / 4, fullAngle / 4);
            // left down
            XDrawArc (display, _pixmap, _penGC, xBeg, yEnd - 2 * hElps, wElps * 2, hElps * 2, fullAngle / 2, fullAngle / 4);
            // right up
            XDrawArc (display, _pixmap, _penGC, xEnd - 2 * wElps, yBeg, wElps * 2, hElps * 2, 0, fullAngle / 4);
            // right down
            XDrawArc (display, _pixmap, _penGC, xEnd - 2 * wElps, yEnd - 2 * hElps, wElps * 2, hElps * 2, 0, -fullAngle / 4);
        }

        if (!isBrushTransparent)
        {
            XFillRectangle (display, _pixmap, _brushGC, xBeg + wElps, yBeg, rdWid, yEnd - yBeg);
            XFillRectangle (display, _pixmap, _brushGC, xBeg, yBeg + hElps, xEnd - xBeg, rdHei);

            // left up
            XFillArc (display, _pixmap, _brushGC, xBeg, yBeg, wElps * 2, hElps * 2, fullAngle / 4, fullAngle / 4);
            // left down
            XFillArc (display, _pixmap, _brushGC, xBeg, yEnd - 2 * hElps, wElps * 2, hElps * 2, fullAngle / 2, fullAngle / 4);
            // right up
            XFillArc (display, _pixmap, _brushGC, xEnd - 2 * wElps, yBeg, wElps * 2, hElps * 2, 0, fullAngle / 4);
            // right down
            XFillArc (display, _pixmap, _brushGC, xEnd - 2 * wElps, yEnd - 2 * hElps, wElps * 2, hElps * 2, 0, -fullAngle / 4);
        }
        return true;
    }

    bool GUIContext_XWindow::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
                                      int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        int angle1 = 64 * 90;
        if (xBeg - (xRight - xLeft) / 2 != 0)
            angle1 = (int) (64 * atan ((yBeg - (yBottom - yTop) / 2) / (xBeg - (xRight - xLeft) / 2)));

        int angle2 = 64 * 90 - angle1;
        if (xEnd - (xRight - xLeft) / 2 != 0)
            angle2 = (int) (64 * (atan ((yEnd - (yBottom - yTop) / 2) / (xEnd - (xRight - xLeft) / 2)))) - angle1;
        XDrawArc (display, _pixmap, _penGC, xLeft, yTop, xRight - xLeft, yBottom - yTop,
                  angle1, angle2);
        XFillArc (display, _pixmap, _brushGC, xLeft, yTop, xRight - xLeft, yBottom - yTop,
                  angle1, angle2);
        // dont know what it will do

        return false;
    }

    bool GUIContext_XWindow::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
                                        int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();

        GUIContext_XWindow::DrawArc (xLeft, yTop, xRight, yBottom,
                                     xBeg, yBeg, xEnd, yEnd);//needs spread in XWindow's form if necessary
        GUIContext_XWindow::DrawLine (xBeg, yBeg, xEnd, yEnd);

        return false;
    }

    bool GUIContext_XWindow::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
                                      int xBeg, int yBeg, int xEnd, int yEnd)
    {
        GUIContext_XWindow::DrawArc (xLeft, yTop, xRight, yBottom,
                                     xBeg, yBeg, xEnd, yEnd);
        GUIContext_XWindow::DrawLine (xBeg, yBeg, (xRight - xLeft) / 2, (yBottom - yTop) / 2);
        GUIContext_XWindow::DrawLine (xEnd, yEnd, (xRight - xLeft) / 2, (yBottom - yTop) / 2);

        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawTxt (int xBeg, int yBeg, const char * szText)
    {
        auto display = DisplayManager::display ();
        XDrawString (display, _pixmap, _fontGC, xBeg, yBeg, szText, strlen (szText));
        return true;
    }

    bool GUIContext_XWindow::DrawImg (const char *fileName, int x, int y,
                                      int width, int height, int r, int g, int b)
    {
        // Todo
        return false;
    }

    bool GUIContext_XWindow::SaveAsBmp (const char *fileName) const
    {
        auto display = DisplayManager::display ();
        XWriteBitmapFile (display, fileName, _pixmap, _w, _h, -1, -1);
        return false;
    }

    bool GUIContext_XWindow::SaveAsJpg (const char *fileName) const
    {
        //Todo
        return false;
    }

    bool GUIContext_XWindow::SaveAsPng (const char *fileName) const
    {
        //Todo
        return false;
    }

    size_t GUIContext_XWindow::GetTxtWidth (const char *szText)
    {
        // Todo: add Fontstruct in SetFont
    }

    void GUIContext_XWindow::Clear ()
    {
        auto display = DisplayManager::display ();
        auto screen = DefaultScreen ((Display *) display);
        auto gc = XCreateGC (display, _pixmap, 0, NULL);

        XSetForeground (display, gc, WhitePixel ((Display *) display, screen));
        XSetFillStyle (display, gc, FillSolid);
        XFillRectangle (display, _pixmap, gc, 0, 0, _w, _h);

        XSetForeground (display, gc, BlackPixel ((Display *) display, screen));
        XFreeGC (display, gc);
    }

    void GUIContext_XWindow::PaintOnContext (GUIContext *parentContext,
                                             size_t x, size_t y) const
    {
        // Todo
    }

    // MsgBox

    void MsgBox_Impl (const char * szTxt, const char * szCap)
    {
        // Todo
    }
}

// Test Funcion
// g++ XWindow_Impl.cpp -o test -std=c++11 -lX11 -lpthread
int main (int argc, char *argv[])
{
    using namespace EggAche_Impl;
    {
		// Test Multi-Window
        WindowImpl_XWindow wwnd (10, 20, "J");
		WindowImpl_XWindow wwwnd (10, 20, "J");

		while (!wwnd.IsClosed ())
		{
			std::this_thread::sleep_for (std::chrono::milliseconds (500));
		}
    }
    WindowImpl_XWindow wnd (500, 300, "Hello EggAche");
    GUIContext_XWindow context (500, 300);
    context.Clear ();
    context.DrawTxt (0, 50, "thiefunvierse");
    GUIContext_XWindow context2 (500, 300);
    context2.Clear ();
    context2.DrawTxt (0, 25, "thiefunvierse");

    wnd.OnClick ([&] (int x, int y)
    {
        context.Clear ();
        context.SetPen (4, 200, 100, 100);
        context.SetBrush (false, 20, 100, 150);
        context.DrawLine (0, 0, x, y);
        context.DrawRdRt (50, 50, 220, 220, 30, 30);
        context.DrawRect (x - 50, y - 50, x + 50, y + 50);
        context.DrawTxt (50, 50, "thiefunvierse");
        context.DrawElps (x - 50, y - 50, x + 80, y + 150);
        context.SaveAsBmp ("thief.bmp");
        context.DrawArc (50, 50, 150, 130, 150, 90, 100, 50);//fuck
        wnd.Draw (&context, 0, 0);
        printf ("You Click %03d, %03d\n", x, y);
    });

    wnd.OnResized ([] (int x, int y)
    {
        printf ("You Resized to %03d, %03d\n", x, y);
    });

    wnd.OnPress ([] (char ch)
    {
        printf ("You Typed %c\n", ch);
    });

    wnd.OnRefresh ([&] ()
    {
        wnd.Draw (&context, 0, 0);
        printf ("Your Window Refreshed\n");
    });

    while (!wnd.IsClosed ())
    {
        std::this_thread::sleep_for (std::chrono::milliseconds (500));
    }
    return 0;
}
