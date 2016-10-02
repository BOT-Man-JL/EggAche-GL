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
#include <X11/Xutil.h>

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
                      double angleBeg, double cAngle) override;

        bool DrawChord (int xLeft, int yTop, int xRight, int yBottom,
                        double angleBeg, double cAngle) override;

        bool DrawPie (int xLeft, int yTop, int xRight, int yBottom,
                      double angleBeg, double cAngle) override;

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
        Pixmap _pixmap,_pixmap_mask;
        GC _penGC, _brushGC, _fontGC,_blackGC;
        bool isPenTransparent;
        bool isBrushTransparent;

        const int ANGLE_WEIGHT=64;

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

        _pixmap_mask = XCreatePixmap (display, RootWindow ((Display *) display, screen),
                                      width, height,
                                      DefaultDepth ((Display *) display, screen));
        _penGC = XCreateGC (display, _pixmap, 0, NULL);
        _brushGC = XCreateGC (display, _pixmap, 0, NULL);
        _fontGC = XCreateGC (display, _pixmap, 0, NULL);

        _blackGC=XCreateGC (display, _pixmap, 0, NULL);
        XSetForeground(display,_blackGC,0);
        //XSetFunction(display,_blackGC,GXand);
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
        auto display = DisplayManager::display();
        auto screen = DefaultScreen ((Display *) display);

//        http://www.sbin.org/doc/Xlib/chapt_17_app_A.html
//        For example, say you use the following name to specify a 24-point (size),
//        medium (weight), Italic (slant), Charter (family) font:
//        *charter-medium-i-*-240-*
        char family_size[30];
        family_size[0]='*';
        int i;
        for(i=1;i<strlen(family)+1;i++)
            family_size[i] = family[i - 1];
        family_size[i]='\0';
        char temp[7]={'-','*','-','*','-','*','-'};
        strcat(family_size,temp);
        memset(temp,0,sizeof(temp));
        sprintf(temp,"%d",size*10);
        strcat(family_size,temp);
        temp[0]='-';
        temp[1]='*';
        temp[2]='\0';
        strcat(family_size,temp);
        XFontStruct *font = XLoadQueryFont (display, family_size);
        /* If the font could not be loaded, use the default font. */
        if (!font)
        {
            fprintf (stderr, "unable to load font %s: using fixed\n", family_size);
        }
        else
        {
            XSetFont (display, _fontGC, font->fid);
        }

        XColor xcolor;

//      get the colormap
        Colormap cmap = XCreateColormap (display, DefaultRootWindow ((Display *) display),
                                         DefaultVisual ((Display *) display, screen), AllocNone);

//      set the rgb values
        xcolor.red = r*257;
        xcolor.green = g*257;
        xcolor.blue = b*257;
        xcolor.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(display, cmap, &xcolor);

