#ifndef _DIALOGS_
#define _DIALOGS_
#include "features.h"
#include <commctrl.h>

class DIALOGS
{
	static DIALOGS *This;
   KROSSFEATURES *feat;
//   GLOBAL *Global;

   bool psave;

	BOOL DProc (HWND hwndDlg,UINT Msg,WPARAM wParam);
	bool Message(int NumbMessage);
	bool DPI (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam);

public:
   DIALOGS(KROSSFEATURES *features)
   {
   This = this;
   feat = features;
   }
	int FeaturesDialog (bool NoNew);
   bool ColorDialog (void);
   static BOOL CALLBACK DialogProc (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM);
   static bool CALLBACK DialogProc_info (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam);

};
#endif //_DIALOGS_
