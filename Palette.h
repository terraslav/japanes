#ifndef _PALETTE_
#define _PALETTE_
#include "features.h"
#include <commctrl.h>

const HeighButton = 24;
const WidthButton = 16;

class PALETTE {
   static PALETTE *This;
   HWND HWndPen, HWndRect, HWndEyeDropper, HWndFilled, HWndDigit, HWndLetter;
   HHOOK hhk;
   KROSSFEATURES *features;
   CTOOL OldTool;
	RECT GetToolRect();
	void CreateToolPalette(void);
	void DestroyToolPalette(void);
   void SetControl(CTOOL control);

   LRESULT GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
   bool EnumChildProc(HWND hwndCtrl, LPARAM lParam);
	static LRESULT CALLBACK GetMsgProcA(int nCode, WPARAM wParam, LPARAM lParam)
   	{return This->GetMsgProc(nCode, wParam, lParam);}
   static bool EnumChildProcA(HWND hwndCtrl, LPARAM lParam)
   	{return This->EnumChildProc(hwndCtrl, lParam);}
   virtual void ChangesInFileA()=0;
   virtual void PrepareUndo()=0;
	static bool CALLBACK LetterDialog(HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	bool LetterDialog_CallBack (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	void UpdateMainWindow();

   int *Column, *Line;
   char *Letter;
   bool ExecuteDialog;

protected:
   HWND hWndDialog;
   bool view;
   HWND HWnd, hWndPalette;
   HWND HWndColors[MAXCOLOR+1];
   void CreateColorPalette ();
	void DestroyColorPalette(void);
   void UpdatePalette ();
   bool PaletteButtonClicked(int Id, HWND hWndControl);
   void PaletteWindowSizing(void);
   void SetButtonState(void);
   void RestoreOldButtonState(void){features->MouseInput = OldTool;SetButtonState();}
   virtual void ScreenInitialize()=0;
   virtual void SetCurrentInput(CTOOL) =0;

public:
   virtual void DrawCell (HDC, int, int)=0;
   virtual POINT *GetMapViewPort()=0;
	void PaintColorPal(DRAWITEMSTRUCT *dis);
   void SetCurColor (HWND hwndcolor);
   void TuningColor (HWND hwnd);
	void SetFeatures (KROSSFEATURES *feat);
	bool DoCreateTooltip(void);
   LRESULT PalNeedText(LPTOOLTIPTEXT ttt);
	PALETTE()
   {
   ExecuteDialog=false;
   hhk = NULL;
   hWndPalette = NULL;
   }
   ~PALETTE()
   {
	DestroyColorPalette();
	DestroyToolPalette();
   }
};
#endif //_PALETTE_
