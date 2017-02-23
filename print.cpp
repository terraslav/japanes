#include "print.h"

bool PRINTS::bPrint;
HWND PRINTS::hdlgCancel;
KROSSFEATURES *PRINTS::features;
PRINTS *PRINTS::This;

int  CALLBACK PRINTS::AbortProc(HDC , int )
{
	MSG msg;
// Извлеките и удалите сообщения из очереди сообщения нить.
   while (PeekMessage((LPMSG) &msg, (HWND) NULL, 0, 0, PM_REMOVE))
   	{
// Обработайте любые сообщения для диалогового меню Отмены.
      if (!IsDialogMessage(hdlgCancel, (LPMSG) &msg))
      	{
      	TranslateMessage((LPMSG) &msg);
         DispatchMessage((LPMSG) &msg);
         }
		}

//Return the global bPrint flag (which is set to FALSE
// если пользователь нажимает кнопку Отмены).
	return bPrint;
}

bool CALLBACK PRINTS::AbortPrintJob(HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch (Msg) {
   	case WM_INITDIALOG:
			// Проинициализируйте статическое текстовое управление.
         SetDlgItemText(hwndDlg, IDD_FILE, features->Global->FileTitle);
         return TRUE;

      case WM_COMMAND:
			// Пользователь нажал "Отмену" распечатки кнопки-останов работы.
      	MessageBox(hwndDlg, "Задание отменено.", "Печать.", MB_OK);
         bPrint = FALSE;
         return TRUE;

		default:
			break;
   	}
	UNREFERENCED_PARAMETER(lParam);
   UNREFERENCED_PARAMETER(wParam);
   UNREFERENCED_PARAMETER(Msg);
   return FALSE;
}

UINT APIENTRY PRINTS::PagePaintHook(HWND /*hdlg*/, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
   static BITMAPS *bmi;
   static RECT *Margin;
	switch(uiMsg){
   case WM_PSD_PAGESETUPDLG:
      bmi = (BITMAPS *)(((LPPAGESETUPDLG)lParam)->lCustData);
      Margin = &((LPPAGESETUPDLG)lParam)->rtMargin;
   	return 0;
   case WM_PSD_GREEKTEXTRECT:
      bmi->PaintSample((HDC) wParam, (LPRECT) lParam, Margin);
   	return 1;
   }
   return 0;
}

void PRINTS::FillPageSetupStructure(void *param)
{
	POINT c = {0,0};
   RECT d ={0,0,0,0};
   lppsd.lStructSize = sizeof(PAGESETUPDLG);
   lppsd.hwndOwner = hWnd;
   lppsd.hDevMode = NULL;  //HGLOBAL
   lppsd.hDevNames = NULL; //HGLOBAL
   lppsd.Flags = PSD_INHUNDREDTHSOFMILLIMETERS | PSD_ENABLEPAGEPAINTHOOK;// | PSD_DISABLEPRINTER
   lppsd.ptPaperSize = c; //POINT
   lppsd.rtMinMargin = d; //RECT
   lppsd.rtMargin = d;    //RECT
   lppsd.hInstance = hinst;
   lppsd.lCustData = (LPARAM) param; // LPARAM
   lppsd.lpfnPageSetupHook = NULL; //LPPAGESETUPHOOK
   lppsd.lpfnPagePaintHook = PagePaintHook; //LPPAGEPAINTHOOK
   lppsd.lpPageSetupTemplateName = NULL; //LPCTSTR

   lppsd.hPageSetupTemplate = NULL; //HGLOBAL
}

bool PRINTS::PageSetup(void *param)
{
//   if(!features->Global->GetSetupValue((LPBYTE)&lppsd, "PrinterPageSetup", sizeof(PAGESETUPDLG)))
 	FillPageSetupStructure(param);
	bool res = PageSetupDlg(&lppsd);
//   if(res) features->Global->SetSetupValue((LPBYTE)&lppsd,"PrinterPageSetup", sizeof(PAGESETUPDLG));
   return res;
}

