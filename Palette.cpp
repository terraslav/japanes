#include "palette.h"

PALETTE *PALETTE::This;

void PALETTE::SetButtonState(void)
{
	SendMessage (HWndPen, BM_SETCHECK,(WPARAM) BST_UNCHECKED, NULL);
	SendMessage (HWndRect, BM_SETCHECK,(WPARAM) BST_UNCHECKED, NULL);
	SendMessage (HWndEyeDropper, BM_SETCHECK,(WPARAM) BST_UNCHECKED, NULL);
	SendMessage (HWndFilled, BM_SETCHECK,(WPARAM) BST_UNCHECKED, NULL);
	SendMessage (HWndDigit, BM_SETCHECK,(WPARAM) BST_UNCHECKED, NULL);
   HWND current;
	switch(features->MouseInput){
   	case CPEN:
      	current = HWndPen;
      	break;
      case CRECT:
      	current = HWndRect;
      	break;
      case CEYE:
      	current = HWndEyeDropper;
      	break;
      case CFILLED:
      	current = HWndFilled;
      	break;
      case CDIGITAL:
      	current = HWndDigit;
      	break;
      }
	SendMessage (current, BM_SETCHECK,(WPARAM) BST_CHECKED, NULL);
}

bool PALETTE::PaletteButtonClicked(int, HWND hWndControl)
{
   if(hWndControl!=HWndPen && hWndControl!=HWndRect && hWndControl!=HWndEyeDropper &&
   	hWndControl!=HWndFilled && hWndControl!=HWndDigit && hWndControl!=HWndLetter) return false;
   OldTool = features->MouseInput;
	if     (hWndControl==HWndPen) SetCurrentInput(CPEN);
	else if(hWndControl==HWndRect) SetCurrentInput(CRECT);
	else if(hWndControl==HWndEyeDropper) SetCurrentInput(CEYE);
	else if(hWndControl==HWndFilled) SetCurrentInput(CFILLED);
	else if(hWndControl==HWndDigit) SetCurrentInput(CDIGITAL);
	else if(hWndControl==HWndLetter)
            if(!ExecuteDialog && !features->ExecDemo)
					hWndDialog=CreateDialog(features->hinst,MAKEINTRESOURCE(IDD_LETTER_DIALOG),
                                 HWnd,(DLGPROC) PALETTE::LetterDialog);
	SetFocus(HWnd);
	return true;
}

void PALETTE::SetFeatures (KROSSFEATURES *feat)
{
   This = this;
   features = feat;
   feat->CurrentColor = BLACK;	// номер текущего цвета
   view = (feat->Colors > 1) ? true : false;
   HWnd=feat->Global->hWnd;
   hWndPalette = NULL;
   hhk = NULL;
	CreateToolPalette();
   CreateColorPalette();
//   DoCreateTooltip(hWndPalette);
}

RECT PALETTE::GetToolRect()
{
   RECT tr;
   GetClientRect(HWnd, &tr);
   tr.top = tr.top+features->Global->WindowToolHeight;
	tr.bottom = tr.bottom-features->Global->StatusHight;
   return tr;
}

void PALETTE::PaletteWindowSizing(void)
{
   RECT tp = GetToolRect();
	MoveWindow(hWndPalette, tp.left, tp.top, WidthButton*2+4, tp.bottom, true);
}

