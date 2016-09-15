//
// Interface of EggAche Graphics Library
// By BOT Man, 2016
//

#ifndef EGGACHE_GL
#define EGGACHE_GL

// Enable Windows Version
#define EGGACHE_WINDOWS

//#define EGGACHE_LINUX
//#define EGGACHE_MAC

#include <functional>
#include <list>
#include "EggAche_Impl.h"

namespace EggAche
{
	class Egg;

	//===================EggAche Window========================

	class Window
	{
	public:
		Window (size_t width = 1000, size_t height = 750,		// Size at least 240 * 120
				const char *cap_string = "Hello EggAche");		// Caption String
		// Remarks:
		// 1. Create a Window of Logic Size width * height with Caption cap_string;
		// 2. When an error occurs, throw std::runtime_error

		~Window ();
		// Remarks:
		// Destroy the Window

		Egg *GetBackground ();								// Get Background Egg
		// Remarks:
		// DON'T DELETE this Egg...

		void Refresh ();									// Refresh the Window
		// Remarks:
		// 1. Call this function everytime you want to Render
		//    the Background Egg and its Sub Eggs
		// 2. Why NO Auto Refresh?
		//    Auto Refresh will cost much more resource if there are too many
		//    drawing changes at a time...

		bool IsClosed () const;								// Is Window closed
		// Remarks:
		// If the Window has been Closed by User, it will return false

		void OnClick (std::function<void (int, int)> fn);
		void OnPress (std::function<void (char)> fn);
		void OnResized (std::function<void (int, int)> fn);
		// Remarks:
		// If you click or press a key on Window, back-end will callback fn;
		// 1. Calling fnClick with (int x, int y) means point (x, y) is Clicked;
		// 2. Calling fnPress with (char ch) means character 'ch' is Inputted;
		// 3. Calling fnResized with (int x, int y) means Currently Window Size is x * y;

	private:
		EggAche_Impl::WindowImpl *windowImpl;				// Window Impl Bridge
		Egg *bgEgg;											// Background Egg

		Window (const Window &) = delete;					// Not allow to copy
		void operator= (const Window &) = delete;			// Not allow to copy
	};

	//===========================Egg===========================

	class Egg
	{
	public:
		Egg (size_t width, size_t height,					// Egg's size
			 int pos_x = 0, int pos_y = 0);					// Egg's initial postion
		// Remarks:
		// When an error occurs, throw std::runtime_error

		~Egg ();
		// Remarks:
		// Destroy the Egg

		int GetX () const;									// Get Egg's coordinate x
		int GetY () const;									// Get Egg's coordinate y
		void MoveTo (int pos_x, int pos_y);					// Place Egg to (pos_x, pos_y)
		void Move (int scale_x, int scale_y);				// Move Egg
		// Remarks:
		// If scale_x > 0, Egg will be moved right scale_x units; else moved left -scale_x;
		// Similarly move scale_y;

		void AddEgg (Egg *egg);								// Add Sub Eggs
		void RemoveEgg (Egg *egg);							// Remove Sub Eggs
		// Remarks:
		// Associated Eggs will be rendered after this Egg

		bool SetPen (unsigned int width,					// Pen Width
					 unsigned int r = 0,					// Pen Color
					 unsigned int g = 0,
					 unsigned int b = 0);
		bool SetBrush (bool isTransparent,					// Is Transparent
					   unsigned int r,						// Brush Color
					   unsigned int g,
					   unsigned int b);
		bool SetFont (unsigned int size = 18,				// Font Size
					  const char *family = "Consolas",		// Font Family
					  unsigned int r = 0,					// Font Color
					  unsigned int g = 0,
					  unsigned int b = 0);
		// Remarks:
		// If SetBrush's isTransparent is set, the Color will be ignored

		void Clear ();										// Clear the Egg
		// Remarks:
		// Erase the content in Egg

		bool DrawTxt (int xBeg, int yBeg, const char *szText);
		// Remarks:
		// Draw the szText with a upper left point (xBeg, yBeg)

		bool DrawImg (const char *fileName,					// Source: "path/name.*"
					  int x, int y);						// Position to paste in Egg