PRINTS::PRINTS(KROSSFEATURES *feat, LOGFONT *plf)
{
   This = this;
   hWnd = feat->hWnd;
   hinst = feat->hinst;
   features = feat;
   BITMAPS bmi(feat, plf, true);
   if(!PageSetup((LPVOID)&bmi)) return;
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hDevMode = lppsd.hDevMode; //(HANDLE) NULL;
	pd.hDevNames = lppsd.hDevNames; //(HANDLE) NULL;
	pd.Flags = PD_RETURNDC|PD_NOPAGENUMS|PD_NOSELECTION;
	pd.hwndOwner = hWnd;
	pd.hDC = (HDC) NULL;
	pd.nFromPage = 1;
	pd.nToPage = 1;
	pd.nMinPage = 1;
	pd.nMaxPage = 1;
	pd.nCopies = 1;
	pd.hInstance = hinst;
	pd.lCustData = 0L;
	pd.lpfnPrintHook = (LPPRINTHOOKPROC) NULL;
	pd.lpfnSetupHook = (LPSETUPHOOKPROC) NULL;
	pd.lpPrintTemplateName = (LPSTR) NULL;

	pd.lpSetupTemplateName = (LPSTR)  NULL;
	pd.hPrintTemplate = (HANDLE) NULL;
	pd.hSetupTemplate = (HANDLE) NULL;

	if(!PrintDlg(&pd)) return;

// Изучите растровые возможности устройства идентифицирующего pd.hDC,
// чтобы проверить, как он поддерживает BitBlt функцию.

	if (!(GetDeviceCaps(pd.hDC, RASTERCAPS) & RC_BITBLT))
   	{
    	DeleteDC(pd.hDC);
    	MessageBox(hWnd, "Принтер не может отобразить рисунок.",
      	"Ошибка принтера.", MB_OK);
    	return;
		}

// Установите флаг использованный AbortPrintJob диалоговой процедурой.
	bPrint = TRUE;

// Зарегистрируйте прикладную AbortProc функцию с GDI.

	SetAbortProc(pd.hDC, AbortProc);

// Отобразите диалоговое меню независимой Отмены.
	hdlgCancel = CreateDialog(hinst, (LPTSTR) "AbortDlg",
   	hWnd, (DLGPROC) AbortPrintJob);

// Выведите Из Строя прикладное окно.
	EnableWindow(hWnd, FALSE);

// Извлеките количество пикселей-за-логический-дюйм в горизонтальных
// и вертикальных направлениях для дисплея на которых побитовое
// отображение было создано.
   HDC hDC = GetDC(hWnd);
	float fLogPelsX1 = (float) GetDeviceCaps(hDC, LOGPIXELSX);
   float fLogPelsY1 = (float) GetDeviceCaps(hDC, LOGPIXELSY);
   ReleaseDC(hWnd, hDC);

// Извлеките количество пикселей-за-логический-дюйм в горизонтальных
// и вертикальных направлениях для принтера на которых побитовое
// отображение будет напечатано.

   float fLogPelsX2 = (float) GetDeviceCaps(pd.hDC,
                     LOGPIXELSX);
   float fLogPelsY2 = (float) GetDeviceCaps(pd.hDC,
                     LOGPIXELSY);

// Определите масштабирование показателей требующееся, чтобы напечатать
// побитовое отображение и сохранить свои исходные пропорции.
   float fScaleX, fScaleY;
   if (fLogPelsX1 > fLogPelsX2)
   	fScaleX = (fLogPelsX1 / fLogPelsX2);
   else
   	fScaleX = (fLogPelsX2 / fLogPelsX1);

   if (fLogPelsY1 > fLogPelsY2)
   	fScaleY = (fLogPelsY1 / fLogPelsY2);
   else
   	fScaleY = (fLogPelsY2 / fLogPelsY1);

// Вычислите координату верхнего левого угла центрированного побитового
// отображения. inch = 2.54 см
	#define INCH_DEV 2.54
   POINT p = lppsd.ptPaperSize; //POINT
   RECT  r = lppsd.rtMargin;    //RECT
   int cWidthPels  = (int)((float)(p.x - r.left - r.right)/INCH_DEV*fLogPelsX2/1000);
   int cHeightPels = (int)((float)(p.y - r.top - r.bottom)/INCH_DEV*fLogPelsY2/1000);


// Создаем рисунок требуемого размера
   int CellSize = (int)MIN((cWidthPels/fScaleX)/(features->hSize+features->vNumbSize),(cHeightPels/fScaleX)/(features->vSize+features->hNumbSize));
   bmi.CreatePictures(CellSize);

   int hdcXsize = GetDeviceCaps(pd.hDC,HORZRES);
   int hdcYsize = GetDeviceCaps(pd.hDC,VERTRES);

   int xLeft = (hdcXsize/2)-((int)((float)bmi.bmp.bmWidth
                * fScaleX)/2);
   int yTop =  (hdcYsize/2)-((int)((float)bmi.bmp.bmHeight
                * fScaleY)/2);

// Проинициализируйте участников DOCINFO структуры.
   di.cbSize = sizeof(DOCINFO);
   di.lpszDocName = feat->Global->FileTitle;
   di.lpszOutput = (LPTSTR) NULL;
   di.lpszDatatype = (LPTSTR) NULL;
   di.fwType = 0;

// Начните работу распечатки вызывая StartDoc функцию.

	int nError = StartDoc(pd.hDC, &di);
   if (nError == SP_ERROR) {
   	errhandler("StartDoc", hWnd);
   	EnableWindow(hWnd, TRUE);
   	DestroyWindow(hdlgCancel);
		DeleteDC(pd.hDC);
   	return;
      }

// Сообщите драйверу, что приложение собирается начинать посылать данные.
   for(int i=0; i<pd.nCopies; i++)
   {
	nError = StartPage(pd.hDC);
   if (nError <= 0) {
   	errhandler("StartPage", hWnd);
   	EnableWindow(hWnd, TRUE);
   	DestroyWindow(hdlgCancel);
		DeleteDC(pd.hDC);
   	return;
      }

// Используйте StretchBlt функцию, чтобы масштабировать побитовое
// отображение и поддержать свои исходные пропорции (что, если побитовое
// отображение было квадрат, когда оно появлялось в прикладной области
// клиента, это должно также появиться квадрат на странице).
   if (!StretchBlt(pd.hDC, xLeft, yTop,
                   (int)((float)bmi.bmp.bmWidth *fScaleX),
                   (int)((float)bmi.bmp.bmHeight*fScaleY),
                   bmi.hMemDC, 0, 0,
                   bmi.bmp.bmWidth, bmi.bmp.bmHeight,
                   SRCCOPY))
		if(bPrint)errhandler("StretchBlt Failed", hWnd);

// Определите нажал ли пользователь кнопку Отмены в AbortPrintJob
// диалоговое меню; если кнопка нажата, назовите AbortDoc функцию.
// В противном случае, сообщите спулеру, что страница завершенна.

   if(!bPrint) break;

	nError = EndPage(pd.hDC);
	if (nError <= 0) {

   	if(bPrint) errhandler("EndPage", hWnd);
   	EnableWindow(hWnd, TRUE);
   	DestroyWindow(hdlgCancel);
		DeleteDC(pd.hDC);
   	return;
      }
   }
// Сообщите драйверу, что документ закончился.
   if(bPrint){
		nError = EndDoc(pd.hDC);
   	if (nError <= 0) errhandler("EndDoc", hWnd);
      }
   else AbortDoc(pd.hDC);
   EnableWindow(hWnd, TRUE);
   DestroyWindow(hdlgCancel);
	DeleteDC(pd.hDC);
}

