// WinMain.cpp - описание членов класса главного окна
#include "winmain.h"
#include "bitmaps.h"
#include <stdlib.h>
#include <time.h>

//#define _RANDOMDEMO_

WINMAIN *WINMAIN::This;

WINMAIN::WINMAIN (HINSTANCE hInstance, LPSTR lpCmLn, int CmdShow)
:  DIALOGS(KROSSFEATURES::This)
{
   FolderPath = new char[MAX_PATH]; // Иниц. членов Демо-режима 
   *FolderPath=0;
   hDemoFile = INVALID_HANDLE_VALUE;

   HWND hPrewWnd=FindWindow(CLASS_NAME, NULL);
   if(!GLOBAL::MultyInstance && (bool)hPrewWnd) {
      if(IsIconic(hPrewWnd))
   		ShowWindow(hPrewWnd, SW_RESTORE);
      SetForegroundWindow(hPrewWnd);
      SendMessage(hPrewWnd, WM_COMMAND, (WPARAM)IDM_EXIT, NULL);
//      InvalidateRect(hPrewWnd, NULL, true);
//      ExitProcess(0);
   	}

   hinst = hInstance;
   This = this;
  	WindowToolHeight = NULL;
   hSize = vSize = 20;  //Size kross table
   hNumbSize = vNumbSize = 6; //Size table numbers
   Colors = 0;               //Numbers of colors

   startk = CreateKross = init = false;

   RButton = LButton = false;
   WaitForNextDemo = 0;

   if (!RegisterClass()) return;
   Create(CmdShow);
   if(!hWnd) return;
   PALETTE::SetFeatures(KROSSFEATURES::This);
  	hWndToolTip = NULL;
  	DoCreateTooltip();

// проверка командной строки
	SetAppData();	// Установка переменных в классе ввода-вывода
	bool CmLn = ((bool)(strlen(lpCmLn)) & (*lpCmLn=='"'));
	if(CmLn || (bool)(*LastFile) || Demo) // Check file path in Command Line
		{
		if(CmLn)
			{
			if(!LastFile)
				LastFile = new char[FILEPATHNAME];
			strcpy(LastFile,lpCmLn+1);
			*(LastFile+strlen(LastFile)-1)=0;
			GetFileTitle(LastFile,FileTitle,FILETITLE);
         Read_from_File(false);
			}
		else if(Demo) // Если включен режим демо
			StartDemo();
		else Read_from_File(false);
      CreateKross = true;
		}

   Menu = GetMenu(hWnd);

	if(GLOBAL::ToolBar) hToolWnd = CreateToolbar (SW_SHOWDEFAULT);

   SetMenuItem(hWnd);
   WINMAIN::StatusBox=NULL;
   Id[0] = 0; Id[1] = 0;//IDM_ABOUT;
   EnableMenuItem(Menu,IDM_SAVE ,MF_GRAYED);
   return;
}

void WINMAIN::MessageProc()
{
   init = true;
	while (GetMessage(&Msg,NULL,0,0))
		  {
		  if(!TranslateAccelerator(hWnd,hAccel,&Msg))
				 {
				 TranslateMessage(&Msg);
				 DispatchMessage(&Msg);
				 }
		  }
	UnregisterClass (CLASS_NAME, hinst);
   delete[] FolderPath;
}

LRESULT WINMAIN::OpenDemoFolder()
{
   if (hDemoFile!=INVALID_HANDLE_VALUE){
		FindClose(hDemoFile);
      hDemoFile=INVALID_HANDLE_VALUE;
      }
   FindClose(hDemoFile);
   hDemoFile=INVALID_HANDLE_VALUE;
	BrowsFolders (FolderPath);
   if(*FolderPath) StartDemo();
   return NULL;
}

bool WINMAIN::StartDemo()
{
   if(!*FolderPath)
      PathToFolder(strcpy(FolderPath, LastFile));
  	strcat(FolderPath, "\\*.*");
	ExecDemo = true;
	return NextDemo();
}

void WINMAIN::EndDemo()
{
	ExecDemo = false;
//	FindClose(hDemoFile);
//   hDemoFile=INVALID_HANDLE_VALUE;
}