//      using set the rgb values of foreground to set the rgb values of brush
        XSetForeground(display, _fontGC, xcolor.pixel);

        XFreeColors (display, cmap, &xcolor.pixel, 1, 0);
        XFreeColormap (display, cmap);
        return false;
    }

    bool GUIContext_XWindow::DrawLine (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        if (!isPenTransparent)
        {
            XDrawLine (display, _pixmap, _penGC, xBeg, yBeg, xEnd, yEnd);

            //for mask
            XDrawLine(display,_pixmap_mask,_blackGC,xBeg,yBeg,xEnd,yEnd);
        }
        return true;
    }

    bool GUIContext_XWindow::DrawRect (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        if (!isPenTransparent)
        {
            XDrawRectangle (display, _pixmap, _penGC, xBeg, yBeg, xEnd, yEnd);
            XDrawRectangle(display,_pixmap_mask,_blackGC,xBeg,yBeg,xEnd,yEnd);
        }
        if (!isBrushTransparent)
        {
            XFillRectangle (display, _pixmap, _brushGC, xBeg, yBeg, xEnd, yEnd);
            XFillRectangle (display, _pixmap_mask, _blackGC, xBeg, yBeg, xEnd, yEnd);
        }

        return true;
    }

    bool GUIContext_XWindow::DrawElps (int xBeg, int yBeg, int xEnd, int yEnd)
    {
        auto display = DisplayManager::display ();
        const int fullAngle = 360 * ANGLE_WEIGHT;
        if (!isPenTransparent)
        {
            XDrawArc (display, _pixmap, _penGC, xBeg, yBeg, xEnd - xBeg, yEnd - yBeg, fullAngle, fullAngle);
            XDrawArc (display, _pixmap_mask, _blackGC, xBeg, yBeg, xEnd - xBeg, yEnd - yBeg, fullAngle, fullAngle);
        }

        if (!isBrushTransparent)
        {
            XFillArc (display, _pixmap, _brushGC, xBeg, yBeg, xEnd - xBeg, yEnd - yBeg, fullAngle, fullAngle);
            XFillArc (display, _pixmap_mask, _blackGC, xBeg, yBeg, xEnd - xBeg, yEnd - yBeg, fullAngle, fullAngle);
        }
        return true;
    }

    // Round-Corner Rectangle
    bool GUIContext_XWindow::DrawRdRt (int xBeg, int yBeg, int xEnd, int yEnd,
                                       int wElps, int hElps)
    {
        auto display = DisplayManager::display ();
        const int fullAngle = 360 * ANGLE_WEIGHT;
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



            //for mask
            XDrawLine (display, _pixmap_mask, _blackGC, xBeg + wElps, yBeg, xEnd - wElps, yBeg);
            XDrawLine (display, _pixmap_mask, _blackGC, xBeg, yBeg + hElps, xBeg, yEnd - hElps);
            XDrawLine (display, _pixmap_mask, _blackGC, xBeg + wElps, yEnd, xEnd - wElps, yEnd);
            XDrawLine (display, _pixmap_mask, _blackGC, xEnd, yBeg + hElps, xEnd, yEnd - hElps);

            // left up
            XDrawArc (display, _pixmap_mask, _blackGC, xBeg, yBeg, wElps * 2, hElps * 2, fullAngle / 4, fullAngle / 4);
            // left down
            XDrawArc (display, _pixmap_mask, _blackGC, xBeg, yEnd - 2 * hElps, wElps * 2, hElps * 2, fullAngle / 2, fullAngle / 4);
            // right up
            XDrawArc (display, _pixmap_mask, _blackGC, xEnd - 2 * wElps, yBeg, wElps * 2, hElps * 2, 0, fullAngle / 4);
            // right down
            XDrawArc (display, _pixmap_mask, _blackGC, xEnd - 2 * wElps, yEnd - 2 * hElps, wElps * 2, hElps * 2, 0, -fullAngle / 4);

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



            //for mask
            XFillRectangle (display, _pixmap_mask, _blackGC, xBeg + wElps, yBeg, rdWid, yEnd - yBeg);
            XFillRectangle (display, _pixmap_mask, _blackGC, xBeg, yBeg + hElps, xEnd - xBeg, rdHei);

            // left up
            XFillArc (display, _pixmap_mask, _blackGC, xBeg, yBeg, wElps * 2, hElps * 2, fullAngle / 4, fullAngle / 4);
            // left down
            XFillArc (display, _pixmap_mask, _blackGC, xBeg, yEnd - 2 * hElps, wElps * 2, hElps * 2, fullAngle / 2, fullAngle / 4);
            // right up
            XFillArc (display, _pixmap_mask, _blackGC, xEnd - 2 * wElps, yBeg, wElps * 2, hElps * 2, 0, fullAngle / 4);
            // right down
            XFillArc (display, _pixmap_mask, _blackGC, xEnd - 2 * wElps, yEnd - 2 * hElps, wElps * 2, hElps * 2, 0, -fullAngle / 4);

        }
        return true;
    }

    bool GUIContext_XWindow::DrawArc (int xLeft, int yTop, int xRight, int yBottom,
                                      double angleBeg, double cAngle)
    {

        auto display=DisplayManager::display();
        XDrawArc(display,_pixmap,_penGC,xLeft,yTop,xRight-xLeft,yBottom-yTop,angleBeg*ANGLE_WEIGHT,cAngle*ANGLE_WEIGHT);

        //for mask
        XDrawArc(display,_pixmap_mask,_blackGC,xLeft,yTop,xRight-xLeft,yBottom-yTop,angleBeg*ANGLE_WEIGHT,cAngle*ANGLE_WEIGHT);
        return false;
    }

    bool GUIContext_XWindow::DrawChord (int xLeft, int yTop, int xRight, int yBottom,
                                        double angleBeg, double cAngle)
    {
        auto display = DisplayManager::display ();

        XDrawArc(display,_pixmap,_penGC,xLeft,yTop,xRight-xLeft,yBottom-yTop,angleBeg*ANGLE_WEIGHT,cAngle*ANGLE_WEIGHT);
        //GUIContext_XWindow::DrawLine (xBeg, yBeg, xEnd, yEnd);

        return false;
    }

    bool GUIContext_XWindow::DrawPie (int xLeft, int yTop, int xRight, int yBottom,
                                      double angleBeg, double cAngle)
    {
        //GUIContext_XWindow::DrawArc (xLeft, yTop, xRight, yBottom,
//                                     xBeg, yBeg, xEnd, yEnd);
        //GUIContext_XWindow::DrawLine (xBeg, yBeg, (xRight - xLeft) / 2, (yBottom - yTop) / 2);
        //GUIContext_XWindow::DrawLine (xEnd, yEnd, (xRight - xLeft) / 2, (yBottom - yTop) / 2);

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
        auto display=DisplayManager::display();
        auto screen=XDefaultScreen(display);
        FILE* image_file=fopen(fileName,"r");

        int size=600000;
        char img_buffer[size];

        if (image_file==NULL)
        {
            perror("Error open file");
        }
        else
        {
            if (!fgets(img_buffer,size,image_file))
            {
                perror("Error read file");
            }
            fclose(image_file);
        }

        XImage *image=XCreateImage(display,XDefaultVisual(display,screen),
                                   XDefaultDepth(display,screen),ZPixmap,0,(char*)img_buffer,width,height,32,0);

        XPutImage(display,_pixmap,_penGC,image,x,y,x+width,y+height,width,height);


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


        //for mask
        auto gc_black = XCreateGC (display, _pixmap_mask, 0, NULL);

        XSetForeground (display, gc_black, WhitePixel ((Display *) display, screen));
        XSetFillStyle (display, gc_black, FillSolid);
        XFillRectangle (display, _pixmap_mask, gc_black, 0, 0, _w, _h);

        XSetForeground (display, gc_black, BlackPixel ((Display *) display, screen));
        XFreeGC (display, gc_black);
    }

    void GUIContext_XWindow::PaintOnContext (GUIContext *parentContext,
                                             size_t x, size_t y) const
    {

        auto display=DisplayManager::display();
        auto screen=XDefaultScreen(display);
        auto _context = static_cast<const GUIContext_XWindow *> (parentContext);

        //several GCs and can't draw pixmap completely

        /*XSetFunction(display,_brushGC,GXand);
        XCopyArea (display,  _context->_pixmap_mask, _pixmap, _brushGC,
                   0, 0, _context->_w, _context->_h, x, y);

        XSetFunction(display,_context->_brushGC,GXxor);
        XCopyArea (display,  _context->_pixmap, _context->_pixmap_mask, _context->_brushGC,
                   0, 0, _context->_w, _context->_h, x, y);

        XSetFunction(display,_brushGC,GXor);
        XCopyArea (display,  _context->_pixmap_mask, _pixmap, _brushGC,
                   0, 0, _context->_w, _context->_h, x, y);
        XSetFunction(display,_brushGC,1);
        XSetFunction(display,_context->_brushGC,1);
*/

        //use penGC
        XSetFunction(display,_penGC,GXand);
        XCopyArea (display,  _context->_pixmap_mask, _pixmap, _penGC,
                   0, 0, _context->_w, _context->_h, x, y);

        XSetFunction(display,_context->_penGC,GXxor);
        XCopyArea (display,  _context->_pixmap, _context->_pixmap_mask, _context->_penGC,
                   0, 0, _context->_w, _context->_h, x, y);

        XSetFunction(display,_penGC,GXor);
        XCopyArea (display,  _context->_pixmap_mask, _pixmap, _penGC,
                   0, 0, _context->_w, _context->_h, x, y);
        XSetFunction(display,_penGC,1);
        XSetFunction(display,_context->_penGC,1);
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
   /*{
           // Test Multi-Window
           WindowImpl_XWindow wwnd (10, 20, "J");
           WindowImpl_XWindow wwwnd (10, 20, "J");

           while (!wwnd.IsClosed ())
           {
               std::this_thread::sleep_for (std::chrono::milliseconds (500));
           }
       }*/

    WindowImpl_XWindow wnd (500, 300, "Hello EggAche");
    WindowImpl_XWindow wndg (500, 300, "Hello mask");
    GUIContext_XWindow context (500, 300);
    context.Clear ();
    context.DrawTxt (0, 50, "thiefunvierse");
    GUIContext_XWindow context2 (500, 300);
    context2.Clear ();
    context2.DrawTxt (0, 25, "thiefunvierse");



    wnd.OnClick ([&] (int x, int y)
                 {
                     context.Clear ();
                     context2.Clear();
                     context.SetPen (4, 200, 100, 100);

                     context.SetBrush (false, 20, 100, 150);
                     context2.SetBrush (false, 20, 100, 50);

                     context.DrawLine (0, 0, x, y);
                     context.DrawRect (x - 50, y - 50, x + 50, y + 50);
                     context.DrawTxt (50, 50, "thiefunvierse");
                     context.DrawElps (x - 50, y - 50, x + 80, y + 150);

                     context2.DrawRdRt (50, 80, 220, 150, 30, 30);

                     //  context.SaveAsBmp ("thief.bmp");
                   //  context.DrawImg("t.bmp",0,0,60,40,3,3,3);   //can't draw image now
                     //   context.DrawArc (50, 50, 150, 130, 150, 90, 100, 50);//fuck
                     wnd.Draw (&context, 0, 0);
                     context2.PaintOnContext (&context,0,0);          //draw context on context2
                     wndg.Draw (&context2,0,0);
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
