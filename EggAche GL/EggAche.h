//
// User Interface of EggAche Graphical Library
// By LJN-BOT Man, 2016
//

#ifndef EGGACHE_GL
#define EGGACHE_GL

#include <list>
#include "EggAche_impl.h"

namespace EggAche
{
	class Egg;

	typedef void (*ONCLICK)(int, int);
	typedef void (*ONPRESS)(char);
	typedef void (*ONTIMER)();

	//===================EggAche Window========================
	// (0, 0)------------------ width
	//   |
	//   |
	//   |
	//   |
	//   |
	// height

	class Window : private _Window
	{
	public:
		Window (
			int width = 1000, int height = 750,				// Size at least 240 * 120
			const char *cap_string = "Hello EggAche",		// Caption String
			ONCLICK fnClick = nullptr,						// Callback void fnClick (int, int)
			ONPRESS fnPress = nullptr);						// Callback void fnPress (char)
		Window (const Window &origin);
		~Window ();
		
		void AddEgg (Egg& e);								// Add Egg to Window
		std::list<Egg *> &GetLayer ();						// Get Egg layer list

		bool Refresh () override;							// Refresh the Window
		bool IsClosed () const;								// Is Window closed
	protected:
		std::list<Egg *> ly;								// Egg layer data
	};

	//===========================Egg===========================

	class Egg : protected _DrawContext
	{
	public:
		friend class Window;

		Egg (unsigned int width, unsigned int height,		// Egg size
			 int pos_x = 0, int pos_y = 0);					// Egg initial postion

		void Move (int scale_x, int scale_y);				// Move Egg
		void MoveTo (int pos_x, int pos_y);					// Place Egg
		int GetX () const;									// Get current X scale
		int GetY () const;									// Get current Y scale

		bool SetPen (int width,								// Pen width
					int r = 0, int g = 0, int b = 0);		// Pen color
		bool SetBrush (int r, int g, int b);				// Brush color

		bool DrawLine (int xBeg, int yBeg, int xEnd, int yEnd);
		bool DrawRect (int xBeg, int yBeg, int xEnd, int yEnd);
		bool DrawElps (int xBeg, int yBeg, int xEnd, int yEnd);
		bool DrawRdRt (int xBeg, int yBeg,
					   int xEnd, int yEnd, int wElps, int hElps);
		bool DrawArc (int xLeft, int yTop, int xRight, int yBottom,
					  int xBeg, int yBeg, int xEnd, int yEnd);
		bool DrawChord (int xLeft, int yTop, int xRight, int yBottom,
						int xBeg, int yBeg, int xEnd, int yEnd);
		bool DrawPie (int xLeft, int yTop, int xRight, int yBottom,
					  int xBeg, int yBeg, int xEnd, int yEnd);
		bool DrawTxt (int xBeg, int yBeg, const char *szText);

		bool DrawBmp (const char *szPath,					// Source: "path/name.bmp"
					  int x = 0, int y = 0,					// Position to paste
					  int width = -1, int height = -1,		// Size to paste (-1 as default)
					  int r = -1,							// Red color of mask (-1 is not used)
					  int g = -1,							// Green color of mask
					  int b = -1);							// Blue color of mask

		void Clear ();										// Clear the Egg
	private:
		int x, y;											// Postion data
	};

	//======================Message Box========================

	void MsgBox (
		const char *szTxt,				// the Text String
		const char *szCap);				// the Caption String

	//===================Timer Function========================

	class Timer : private _Timer
	{
	public:
		Timer (
			int gap_ms,					// Time gap, millisecond
			ONTIMER fn_callback);		// Callback void fn () per Tick

		Timer (const Timer&) = delete;	// Not allow to copy

		~Timer ();						// Stop the Timer
	};
}

#endif  //EGGACHE_GL