bool WINMAIN::NextDemo (void)
{
	if(!ExecDemo) return false;
   WaitForNextDemo = DEMOWAIT;
   EnableMenuItem(GetMenu(hWnd),IDM_SAVE_AS   ,MF_ENABLED);
   EnableMenuItem(GetMenu(hWnd),IDM_FEATURES  ,MF_ENABLED);

   WIN32_FIND_DATA wfd;
	TCHAR exp[][6] = {{"jcr"},{"pp\0"},{"pps"},{"ppc"},{"ico"},{"jcc"}};
   bool res, end=false;

   do{
   	res=true;
   	if(hDemoFile==INVALID_HANDLE_VALUE){
   		hDemoFile 	= FindFirstFile(FolderPath, &wfd);
         end=true;
         }
   	else res 		= FindNextFile(hDemoFile, &wfd);
      if(!res)
      	if(GetLastError() == ERROR_NO_MORE_FILES && !end){ //Конец папки
      		FindClose(hDemoFile);
            strcat(PathToFolder(FolderPath),"\\*.*");
            hDemoFile 	= FindFirstFile(FolderPath, &wfd);
         	res = true;
            }
   	if(!res || hDemoFile==INVALID_HANDLE_VALUE) {
   		EndDemo();
	      MessageBox(hWnd,"В выбранной папке файлы с кроссвордами отсутствуют.", "Matrix error.", MB_OK|MB_ICONSTOP);
   	   return false;
      	}
      char *ptr=wfd.cFileName+strlen(wfd.cFileName)-1;
      while(*ptr!='.') ptr--;
      ptr++;
		res = false;
      for(int i=0; i<6; i++)
      	res |= ((exp[i][0]==ptr[0])&&(exp[i][1]==ptr[1])&&(exp[i][2]==ptr[2]));
   	} while(!res);

   strcat(strcat(PathToFolder(FolderPath),"\\"),wfd.cFileName);
	SelectNewFileName(FolderPath);
   if(Read_from_File(false) && (bool)GetDataLen())
   	if(init) CreateKrossword();
   	else CreateKross = true;
   else InvalidateRect(hWnd,NULL,false);
   return true;
}

void WINMAIN::Create  ( int CmdShow )
{

   hAccel = GLOBAL::hAccel = LoadAccelerators(hinst,MAKEINTRESOURCE(IDC_ACCELERATORS));
   hWnd = GLOBAL::hWnd = CreateWindowEx (
         WS_EX_WINDOWEDGE,
         CLASS_NAME,
         CAPTION_TEXT,
         WS_OVERLAPPED|WS_THICKFRAME|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN,
         WindowPosX,
         WindowPosY,
         WindowSizeX,
         WindowSizeY,
         NULL,
         NULL,
         hinst,
         (LPSTR)NULL);
   if (hWnd){
   	ShowWindow(hWnd, CmdShow);
      }
}

bool WINMAIN::RegisterClass (void)
{
   WNDCLASSEX WC;

   WC.cbSize = (UINT)sizeof(WC);
   WC.style = CS_HREDRAW|CS_VREDRAW;
   WC.lpfnWndProc = WINMAIN::WndProc;
   WC.cbClsExtra = 0;
   WC.cbWndExtra = 0;
   WC.hInstance = hinst;
   WC.hIcon = LoadIcon (hinst,MAKEINTRESOURCE (IDI_ICON1));
   WC.hIconSm = NULL;
   WC.hCursor = LoadCursor (NULL, IDC_ARROW);
   WC.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
   WC.lpszMenuName = MAKEINTRESOURCE (IDM_MENU1);
   WC.lpszClassName = CLASS_NAME;
   return (bool)::RegisterClassEx(&WC);
}

LRESULT WINMAIN::Close (void)
{
   if(!Save()) return 1;
   DestroyWindow (hWnd);       // посылает сообщение WM_DESTROY
   return 0;
}

LRESULT CALLBACK WINMAIN::WndProc(HWND HWnd,UINT Msg,WPARAM WParam,LPARAM LParam)
{
	return This->WProc(HWnd,Msg,WParam,LParam);
}