void PALETTE::CreateToolPalette(void)
{
   RECT tp = GetToolRect();
   DWORD style = WS_CHILD|BS_BITMAP|BS_PUSHLIKE|BS_CHECKBOX;//|BS_PUSHBUTTON

// создаем окно палитры
	hWndPalette =  CreateWindowEx(WS_EX_PALETTEWINDOW|WS_EX_WINDOWEDGE,CLASS_NAME/*TOOLBARCLASSNAME*/, "Панель инструментов",
      WS_CHILD | TBSTYLE_TOOLTIPS,
    	tp.left, tp.top, WidthButton*2+4, tp.bottom-tp.top, HWnd, NULL,
    	features->Global->hinst, NULL);
      if (hWndPalette) ShowWindow(hWndPalette, SW_SHOWDEFAULT);
      else {view = false; return;}
   if(!hWndPalette) return;
   GetClientRect(hWndPalette, &tp);
   features->Global->PanelWidth = tp.right - tp.left;

// Создаем кнопки инструментов
   HWndPen = CreateWindow(
    	"BUTTON", "", style,
    	0, 0, WidthButton*2+2, HeighButton, hWndPalette, NULL,
    	features->Global->hinst, (LPVOID) NULL);
   if (HWndPen)
   	{
      ShowWindow(HWndPen, SW_SHOWDEFAULT);
      SendMessage(HWndPen, BM_SETIMAGE, IMAGE_BITMAP,
      				(LPARAM)CreateMappedBitmap(features->hinst, IDB_BITMAP2,
    								0, (LPCOLORMAP) NULL, 0));
      }

   HWndRect = CreateWindow(
    	"BUTTON", "", style,
    	0, HeighButton, WidthButton*2+2, HeighButton, hWndPalette, NULL,
    	features->Global->hinst, (LPVOID) NULL);
   if (HWndRect)
      {
   	ShowWindow(HWndRect, SW_SHOWDEFAULT);
      SendMessage(HWndRect, BM_SETIMAGE, IMAGE_BITMAP,
						(LPARAM)CreateMappedBitmap(features->hinst, IDB_BITMAP3,
    								0, (LPCOLORMAP) NULL, 0));
      }

   HWndEyeDropper = CreateWindow(
    	"BUTTON", "", style,
    	0, HeighButton*2, WidthButton*2+2, HeighButton, hWndPalette, NULL,
    	features->Global->hinst, (LPVOID) NULL);
   if (HWndEyeDropper)
   	{
      ShowWindow(HWndEyeDropper, SW_SHOWDEFAULT);
      SendMessage(HWndEyeDropper, BM_SETIMAGE, IMAGE_BITMAP,
						(LPARAM)CreateMappedBitmap(features->hinst, IDB_BITMAP4,
    								0, (LPCOLORMAP) NULL, 0));
      }

   HWndFilled = CreateWindow(
    	"BUTTON", "", style,
    	0, HeighButton*3, WidthButton*2+2, HeighButton, hWndPalette, NULL,
    	features->Global->hinst, (LPVOID) NULL);
   if (HWndFilled)
   	{
      ShowWindow(HWndFilled, SW_SHOWDEFAULT);
      SendMessage(HWndFilled, BM_SETIMAGE, IMAGE_BITMAP,
						(LPARAM)CreateMappedBitmap(features->hinst, IDB_BITMAP5,
    								0, (LPCOLORMAP) NULL, 0));
      }

   HWndDigit = CreateWindow(
    	"BUTTON", "", style,
    	0, HeighButton*4, WidthButton*2+2, HeighButton, hWndPalette, NULL,
    	features->Global->hinst, (LPVOID) NULL);
   if (HWndDigit)
   	{
      ShowWindow(HWndDigit, SW_SHOWDEFAULT);
      SendMessage(HWndDigit, BM_SETIMAGE, IMAGE_BITMAP,
						(LPARAM)CreateMappedBitmap(features->hinst, IDB_BITMAP6,
    								0, (LPCOLORMAP) NULL, 0));
      }
   HWndLetter = CreateWindow(
    	"BUTTON", "", style,
    	0, HeighButton*5, WidthButton*2+2, HeighButton, hWndPalette, NULL,
    	features->Global->hinst, (LPVOID) NULL);
   if (HWndLetter)
   	{
      ShowWindow(HWndLetter, SW_SHOWDEFAULT);
      SendMessage(HWndLetter, BM_SETIMAGE, IMAGE_BITMAP,
						(LPARAM)CreateMappedBitmap(features->hinst, IDB_BITMAP7,
    								0, (LPCOLORMAP) NULL, 0));
      }
}

