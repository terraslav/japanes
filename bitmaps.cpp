#include "bitmaps.h"
#include <commctrl.h>
BITMAPS *BITMAPS::BMPS;

BOOL CALLBACK BITMAPS::ImpDialogProc(HWND hwndDlg, UINT Msg, WPARAM wParam, LPARAM)
{
   int res;
	HWND hControl;
	switch (Msg){
   	case WM_INITDIALOG:
         CheckDlgButton(hwndDlg,IDC_IMPMARK,BST_CHECKED);
         CheckDlgButton(hwndDlg,IDC_IMPDIG,BST_CHECKED);
         CheckDlgButton(hwndDlg,IDC_IMPWEB,BST_CHECKED);
         CheckDlgButton(hwndDlg,IDC_IMPPICT,BST_UNCHECKED);
#ifdef DEMOVER
			EnableWindow(GetDlgItem(hwndDlg, IDC_RADIOBUTTON1), false);
         CheckRadioButton(hwndDlg,IDC_RADIOBUTTON1,IDC_RADIOBUTTON2,IDC_RADIOBUTTON2);

			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPDIG), true);
			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPMARK), true);
			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPPICT), true);
			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPWEB), true);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CELLSIZE), true);
         EnableWindow(GetDlgItem(hwndDlg, IDC_FONT), true);
			EnableWindow(GetDlgItem(hwndDlg, IDC_UPDOWN1), true);
#else
         CheckRadioButton(hwndDlg,IDC_RADIOBUTTON1,IDC_RADIOBUTTON2,IDC_RADIOBUTTON1);

			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPDIG), false);
			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPMARK), false);
			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPPICT), false);
			EnableWindow(GetDlgItem(hwndDlg, IDC_IMPWEB), false);
			EnableWindow(GetDlgItem(hwndDlg, IDC_CELLSIZE), false);
         EnableWindow(GetDlgItem(hwndDlg, IDC_FONT), false);
			EnableWindow(GetDlgItem(hwndDlg, IDC_UPDOWN1), false);
#endif
      	SetDlgItemInt(hwndDlg,IDC_CELLSIZE,15,false);
         hControl = GetDlgItem(hwndDlg,IDC_UPDOWN1);//
         SendMessage(hControl,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)100, (short)1));
      	return true;

      case WM_COMMAND:
      	switch(LOWORD(wParam)){
            case IDOK:
      			res = GetDlgItemInt(hwndDlg,IDC_CELLSIZE,NULL,false);
/*               {
               HDC hDC = GetDC(GetParent(hwndDlg));
               float LogPixelInch = GetDeviceCaps(hDC,LOGPIXELSX);
               ReleaseDC(GetParent(hwndDlg), hDC);
               res = (int)(float)((LogPixelInch / 2.5 * res) / 10);
               }*/
               res <<= 8;
               if(IsDlgButtonChecked(hwndDlg,IDC_RADIOBUTTON1)==BST_CHECKED)
               	{
                  res = 0x1;
                  }
               else
               	{
                  if(IsDlgButtonChecked(hwndDlg,IDC_IMPDIG)==BST_CHECKED) res |= 0x2;
                  if(IsDlgButtonChecked(hwndDlg,IDC_IMPMARK)==BST_CHECKED) res |= 0x10;
                  if(IsDlgButtonChecked(hwndDlg,IDC_IMPPICT)==BST_CHECKED) res |= 0x4;
                  if(IsDlgButtonChecked(hwndDlg,IDC_IMPWEB)==BST_CHECKED) res |= 0x8;
                  }
               EndDialog(hwndDlg, res);
               return true;

            case IDCANCEL:
               EndDialog(hwndDlg, -1);
               return true;

            case IDC_FONT:
               BMPS->ChooseFonts();
               return true;

         	case IDC_RADIOBUTTON1:
         	case IDC_RADIOBUTTON2:
              if(IsDlgButtonChecked(hwndDlg,IDC_RADIOBUTTON1)==BST_CHECKED)
              	{
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPMARK), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPDIG), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPWEB), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPPICT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_CELLSIZE), false);
               EnableWindow(GetDlgItem(hwndDlg, IDC_FONT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_UPDOWN1), false);
               }
              else if(IsDlgButtonChecked(hwndDlg,IDC_RADIOBUTTON2)==BST_CHECKED)
              	{
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPMARK), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPDIG), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPWEB), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IMPPICT), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_CELLSIZE), true);
               EnableWindow(GetDlgItem(hwndDlg, IDC_FONT), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_UPDOWN1), true);
               }
            default: return false;
         	}
		default: return false;
      }
}