LRESULT WINMAIN::WProc(HWND HWnd,UINT Msg,WPARAM WParam,LPARAM LParam)
{
   char *name;
	TOOLTIPTEXT *ttt;
   WINDOWPLACEMENT wndpl;
   if(Msg == WM_CREATE)
   	{
      Timer = SetTimer(HWnd, 1, 1, NULL);
      return 0;
      }
   if(!init) return DefWindowProc (HWnd, Msg, WParam, LParam);
   else if(!startk){
   	startk = true;
      if(CreateKross)
      	CreateKrossword();
      }
   switch (Msg) {
/*        case WM_CREATE:
           Timer = SetTimer(HWnd, 1, 1, NULL);
           return 0;
*/
        case WM_ERASEBKGND:
           if(HWnd == hWnd) return 0;
           return DefWindowProc (HWnd, Msg, WParam, LParam);

			case WM_INITMENUPOPUP:
    		  CInitMenu((HMENU) WParam);
    		  return DefWindowProc (HWnd, Msg, WParam, LParam);

        case WM_MENUSELECT:
        	  if(StatusBox)
           	  MenuHelp(Msg, WParam, LParam, Menu, hinst, hStatusWnd, Id);
           return 0;

        case WM_COMMAND:           // обработка сообщений меню

           if(	LOWORD(WParam) != IDC_ENDCALC &&
           		LOWORD(WParam) != IDC_WORKNOLOGIC &&
            	LOWORD(WParam) != IDM_MAX &&
            	LOWORD(WParam) != IDM_MIN &&
            	LOWORD(WParam) != IDM_NORM &&
            	LOWORD(WParam) != IDM_VIEWWEB &&
            	LOWORD(WParam) != IDM_WEB5x5 &&
            	LOWORD(WParam) != IDM_STATUSBOX){
               	if(LOWORD(WParam) != IDM_START_DEMO) ExecDemo = false;
						SendMessage(PALETTE::hWndDialog, WM_COMMAND, IDCANCEL, 0);
                  }
           switch(LOWORD(WParam))  // обработка команд
              {
              case IDM_ABOUT:
                 DialogBox(hinst,MAKEINTRESOURCE(IDD_DIALOG1),
                                 HWnd,(DLGPROC) DialogProc_info);
                 return 0;

              case IDM_NEW:
                 FindClose(hDemoFile);
					  hDemoFile=INVALID_HANDLE_VALUE;

                 if(!FeaturesDialog(false)) return 0;
                 if(!Save()) return 0;
                 strcpy(FileTitle, NEW_FILE_NAME);
					  SelectNewFileName(FileTitle);
   				  EnableMenuItem(Menu,IDM_SAVE ,MF_ENABLED);
   				  EnableMenuItem(Menu,IDM_SAVE_AS ,MF_ENABLED);
                 CreateKrossword();
                 return 0;

              case IDM_OPEN:
                 FindClose(hDemoFile);
					  hDemoFile=INVALID_HANDLE_VALUE;

					  name = new char[FILEPATHNAME];
					  if(!GetOpenFileName(name))
                 	  {
                    delete[] name;
                    return 0;
                    }
                 if(!Save())
                    {
                    delete[] name;
                    return 0;
                    }
                 SelectNewFileName(name);
                 delete[] name;
                 if(Read_from_File(false) && (bool)GetDataLen())
                    CreateKrossword();
                 else InvalidateRect(HWnd,NULL,false);
                 return 0;

              case IDM_START_DEMO:
                 *FolderPath=0;
					  if(!ExecDemo) 	StartDemo();
					  else 				EndDemo();
                 return 0;

              case IDM_OPEN_FOLDER:
              	  return OpenDemoFolder();

              case IDM_EXIT:
                 return Close();

              case IDC_ENDCALC:	// окончание расчета
                 if(ExecDemo) WaitForNextDemo = DEMOWAIT;
                 EndCalc((bool) LParam);
              	  return 0;

				  case IDM_CPASTE:
					  PasteFromClipboard();
        			  return 0;

		        case IDM_CCOPY:
      		     CopyToClipboard();
		           return 0;

              case IDM_TOOLBAR:
					  if(GLOBAL::ToolBar) DestroyToolBar();
					  else
                 	{
                  hToolWnd = CreateToolbar (SW_SHOWDEFAULT);
                  InvalidateRect(HWnd, NULL, true);
                  }
                 WindowSizing();
					  return 0;

      			case IDM_AUTOCLEAR:
      			case IDM_VIEWPICTURES:
      			case IDM_VIEWWEB:
      			case IDM_WEB5x5:
      			case IDM_VIEWLOG:
      			case IDM_DEMO:
               case IDM_DIAGONALPOTTING:
               case IDM_MULTYINSTANCE:
               	GLOBAL::CommandProcessing (LOWORD(WParam));
                 	return 0;

					case IDM_FEATURES:
               	PrepareUndo();
               	if(FeaturesDialog(true)) CommandProcessing(Msg,WParam,LParam);
                  else Undo_(true);
                  return 0;

              	case BN_CLICKED:
              		if(PaletteButtonClicked((int) LOWORD(WParam), (HWND) LParam))
                  	return 0;
						if(HIWORD(WParam)==STN_CLICKED)
      	     			{
         	  			SetCurColor((HWND) LOWORD(LParam));
            	   	return 0;
               		}
               	else if(HIWORD(WParam)==STN_DBLCLK)
               			TuningColor((HWND) LOWORD(LParam));
                  return 0;//DefWindowProc (HWnd, Msg, WParam, LParam);

              	default:
         	  	  CommandProcessing(Msg,WParam,LParam);
                 return 0;
				  }

        case WM_MBUTTONDOWN:
        case WM_MOUSEWHEEL:
           CommandProcessing(Msg, WParam, LParam);
           return 0;

        case WM_LBUTTONDOWN:
			  CommandProcessing(Msg,WParam,LParam);
           LButton=true;
           Count=Speed=SPEEDCLICKS;
           StartX=x=(int)LOWORD(LParam);
           StartY=y=(int)HIWORD(LParam);
           MouseFlag=(WORD)WParam;
           return 0;

		  case WM_RBUTTONDOWN:
			  CommandProcessing(Msg,WParam,LParam);
           RButton=true;
			  Count=Speed=SPEEDCLICKS;
           StartX=x=(int)LOWORD(LParam);
           StartY=y=(int)HIWORD(LParam);
           MouseFlag=(WORD)WParam;
           return 0;

        case WM_RBUTTONUP:
           RButton=false;
        case WM_MBUTTONUP:
           CommandProcessing(Msg,(WPARAM)StartX,(LPARAM)StartY);
           return 0;

        case WM_LBUTTONUP:
           LButton=false;
           CommandProcessing(Msg,(WPARAM)StartX,(LPARAM)StartY);
           return 0;

        case WM_MOUSEMOVE:
           CommandProcessing(Msg,WParam,LParam);
           RButton = WParam != MK_RBUTTON ? false : true;
           LButton = WParam != MK_LBUTTON ? false : true;
           x=(int)LOWORD(LParam);
			  y=(int)HIWORD(LParam);
           return 0;

        case WM_TIMER:
           if(!init) return 0;
           if(WaitForNextDemo == 1) NextDemo();
           if(WaitForNextDemo) WaitForNextDemo--;

           CommandProcessing(Msg,WParam,LParam);
/*           if(!LButton && !RButton) return 0;
           if(Count--) return 0;
           if(Speed>MAXSPEED) Speed-=DELTASPEED;
           Count=Speed;
           if(LButton){
           	  CommandProcessing(WM_LBUTTONDOWN, MouseFlag, MAKELONG(x, y));
              }
           else {
           	  CommandProcessing(WM_RBUTTONDOWN, MouseFlag, MAKELONG(x, y));
              }*/
			  return 0;

        case WM_SIZE:

     		  if(GLOBAL::StatusBox) StatusBox->Sizing(WParam, LParam);
        	  SendMessage(hToolWnd,TB_AUTOSIZE,0,0);
			  RECT rc;
  			  GetWindowRect(hToolWnd, &rc);
  			  if (GLOBAL::ToolBar) WindowToolHeight = rc.bottom - rc.top;
        case WM_KEYDOWN:
        case WM_HSCROLL:
        case WM_VSCROLL:
        case WM_DRAWITEM:
           CommandProcessing(Msg,WParam,LParam);
			  return 0;

         case WM_PAINT:
           if(HWnd != hWnd) return DefWindowProc (HWnd, Msg, WParam, LParam);
           else CommandProcessing(Msg,WParam,LParam);
			  return 0;

         case WM_CLOSE:             // завершение программы с подтверждением
           wndpl.length = sizeof(WINDOWPLACEMENT);
			  GetWindowPlacement(HWnd,&wndpl);
           WindowPosX=wndpl.rcNormalPosition.left;
           WindowPosY=wndpl.rcNormalPosition.top;
           WindowSizeX=wndpl.rcNormalPosition.right-wndpl.rcNormalPosition.left;
           WindowSizeY=wndpl.rcNormalPosition.bottom-wndpl.rcNormalPosition.top;
           return Close();

         case WM_NOTIFY:
           if(((LPNMHDR)LParam)->hwndFrom==hToolWnd)
           		return ToolBarNotify(LParam);
           if(((LPNMHDR)LParam)->hwndFrom==hWndToolTip)
           	{
            if(((LPNMHDR)LParam)->code == TTN_NEEDTEXT)
               {
            	ttt = (LPTOOLTIPTEXT) LParam;
            	if(IsChild(hWndPalette, (HWND)ttt->hdr.idFrom))
                  return PalNeedText(ttt);
			  		((LPTOOLTIPTEXT)LParam)->lpszText = (char *)WParam;
           			((LPTOOLTIPTEXT)LParam)->hinst = hinst;
               return 0;
               }
            }
           return DefWindowProc (HWnd, Msg, WParam, LParam);

         case WM_DESTROY:
           if(HWnd != hWnd) return DefWindowProc (HWnd, Msg, WParam, LParam);
           KillTimer (HWnd,Timer);// уничтожает таймер
           PostQuitMessage (0);    // посылает сообщ. WM_QUIT и заставляет
                                   // функ. GetMessage вернуть ноль
           return 0;

		default:
		  return DefWindowProc (HWnd, Msg, WParam, LParam);
		}
}