bool PALETTE::DoCreateTooltip(void)
{
// DoCreateTooltip - создает tooltip управление и добавляет некоторые
// инструментальные средства к нему.
// Если удачно, то возвращает хэдер tooltip управления или NULL
// в противном случае.
// hwndOwner - Хэндл родительского окна

   if(!hWndPalette) return false;

   if(!features->hWndToolTip)
   	features->hWndToolTip = CreateWindow(TOOLTIPS_CLASS, (LPSTR) NULL, TTS_ALWAYSTIP,
        			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        			NULL, (HMENU) NULL, features->hinst, NULL);

   if (features->hWndToolTip == (HWND) NULL)
   	return false;

   if (!EnumChildWindows(hWndPalette, (WNDENUMPROC) EnumChildProcA, 0))
        return FALSE;

   if (!hhk) hhk = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProcA,
        (HINSTANCE) NULL, GetCurrentThreadId());

   if (hhk == (HHOOK) NULL)
        return FALSE;
	SendMessage(features->hWndToolTip, TTM_ACTIVATE, (WPARAM)true,(LPARAM) 0);
//   SendMessage(features->hWndToolTip, TTM_SETDELAYTIME, (WPARAM)TTDT_AUTOMATIC, (LPARAM)200);
	return true;
}

LRESULT PALETTE::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSG *lpmsg;
    lpmsg = (MSG *) lParam;
    if (nCode < 0 || (!(IsChild(hWndPalette, lpmsg->hwnd)) &&!(features->Global->hToolWnd==lpmsg->hwnd)))
        return (CallNextHookEx(hhk, nCode, wParam, lParam));

    switch (lpmsg->message) {
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            if (IsChild(hWndPalette, lpmsg->hwnd) && features->hWndToolTip != NULL)
                SendMessage(features->hWndToolTip, TTM_RELAYEVENT, 0, (LPARAM)lpmsg);
            break;
        default:
            break;
    }
    return CallNextHookEx(hhk, nCode, wParam, lParam);
}

bool PALETTE::EnumChildProc(HWND hwndCtrl, LPARAM)
{
   TOOLINFO ti;
	static char *szTips[6] =   // tip text
   {
   "Карандаш", "Прямоугольник", "Пипетка", "Заливка", "Ввод оцифровки", "Задать слово-подсказку"
   };

        ti.cbSize = sizeof(TOOLINFO);
        ti.uFlags = TTF_IDISHWND;

        ti.hwnd = HWnd;
        ti.uId = (UINT) hwndCtrl;
        ti.hinst = 0;
        if(hwndCtrl==HWndPen)             ti.lpszText = szTips[0];
        else if(hwndCtrl==HWndRect) 		ti.lpszText = szTips[1];
        else if(hwndCtrl==HWndEyeDropper) ti.lpszText = szTips[2];
        else if(hwndCtrl==HWndFilled)     ti.lpszText = szTips[3];
        else if(hwndCtrl==HWndDigit)      ti.lpszText = szTips[4];
        else if(hwndCtrl==HWndLetter)     ti.lpszText = szTips[5];
        else  										ti.lpszText = LPSTR_TEXTCALLBACK;
        SendMessage(features->hWndToolTip, TTM_ADDTOOL, 0,
            (LPARAM) (LPTOOLINFO) &ti);

    return TRUE;
}

LRESULT PALETTE::PalNeedText(LPTOOLTIPTEXT ttt)
{
	int numbcolor;
   char *text = ttt->szText;
   ttt->lpszText = text;
   ttt->hinst = NULL;
   *text = NULL;
   for(numbcolor = 0; numbcolor<=MAXCOLOR; numbcolor++)
   	if(HWndColors[numbcolor] == (HWND)ttt->hdr.idFrom) break;
	if(numbcolor == MAXCOLOR) numbcolor = features->CurrentColor-FIRST;
   features->Global->NumberColorToString(text, numbcolor);
	return 0;
}