		bool DrawImg (const char *fileName,					// Source: "path/name.*"
					  int x, int y,							// Position to paste in Egg
					  int width, int height);				// Size to paste in Egg

		bool DrawImg (const char *fileName,					// Source: "path/name.bmp"
					  int x, int y,							// Position to paste in Egg
					  int width, int height,				// Size to paste in Egg
					  unsigned int r,						// Red color of mask
					  unsigned int g,						// Green color of mask
					  unsigned int b);						// Blue color of mask
		// Remarks:
		// 1. The Image file will be stretched into width * height in Egg;
		// 2. Support Bitmap (.bmp), JPEG (.jpg/.jpeg), PNG (.png), GIF (.gif);
		//    MinGW only Support Bitmap (.bmp) file... (Impl.ed by GDI)
		// 3. For Opaque Images, you can set ColorMask to Draw Transparently;

		bool DrawLine (int xBeg, int yBeg, int xEnd, int yEnd);
		// Remarks:
		// Draw a Line from (xBeg, yBeg) to (xEnd, yEnd);

		bool DrawRect (int xBeg, int yBeg, int xEnd, int yEnd);
		// Remarks:
		// Draw a Rectangle
		//   (xBeg, yBeg)-----------------(xEnd, yBeg)
		//        |                            |
		//        |                            |
		//        |                            |
		//        |                            |
		//   (xBeg, yEnd)-----------------(xEnd, yEnd);

		bool DrawElps (int xBeg, int yBeg, int xEnd, int yEnd);
		// Remarks:
		// Draw the Ellipse in such an imaginary bounding box;

		bool DrawRdRt (int xBeg, int yBeg,
					   int xEnd, int yEnd, int wElps, int hElps);
		// Remarks:
		// Draw a round conner Rectangle;
		// wElps, hElps = the width/height of the rounded corners Ellipse;

		bool DrawArc (int xLeft, int yTop, int xRight, int yBottom,
					  int xBeg, int yBeg, int xEnd, int yEnd);
		// Remarks:
		// The points (xLeft, yTop) and (xRight, yBottom) specify the bounding box;
		// An Ellipse formed by the bounding box defines the curve of the Arc;
		// The Arc extends in the current drawing direction from the point
		// where it intersects the radial from the center to (xBeg, yBeg);
		// The Arc ends where it intersects the radial to (xEnd, yEnd);

		bool DrawChord (int xLeft, int yTop, int xRight, int yBottom,
						int xBeg, int yBeg, int xEnd, int yEnd);
		// Remarks:
		// The points (xLeft, yTop) and (xRight, yBottom) specify the bounding box;
		// An Ellipse formed by the bounding box defines the curve of the Chord;
		// The curve begins at the point where the Ellipse intersects the first radial
		// and extends counterclockwise to the point where the second radial intersects;
		// The Chord is closed by drawing a line from the intersection of the first radial
		// and the curve to the intersection of the second radial and the curve;

		bool DrawPie (int xLeft, int yTop, int xRight, int yBottom,
					  int xBeg, int yBeg, int xEnd, int yEnd);
		// Remarks:
		// The points (xLeft, yTop) and (xRight, yBottom) specify the bounding box;
		// An Ellipse formed by the bounding box defines the curve of the Pie;
		// The curve begins at the point where the Ellipse intersects the first radial
		// and extends counterclockwise to the point where the second radial intersects;

		bool SaveAsBmp (const char *fileName);				// "path/name.bmp"
		// Remarks:
		// Save Window's Content into a Bitmap (.bmp) File;

	private:
		int x, y, w, h;										// Postion and Size
		std::list<const Egg *> subEggs;						// Sub Eggs
		EggAche_Impl::GUIContext *context;					// GUI Impl Bridge

		void RecursiveDraw (EggAche_Impl::GUIContext *,		// Helper Function of
							size_t, size_t) const;			// Window.Refresh
		friend void Window::Refresh ();

		Egg (const Egg &) = delete;							// Not allow to copy
		void operator= (const Egg &) = delete;				// Not allow to copy
	};

	//======================Message Box========================

	void MsgBox (
		const char *szTxt,						// Text String
		const char *szCap = "Hello EggAche");	// Caption String
	// Remarks:
	// Pop up a Message Box;
}

#endif  //EGGACHE_GL