LRESULT WINMAIN::ToolBarNotify(LPARAM arg)
{
	static char ButtonName[][20] ={"Новый...","Откыть","Сохранить","Сохранить как...",
  					"","Печать...","","Отмена","Повтор","","Увеличить","Уменьшить",
               "Во весь экран","Шрифт","","Очистить картинку","Расчет рисунка",
               "Расчет оцифровки","","Показать сетку", "Показать сетку 5x5"};
   LPTOOLTIPTEXT ttt = (LPTOOLTIPTEXT) arg;
   if(ttt->hdr.code == TTN_NEEDTEXT)
      {
      strcpy(ttt->szText,"нужен текст");
		ttt->lpszText = ttt->szText;
      ttt->hinst = NULL;
      return 0;
      }
	LPNMHDR nmhdr = (LPNMHDR) arg;
	TBNOTIFY *tbn  = (TBNOTIFY FAR *) arg;
	TBSAVEPARAMS tbsp;
	switch(nmhdr->code)
   	{
		case TBN_GETBUTTONINFO:
         if(tbn->iItem<NUMBBUTTON)
         	{
            strcpy(tbn->pszText, ButtonName[tbn->iItem]);
            tbn->tbButton = button[tbn->iItem];
         	return 1;
            }
         return 0;
		case TBN_QUERYDELETE:
      	return 1;
		case TBN_QUERYINSERT:
      	return 1;
      case TBN_BEGINADJUST:
         tbsp.hkr = HKEY_LOCAL_MACHINE;
         tbsp.pszSubKey = SubKey;
         tbsp.pszValueName = "ToolBarState";
      	SendMessage(nmhdr->hwndFrom, TB_SAVERESTORE, (WPARAM) true, (LPARAM) &tbsp);
         return 1;
      case TBN_RESET:
   		tbsp.hkr = HKEY_LOCAL_MACHINE;
   		tbsp.pszSubKey = SubKey;
   		tbsp.pszValueName = "ToolBarState";
   		SendMessage(hToolWnd, TB_SAVERESTORE, (WPARAM) false, (LPARAM) &tbsp);
      return 1;
		}
   return 0;
}

