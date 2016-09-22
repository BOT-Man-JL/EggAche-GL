//
// Implementation Interface of EggAche Graphics Library
// By BOT Man, 2016
//

#ifndef EGGACHE_GL_IMPL
#define EGGACHE_GL_IMPL

#include <functional>
#include <utility>

namespace EggAche_Impl
{
	class GUIContext;

	class WindowImpl
	{
	public:
		virtual ~WindowImpl () {}

		virtual void Draw (const GUIContext *context,
						   size_t x, size_t y) = 0;

		virtual std::pair<size_t, size_t> GetSize () = 0;
		virtual bool IsClosed () const = 0;

		virtual void OnClick (std::function<void (int, int)> fn) = 0;
		virtual void OnPress (std::function<void (char)> fn) = 0;
		virtual void OnResized (std::function<void (int, int)> fn) = 0;
		virtual void OnRefresh (std::function<void ()> fn) = 0;
	};

	class GUIContext
	{
	public:
		virtual ~GUIContext () {}

		virtual bool SetPen (unsigned int width,
							 unsigned int r = 0,
							 unsigned int g = 0,
							 unsigned int b = 0) = 0;

		virtual bool SetBrush (bool isTransparent,
							   unsigned int r,
							   unsigned int g,
							   unsigned int b) = 0;

		virtual bool SetFont (unsigned int size = 18,
							  const char *family = "Consolas",
							  unsigned int r = 0,
							  unsigned int g = 0,
							  unsigned int b = 0) = 0;

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

		virtual bool DrawTxt (int xBeg, int yBeg, const char *szText) = 0;
		virtual size_t GetTxtWidth (const char *szText) = 0;

		virtual bool DrawImg (const char *fileName,
							  int x = 0, int y = 0,
							  int width = -1, int height = -1,
							  int r = -1,
							  int g = -1,
							  int b = -1) = 0;

		virtual bool SaveAsJpg (const char *fileName) const = 0;
		virtual bool SaveAsPng (const char *fileName) const = 0;
		virtual bool SaveAsBmp (const char *fileName) const = 0;

		virtual void Clear () = 0;

		virtual void PaintOnContext (GUIContext *,
									 size_t x, size_t y) const = 0;
	};

	void MsgBox_Impl (const char *szTxt, const char *szCap);

	class GUIFactory
	{
	public:
		virtual WindowImpl *NewWindow (size_t width, size_t height,
									   const char *cap_string) = 0;
		virtual GUIContext *NewGUIContext (size_t width, size_t height) = 0;
	};

	class GUIFactory_Windows : public GUIFactory
	{
	public:
		WindowImpl *NewWindow (size_t width, size_t height,
							   const char *cap_string) override;
		GUIContext *NewGUIContext (size_t width, size_t height) override;
	};

	class GUIFactory_XWindow : public GUIFactory
	{
	public:
		WindowImpl *NewWindow (size_t width, size_t height,
							   const char *cap_string) override;
		GUIContext *NewGUIContext (size_t width, size_t height) override;
	};
}

#endif  //EGGACHE_GL_IMPL