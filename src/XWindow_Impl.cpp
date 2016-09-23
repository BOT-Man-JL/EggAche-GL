//
// X Window System Implementation of EggAche Graphics Library
// By BOT Man, 2016
//

#include <exception>
#include <thread>
#include <chrono>
#include <cstring>
#include <unordered_map>
#include <X11/Xlib.h>

#include "EggAche_Impl.h"

namespace EggAche_Impl
{

    class WindowImpl_XWindow;
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
                _display = XOpenDisplay (NULL);
                if (_display == NULL)
                    throw std::runtime_error ("Failed at XOpenDisplay");
            }
            refCount++;
        }

        ~DisplayManager ()
        {
            refCount--;
            if (refCount == 0)
                XCloseDisplay (_display);
        }

        static Display *display ()
        {
            return _display;
        }
    };
    size_t DisplayManager::refCount = 0;
    Display *DisplayManager::_display = nullptr;

    // Window Manager

    class WindowManager
    {
    private:
        static std::unordered_map<Window, WindowImpl_XWindow *> *_hwndMapper;
        static size_t refCount;
    public:
        WindowManager ();


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

        static void EventHandler ();

    protected:
        int			_cxCanvas, _cyCanvas;
        int			_cxClient, _cyClient;

        Window		_window;

        DisplayManager _displayManager;
        WindowManager _windowManager;

        std::function<void (int, int)> onClick;
        std::function<void (char)> onPress;
        std::function<void (int, int)> onResized;
        std::function<void ()> onRefresh;

        WindowImpl_XWindow (const WindowImpl_XWindow &) = delete;		// Not allow to copy
        void operator= (const WindowImpl_XWindow &) = delete;			// Not allow to copy
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
        GC _gc, _penGC, _brushGC, _fontGC;


        bool isBrushTransparant;
        DisplayManager _displayManager;

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

    void WindowImpl_XWindow::EventHandler ()
    {
        XEvent event;
        while (auto wndMapper = WindowManager::hwndMapper ())
        {
            auto display = DisplayManager::display ();
			Atom wmDeleteMessage = XInternAtom (display, "WM_DELETE_WINDOW", True);

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

				case ClientMessage:
					if (event.xclient.data.l[0] == wmDeleteMessage)
					{
						(*wndMapper)[event.xdestroywindow.window]->_window = 0;
						wndMapper->erase (event.xdestroywindow.window);
					}
					break;

                default:
                    break;
            }
        }
    }

    WindowImpl_XWindow::WindowImpl_XWindow (size_t width, size_t height,
                                            const char *cap_string)
            : _cxCanvas (width), _cyCanvas (height), _cxClient (width), _cyClient (height),
              _window (0)
    {
        auto display = DisplayManager::display ();
        auto screen = DefaultScreen (display);

        auto initX = 10, initY = 10, initBorder = 1;
        _window = XCreateSimpleWindow (display, RootWindow (display, screen),
                                       initX, initY, width, height, initBorder,
                                       BlackPixel (display, screen),
                                       WhitePixel (display, screen));

        XStoreName (display, _window, cap_string);

		Atom wmDeleteMessage = XInternAtom (display, "WM_DELETE_WINDOW", True);
		XSetWMProtocols (display, _window, &wmDeleteMessage, 1);

        /* select kind of events we are interested in */
        XSelectInput (display, _window,
                      ExposureMask | KeyPressMask | ButtonPressMask |
                      ResizeRedirectMask | StructureNotifyMask | SubstructureNotifyMask);

        /* map (show) the window */
        XMapWindow (display, _window);

        (*WindowManager::hwndMapper ())[_window] = this;
    }

    WindowImpl_XWindow::~WindowImpl_XWindow ()
    {
        auto display = DisplayManager::display ();
        XDestroyWindow (display, _window);
    }

    void WindowImpl_XWindow::Draw (const GUIContext *context,
                                   size_t x, size_t y)
    {
        auto display = DisplayManager::display ();
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
        auto display = DisplayManager::display ();
        auto screen = DefaultScreen (display);

        _pixmap = XCreatePixmap (display, RootWindow (display, screen),
                                 width, height,
                                 DefaultDepth (display, screen));
        _gc = XCreateGC (display, _pixmap, 0, NULL);
        _penGC = XCreateGC (display, _pixmap, 0, NULL);
        _brushGC = XCreateGC (display, _pixmap, 0, NULL);
        _fontGC = XCreateGC (display, _pixmap, 0, NULL);

        Clear ();

        // this->SetBrush(True,255,255,255);
//        this->SetPen(4,10,200,100);
    }

    GUIContext_XWindow::~GUIContext_XWindow ()
    {
        auto display = DisplayManager::display ();
        XFreeGC (display, _gc);
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
        auto display = DisplayManager::display();
        auto screen = DefaultScreen (display);
        XColor xcolor;

//      set the attributes of the line
        XSetLineAttributes(display,_penGC,width,LineSolid,CapRound,JoinBevel);

//      get the colormap
		Colormap cmap = XCreateColormap (display, DefaultRootWindow (display),
										 DefaultVisual (display, screen), AllocNone);

//      set the rgb values
        xcolor.red = r*257;
        xcolor.green = g*257;
        xcolor.blue = b*257;
        xcolor.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(display, cmap, &xcolor);

//      using set the rgb values of foreground to set the rgb values of pen
        XSetForeground(display, _penGC, xcolor.pixel);

		XFreeColors (display, cmap, &xcolor.pixel, 1, 0);
		XFreeColormap (display, cmap);
        return true;
    }

    bool GUIContext_XWindow::SetBrush (bool isTransparent,
                                       unsigned int r,
                                       unsigned int g,
                                       unsigned int b)
    {
        this->isBrushTransparant=isTransparent;

        auto display = DisplayManager::display();
        auto screen = DefaultScreen (display);
        XColor xcolor;

//      get the colormap
        Colormap cmap = XCreateColormap (display, DefaultRootWindow (display),
										 DefaultVisual (display, screen), AllocNone);

//      set the rgb values
        xcolor.red = r*257;
        xcolor.green = g*257;
        xcolor.blue = b*257;
        xcolor.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(display, cmap, &xcolor);

//      using set the rgb values of foreground to set the rgb values of brush
        XSetForeground(display, _brushGC, xcolor.pixel);

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
        //  auto screen = DefaultScreen (display);
        XDrawLine (display, _pixmap, _penGC, xBeg, yBeg, xEnd, yEnd);
        // Todo
        return false;
    }

    bool GUIContext_XWindow::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display= DisplayManager::display();
        //  auto screen=DefaultScreen(display);
        XDrawRectangle(display,_pixmap,_penGC,xBeg,yBeg,xEnd,yEnd);

        if(!isBrushTransparant)
        XFillRectangle (display, _pixmap, _brushGC, xBeg, yBeg, xEnd, yEnd);
        return false;
    }

    bool GUIContext_XWindow::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display= DisplayManager::display();
        const int fullAngle=23040;
        XDrawArc(display,_pixmap,_penGC,xBeg,yBeg,xEnd-xBeg,yEnd-yBeg,fullAngle,fullAngle);
        if(!isBrushTransparant)
        XFillArc(display, _pixmap, _brushGC, xBeg,yBeg,xEnd-xBeg,yEnd-yBeg,fullAngle,fullAngle);
        return false;
    }

    //draw RoundRectangle
    bool GUIContext_XWindow::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
                                       int wElps, int hElps)
    {
        auto display= DisplayManager::display();
        const int fullAngle=23040;
        int rdWid=xEnd-xBeg-2*wElps;
        int rdHei=yEnd-yBeg-2*hElps;

        XDrawLine(display,_pixmap,_penGC,xBeg+wElps,yBeg,xEnd-wElps,yBeg);
        XDrawLine(display,_pixmap,_penGC,xBeg,yBeg+hElps,xBeg,yEnd-hElps);
        XDrawLine(display,_pixmap,_penGC,xBeg+wElps,yEnd,xEnd-wElps,yEnd);
        XDrawLine(display,_pixmap,_penGC,xEnd,yBeg+hElps,xEnd,yEnd-hElps);

        //left up
        XDrawArc(display,_pixmap,_penGC,xBeg,yBeg,wElps*2,hElps*2,fullAngle/4,fullAngle/4);
        //left down
        XDrawArc(display,_pixmap,_penGC,xBeg,yEnd-2*hElps,wElps*2,hElps*2,fullAngle/2,fullAngle/4);
        //right up
        XDrawArc(display,_pixmap,_penGC,xEnd-2*wElps,yBeg,wElps*2,hElps*2,0,fullAngle/4);
        //right down
        XDrawArc(display,_pixmap,_penGC,xEnd-2*wElps,yEnd-2*hElps,wElps*2,hElps*2,0,-fullAngle/4);

        if(!isBrushTransparant)
        {
            //left up
            XFillArc(display,_pixmap,_brushGC,xBeg,yBeg,wElps*2,hElps*2,fullAngle/4,fullAngle/4);
            //left down
            XFillArc(display,_pixmap,_brushGC,xBeg,yEnd-2*hElps,wElps*2,hElps*2,fullAngle/2,fullAngle/4);
            //right up
            XFillArc(display,_pixmap,_brushGC,xEnd-2*wElps,yBeg,wElps*2,hElps*2,0,fullAngle/4);
            //right down
            XFillArc(display,_pixmap,_brushGC,xEnd-2*wElps,yEnd-2*hElps,wElps*2,hElps*2,0,-fullAngle/4);


            XFillRectangle(display,_pixmap,_brushGC,xBeg+wElps,yBeg,rdWid,yEnd-yBeg);
            XFillRectangle(display,_pixmap,_brushGC,xBeg,yBeg+hElps,xEnd-xBeg,rdHei);

        }


        return false;
    }

    bool GUIContext_XWindow::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
                                      int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display();
        XDrawArc(display,_pixmap,_gc,xLeft,yTop,xRight-xLeft,yBottom-yTop,xEnd-xBeg,yEnd-yBeg);
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
        auto display = DisplayManager::display();
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
        auto display=DisplayManager::display();
        unsigned int width=120;
        unsigned int height=120;
        XWriteBitmapFile(display,fileName,_pixmap,width,height,40,40);
        // Todo

        //!!! it doesn't work !!!!!!!!!!!
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


    size_t GUIContext_XWindow::GetTxtWidth(const char *szText) {
		// Todo: add Fontstruct in SetFont
    }

    void GUIContext_XWindow::Clear ()
    {
        auto display = DisplayManager::display ();
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




    //Windows Manager
    WindowManager::WindowManager ()
    {
        if (refCount == 0 && _hwndMapper == nullptr)
        {
            _hwndMapper = new std::unordered_map<Window, WindowImpl_XWindow *> ();

            // Create a new thread
            std::thread thread (WindowImpl_XWindow::EventHandler);
            thread.detach ();
        }
        refCount++;
    }
}

// Test Funcion
// g++ XWindow_Impl.cpp -o test -std=c++11 -lX11 -lpthread
int main (int argc, char *argv[])
{
    using namespace EggAche_Impl;
    WindowImpl_XWindow wnd (500, 300, "Hello EggAche");
    GUIContext_XWindow context (500, 300);
    context.DrawTxt(0,50,"thiefunvierse");

    wnd.OnClick ([&] (int x, int y)
                 {
                     context.Clear ();
                     context.SetPen(4,200,100,100);//values are 0~65535
                     context.SetBrush(false,20,100,150);
                     context.DrawLine (0, 0, x, y);
                     context.DrawRdRt(50,50,220,220,30,30);
                     context.DrawRect(x-50,y-50,x+50,y+50);
                     context.DrawTxt(50,50,"thiefunvierse");
                     context.DrawElps(x-50,y-50,x+80,y+150);
                     context.SaveAsBmp("thief.bmp");
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

	while (!wnd.IsClosed ())
	{
		std::this_thread::sleep_for (std::chrono::milliseconds (500));
	}
    return 0;
}