// Панель инструментов
TBBUTTON WINMAIN::button[NUMBBUTTON];

HWND WINMAIN::CreateToolbar (int CmdShow, bool size)
{
#define TOOLBARMAIN 100

// Кнопки панели инструментов
    int i=0, b, c=0;
    RECT rc;
    button[i++].idCommand = IDM_NEW;		  // Новый...
    button[i++].idCommand = IDM_OPEN;       // Откыть
    button[i++].idCommand = IDM_SAVE;       // Сохранить
    button[i++].idCommand = IDM_SAVE_AS;    // Сохранить как...
//
    button[i++].idCommand = NULL;    			// -Сепаратор-
    button[i++].idCommand = IDM_PRINT;      // Печать...
//
    button[i++].idCommand = NULL;    			// -Сепаратор-
    button[i++].idCommand = IDM_UNDO;       // Отмена
    button[i++].idCommand = IDM_REDO;       // Повтор
//
    button[i++].idCommand = NULL;    		  // -Сепаратор-
    button[i++].idCommand = IDM_MAX;        // Увеличить
    button[i++].idCommand = IDM_MIN;        // Уменьшить
    button[i++].idCommand = IDM_NORM;       // Во весь экран
    button[i++].idCommand = IDM_CHOOSEFONT; // Шрифт
//
    button[i++].idCommand = NULL;    		  // -Сепаратор-
    button[i++].idCommand = IDM_CLEARFIELD; // Очистить картинку
    button[i++].idCommand = IDM_CALCULATE;  // Расчет рисунка
    button[i++].idCommand = IDM_CORRECTED;  // Расчет оцифровки
//
    button[i++].idCommand = NULL;    			// -Сепаратор-
    int webcheck = i;
    button[i++].idCommand = IDM_VIEWWEB; 		// Показать сетку
    button[i++].idCommand = IDM_WEB5x5; 		// Показать сетку 5x5

    for(b=0; b<i; b++){
      button[b].dwData    = 0;
    	if(button[b].idCommand)
      	{
      	button[b].fsStyle   = TBSTYLE_BUTTON;
      	button[b].fsState   = TBSTATE_ENABLED;//GetMenuItemState(button[b].idCommand) ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE;
    		button[b].iBitmap   = c;
     		button[b].iString   = c;
         c++;
         }
      else
      	{
      	button[b].fsStyle   = TBSTYLE_SEP;
      	button[b].fsState   = (BYTE)TB_INDETERMINATE;
     		button[b].iString   = NULL;
         }
      }
   button[webcheck].fsStyle = button[webcheck+1].fsStyle = TBSTYLE_CHECK;
   int wsize = size ? 20 : 16;
	HWND hToolWnd = CreateToolbarEx(hWnd, TBSTYLE_SEP | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE | TBSTYLE_ALTDRAG |WS_CHILD | CCS_ADJUSTABLE, TOOLBARMAIN,
   			NUMBBUTTON, hinst,
   			IDB_BITMAP_TOOL1, (const TBBUTTON *)&button, NUMBBUTTON,
            wsize, wsize, wsize, wsize, sizeof(TBBUTTON));
   if(hToolWnd) CheckMenuItem(Menu, IDM_TOOLBAR, MF_CHECKED);
   else {
    	CheckMenuItem(Menu, IDM_TOOLBAR, MF_UNCHECKED);
      WindowToolHeight = 0;
    	return 0;
      }

   if(hWndToolTip) DestroyWindow(hWndToolTip);
   hWndToolTip = (HWND)SendMessage(hToolWnd,TB_GETTOOLTIPS,0,0);
   DoCreateTooltip();

   SendMessage(hToolWnd,TB_AUTOSIZE,0,0);
   GetWindowRect(hToolWnd, &rc);
   WindowToolHeight = rc.bottom - rc.top;
   ShowWindow(hToolWnd,CmdShow);
   GLOBAL::ToolBar = true;
   return hToolWnd;
}