void BITMAPS::ChooseFonts()
{
   cf.lStructSize=sizeof(CHOOSEFONT);
   cf.hwndOwner = hWnd;
   cf.hDC       = hMemDC;
   cf.lpLogFont = &lf;
   cf.iPointSize= NULL;
   cf.Flags 	 = CF_BOTH | CF_FORCEFONTEXIST | CF_INITTOLOGFONTSTRUCT;
   cf.rgbColors = NULL;
   cf.lCustData = NULL;
   cf.lpfnHook  = NULL;
   cf.lpTemplateName = NULL;
   cf.hInstance = hinst;
   cf.lpszStyle = NULL;
   cf.nFontType = PRINTER_FONTTYPE|SCREEN_FONTTYPE;
   cf.nSizeMin  = NULL;
   cf.nSizeMax  = NULL;
   if(ChooseFont(&cf))
   	DeleteObject(SelectObject(hMemDC, CreateFontIndirect(cf.lpLogFont)));;
}

void BITMAPS::SelectFonts (void)
{
   lf.lfHeight=-MulDiv(RectSize-RectSize/4, GetDeviceCaps(hMemDC, LOGPIXELSY), 72); // высота символов
   lf.lfWidth=0;
   DeleteObject(SelectObject(hMemDC, CreateFontIndirect(&lf)));
   return;
}

BITMAPS::BITMAPS(KROSSFEATURES *features, LOGFONT *plf, bool printing) //bool matrix
{  // выводим диалоговое окно параметров импорта
   error=false;
   feat = features;
   objects = false;
   hbmp = NULL;
	hinst = features->Global->hinst;
   hWnd = features->Global->hWnd;
   Maps=features->BitMap;
	FileInMemory = NULL;
   hSize=features->hSize;
   vSize=features->vSize;
   hExtSize=features->hNumbSize;
   vExtSize=features->vNumbSize;
   hTotalSize=vExtSize+hSize;
   vTotalSize=hExtSize+vSize;
   HDC hDC = GetDC(hWnd);
   hMemDC = CreateCompatibleDC(hDC);
   ReleaseDC (hWnd, hDC);
	BMPS = this;
   Colors            = features->ColorTable;
   Brushes           = new HBRUSH [FIRST+MAXCOLOR+EXTCOLOR];
   Brushes[BACK]     = CreateSolidBrush(BACKCOLOR);
// основные цвета
	for(int i=FIRST; i<MAXCOLOR+FIRST; i++)
   	Brushes[i] = CreateSolidBrush(Colors[i]); //Creats Master Brushes
// служебные цвета
   Brushes[GRAY]     = CreateSolidBrush(RGB_GRAY);
   Brushes[LIGHTGRAY]= CreateSolidBrush(RGB_SILVER);
// кисти
   SolidPen          = CreatePen(PS_SOLID, 1, Colors[BLACK]);//Create Pens
   DotPen            = CreatePen(PS_SOLID, 1, Colors[LIGHTGRAY]);
   DeleteObject(SelectObject(hMemDC,Brushes[BACK]));
   objects = true;

//   if(printing) return;
	if(!plf)
   	{
      FileInMemory = CreateBitmapFiles();
      return;
      }
	RectSize = 10;
   {
   byte *dlf = (byte *)&lf;
   byte *slf = (byte *)plf;
   for (int i=0; i<sizeof(LOGFONT); i++) dlf[i] = slf[i];
   SelectFonts();
   }
	RectSize = 15;
   DWORD res = (DWORD) DialogBox(hinst,MAKEINTRESOURCE(IDD_IMPORT_DIALOG),
                                 hWnd,(DLGPROC) BITMAPS::ImpDialogProc);
   if(res==(DWORD)((int)-1)){
      error = true;
   	return;
      }
   kross = !(bool)(res & 0x1);
   if(kross)
   	{
   	dig = (bool)(res & 0x2);
      pict = (bool)(res & 0x4);
   	web = (bool)(res & 0x8);
      Web5x5 = (bool)(res & 0x10);
   	RectSize = res >> 8;
	   RectSize = RectSize ? RectSize : 15;
   	Calculate(true);
      if(!hbmp) {error=true; return;}
   	PaintBMP();
      }
   else {
   	dig = Web5x5 = pict = web = false;
      RectSize = 1;
      vTotalSize = vSize;
      hTotalSize = hSize;
   	Calculate(false);
      PaintIconic();
      FileInMemory = CreateBitmapFiles();
      }
}