void PALETTE::CreateColorPalette ()
{
   int x,y;
   RECT client = GetToolRect();

   if((features->Colors <= 1))
   	{
      view = false;
      return;
      }
   else view = true;
   int OffX = 0;
   int OffY = (HeighButton)*6+2;
// создаем окошки выбора цвета
   DWORD style = WS_CHILD | SS_NOTIFY | SS_OWNERDRAW | WS_BORDER;
   TOOLINFO ti;
   for(int i=0; i<MAXCOLOR; i++)
   	{
		x=(i/(MAXCOLOR/2))*(WidthButton+1)+OffX;
      y=(i%(MAXCOLOR/2))*(WidthButton+1)+OffY;
      HWndColors[i] = CreateWindow(
    	"STATIC", "", style,
    	x, y, WidthButton, WidthButton, hWndPalette, NULL,
    	features->Global->hinst, (void *) &features->ColorTable[i+FIRST]);
      if (HWndColors[i])
	      {
   	   ShowWindow(HWndColors[i], SW_SHOWDEFAULT);
   	   ti.cbSize = sizeof(TOOLINFO);
      	ti.hwnd = HWnd;
	      ti.uId  = (UINT)HWndColors[i];
      	ti.uFlags = TTF_IDISHWND;
        	ti.hinst = 0;
         ti.lpszText = LPSTR_TEXTCALLBACK;
	      SendMessage(features->hWndToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
   	   }
      }
// окно текущего цвета
   HWndColors[MAXCOLOR] = CreateWindowEx(WS_EX_CLIENTEDGE,"STATIC","",style,
   x-(WidthButton+2), y+WidthButton+2, WidthButton*2+2, WidthButton*2+2, hWndPalette,
   NULL, features->Global->hinst, NULL);
   if (HWndColors[MAXCOLOR])
   	{
      ShowWindow(HWndColors[MAXCOLOR], SW_SHOWDEFAULT);
      ti.cbSize = sizeof(TOOLINFO);
      ti.hwnd = HWnd;
	   ti.uId  = (UINT)HWndColors[MAXCOLOR];
      ti.uFlags = TTF_IDISHWND;
      ti.hinst = 0;
      ti.lpszText = LPSTR_TEXTCALLBACK;
	   SendMessage(features->hWndToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
      }
}

void PALETTE::DestroyToolPalette(void)
{
	UnhookWindowsHookEx(hhk);
   hhk = NULL;
   DestroyWindow(features->hWndToolTip);
   DestroyWindow(HWndPen);
   DestroyWindow(HWndRect);
   DestroyWindow(HWndEyeDropper);
   DestroyWindow(HWndFilled);
   DestroyWindow(HWndDigit);
   DestroyWindow(HWndLetter);
   DestroyWindow(hWndPalette);
   features->Global->PanelWidth = 0;
   features->hWndToolTip = hWndPalette = NULL;
}

void PALETTE::DestroyColorPalette(void)
{
   if(!view) return;
   view = false;
   TOOLINFO ti;
   for(int i=0; i<MAXCOLOR+1; i++)
		{
      ti.hwnd = HWnd;
      ti.uId  = (UINT)HWndColors[i];
      ti.cbSize = sizeof(TOOLINFO);
      ti.uFlags = TTF_IDISHWND;
      SendMessage(features->hWndToolTip, TTM_DELTOOL, 0, (LPARAM)&ti);
      DestroyWindow(HWndColors[i]);
      }
}

void PALETTE::PaintColorPal(DRAWITEMSTRUCT *dis)
{
	int numbcolor;
   if(!view) return;
   RECT clr;
   COLORREF color;
   GetClientRect(dis->hwndItem,&clr);
   for(numbcolor = 0; numbcolor<=MAXCOLOR; numbcolor++)
   	if(HWndColors[numbcolor] == dis->hwndItem) break;
	if(numbcolor!=MAXCOLOR) color = features->ColorTable[numbcolor+FIRST];
	else							color = features->ColorTable[features->CurrentColor];
   if(color == VOIDCOLOR)	color = GetSysColor(COLOR_MENU);
  	DeleteObject(SelectObject(dis->hDC,CreateSolidBrush(color)));
 	PatBlt(dis->hDC, clr.left, clr.top, clr.right, clr.bottom, PATCOPY);
	ReleaseDC(dis->hwndItem,dis->hDC);
	return;
}

void PALETTE::SetCurColor (HWND hwndcolor)
{
	byte numbcolor;
   if(!view) return;
   for(numbcolor = 0; numbcolor<=MAXCOLOR; numbcolor++)
   	if(HWND(LOWORD(HWndColors[numbcolor])) == hwndcolor) break;
	if(numbcolor == MAXCOLOR ||
   							features->ColorTable[numbcolor+FIRST] == VOIDCOLOR) return;
	features->CurrentColor = (byte)(numbcolor+FIRST);
   RedrawWindow(HWndColors[MAXCOLOR],NULL,NULL,RDW_INVALIDATE);
}

void PALETTE::UpdatePalette()
{
	HDC hDC;
	if(!view && (features->Colors > 1)){
      CreateColorPalette();
      }
   else if(view && (features->Colors <= 1)){
      DestroyColorPalette();
      }
   else if(view) for(int i=0; i<=MAXCOLOR; i++)
      {
      if(i!=MAXCOLOR)
			{
         hDC = GetDC(HWndColors[i]);
      	DeleteObject(SelectObject(hDC,CreateSolidBrush(features->ColorTable[i+FIRST])));
         ReleaseDC (HWndColors[i], hDC);
         }
   	RedrawWindow(HWndColors[i],NULL,NULL,RDW_INVALIDATE);
      }
}

void PALETTE::TuningColor (HWND hwnd)
{
   if(features->GetExtension()==JCR_FILE) return;
	CHOOSECOLOR lpcc;
   COLORREF ClrTbl[MAXCOLOR];
	int numbcolor;
   if(!view) return;
   for(numbcolor = 0; numbcolor<=MAXCOLOR; numbcolor++)
   	if((HWND)LOWORD(HWndColors[numbcolor]) == hwnd) break;
//   if(numbcolor>9) return;
	if(numbcolor == MAXCOLOR || //!numbcolor ||
   							features->ColorTable[numbcolor+FIRST] == VOIDCOLOR) return;
   for(int i=0; i<MAXCOLOR; i++) ClrTbl[i] = VOIDCOLOR;
   lpcc.lStructSize  = (DWORD)sizeof(CHOOSECOLOR);
   lpcc.hwndOwner    = features->Global->hWnd;
   lpcc.hInstance    = NULL;//(HWND)GlobalsParameter.hInstance;
   lpcc.rgbResult    = features->ColorTable[numbcolor+FIRST];
   lpcc.lpCustColors = (COLORREF *)ClrTbl;
   lpcc.Flags        = CC_FULLOPEN | CC_RGBINIT;
   lpcc.lCustData    = NULL;
   lpcc.lpfnHook     = NULL;
   lpcc.lpTemplateName=NULL;
	if(ChooseColor(&lpcc))
   	{
      if((bool)(features->ColorTable[numbcolor+FIRST]!=lpcc.rgbResult)){
         PrepareUndo();
			ChangesInFileA();
      	features->ColorTable[numbcolor+FIRST] = lpcc.rgbResult;
			ScreenInitialize();
      	InvalidateRect(HWnd, NULL, false);
			}
      }
}

bool CALLBACK PALETTE::LetterDialog
			(HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	return This->LetterDialog_CallBack
   				(hwndDlg,Msg,wParam,lParam);
}

bool PALETTE::LetterDialog_CallBack (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
   static HWND hOldFocusWnd;
   static char tmp[20];
   HWND hList;
   WPARAM Limit;
   int x,y,z;
//   bool volatile pass;
   switch(Msg)
      {
      case WM_CREATE:
      case WM_INITDIALOG:
         if(ExecuteDialog) return 0;
         ExecuteDialog=true;
         Letter = new char[MAX_PATH];
         Column = new int [MAX_PATH];
         Line 	 = new int [MAX_PATH];
         memset(Letter,0,MAX_PATH*sizeof(char));
			for(Limit=0; Limit<MAX_PATH; Limit++){
            Column[Limit]=(Limit%features->hSize)+1;
            Line  [Limit]=(Limit/features->hSize)+1;
            }
         if(!features->LetterMap) {
            Limit=features->hSize*features->vSize;
         	features->LetterMap = new byte[Limit];
            memset(features->LetterMap, 0, Limit);
            }
         if(features->StartLetter){
         	features->StartLetter->GetData(Letter, Column, Line);
         	Letter[features->StartLetter->GetLen()]='\0';
            }
         else *Letter=' ';
         SetDlgItemText(hwndDlg,IDC_LETTER_TEXT,Letter);

//         pass=true;
         Limit=features->hSize;
         if 	(Limit/100) 	Limit=3;
         else 	if(Limit/10)	Limit=2;
         else						Limit=1;
         hList = GetDlgItem(hwndDlg, IDC_COLUMN);
         SendMessage(hList,EM_SETLIMITTEXT,(WPARAM)Limit,0);

         hList = GetDlgItem(hwndDlg,IDC_UPDOWN_COLUMN); // диапазон колонок
         SendMessage(hList,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)features->hSize, (short)1));

         Limit=features->vSize;
         if 	(Limit/100) 	Limit=3;
         else 	if(Limit/10)	Limit=2;
         else						Limit=1;
         hList = GetDlgItem(hwndDlg, IDC_LINE);               // Set Limits
         SendMessage(hList,EM_SETLIMITTEXT,(WPARAM)Limit,0);
//         pass=false;

         hList = GetDlgItem(hwndDlg,IDC_UPDOWN_LINE); // диапазон строк
         SendMessage(hList,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)1, (short)features->vSize));

         int tabs[2];
         tabs[0]=23; tabs[1]=40;
         hList=GetDlgItem(hwndDlg, IDC_LETTER_LIST);
         SendMessage(hList,LB_SETTABSTOPS,2,(LPARAM)&tabs);

         hOldFocusWnd=SetFocus(GetDlgItem(hwndDlg,IDC_LETTER_LIST));
         break;


      case WM_COMMAND:
         if(!ExecuteDialog) return 0;
			Limit = (WPARAM) HIWORD(wParam); // identifier of edit control
			hList = (HWND) lParam;      // handle of edit control
         switch(LOWORD(wParam))
            {
            case IDC_LETTER_LIST:
            	if(Limit==LBN_SELCHANGE){ // Изменился выбранный пункт ListBox
                  Limit=SendMessage(hList,LB_GETCURSEL,0,0);
//                  pass=true;
                  SetDlgItemInt(hwndDlg,IDC_COLUMN,Column[Limit],false);
                  SetDlgItemInt(hwndDlg,IDC_LINE  ,Line  [Limit],false);
//			         pass=false;
               	}
               break;

            case IDC_LETTER_TEXT:                  // Изменился текст EditBox
					if(!Column || !Line) break;
					if(Limit==EN_CHANGE){
                  hList=GetDlgItem(hwndDlg, IDC_LETTER_LIST);
                  SendMessage(hList, LB_RESETCONTENT, 0, 0);
                  GetDlgItemText(hwndDlg, IDC_LETTER_TEXT, Letter, MAX_PATH);
                  if(!*Letter) strcpy(Letter," ");
         			Limit=0;
         			while(Letter[Limit]){
            			memset(tmp,0,20);
         				tmp[0]=Letter[Limit];
            			tmp[1]=0x9;
            			itoa(Column[Limit], tmp+2, 10);
            			tmp[strlen(tmp)]=0x9;
            			itoa(Line[Limit++], tmp+strlen(tmp), 10);
							SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)tmp);
   						}
         			SendMessage(hList,LB_SETCURSEL, strlen(Letter)-1, 0);
