//
// Implementation Interface of EggAche Graphics Library
// By BOT Man, 2016
//

#ifndef EGGACHE_GL_IMPL
#define EGGACHE_GL_IMPL

namespace EggAche
{
	class GUIContext;

	class WindowImpl
	{
	public:
		WindowImpl (size_t, size_t, const char *) {}
		virtual ~WindowImpl () {}

		virtual void Draw (const GUIContext *context,
						   size_t x, size_t y) = 0;
		virtual void Clear () = 0;
		virtual bool IsClosed () const = 0;
	};

	class GUIContext
	{
	public:
		GUIContext (size_t, size_t) {}
		virtual ~GUIContext () {}

		virtual bool SetPen (unsigned int width,
							 unsigned int r = 0,
							 unsigned int g = 0,
							 unsigned int b = 0) = 0;

		virtual bool SetBrush (unsigned int r,
							   unsigned int g,
							   unsigned int b) = 0;

		virtual bool DrawLine (int xBeg, int yBeg, int xEnd, int yEnd) = 0;

		virtual bool DrawRect (int xBeg, int yBeg, int xEnd, int yEnd) = 0;

		virtual bool DrawElps (int xBeg, int yBeg, int xEnd, int yEnd) = 0;

		virtual bool DrawRdRt (int xBeg, int yBeg,
							   int xEnd, int yEnd, int wElps, int hElps) = 0;

		virtual bool DrawArc (int xLeft, int yTop, int xRight, int yBottom,
							  int xBeg, int yBeg, int xEnd, int yEnd) = 0;

		virtual bool DrawChord (int xLeft, int yTop, int xRight, int yBottom,
								int xBeg, int yBeg, int xEnd, int yEnd) = 0;

		virtual bool DrawPie (int xLeft, int yTop, int xRight, int yBottom,
							  int xBeg, int yBeg, int xEnd, int yEnd) = 0;

		virtual bool DrawTxt (int xBeg, int yBeg, const char *szText,
							  size_t fontSize, const char *fontFamily) = 0;

		virtual bool DrawBmp (const char *szPath,
							  int x = 0, int y = 0,
							  int width = -1, int height = -1,
							  int r = -1,
							  int g = -1,
							  int b = -1) = 0;

		virtual void Clear () = 0;
	};

	class GUIFactory
	{
	public:
		virtual WindowImpl *NewWindow (size_t width, size_t height,
									   const char *cap_string) = 0;
		virtual GUIContext *NewGUIContext (size_t width, size_t height) = 0;
	};

	void MsgBox_Impl (const char *szTxt, const char *szCap);
}

#endif  //EGGACHE_GL_IMPL