void WINMAIN::DestroyToolBar()
{
	DestroyWindow(hToolWnd);
	WindowToolHeight = NULL;
   GLOBAL::ToolBar = false;
  	CheckMenuItem(Menu, IDM_TOOLBAR, MF_UNCHECKED);
   WindowSizing();
   InvalidateRect(hWnd, NULL, true);
   hWndToolTip = NULL;
   DoCreateTooltip();
}

void WINMAIN::StatusCheck  (void)
{
   if(!GLOBAL::StatusBox)
     {
     CheckMenuItem(Menu, IDM_STATUSBOX, MF_CHECKED);
     WINMAIN::StatusBox = new STATUSWND(KROSSFEATURES::This);
     GLOBAL::StatusBox=true;
     StatusHight=StatusBox->GetStatusBoxHight();
     hStatusWnd = StatusBox->GetHandle();
     }
   else
     {
     CheckMenuItem(Menu, IDM_STATUSBOX, MF_UNCHECKED);
     if(StatusBox) delete StatusBox;
     WINMAIN::StatusBox=NULL;
     StatusHight=0;
     GLOBAL::StatusBox=false;
     }
   WindowSizing();
   InvalidateRect(hWnd,NULL,true);
}

STATUSWND::STATUSWND(KROSSFEATURES *feat)
{
  features = feat;
  hWnd = features->Global->hWnd;
  RECT Client;
  hStatus = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        STATUSCLASSNAME,         // name of status window class
        (LPCTSTR) NULL,          // no text when first created
        SBARS_SIZEGRIP |         // includes a sizing grip
        WS_CHILD|WS_VISIBLE|WS_OVERLAPPED|WS_TILED,     // creates a child window
        0, 0, 0, 0,              // ignores size and position
        hWnd,                    // handle to parent window
        (HMENU) ID_STATUSBOX,    // child window identifier
        features->Global->hinst,// handle to application instance
        NULL);
  GetClientRect(hWnd, &Client);
  int len = Client.right/(SB_TIME+2);
  int cur = 0;
  int *PartStart = new int[SB_TIME+1];
  for(int i=0; i<SB_TIME+1; i++) {
     if(i==SB_STATUSTEXT) cur+=len*2;
     else cur+=len;
     PartStart[i]=cur;
     }
  SendMessage(hStatus,SB_SETPARTS,(WPARAM)SB_TIME+1,(LPARAM)(LPINT)PartStart);
  delete[] PartStart;
  GetWindowRect(hStatus, &StatusRect);
  GetClientRect(hWnd,&Client);
  Hight = StatusRect.bottom-StatusRect.top-3;
}

