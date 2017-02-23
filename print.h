#include "bitmaps.h"

class PRINTS {
	static PRINTS *This;
	HWND hWnd;
   HINSTANCE hinst;
   DOCINFO di;
   PAGESETUPDLG lppsd;
  	PRINTDLG pd;
   static bool bPrint;
   static HWND hdlgCancel;
   static KROSSFEATURES *features;
   static int  CALLBACK AbortProc(HDC , int );
   static bool CALLBACK AbortPrintJob(HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM);
   static UINT APIENTRY PagePaintHook(HWND /*hdlg*/, UINT uiMsg, WPARAM wParam, LPARAM lParam);
   void FillPageSetupStructure(void *param);
	bool PageSetup(void *param);

public:

	PRINTS(KROSSFEATURES *feat, LOGFONT *plf);
};
