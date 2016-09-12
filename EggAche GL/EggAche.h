//
// Interface of EggAche Graphics Library
// By BOT Man, 2016
//

#ifndef EGGACHE_GL
#define EGGACHE_GL

#include <functional>
#include <list>
#include "EggAche_impl.h"

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

		Window (Window &&);
		// Remarks:
		// Move Window Object

		~Window ();
		// Remarks:
		// Destroy the Window

		Egg *GetBackground ();								// Get Background Egg
		// Remarks:
		// DON'T MOVE or DELETE this Egg...

		void Refresh ();									// Refresh the Window
		// Remarks:
		// 1. Call this function everytime you want to Render
		//    the Background Egg and its Sub Eggs
		// 2. When an error occurs, throw std::runtime_error

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
		void DrawEgg (GUIContext *, const Egg *,			// Helper Function of Refresh
					  size_t, size_t);
		WindowImpl *windowImpl;								// Window Impl Bridge
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

		Egg (Egg &&);
		// Remarks:
		// Move Egg Object

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

		bool SetPen (unsigned int width,					// Pen width
					 unsigned int r = 0,					// Pen color
					 unsigned int g = 0,
					 unsigned int b = 0);
		bool SetBrush (unsigned int r,						// Brush color
					   unsigned int g,
					   unsigned int b);
		// Remarks:
		// If one of r/g/b = -1, the Pen/Brush will be set Transparent;

		void Clear ();										// Clear the Egg
		// Remarks:
		// Erase the content in Egg

		bool DrawTxt (int xBeg, int yBeg, const char *szText,
					  size_t fontSize = 18, const char *fontFamily = "Consolas");
		// Remarks:
		// Draw the szText with a upper left point (xBeg, yBeg)

		bool DrawBmp (const char *szPath,					// Source: "path/name.bmp"
					  int x, int y);						// Position to paste in Egg

		bool DrawBmp (const char *szPath,					// Source: "path/name.bmp"
					  int x, int y,							// Position to paste in Egg
					  int width, int height,				// Size to paste in Egg
					  int r = -1,							// Red color of mask (-1 is not used)
					  int g = -1,							// Green color of mask
					  int b = -1);							// Blue color of mask
		// Remarks:
		// 1. The bmp file will be stretched into width * height in Egg;
		// 2. The color of colorMask will be set to Transparent;
		//    If one of r/g/b is -1, the Egg will be set Opaque;

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

	private:
		int x, y;											// Postion
		std::list<const Egg *> subEggs;						// Sub Eggs
		GUIContext *context;								// GUI Impl Bridge

		Egg (const Egg &) = delete;							// Not allow to copy
		void operator= (const Egg &) = delete;				// Not allow to copy
		friend class Window;
	};

	//======================Message Box========================

	void MsgBox (
		const char *szTxt,						// Text String
		const char *szCap = "Hello EggAche");	// Caption String
	// Remarks:
	// Pop up a Message Box;
}

#endif  //EGGACHE_GL