void STATUSWND::Sizing(WPARAM wParam, LPARAM lParam)
{
  RECT Client;
  GetClientRect(hWnd, &Client);
  int len = Client.right/(SB_TIME+2);
  int cur = 0;
  int *PartStart = new int[SB_TIME+1];
  for(int i=0; i<SB_TIME+1; i++) {
     if(i==SB_STATUSTEXT) cur+=len*2;
     else cur+=len;
     PartStart[i]=cur;
     }
  SendMessage(hStatus,SB_SETPARTS,(WPARAM)SB_TIME+1,(LPARAM)(LPINT)PartStart);
  delete[] PartStart;
  SendMessage(hStatus,WM_SIZE, wParam, lParam);
  GetWindowRect(hStatus, &StatusRect);
  Hight = StatusRect.bottom-StatusRect.top-3;
}

void WINAPI WINMAIN::CInitMenu(HMENU hmenu)
{
	int  cMenuItems = GetMenuItemCount(hmenu);
   UINT fuFlags;

   for (int nPos = 0; nPos < cMenuItems; nPos++)
   	{
      UINT id = GetMenuItemID(hmenu, nPos);
      bool a;
      switch (id)
      	{
         case IDM_CCOPY:
         	if (Containers != EMPTY)
               {
            	a=false;
            	for(int i=0; i<hSize*vSize; i++) if(BitMap[i] != ZEROS) a=true;
               if(!a) fuFlags = MF_BYCOMMAND | MF_GRAYED;
               else   fuFlags = MF_BYCOMMAND | MF_ENABLED;
               }
            EnableMenuItem(hmenu, id, fuFlags);
            break;

			case IDM_CPASTE:
            a = IsClipboardFormatAvailable(CF_DIB);
//            a|= IsClipboardFormatAvailable(CF_BITMAP);
            EnableMenuItem(hmenu, id, a ?
                     	MF_BYCOMMAND | MF_ENABLED :
                        MF_BYCOMMAND | MF_GRAYED
               			);
        	}
    	}
}

