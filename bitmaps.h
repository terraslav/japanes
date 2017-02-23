#ifndef _BITMAPS_
#define _BITMAPS_
#include "features.h"

class BITMAPS {
   bool objects;
	HWND hWnd;
   HINSTANCE hinst;
   byte *Maps;
   int hSize, vSize, hExtSize, vExtSize, hTotalSize, vTotalSize, RectSize;
   HPEN SolidPen, DotPen;
   COLORREF *Colors;
   HBRUSH *Brushes;
  	byte WhiteIndex;
   bool pict, web, Web5x5, dig, kross;
   LOGFONT lf;
  	CHOOSEFONT cf;
   KROSSFEATURES *feat;
   RECT Client, Border, hDig, vDig, Fild, Icon;
   POINT VPort,                 //Стартовая точка вывода
                WorkFild;       //  и прямоугольник рабочего поля
   static BOOL CALLBACK ImpDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
   static BITMAPS *BMPS;
   void PaintBMP		(void);
	void ChooseFonts	(void);
   void SelectFonts 	(void);
   void ViewTable		(void);
	void DrawCell 		(int x, int y);
	void MakeRect		(void);
	void PaintIconic 	(void);
   void Calculate		(bool fullcolors);
   void ViewDigit(int x, int y, bool Whu);
   RGBQUAD Convert (COLORREF cr);

public:
	bool error;
	byte *FileInMemory;
   HDC hMemDC;
   BITMAP bmp;
   HBITMAP hbmp;
   BITMAPS(KROSSFEATURES *features, LOGFONT *plf, bool printing = false);
   ~BITMAPS();
   byte GetIndexColor(byte number);
	byte *CreateBitmapFiles();
   void PaintSample(HDC hDC, LPRECT rc, LPRECT);// рисование миниатюры при уст. парам. страницы
   void CreatePictures(int CellSize){RectSize = CellSize; SelectFonts(); Calculate(true);	PaintBMP();}
};

#endif // _BITMAPS_