BITMAPS::~BITMAPS()
{
	if(objects)
   	{
		for(int i=0; i<FIRST+MAXCOLOR+EXTCOLOR; i++)
   		DeleteObject(Brushes[i]);
	   DeleteDC(hMemDC);
      }
   DELETE(FileInMemory);
}

void BITMAPS::PaintSample(HDC hDC, LPRECT rc, LPRECT)
{// рисование миниатюры при уст. парам. страницы
   float a = ((float)(rc->bottom - rc->top)/vTotalSize),
   		b = ((float)(rc->right - rc->left)/hTotalSize);

	a = MIN(a,b);

   int xS = (int)(rc->left+((rc->right-rc->left)/2)-a*hTotalSize/2);
   int xL = (int)(xS + a*hTotalSize);
   int yS = (int)(rc->top+((rc->bottom-rc->top)/2)-a*vTotalSize/2);
   int yL = (int)(yS + a*vTotalSize);
	RectSize = 3;
  	dig = pict = true;
   Web5x5 = web = true;
  	Calculate(true);
  	PaintBMP();
   StretchBlt(hDC, xS, yS, xL-xS, yL-yS,
                   hMemDC, 0, 0, RectSize*hTotalSize, RectSize*vTotalSize,
                   SRCCOPY);
}

inline RGBQUAD BITMAPS::Convert (COLORREF cr)
{
   RGBQUAD rq;
   rq.rgbRed = GetRValue(cr);
   rq.rgbGreen = GetGValue(cr);
   rq.rgbBlue = GetBValue(cr);
   rq.rgbReserved = 0;
	return rq;
}

void BITMAPS::Calculate(bool fullcolors)
{
   Client.top = Client.left = 0;
   Client.bottom = RectSize * (dig ? vTotalSize : vSize)+1;
   Client.right  = RectSize * (dig ? hTotalSize : hSize)+1;
   HDC hdc;
   HLOCAL ptr = LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*(feat->Colors>1 ? 16 : 2));
   BITMAPINFO *lpbi = (BITMAPINFO *)ptr;
   BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)ptr;
   bih->biSize = sizeof(BITMAPINFOHEADER);
   bih->biWidth = hSize;
   bih->biHeight = vSize;
   bih->biPlanes = 1;
   bih->biBitCount = (WORD)(feat->Colors>1 ? 4 : 1);
   bih->biCompression = BI_RGB;
   bih->biSizeImage = 0;//(bih->biWidth+7)/8*bih->biHeight*bih->biBitCount;
   bih->biXPelsPerMeter = 0;
   bih->biYPelsPerMeter = 0;
   bih->biClrUsed = 0;//feat->Colors>1 ? 16 : 1;
   bih->biClrImportant = bih->biClrUsed;
   lpbi->bmiColors[0] = Convert(Colors[BACK]);
   for(int i=0; i<(feat->Colors>1 ? 16 : 2); i++) lpbi->bmiColors[i] = Convert(Colors[BACK+i]);

	hdc = GetDC(hWnd);

   if(fullcolors) hbmp = CreateCompatibleBitmap(hdc, Client.right, Client.bottom);
   else           if(feat->Colors>1)hbmp = CreateCompatibleBitmap(hdc, hSize, vSize);
                  else {
                  	hbmp = CreateDIBitmap(hMemDC,bih,CBM_INIT,(void *)Maps,lpbi,DIB_PAL_COLORS);
                     }

  	DeleteObject(SelectObject(hMemDC,hbmp));
  	ReleaseDC (hWnd, hdc);
   LocalFree(ptr);

   GetObject(hbmp, sizeof (BITMAP), (void *)&bmp);

//Определяем стартовую точку рисования
	VPort.x=0;
	VPort.y=0;
//Определяем прямоугольник оконтовки
   Border.top=0; Border.left=0;
   Border.bottom=Client.bottom;
   Border.right=Client.right;
//Определяем стартовую точку и прямоугольник рабочего поля
	if(dig)
   	{
      WorkFild.x=vExtSize*RectSize; WorkFild.y=hExtSize*RectSize;
      }
   else
   	{
      WorkFild.x=0; WorkFild.y=0;
      }
   Fild.left=WorkFild.x; Fild.right=WorkFild.x+hSize*RectSize;
   Fild.top=WorkFild.y; Fild.bottom=WorkFild.y+vSize*RectSize;
