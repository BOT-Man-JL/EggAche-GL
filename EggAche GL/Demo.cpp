//
// A demo of EggAche Graphics Library
// By BOT Man, 2016
//

#include "EggAche.h"	// ����EggAche.h��ͼ
#include <thread>		// ���ڵ���std::this_thread::sleep_for����ʱ

void fnClick (int x, int y);	// ���������
void fnPress (char ch);			// �����û�����

EggAche::Window g_window (1000, 750, "Demo", fnClick, fnPress);
	// �½�һ��1000 * 750�Ĵ��ڣ�����Ϊ��Demo��
	// ��������������fnClick��������������
	// ����м������룬����fnPress�������������ַ�

EggAche::Egg g_egg (1000, 750);		// �½�һ���ʹ���һ�����egg

int main ()
{
	EggAche::Egg background (1000, 750), egg (60, 80);		// �½�����egg

	// ��background��д��
	background.DrawTxt (0, 0, "Adapted and Responsive design.");
	background.DrawTxt (0, 50, "You can resize the window, and everything will be stretched.");
	background.DrawTxt (0, 100, "You can click and press a key on the window.");

	// ��egg�ϲ���ƽ�̷�ʽ��ͼ��������ɫ(255,255,255)���͸��
	egg.DrawBmp ("Egg.bmp", 0, 0, -1, -1, 255, 255, 255);

	// ��egg�Ƶ���(470, 355)
	egg.MoveTo (470, 355);

	// ������Egg��˳�������g_window
	g_window.AddEgg (background);
	g_window.AddEgg (g_egg);
	g_window.AddEgg (egg);

	// g_window�Զ�ˢ�¹�����Egg
	g_window.Refresh ();

	// �жϴ����Ƿ�ر�
	while (!g_window.IsClosed ())
	{
		// egg�����ƶ�4����λ������3����λ
		egg.Move (4, 3);

		// ����ƹ�ͷ�˾ͷŻ���
		if (egg.GetX () >= 940 || egg.GetY () >= 670)
			egg.MoveTo (470, 355);

		// ˢ��һ��g_window
		g_window.Refresh ();

		// ��ʱ50����
		std::this_thread::sleep_for (std::chrono::milliseconds (50));
	}

	return 0;
}

// �����g_window�󣬴���������(x, y)
void fnClick (int x, int y)
{
	// ��֮ǰ��g_egg���
	g_egg.Clear ();

	// ��g_egg��һ����(0, 0)��(x, y)��ֱ��
	g_egg.DrawLine (0, 0, x, y);

	// ����ˢ��g_window
	g_window.Refresh ();
}

// ���������뵽g_window�󣬴��������ַ�ch
void fnPress (char ch)
{
	char str[2];
	
	// ����������AsciiתΪ��ʱ�ַ������Ի������
	str[0] = ch;
	str[1] = 0;
	EggAche::MsgBox (str, "ASCII Value");
}