//
// A demo of EggAche Graphics Library
// By BOT Man, 2016
//

#include "EggAche.h"	// 引用EggAche.h画图
#include <thread>		// 用于调用std::this_thread::sleep_for来延时

void fnClick (int x, int y);	// 鼠标点击函数
void fnPress (char ch);			// 键盘敲击函数

EggAche::Window g_window (1000, 750, "Demo", fnClick, fnPress);
	// 新建一个1000 * 750的窗口，标题为“Demo”
	// 如果被点击，调用fnClick，并传入点击坐标
	// 如果有键盘输入，调用fnPress，并传入输入字符

EggAche::Egg g_egg (1000, 750);		// 新建一个和窗口一样大的egg

int main ()
{
	EggAche::Egg background (1000, 750), egg (60, 80);		// 新建两个egg

	// 在background上写字
	background.DrawTxt (0, 0, "Adapted and Responsive design.");
	background.DrawTxt (0, 50, "You can resize the window, and everything will be stretched.");
	background.DrawTxt (0, 100, "You can click and press a key on the window.");

	// 在egg上采用平铺方式贴图，并将白色(255,255,255)变成透明
	egg.DrawBmp ("Egg.bmp", 0, 0, -1, -1, 255, 255, 255);

	// 将egg移到到(470, 355)
	egg.MoveTo (470, 355);

	// 将三个Egg按顺序关联到g_window
	g_window.AddEgg (background);
	g_window.AddEgg (g_egg);
	g_window.AddEgg (egg);

	// g_window自动刷新关联的Egg
	g_window.Refresh ();

	// 判断窗口是否关闭
	while (!g_window.IsClosed ())
	{
		// egg向右移动4个单位，向下3个单位
		egg.Move (4, 3);

		// 如果移过头了就放回来
		if (egg.GetX () >= 940 || egg.GetY () >= 670)
			egg.MoveTo (470, 355);

		// 刷新一下g_window
		g_window.Refresh ();

		// 延时50毫秒
		std::this_thread::sleep_for (std::chrono::milliseconds (50));
	}

	return 0;
}

// 当点击g_window后，传入点击坐标(x, y)
void fnClick (int x, int y)
{
	// 将之前的g_egg清空
	g_egg.Clear ();

	// 在g_egg画一条从(0, 0)到(x, y)的直线
	g_egg.DrawLine (0, 0, x, y);

	// 立即刷新g_window
	g_window.Refresh ();
}

// 当键盘输入到g_window后，传入输入字符ch
void fnPress (char ch)
{
	char str[2];
	
	// 将键盘输入Ascii转为临时字符串来对话框输出
	str[0] = ch;
	str[1] = 0;
	EggAche::MsgBox (str, "ASCII Value");
}