//Определяем прямоугольники полей оцифровки
	if(dig)
   	{
	   hDig=Fild; hDig.bottom=hDig.top; hDig.top=VPort.y;
   	vDig=Fild; vDig.right=vDig.left; vDig.left=VPort.x;
//Определяем прямоугольник миниатюры
   	Icon=vDig; Icon.bottom=Icon.top; Icon.top=VPort.y;
      }
   else
   	{
      vDig.top = vDig.bottom = vDig.left = vDig.right = NULL;
	   Icon=hDig=vDig;
      }
}

void BITMAPS::PaintBMP(void)
{
   POINT LastPoint, OldViewPort;
   int x=vExtSize*RectSize, y=hExtSize*RectSize, i;

//Рисуем бордюр
	FillRect(hMemDC,&Border,Brushes[BACK]);
   FrameRect(hMemDC,&Border,Brushes[BLACK]);
//Позиционируем координаты вывода
   SetViewportOrgEx(hMemDC, VPort.x, VPort.y, &OldViewPort);

////////////////////////Рисуем сетку///////////////////////////////
if(dig){
//Горизонтальная оцифровка
   SelectObject(hMemDC,DotPen);
   for(i=1; i<hExtSize; i++)
      {
      MoveToEx(hMemDC,x,i*RectSize-1,&LastPoint);
      LineTo(hMemDC,RectSize*hTotalSize,i*RectSize-1);
      }
   for(i=vExtSize+1; i<hTotalSize; i++)
      {
      MoveToEx(hMemDC,i*RectSize,0,&LastPoint);
      if((i-vExtSize)%5)
        LineTo(hMemDC,i*RectSize, y);
      else
        {
        SelectObject(hMemDC,SolidPen);
        LineTo(hMemDC,i*RectSize, y);
        SelectObject(hMemDC,DotPen);
        }
      }
//Горизонтальный разделитель оцифровки и рабочего поля
   SelectObject(hMemDC,SolidPen);
   MoveToEx(hMemDC,0,hExtSize*RectSize,&LastPoint);
   LineTo(hMemDC,RectSize*hTotalSize,hExtSize*RectSize);
   MoveToEx(hMemDC,0,hExtSize*RectSize-1,&LastPoint);
   LineTo(hMemDC,RectSize*hTotalSize,hExtSize*RectSize-1);
//Вертикальная оцифровка
   SelectObject(hMemDC,DotPen);
   for(i=1; i<vExtSize; i++)
      {
      MoveToEx(hMemDC,i*RectSize-1,y+1,&LastPoint);
      LineTo(hMemDC,i*RectSize-1,vTotalSize*RectSize);
      }
   for(i=hExtSize+1; i<vTotalSize; i++)
      {
      MoveToEx(hMemDC,0,i*RectSize,&LastPoint);
      if((i-hExtSize)%5)
        LineTo(hMemDC,x,i*RectSize);
      else
        {
        SelectObject(hMemDC,SolidPen);
        LineTo(hMemDC,x,i*RectSize);
        SelectObject(hMemDC,DotPen);
        }
      }
//Вертикальный разделитель оцифровки и рабочего поля
   SelectObject(hMemDC,SolidPen);
   MoveToEx(hMemDC,vExtSize*RectSize,-1,&LastPoint);
   LineTo(hMemDC,vExtSize*RectSize,RectSize*vTotalSize);
   MoveToEx(hMemDC,vExtSize*RectSize-1,-1,&LastPoint);
   LineTo(hMemDC,vExtSize*RectSize-1,RectSize*vTotalSize);
}
//Рабочее поле
   SetViewportOrgEx(hMemDC, WorkFild.x, WorkFild.y, &LastPoint);


// рисуем основную сетку
   SelectObject(hMemDC,DotPen);
   if(web)
      {
	   for(i=1; i<vSize; i++)
   	   if(i%5 || !Web5x5) {
      	   MoveToEx(hMemDC,1,i*RectSize,&LastPoint);
         	LineTo(hMemDC,RectSize*hSize,i*RectSize);
	         }
   	for(i=1; i<hSize; i++)
      	if(i%5 || !Web5x5) {
         	MoveToEx(hMemDC,i*RectSize,1,&LastPoint);
	         LineTo(hMemDC,i*RectSize,RectSize*vSize);
   	      }
      }
   if(Web5x5)
   	{
// делим ее на квадраты по 5 клеток
	   SelectObject(hMemDC,SolidPen);
   	for(i=5; i<vSize; i+=5) {
      	MoveToEx(hMemDC,1,i*RectSize,&LastPoint);
	      LineTo(hMemDC,RectSize*hSize,i*RectSize);
   	   }
	   for(i=5; i<hSize; i+=5) {
   	   MoveToEx(hMemDC,i*RectSize,1,&LastPoint);
      	LineTo(hMemDC,i*RectSize,RectSize*vSize);
	      }
      }

//Прорисовываем рисунок
   MakeRect();

//Восстанавливаем порт вывода
   SetViewportOrgEx(hMemDC, OldViewPort.x, OldViewPort.y, &OldViewPort);

//Рисуем оцифровку
   if(dig) ViewTable();
}