//                  pass=true;
                  SetDlgItemInt(hwndDlg,IDC_COLUMN,Column[Limit-1],false);
                  SetDlgItemInt(hwndDlg,IDC_LINE  ,Line  [Limit-1],false);
//                  pass=false;
                  UpdateMainWindow();
						}
               break;

            case IDC_COLUMN:
            case IDC_LINE:
					if(/*pass || */!Column || !Line) break;
               x=(int)GetDlgItemInt(hwndDlg, IDC_COLUMN, NULL, false);
               y=(int)GetDlgItemInt(hwndDlg, IDC_LINE,   NULL, false);
               hList=GetDlgItem(hwndDlg, IDC_LETTER_LIST);
               z=SendMessage(hList,LB_GETCURSEL,0,0);
               if(Column[z]==x && Line[z]==y) break;
               if(Limit==EN_CHANGE){
            		hList=GetDlgItem(hwndDlg, IDC_LETTER_LIST);
               	Limit=SendMessage(hList,LB_GETCURSEL,0,0);
                  if((bool)x && (bool)y && x<=features->hSize && y<=features->vSize){
                     int saveX, saveY, i;
                     saveX=Column[Limit]; saveY=Line[Limit];
                  	if(LOWORD(wParam)==IDC_COLUMN)
                     	Column[Limit]=x;
                     else
                     	Line[Limit]=y;
         				i=Limit;
           				memset(tmp,0,20);
        					tmp[0]=Letter[i];
           				tmp[1]=0x9;
           				itoa(Column[i], tmp+2, 10);
           				tmp[strlen(tmp)]=0x9;
           				itoa(Line[i++], tmp+strlen(tmp), 10);
                     SendMessage(hList, LB_DELETESTRING, Limit, 0);
                     SendMessage(hList, LB_INSERTSTRING, Limit, (LPARAM)tmp);
                     SendMessage(hList,LB_SETCURSEL, Limit, 0);

                     if(saveX!=x || saveY!=y){
               			UpdateMainWindow();
                        }
                     }
                  }
            	break;

            case IDOK:
					if(features->StartLetter){delete features->StartLetter; features->StartLetter=NULL;}
               Limit=0;
					memset(features->LetterMap, 0, features->hSize*features->vSize);
					while(Letter[Limit]==' ') Limit++;
               if(strlen(Letter+Limit)){
						while(Letter[Limit]){
                     if(features->StartLetter)
                     		features->StartLetter->AddLetter((byte)Column[Limit], (byte)Line[Limit], Letter[Limit]);
                     else	features->StartLetter = new Char_In_Matrix((byte)Column[Limit], (byte)Line[Limit], Letter[Limit]);
                     Limit++;
                     }
               	features->StartLetter->UpdateMatrix(features->LetterMap, features->hSize, features->vSize);
                  }
               ChangesInFileA();