void WINMAIN::PasteFromClipboard(void)
{
   if (!IsClipboardFormatAvailable(CF_DIB)) return;
	if (!OpenClipboard(hWnd)) return;
   HGLOBAL hglb = GetClipboardData(CF_DIB);
   byte *Sour = (LPBYTE)GlobalLock(hglb);

   BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)Sour;
   DWORD NumbColor = (WORD)(0xffff << bih->biBitCount);
   NumbColor ^= 0xffff;
   NumbColor++;
   DWORD ClrTblLen = (DWORD)(bih->biBitCount < 24 ? NumbColor*sizeof(RGBQUAD) : NULL);
   DWORD DibLen = bih->biSize+ClrTblLen+bih->biHeight*
                  ((bih->biWidth*bih->biBitCount+31)/32)*4;
   byte *Dist = new byte[sizeof(BITMAPFILEHEADER)+DibLen];
   BITMAPFILEHEADER *bfh = (BITMAPFILEHEADER *)Dist;
   bfh->bfType = 0x4d42;
   bfh->bfSize = DibLen+sizeof(BITMAPFILEHEADER);
   bfh->bfReserved1 = 0;
   bfh->bfReserved2 = 0;
   bfh->bfOffBits   = sizeof(BITMAPFILEHEADER)+bih->biSize+ClrTblLen;
   byte *d = Dist+sizeof(BITMAPFILEHEADER);
	for(DWORD i=0; i<DibLen; i++) *(d++) = Sour[i];
	GlobalUnlock(hglb);
	CloseClipboard();

//	SetData  (Dist, (int)(bfh->bfSize));
   PasteBitmap(Dist);
   delete[] Dist;
//	CreateKrossword();
}

bool WINMAIN::CopyToClipboard(void)
{
	if (!OpenClipboard(hWnd)) return false;
		EmptyClipboard();
   BITMAPS bm(KROSSFEATURES::This, NULL);
   if(!bm.FileInMemory) {CloseClipboard(); return false;}
   DWORD MemoryLen = (DWORD)(((BITMAPFILEHEADER *)bm.FileInMemory)->bfSize -
             sizeof(BITMAPFILEHEADER));
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_DDESHARE,MemoryLen);
   if (hglbCopy == NULL){CloseClipboard(); return false;}
   byte *SourData = bm.FileInMemory + sizeof(BITMAPFILEHEADER);
   byte *DistData = (LPBYTE)GlobalLock(hglbCopy);
	for(DWORD i=0; i<MemoryLen; i++) *(DistData++) = *(SourData++);
	GlobalUnlock(hglbCopy);
   SetClipboardData(CF_DIB, hglbCopy);
	CloseClipboard();
   return true;
}

bool WINMAIN::BrowsFolders (char *path)
{
	if(*path) PathToFolder(path);
	BROWSEINFO bi;
   bi.hwndOwner=NULL;//hWnd;
   bi.pidlRoot=NULL;
   bi.pszDisplayName=path;
   bi.lpszTitle="Выберите папку с кроссвордами.";
   bi.ulFlags=BIF_RETURNONLYFSDIRS	;//BIF_RETURNFSANCESTORS;
   bi.lpfn=NULL;
   bi.lParam=NULL;
   bi.iImage=NULL;
   ITEMIDLIST *pidl = SHBrowseForFolder((LPBROWSEINFO)&bi);
   return SHGetPathFromIDList(pidl, bi.pszDisplayName);
}

