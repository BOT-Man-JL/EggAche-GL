//
// X Window System Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include <exception>
#include <thread>
#include <cstring>
#include <unordered_map>
#include <X11/Xlib.h>

#include "EggAche_Impl.h"

namespace EggAche_Impl
{
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

        static void EventHandler ();

    protected:
        int			_cxCanvas, _cyCanvas;
        int			_cxClient, _cyClient;

        Window		_window;

        std::function<void (int, int)> onClick;
        std::function<void (char)> onPress;
        std::function<void (int, int)> onResized;
        std::function<void ()> onRefresh;

        WindowImpl_XWindow (const WindowImpl_XWindow &) = delete;		// Not allow to copy
        void operator= (const WindowImpl_XWindow &) = delete;			// Not allow to copy
    };

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

        bool DrawImg (const char *fileName,
                      int x, int y,
                      int width = -1, int height = -1,
                      int r = -1,
                      int g = -1,
                      int b = -1) override;

        bool SaveAsBmp (const char *fileName) const override;
        bool SaveAsJpg (const char *fileName) const override ;
        bool SaveAsPng (const char *fileName) const override ;

        void Clear () override;

        void PaintOnContext (GUIContext *,
                             size_t x, size_t y) const override;

    protected:
        size_t _w, _h;
        Pixmap _pixmap;
        GC _gc;

        friend void WindowImpl_XWindow::Draw (const GUIContext *context,
                                              size_t x, size_t y);
        GUIContext_XWindow (const GUIContext_XWindow &) = delete;		// Not allow to copy
        void operator= (const GUIContext_XWindow &) = delete;			// Not allow to copy
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

    // Window

    class WindowManager
    {
    private:
        static Display *_display;
        static std::unordered_map<Window, WindowImpl_XWindow *> *_hwndMapper;
    protected:
        WindowManager () {}
    public:
        static bool isEventHandlerRunning;

        static Display *display ()
        {
            if (_display == nullptr)
            {
                /* open connection with the server */
                _display = XOpenDisplay (NULL);
                if (_display == NULL)
                    throw std::runtime_error ("Failed at XOpenDisplay");
            }
            return _display;
        }

        static void CloseDisplay ()
        {
            /* close connection to server */
            XCloseDisplay (_display);
        }

        static std::unordered_map<Window, WindowImpl_XWindow *> *wndMapper ()
        {
            if (_hwndMapper == nullptr)
                _hwndMapper = new std::unordered_map<Window, WindowImpl_XWindow *> ();
            return _hwndMapper;
        }

        static bool IsRefed ()
        {
            return _hwndMapper != nullptr && !_hwndMapper->empty ();
        }

        static void DeleteMapper ()
        {
            delete _hwndMapper;
            _hwndMapper = nullptr;
        }
    };

    bool WindowManager::isEventHandlerRunning = false;
    Display *WindowManager::_display = nullptr;
    std::unordered_map<Window, WindowImpl_XWindow *> *WindowManager::_hwndMapper = nullptr;

    void WindowImpl_XWindow::EventHandler ()
    {
        auto display = WindowManager::display ();
        auto screen = DefaultScreen (display);
        auto wndMapper = WindowManager::wndMapper ();

        XEvent event;
        while (WindowManager::IsRefed ())
        {
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

                case DestroyNotify:
                    (*wndMapper)[event.xdestroywindow.window]->_window = 0;
                    wndMapper->erase (event.xdestroywindow.window);
                    break;

                default:
                    break;
            }
        }

        WindowManager::isEventHandlerRunning = false;
        WindowManager::CloseDisplay ();
    }

    WindowImpl_XWindow::WindowImpl_XWindow (size_t width, size_t height,
                                            const char *cap_string)
            : _cxCanvas (width), _cyCanvas (height), _cxClient (width), _cyClient (height),
              _window (0)
    {
        auto display = WindowManager::display ();
        auto screen = DefaultScreen (display);

        auto initX = 10, initY = 10, initBorder = 1;
        _window = XCreateSimpleWindow (display, RootWindow (display, screen),
                                       initX, initY, width, height, initBorder,
                                       BlackPixel (display, screen),
                                       WhitePixel (display, screen));

        XStoreName (display, _window, cap_string);

        /* select kind of events we are interested in */
        XSelectInput (display, _window,
                      ExposureMask | KeyPressMask | ButtonPressMask |
                      ResizeRedirectMask | SubstructureNotifyMask);

        /* map (show) the window */
        XMapWindow (display, _window);

        (*WindowManager::wndMapper ())[_window] = this;

        if (!WindowManager::isEventHandlerRunning)

        {
            std::thread eventHandler (WindowImpl_XWindow::EventHandler);
            eventHandler.detach ();
            WindowManager::isEventHandlerRunning = true;
        }
    }

    WindowImpl_XWindow::~WindowImpl_XWindow ()
    {
        auto display = WindowManager::display ();
        XDestroyWindow (display, _window);

        if (!WindowManager::IsRefed ())
            WindowManager::DeleteMapper ();
    }

    void WindowImpl_XWindow::Draw (const GUIContext *context,
                                   size_t x, size_t y)
    {
        auto display = WindowManager::display ();
        auto screen = DefaultScreen (display);

        auto _context = static_cast<const GUIContext_XWindow *> (context);
        XCopyArea (display, _context->_pixmap, _window, DefaultGC (display, screen),
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
            : _w (width), _h (height), _pixmap (0), _gc (0)
    {
        auto display = WindowManager::display ();
        auto screen = DefaultScreen (display);

        _pixmap = XCreatePixmap (display, RootWindow (display, screen),
                                 width, height,
                                 DefaultDepth (display, screen));
        _gc = XCreateGC (display, _pixmap, 0, NULL);

        Clear ();
    }

    GUIContext_XWindow::~GUIContext_XWindow ()
    {
        auto display = WindowManager::display ();
        XFreeGC (display, _gc);
        XFreePixmap (display, _pixmap);
    }

    bool GUIContext_XWindow::SetPen (unsigned int width,
                                     unsigned int r,
                                     unsigned int g,
                                     unsigned int b)
    {
        auto display=WindowManager::display();
        XColor penColor;
        penColor.red=r;
        penColor.green=g;
        penColor.blue=b;

        XSetLineAttributes(display,_gc,width,LineSolid,CapRound,JoinBevel);
        // Todo     no color change!!!!!!!!!

        return false;
    }

    bool GUIContext_XWindow::SetBrush (bool isTransparent,
                                       unsigned int r,
                                       unsigned int g,
                                       unsigned int b)
    {
        XColor brushColor;
        brushColor.red=r;
        brushColor.green=g;
        brushColor.blue=b;

        // Todo
        return false;
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
        auto display = WindowManager::display ();
      //  auto screen = DefaultScreen (display);
        XDrawLine (display, _pixmap, _gc, xBeg, yBeg, xEnd, yEnd);
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display=WindowManager::display();
      //  auto screen=DefaultScreen(display);
        XDrawRectangle(display,_pixmap,_gc,xBeg,yBeg,xEnd,yEnd);
        // Todo

        return false;
    }

    bool GUIContext_XWindow::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display=WindowManager::display();
        const int fullAngle=23040;
        XDrawArc(display,_pixmap,_gc,xBeg,yBeg,xEnd-xBeg,yEnd-yBeg,fullAngle,fullAngle);
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
                                       int wElps, int hElps)
    {
        auto display=WindowManager::display();
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
                                      int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display=WindowManager::display();
        XDrawArc(display,_pixmap,_gc,xLeft,yTop,xRight-xLeft,yBottom-yTop,xEnd-xBeg,yEnd-yBeg);
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
                                        int xBeg, int yBeg, int xEnd, int yEnd)
    {
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
                                      int xBeg, int yBeg, int xEnd, int yEnd)
    {
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawTxt (int xBeg, int yBeg, const char * szText)
    {
        auto display=WindowManager::display();
        //XDrawText(display,_pixmap,_gc,xBeg,yBeg, ,0);
        XDrawString(display,_pixmap,_gc,xBeg,yBeg,szText,strlen(szText));
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawImg (const char *fileName, int x, int y,
                                      int width, int height, int r, int g, int b)
    {
        // Todo
        return false;
    }

    bool GUIContext_XWindow::SaveAsBmp (const char *fileName) const
    {
        // Todo
        return false;
    }


    bool GUIContext_XWindow::SaveAsJpg(const char *fileName) const
    {

        //Todo
        return false;
    }


    bool GUIContext_XWindow::SaveAsPng(const char *fileName) const
    {

        //Todo
        return false;
    }


    void GUIContext_XWindow::Clear ()
    {
        auto display = WindowManager::display ();
        auto screen = DefaultScreen (display);

        XSetForeground (display, _gc, WhitePixel (display, screen));
        XSetFillStyle (display, _gc, FillSolid);
        XFillRectangle (display, _pixmap, _gc, 0, 0, _w, _h);

        XSetForeground (display, _gc, BlackPixel (display, screen));
        XFlushGC (display, _gc);
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
    WindowImpl_XWindow wnd (500, 300, "Hello EggAche");
    GUIContext_XWindow context (500, 300);

    wnd.OnClick ([&] (int x, int y)
                 {
                     context.SetPen(4,0,0,0);
                     context.Clear ();
                     context.DrawLine (0, 0, x, y);
                     context.DrawRect(x-50,y-50,x+50,y+50);
                     context.DrawTxt(50,50,"thiefunvierse");
                     context.DrawElps(x-50,y-50,x+80,y+150);
                     //context.DrawArc(0,10,90,80,10,50,70,80);
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

    getchar ();
    return 0;
}