//            	InvalidateRect(HWnd, NULL, false);
            case IDCANCEL:
         		delete[] Letter; Letter=NULL;
         		delete[] Column; Column=NULL;
         		delete[] Line; Line=NULL;
            	ExecuteDialog=false;
               DestroyWindow(hwndDlg);
            	return 0;
            }
      }
	return FALSE;
}

void PALETTE::UpdateMainWindow()
{
   byte *TempMap,*b;
   int i, Limit;
   HDC hdc;
   POINT OldViewPort, *MapPort;
   hdc = GetDC(HWnd);
   MapPort = GetMapViewPort();
   SetViewportOrgEx(hdc, MapPort->x, MapPort->y, &OldViewPort);
   Limit=features->hSize*features->vSize;
   TempMap = new byte[Limit];
   memset(TempMap, 0, Limit);
   for(i=0; i<(int)strlen(Letter); i++)
   	TempMap[(Line[i]-1)*features->hSize+(Column[i]-1)]=Letter[i];
   b=TempMap; TempMap=features->LetterMap; features->LetterMap=b;
	i=-1;
	while(++i<(int)Limit)
		if(features->LetterMap[i]!=TempMap[i])
   		DrawCell(hdc, i%features->hSize, i/features->hSize);
	SetViewportOrgEx(hdc, OldViewPort.x, OldViewPort.y, &OldViewPort);
	ReleaseDC(HWnd,hdc);
	delete TempMap;
}