void BITMAPS::ViewTable(void)
{
   for(int x=0; x<hSize; x++)   //Выводим горизонтальную оцифровку
      for(int y=hExtSize; y>0; y--)
         ViewDigit(x, y-1, true);
   for(int y=0; y<vSize; y++)   //Выводим вертикальную оцифровку
      for(int x=vExtSize; x>0; x--)
         ViewDigit(x-1, y, false);
}

void BITMAPS::ViewDigit(int x, int y, bool Whu)
{
//Whu == true - hDigitals; false - vDigitals

   byte *Digit, *DigitColor, TxtColor, BkColor;
   RECT a;
   char b[17];
   int c;
   COLORREF OldBkCol;

   if(Whu) {        //Находим адрес и координату горизонтальной цифры
           Digit = feat->hData->Digits +(hExtSize-y-1)+x*hExtSize;
           DigitColor = feat->hData->DigitColors +(hExtSize-y-1)+x*hExtSize;
           a.left   = hDig.left+x*RectSize+1;
           a.top    = hDig.top+y*RectSize;
           a.right  = a.left+RectSize-1;
           a.bottom = a.top+RectSize-1;
           }
   else    {        //Находим адрес и координату вертикальной цифры
           Digit = feat->vData->Digits +y*vExtSize+(vExtSize-x-1);
           DigitColor = feat->vData->DigitColors +y*vExtSize+(vExtSize-x-1);
           a.left   = vDig.left+x*RectSize;
           a.top    = vDig.top+y*RectSize+1;
           a.right  = a.left+RectSize-1;
           a.bottom = a.top+RectSize-1;
           }

   if(feat->Containers == COLORS){
      BkColor = *DigitColor;
      COLORREF cur = Colors[*DigitColor];
      int akkum = (GetRValue(cur) + GetGValue(cur) + GetBValue(cur))/3;
      TxtColor = (BYTE)(akkum > 100 ? BLACK : BACK);
   	}
   else {
   	TxtColor = BLACK;
      BkColor = BACK;
      }
   SetTextColor(hMemDC,Colors[TxtColor]);
   OldBkCol=SetBkColor(hMemDC,Colors[BkColor]);
   FillRect(hMemDC,&a,Brushes[BkColor]);

   if(*Digit){
     itoa((int)*Digit, b, 10);
     c=strlen(b);
     if(c>2) c=2;
     if(!*Digit) strcpy(b,"  ");
     DrawText(hMemDC, b, c, &a, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
     }
   SetBkColor(hMemDC,OldBkCol);
}

void BITMAPS::DrawCell (int x, int y)
{
	if(!pict) return;
   RECT Cur;//, Small;
   byte El = Maps[y*hSize+x];
	Cur.top=y*RectSize; Cur.bottom=Cur.top+RectSize;
   Cur.left=x*RectSize; Cur.right=Cur.left+RectSize;
   if(!web) {Cur.top-=1; Cur.left-=1;}
   if(El!=MARKER && El!=ZEROS) FillRect(hMemDC, &Cur, Brushes[El]);
}

void BITMAPS::MakeRect(void)
{
   POINT OldViewPort;
   SetViewportOrgEx(hMemDC, WorkFild.x, WorkFild.y, &OldViewPort);
//Закрашиваем все поле
   for(int y=0; y<vSize; y++)
      for(int x=0; x<hSize; x++)
			DrawCell (x, y);
   SetViewportOrgEx(hMemDC, OldViewPort.x, OldViewPort.y, &OldViewPort);
}

void BITMAPS::PaintIconic  (void) //Рисует иконку
{
   POINT OldViewPort;
   int color;
   SetViewportOrgEx(hMemDC, 0, 0, &OldViewPort);
   for(int y=0; y<vSize; y++)
      for(int x=0; x<hSize; x++){
         color = Maps[x+hSize*y];
         color = color == MARKER ? BACK : color;
         SetPixel(hMemDC,x,y,Colors[color]);
         }
   SetViewportOrgEx(hMemDC, OldViewPort.x, OldViewPort.y, &OldViewPort);
   return;
}

byte *BITMAPS::CreateBitmapFiles()
{
	// выделяем память и определяем указатели
   int NumbColors  = (feat->Colors>1 ? feat->Colors+FIRST : 2);
	int BitPerPixel = (feat->Colors>1 ? 8 : 1);
   int hSizeInByteAlignDword = (((feat->hSize*BitPerPixel+31)/32)*4);
   DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+
   					  sizeof(RGBQUAD)*(NumbColors)+
                    feat->vSize*hSizeInByteAlignDword;
   byte *FileInMemoryPointer = new byte[FileSize];
	BITMAPFILEHEADER *bfh = (BITMAPFILEHEADER *)FileInMemoryPointer;
   BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)(FileInMemoryPointer+
   												sizeof(BITMAPFILEHEADER));
   RGBQUAD *ColorTab   	 = (RGBQUAD *)(FileInMemoryPointer+sizeof(BITMAPFILEHEADER)+
   												sizeof(BITMAPINFOHEADER));
   byte *PaxelData       = FileInMemoryPointer+sizeof(BITMAPFILEHEADER)+
   												sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*(NumbColors);
// заполняем структуры BITMAPFILEHEADER и BITMAPINFOHEADER
   bfh->bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
	bfh->bfSize = FileSize;
	bfh->bfReserved1 = 0;
	bfh->bfReserved2 = 0;
   bfh->bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*
   																					(NumbColors);

   bih->biSize   = sizeof(BITMAPINFOHEADER);
   bih->biWidth  = (LONG)feat->hSize;
   bih->biHeight = (LONG)feat->vSize;
   bih->biPlanes = 1;
   bih->biBitCount = (WORD)BitPerPixel;
   bih->biCompression = BI_RGB;
   bih->biSizeImage = (DWORD)(feat->vSize*hSizeInByteAlignDword);
   bih->biXPelsPerMeter = (LONG)0;
   bih->biYPelsPerMeter = (LONG)0;
   bih->biClrUsed = (DWORD)(NumbColors);
   bih->biClrImportant = (DWORD) 0;

// определяем индекс белого
	WhiteIndex = 0;
/*   DWORD a = 0;
   for(int i=FIRST; i<FIRST+MAXCOLOR; i++)
      {
      DWORD b = GetRValue(feat->ColorTable[i])+GetGValue(feat->ColorTable[i])+GetBValue(feat->ColorTable[i]);
   	if(a<b) {a=b; WhiteIndex = (byte)(i-FIRST);}
      }*/
// копируем палитру
	if(feat->Colors > 1)
   	for(int i=0; i<NumbColors; i++)
   		ColorTab[i] = Convert(feat->ColorTable[i]);
   else {ColorTab[1] = Convert(feat->ColorTable[BLACK]);
   		ColorTab[0] = Convert(feat->ColorTable[BACK]);}
// копируем массив пикселов
	byte akkum;
   int ptr;
	byte mask = (byte)(feat->Colors > 1 ? 0xff : 0x01);
	for(int vert = feat->vSize-1; vert>=0; vert--) // высота (снизу вверх)
   	for(int hbyte = 0; hbyte<hSizeInByteAlignDword; hbyte++) // ширина в байтах
         {
         ptr = hbyte*(8/BitPerPixel);
         akkum = 0;
      	for(int i=0; i<(8/BitPerPixel); i++)	// размер байта в пикселах
            {
            akkum <<= BitPerPixel;
         	akkum &= (byte)(0xff << BitPerPixel);
            if(ptr<feat->hSize)
            	akkum |= (byte)(GetIndexColor(feat->BitMap[vert*feat->hSize+ptr]) & mask);
            ptr++;
            }
         *(PaxelData++) = akkum;
         }
   return FileInMemoryPointer;
}

byte BITMAPS::GetIndexColor(byte number)
{
/*	if(number == BLACK) return 0;
   else if(number == BACK) return (byte)((feat->Colors > 1) ? WhiteIndex : 1);
   else*/
   if(number == MARKER) return WhiteIndex;//(byte)((feat->Colors > 1) ? WhiteIndex : 1);
   else return (byte)(number); //-FIRST);
}
