//paint.cpp - implementation class a painting
#include "paint.h"
#include <stdlib.h>

//#define _PaintInMemory_

void PAINTING::WindowSizing()
{
  	PaletteWindowSizing();
   RectSize=0;
   SetScroll();
}

void PAINTING::SetCurrentInput (CTOOL control)
{ // Установка текущего инструмента ввода
	if(MouseInput == CDIGITAL && control != CDIGITAL) if(CheckSumm()) ChangeInTable(true, true);
	switch(control){
   	case CPEN:

      	break;
      case CRECT:

      	break;
      case CEYE:

      	break;
      case CFILLED:

      	break;
      case CDIGITAL:

      	break;
      }
   MouseInput = control;
   SetButtonState();
}

PAINTING::~PAINTING()
{
   DelObject();
   DeleteObject(NormalFont);
   DeleteObject(BigFont);
   DeleteObject(hIconBmp);
   DeleteObject(hTrancBmp);
   DeleteDC(hMemDC);
   DeleteDC(hTrancDC);
#ifdef _PaintInMemory_
   DeleteDC(hPaintDC);
#endif
	DELETE(SaveMaps);
}

void PAINTING::DelObject(bool All)
{
   if(GLOBAL::StatusBox && All) delete StatusBox;
   if (All) DestroyWindow(hHScroll);
   if (All) DestroyWindow(vHScroll);
	for(int i=0; i<FIRST+MAXCOLOR+EXTCOLOR; i++){
   	DeleteObject(Brushes[i]);
   	DeleteObject(BoldPen[i]);
      }
   DeleteObject(SolidPen);
   DeleteObject(DotPen);
   DeleteObject(CrossPen);
   delete[] Brushes;
}

void PAINTING::SetIconFeat(void)
{
   static int hExt = -1, vExt = -1, rsize =-1;
   HDC hDC=GetDC(hWnd);
   if(InitScreen)
      if(RectSize!=rsize || hExt!=hNumbSize || vExt!=vNumbSize){
      	if(hIconBmp) DeleteObject(hIconBmp);
      	DeleteObject(hTrancBmp);
      	DeleteObject(BigFont);
         }
	if(hExt!=hNumbSize || vExt!=vNumbSize || RectSize!=rsize){
   	BigFont   = CreateFonts(RectSize*MAX(hNumbSize,vNumbSize));
	  	hIconBmp  = CreateCompatibleBitmap (hDC, hSize, vSize);
     	hTrancBmp = CreateCompatibleBitmap(hDC, vNumbSize*RectSize, hNumbSize*RectSize);
	  	DeleteObject(SelectObject(hMemDC, hIconBmp));
	  	DeleteObject(SelectObject(hTrancDC, hTrancBmp));
		}
   ReleaseDC(hWnd, hDC);
	hExt = hNumbSize; vExt = vNumbSize;
}

// Инициализация экрана
void PAINTING::ScreenInitialize(void)
{
	RegFilled = false;
   if(InitScreen)	DelObject(false);
   hExtSize=hNumbSize;
   vExtSize=vNumbSize;
   NumbColors=KROSSFEATURES::Colors;
   hTotalSize=vExtSize+hSize;
   vTotalSize=hExtSize+vSize;

   Colors            = KROSSFEATURES::ColorTable;
   Brushes           = new HBRUSH [FIRST+MAXCOLOR+EXTCOLOR];
// цвет фона
   Brushes[BACK]     = CreateSolidBrush(BACKCOLOR);
// основные цвета
// кисти
   DotPen            = CreatePen(PS_SOLID, 0, Colors[LIGHTGRAY]);
   CrossPen          = CreatePen(PS_SOLID, 0, Colors[GRAY]);
  	SolidPen 			= CreatePen(PS_SOLID, 0, Colors[BLACK]);//Create Pens
   BoldPen[BACK] 		= CreatePen(PS_SOLID, PENSWIDTH, Colors[BACK]);
   BoldPen[GRAY]     = CreatePen(PS_SOLID, PENSWIDTH, RGB_GRAY);
   BoldPen[LIGHTGRAY]= CreatePen(PS_SOLID, PENSWIDTH, RGB_SILVER);
	for(int i=FIRST; i<MAXCOLOR+FIRST; i++){
   	Brushes[i] = CreateSolidBrush(Colors[i]); //Creats Master Brushes
     	BoldPen[i] = CreatePen(PS_SOLID, PENSWIDTH, Colors[i]);//Create Pens
      }
// служебные цвета
   Brushes[GRAY]     = CreateSolidBrush(RGB_GRAY);
   Brushes[LIGHTGRAY]= CreateSolidBrush(RGB_SILVER);
   GetClientRect(hWnd,&Client);
   DELETE(SaveMaps);
   SaveMaps = new byte[hSize*vSize];
   if(!InitScreen){
      HDC hDC=GetDC(hWnd);
      hMemDC            = CreateCompatibleDC(hDC);
      hTrancDC          = CreateCompatibleDC(hDC);
#ifdef _PaintInMemory_
      hPaintDC 			= CreateCompatibleDC(hDC);
#endif
      ReleaseDC(hWnd, hDC);
      DeleteObject(SelectObject(hMemDC,Brushes[BACK]));
      DeleteObject(SelectObject(hTrancDC,Brushes[BACK]));
      Multyple = 0;
      GLOBAL::StatusBox = !GLOBAL::StatusBox;
      CreateScrollBar();
      RectSize=0;
  	   MakeLogFont(MIN(Client.right/hTotalSize,(Client.bottom-StatusHight)/vTotalSize));
   	StatusCheck();
      }
   EnableMenuItem(GetMenu(hWnd), IDM_MIN, MF_GRAYED);
   EnableMenuItem(GetMenu(hWnd), IDM_MAX, MF_ENABLED);
   EnableMenuItem(GetMenu(hWnd), IDM_NORM, MF_ENABLED);
   SetScroll();
   UpdatePalette();
   InitScreen = true;
}

void PAINTING::Calculate(void)
{
   HDC hDC;
   GetClientRect(hWnd,&Client);
   ClipX=PanelWidth;
   ClipY=StatusHight+WindowToolHeight;

//Определяем размер ячейки
   int Horiz=(Client.right-ClipX)/hTotalSize;
   int Vert=(Client.bottom-ClipY)/vTotalSize;
   int temp=MAX((MIN(Horiz,Vert)+Multyple),1);

   bool rep = false, repe = false;
	if(((Client.right-Client.left)-hTotalSize*temp-ClipX) < 0)
   	{
      ClipY+=SBV;// если выводится нижний скролл
      Client.bottom-=SBV;
      rep = true;
      }
	if(((Client.bottom-Client.top)-vTotalSize*temp-ClipY) < 0)
      {
      ClipX+=SBH;// если выводится правый скролл
      Client.right-=SBH;
      repe = true;
      }
   if(repe && !rep)//повтор проверки если включился правый скролл
	   if(((Client.right+SBH-Client.left)-hTotalSize*temp-ClipX) < 0)
   	   {
         ClipY+=SBV;// если выводится нижний скроллбар
         Client.bottom-=SBV;
         }
   Client.top    += WindowToolHeight;
	Client.bottom -= StatusHight;
   Client.left   += PanelWidth;

   if(temp!=RectSize)
     {                    //Изменяем параметры объектов соответствено масштабу
     if(InitScreen){
        DeleteObject(NormalFont);
        }
     NormalFont = CreateFonts(temp);
	  hSbPoz=(hTotalSize*temp-(Client.right-Client.left))/2;
	  vSbPoz=(vTotalSize*temp-(Client.bottom-Client.top))/2;
	  hDC=GetDC(hWnd);             //Создаем в памяти Bitmap миниатюры
#ifdef _PaintInMemory_
     DeleteObject(SelectObject(hPaintDC,CreateCompatibleBitmap(hDC,Client.right,Client.bottom)));
#endif
	  ReleaseDC(hWnd, hDC);
	  }
	RectSize=temp;

//Определяем стартовую точку рисования
	VPort.x=-hSbPoz+PanelWidth;
	VPort.y=-vSbPoz+WindowToolHeight;
//Определяем прямоугольник оконтовки
   Border.top=VPort.y-1; Border.left=VPort.x-1;
   Border.bottom=VPort.y+vTotalSize*RectSize+1;
   Border.right=VPort.x+hTotalSize*RectSize+1;
//Определяем стартовую точку и прямоугольник рабочего поля
   WorkFild.x=vExtSize*RectSize+VPort.x; WorkFild.y=hExtSize*RectSize+VPort.y;
   Fild.left=WorkFild.x; Fild.right=WorkFild.x+hSize*RectSize;
   Fild.top=WorkFild.y; Fild.bottom=WorkFild.y+vSize*RectSize;
//Определяем прямоугольники полей оцифровки
   hDig=Fild; hDig.bottom=hDig.top; hDig.top=VPort.y;
   vDig=Fild; vDig.right=vDig.left; vDig.left=VPort.x;
//Определяем прямоугольник миниатюры
   Icon=vDig; Icon.bottom=Icon.top; Icon.top=VPort.y;
   SetIconFeat();
}

void PAINTING::ChooseFonts()
{
   CHOOSEFONT cf;
   cf.lStructSize=sizeof(CHOOSEFONT);
   cf.hwndOwner = hWnd;
   cf.hDC       = GetDC(hWnd);
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
//   WORD         ___MISSING_ALIGNMENT__;
   cf.nSizeMin  = NULL;
   cf.nSizeMax  = NULL;
   if(!ChooseFont(&cf))
   	{
		ReleaseDC(hWnd, cf.hDC);
      return;
      }
   ReleaseDC(hWnd, cf.hDC);
   SetSetupValue((LPBYTE)cf.lpLogFont, "LogFont", sizeof(LOGFONT));
   DeleteObject(NormalFont);
   NormalFont = CreateFontIndirect(cf.lpLogFont);
   DeleteObject(BigFont);
   BigFont = CreateFonts(RectSize*MAX(hExtSize,vExtSize));
   InvalidateRect(hWnd,&Client,true);
}

void PAINTING::MakeLogFont (int Size)
{
   char FontName[]={"MS Serif"};
   lf.lfWeight=FW_NORMAL;        // толшина символов
   lf.lfItalic=false;
   lf.lfUnderline=false;
   lf.lfStrikeOut=false;
   for(int i=0; i<sizeof(FontName); i++)
        lf.lfFaceName[i] = FontName[i];
   GetSetupValue((LPBYTE)&lf, "LogFont", sizeof(LOGFONT));
   HDC hDC=GetDC(hWnd);
   lf.lfEscapement=0;          // поворот строки в 0.1 гр.
   lf.lfOrientation=0;
   lf.lfHeight=-MulDiv(Size-Size/4, GetDeviceCaps(hDC, LOGPIXELSY), 72); // высота символов
   lf.lfWidth=0;
   lf.lfCharSet=DEFAULT_CHARSET;
   lf.lfQuality=DRAFT_QUALITY;
   lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
   lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
   lf.lfPitchAndFamily=FIXED_PITCH|FF_DONTCARE;
   ReleaseDC(hWnd,hDC);
   return;
}

HFONT PAINTING::CreateFonts (int Size)
{
   HDC hDC=GetDC(hWnd);
   LOGFONT lf = PAINTING::lf;
   lf.lfHeight=-MulDiv(Size-Size/3, GetDeviceCaps(hDC, LOGPIXELSY), 72); // высота символов
   lf.lfWidth=0;//-MulDiv(Size/3, GetDeviceCaps(hDC, LOGPIXELSY), 72); //ширина символов
   ReleaseDC(hWnd,hDC);
   return CreateFontIndirect(&lf);
}

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Члены-функции прорисовки экрана ///////////////////
//////////////////////////////////////////////////////////////////////////////

LRESULT PAINTING::PaintProc (void)
// обработка сообщения WM_PAINT
{
   if(!InitScreen) return 0;
   ExecPaint = true;
   PAINTSTRUCT ps;
   HDC hDC=BeginPaint(hWnd,&ps);
   HBRUSH old;
   if(ps.fErase)
      {
#ifdef _PaintInMemory_
	   old = (HBRUSH)SelectObject(hDC,Brushes[GRAY]);
   	SelectObject(hPaintDC,Brushes[GRAY]);
   	PatBlt(hPaintDC, 0, 0,Client.right, Client.bottom,PATCOPY);
      SelectObject(hPaintDC,old);
   	Paint(hPaintDC);
   	BitBlt(hDC, 0, 0, Client.right, Client.bottom, hPaintDC, 0, 0, SRCCOPY);
#else
	   old = (HBRUSH)SelectObject(hDC,Brushes[GRAY]);
   	PatBlt(hDC, 0, 0,Client.right, Client.bottom,PATCOPY);
      SelectObject(hDC,old);
   	Paint(hDC);
#endif
      }
   else
   	{
   	Paint(hDC);
		}
   char SbSize[10];
   if(GLOBAL::StatusBox)
     {
     itoa(hSize, SbSize, 10);
     strcpy(SbSize+strlen(SbSize)," x ");
     itoa(vSize, SbSize+strlen(SbSize), 10);
     StatusBox->SetText(SB_SIZE, SbSize);
     }
// конец перерисовки
   EndPaint(hWnd,&ps);
//   ValidateRect(hWnd,NULL);
   ExecPaint = false;
   return 0;
}

void PAINTING::Paint(HDC hDC)
{
   POINT LastPoint, OldViewPort;
   int x=vExtSize*RectSize, y=hExtSize*RectSize, i;

//Рисуем бордюр
   FrameRect(hDC, &Border, Brushes[BLACK]);

//Рисуем иконку
   PaintIconic(hDC);

//Позиционируем координаты вывода
   SetViewportOrgEx(hDC, VPort.x, VPort.y, &OldViewPort);

////////////////////////Рисуем сетку///////////////////////////////

//Горизонтальная оцифровка
   SelectObject(hDC,DotPen);
//   for(i=1; i<hExtSize; i++)
//      {
//      MoveToEx(hDC,x,i*RectSize-1,&LastPoint);
//      LineTo(hDC,RectSize*hTotalSize,i*RectSize-1);
//      }
   for(i=vExtSize+1; i<hTotalSize; i++)
      {
      MoveToEx(hDC,i*RectSize,0,&LastPoint);
      if((i-vExtSize)%5)
        LineTo(hDC,i*RectSize, y);
      else
        {
        SelectObject(hDC,SolidPen);
        LineTo(hDC,i*RectSize, y);
        SelectObject(hDC,DotPen);
        }
      }
//Горизонтальный разделитель оцифровки и рабочего поля
   SelectObject(hDC,SolidPen);
   MoveToEx(hDC,0,hExtSize*RectSize,&LastPoint);
   LineTo(hDC,RectSize*hTotalSize,hExtSize*RectSize);

//Вертикальная оцифровка
   SelectObject(hDC,DotPen);
//   for(i=1; i<vExtSize; i++)
//      {
//      MoveToEx(hDC,i*RectSize-1,y+1,&LastPoint);
//      LineTo(hDC,i*RectSize-1,vTotalSize*RectSize);
//      }
   for(i=hExtSize+1; i<vTotalSize; i++)
      {
      MoveToEx(hDC,0,i*RectSize,&LastPoint);
      if((i-hExtSize)%5)
        LineTo(hDC,x,i*RectSize);
      else
        {
        SelectObject(hDC,SolidPen);
        LineTo(hDC,x,i*RectSize);
        SelectObject(hDC,DotPen);
        }
      }
//Вертикальный разделитель оцифровки и рабочего поля
   SelectObject(hDC,SolidPen);
   MoveToEx(hDC,vExtSize*RectSize,-1,&LastPoint);
   LineTo(hDC,vExtSize*RectSize,RectSize*vTotalSize);

//Рабочее поле
   SetViewportOrgEx(hDC, WorkFild.x, WorkFild.y, &LastPoint);


// рисуем основную сетку
   SelectObject(hDC,DotPen);
   if(ViewWeb)
      {
	   for(i=1; i<vSize; i++)
   	   if(i%5 || !ViewWeb5x5) {
      	   MoveToEx(hDC,1,i*RectSize,&LastPoint);
         	LineTo(hDC,RectSize*hSize,i*RectSize);
	         }
   	for(i=1; i<hSize; i++)
      	if(i%5 || !ViewWeb5x5) {
         	MoveToEx(hDC,i*RectSize,1,&LastPoint);
	         LineTo(hDC,i*RectSize,RectSize*vSize);
   	      }
      }
   if(ViewWeb5x5)
   	{
// делим ее на квадраты по 5 клеток
	   SelectObject(hDC,SolidPen);
   	for(i=5; i<vSize; i+=5) {
      	MoveToEx(hDC,1,i*RectSize,&LastPoint);
	      LineTo(hDC,RectSize*hSize,i*RectSize);
   	   }
	   for(i=5; i<hSize; i+=5) {
   	   MoveToEx(hDC,i*RectSize,1,&LastPoint);
      	LineTo(hDC,i*RectSize,RectSize*vSize);
	      }
      }

//Прорисовываем рисунок
   MakeRect(-1,-1,false,hDC);

//Восстанавливаем порт вывода
   SetViewportOrgEx(hDC, OldViewPort.x, OldViewPort.y, &OldViewPort);

//Рисуем оцифровку
   ViewTable(hDC);
}

void PAINTING::ViewTable(HDC hDC)
{
   HDC hdc = hDC ? hDC : GetDC(hWnd);
   HFONT OldFont = (HFONT) SelectObject(hdc, NormalFont);
   for(int x=0; x<hSize; x++)   //Выводим горизонтальную оцифровку
      for(int y=hExtSize; y>0; y--)
         ViewDigit(x, y-1, true, hdc, (OldLightingPos.x==x && MouseInput!=CDIGITAL) ? true : false);
   for(int y=0; y<vSize; y++)   //Выводим вертикальную оцифровку
      for(int x=vExtSize; x>0; x--)
         ViewDigit(x-1, y, false, hdc, (OldLightingPos.y==y && MouseInput!=CDIGITAL) ? true : false);
   SelectObject(hdc,OldFont);
   if(!hDC) ReleaseDC(hWnd,hdc);

}

void PAINTING::ViewDigit(int x, int y, bool Whu, HDC hdc, bool Lights)
{
//Whu == true - hDigitals; false - vDigitals

   byte volatile *Digit, *DigitColor;
   bool volatile *Marker;
   RECT a;
   char b[17];
   int c, BkColor, TxtColor;
   HDC hDC;
   HFONT OldFont;
   COLORREF OldBkCol;

   if(Whu) {        //Находим адрес и координату горизонтальной цифры
           Digit      = hData->Digits +(hExtSize-y-1)+x*hExtSize;
           DigitColor = hData->DigitColors +(hExtSize-y-1)+x*hExtSize;
           Marker     = hData->DigitMarkers +(hExtSize-y-1)+x*hExtSize;
           a.left   = hDig.left+x*RectSize+1;
           a.top    = hDig.top+y*RectSize;
           a.right  = a.left+RectSize-1;
           a.bottom = a.top+RectSize;
           }
   else    {        //Находим адрес и координату вертикальной цифры
           Digit 		 = vData->Digits +y*vExtSize+(vExtSize-x-1);
           DigitColor = vData->DigitColors +y*vExtSize+(vExtSize-x-1);
           Marker     = vData->DigitMarkers +y*vExtSize+(vExtSize-x-1);
           a.left   = vDig.left+x*RectSize;
           a.top    = vDig.top+y*RectSize+1;
           a.right  = a.left+RectSize;
           a.bottom = a.top+RectSize-1;
           }

   if(hdc) hDC=hdc;
   else {
        hDC=GetDC(hWnd);
        OldFont=(HFONT)SelectObject(hDC,NormalFont);
        }
   if(Containers == COLORS){
      COLORREF cur = Colors[*DigitColor];
      int akkum = GetMidleColorValue(cur);// MACROS
     	BkColor = *DigitColor;
      if(Lights){
         if(akkum>120 && akkum<136) TxtColor=BACK;
         else TxtColor = (akkum > 127 ? GRAY : LIGHTGRAY);
         if(!*Digit)
         	BkColor = (akkum > 127 ? BLACK : BACK);
      	}
      else {
         TxtColor = (akkum > 127 ? BLACK : BACK);
         }
   	}
   else {
      if(!DigitColor || !Digit) return;
      	TxtColor = BLACK;
      	BkColor = Lights ? LIGHTGRAY : BACK;
      }
   SetTextColor(hDC,Colors[TxtColor]);
   OldBkCol=SetBkColor(hDC,Colors[BkColor]);
   FillRect(hDC,&a,Brushes[BkColor]);

   if(*Digit){
     itoa((int)*Digit, b, 10);
     c=strlen(b);
     if(c>3) c=3;
     if(!*Digit) strcpy(b,"   ");
     DrawText(hDC, b, c, &a, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	  if(*Marker){
	     HPEN Saves = (HPEN)SelectObject(hDC, BoldPen[TxtColor]);
		  POINT LastPoint; // выводим зачеркнутое число
		  MoveToEx(hDC,a.left+1, a.top+1, &LastPoint);
   	  LineTo(hDC, a.right-1, a.bottom-1);
	     SelectObject(hDC, Saves);
        }
     }
   LastDigit = *Digit;
   SetBkColor(hDC,OldBkCol);
   if(!hdc) {
     SelectObject(hDC,OldFont);
     ReleaseDC(hWnd, hDC);
     }
}

void PAINTING::PaintCell (int *cells, int number)
{
// прорисовка ячеек при параллельном расчете
// cells - указатель на массив индексов ячеек, number - количество ячеек
   HDC hDC=GetDC(hWnd);
   POINT OldViewPort;
   SetViewportOrgEx(hDC, WorkFild.x, WorkFild.y, &OldViewPort);
   try {
	for (int i=0; i<number; i++) DrawCell (hDC, cells[i]%hSize, cells[i]/hSize);
   if(hSize < vNumbSize * RectSize && vSize < hNumbSize * RectSize && MouseInput!=CDIGITAL)
      {
   	SetViewportOrgEx(hDC, Icon.left+(Icon.right-Icon.left-hSize)/2, Icon.top+(Icon.bottom-Icon.top-vSize)/2, &OldViewPort);
		for (int i=0; i<number; i++)
      	SetPixel(hDC, cells[i]%hSize, cells[i]/hSize, Colors[BitMap[cells[i]] == MARKER ? BACK : BitMap[cells[i]]]);
      }
   SetViewportOrgEx(hDC, OldViewPort.x, OldViewPort.y, &OldViewPort);
	}
   __except(EXCEPTION_EXECUTE_HANDLER) {
      }
   ReleaseDC(hWnd, hDC);

}


void PAINTING::DrawCell (HDC hDC, int x, int y)
{
	if(!BitMap) return;
	HPEN Saves;
   RECT Cur, Small;
   byte El = BitMap[y*hSize+x];
	Cur.top=y*RectSize+1; Cur.bottom=Cur.top+RectSize-1;
   Cur.left=x*RectSize+1; Cur.right=Cur.left+RectSize-1;
   Small.top = Cur.top+1; Small.bottom = Cur.bottom-1;
   Small.left = Cur.left+1; Small.right = Cur.right-1;
   if(!ViewWeb){
      if(y) {Cur.top-=1; Small.top-=1;}
      if(x) {Cur.left-=1; Small.left-=1;}
      }
   if(!LetterMap || !LetterMap[y*hSize+x]){
	   if(El != MARKER)
   	   if(El == ZEROS)
	        	FillRect(hDC, &Cur, Brushes[BACK]);
   	   else {
     			if(ViewWeb)
					{
        			FrameRect(hDC, &Cur, Brushes[GRAY]);
	    	   	FillRect(hDC, &Small, Brushes[El]);
   	    		}
      	   else FillRect(hDC, &Cur, Brushes[El]);
         	}
	   else
   	   {
      	Saves = (HPEN)SelectObject(hDC, CrossPen);
	      FillRect(hDC,&Cur,Brushes[BACK]);

			// выводим точку
         if(!ExecDemo){
//	      	int sx = Small.left+RectSize/2-1, sy=Small.top+RectSize/2-1;
//		      COLORREF cc=ColorTable[BLACK];
//   		   SetPixel(hDC, sx--, sy, cc);
//      		if(RectSize > 8)SetPixel(hDC, sx, sy--, cc);
//		      if(RectSize > 11){SetPixel(hDC, sx++, sy, cc); SetPixel(hDC, sx, sy--, cc);}
//   		   if(RectSize > 15){SetPixel(hDC, sx--, sy, cc); SetPixel(hDC, sx--, sy, cc);}
//      		if(RectSize > 20){SetPixel(hDC, sx, sy++, cc); SetPixel(hDC, sx, sy++, cc); SetPixel(hDC, sx, sy++, cc);}
	   	  	POINT LastPoint; // выводим крестик
	 	     	MoveToEx(hDC,Small.left, Small.top, &LastPoint);
   	 	  	LineTo(hDC, Small.right, Small.bottom);
	        	MoveToEx(hDC,Small.left, Small.bottom-1, &LastPoint);
 	   	  	LineTo(hDC, Small.right, Small.top-1);
				}
	      SelectObject(hDC, Saves);
   	   }
   	}
   else {
   	int BkColor, TxtColor;
      COLORREF OldBkCol;
      if(El == MARKER || El == ZEROS){
		   BkColor = BACK;
         TxtColor = BLACK;
         }
      else {
      	BkColor = El;
   		if((GetMidleColorValue(ColorTable[El]))>127)// MACROS
         		TxtColor = BLACK;
         else	TxtColor = BACK;
         }
	   SetTextColor(hDC,ColorTable[TxtColor]);
   	OldBkCol=SetBkColor(hDC,ColorTable[BkColor]);
      HFONT OldFont=(HFONT)SelectObject(hDC,NormalFont);
//  		if(ViewWeb)
//  			{
//  			FrameRect(hDC, &Cur, Brushes[GRAY]);
//         FillRect(hDC, &Small, Brushes[BkColor]);
//  			DrawText(hDC, (char *)(LetterMap+y*hSize+x), 1, &Small, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
//     		}
//  	   else{
         FillRect(hDC, &Cur, Brushes[BkColor]);
      	DrawText(hDC, (char *)(LetterMap+y*hSize+x), 1, &Cur, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
//			}
      SelectObject(hDC,OldFont);
	   SetBkColor(hDC,OldBkCol);
   	}
}

void PAINTING::MakeRect(int x, int y, bool updateicons, HDC hdc)
{
   HDC hDC= hdc ? hdc : GetDC(hWnd);
   POINT OldViewPort;
   if(updateicons) PaintIconic(hDC);
   SetViewportOrgEx(hDC, WorkFild.x, WorkFild.y, &OldViewPort);
//Закрашиваем квадрат
   if(((unsigned int)x)<MAXSIZE && ((unsigned int)y)<MAXSIZE)
		DrawCell (hDC, x, y);
   else
//Закрашиваем все поле
      for(y=0; y<vSize; y++)
         for(x=0; x<hSize; x++)
				DrawCell (hDC, x, y);
   SetViewportOrgEx(hDC, OldViewPort.x, OldViewPort.y, &OldViewPort);
   if(!hdc) ReleaseDC(hWnd, hDC);
}

HDC PAINTING::PaintIconic  (HDC hDC) //Рисует иконку
{
   POINT OldViewPort;
   HFONT OldFont;
   COLORREF OldBkCol;
   char buf[17];

	if(MouseInput!=CDIGITAL && !BitMap) return hDC;

   int color, hIcon=Icon.right-Icon.left,
              vIcon=Icon.bottom-Icon.top;
   bool sizing = false;
   int PortX=(hIcon-hSize)/2, PortY=(vIcon-vSize)/2;
   float ScaleX = (float)hIcon / (float)hSize;
   float ScaleY = (float)vIcon / (float)vSize;
   float Scaling = MIN(ScaleX, ScaleY);
   int hsize = (int)(hSize * Scaling);
   int vsize = (int)(vSize * Scaling);
  	int DistPortX=(hIcon-hsize)/2, DistPortY=(vIcon-vsize)/2;
   if(hIcon < hSize || vIcon < vSize)
   	{
      sizing = true;
      PortX=PortY=0;
      }
   SetViewportOrgEx(hDC, 0, 0, &OldViewPort);
   if(MouseInput!=CDIGITAL){
   	PatBlt(hTrancDC, 0, 0, hIcon, vIcon, PATCOPY);
//      PatBlt(hMemDC, 0, 0, hSize, vSize, PATCOPY);
      for(int y=0; y<vSize; y++)
         for(int x=0; x<hSize; x++){
            color = BitMap[x+hSize*y];
            color = color == MARKER ? BACK : color;
            SetPixel(hMemDC, x, y, Colors[color]);
            }

      if(sizing) StretchBlt(hTrancDC, DistPortX, DistPortY,hsize,vsize,hMemDC,PortX,PortY,hSize,vSize,SRCCOPY);
      else BitBlt(hTrancDC, PortX, PortY, hSize, vSize, hMemDC,0,0,SRCCOPY);
      BitBlt(hDC, Icon.left, Icon.top, hIcon, vIcon, hTrancDC,0,0,SRCCOPY);
      }
   else {
      FillRect(hDC,&Icon,Brushes[BACK]);
      if(LastDigit){
        itoa(LastDigit, buf, 10);
        OldBkCol=SetBkColor(hDC,Colors[BACK]);
        OldFont = (HFONT)SelectObject(hDC,BigFont);
        DrawText(hDC, buf, strlen(buf), &Icon, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SetBkColor(hDC,OldBkCol);
        SelectObject(hDC, OldFont);
        }
      }
   SetViewportOrgEx(hDC, OldViewPort.x, OldViewPort.y, &OldViewPort);
   return hDC;
}

//////////////////////////////////////////////////////////////////////////////
////////////////////// Обработка сообщений ввода /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void PAINTING::RButtonDown(int x, int y, WORD)
{
   POINT c={x,y};
   if(PtInRect(&Fild,c)) PaintControl((x-Fild.left)/RectSize,(y-Fild.top)/RectSize);
   else if(PtInRect(&hDig,c)) hMakeNumb((x-hDig.left)/RectSize,(y-hDig.top)/RectSize, MARKER);
   else if(PtInRect(&vDig,c)) vMakeNumb((x-vDig.left)/RectSize,(y-vDig.top)/RectSize, MARKER);
}

void PAINTING::LButtonDown(int x, int y, WORD)
{
   POINT c={x,y};
   if(PtInRect(&Fild,c)) PaintControl((x-Fild.left)/RectSize,(y-Fild.top)/RectSize);
   else if(PtInRect(&hDig,c)) hMakeNumb((x-hDig.left)/RectSize,(y-hDig.top)/RectSize, BLACK);
   else if(PtInRect(&vDig,c)) vMakeNumb((x-vDig.left)/RectSize,(y-vDig.top)/RectSize, BLACK);
}

void PAINTING::MButtonDown(int x, int y, WORD)
{
   POINT c={x,y};
   if(PtInRect(&Fild,c)) PaintControl((x-Fild.left)/RectSize,(y-Fild.top)/RectSize);
   else if(PtInRect(&hDig,c)) hMakeNumb((x-hDig.left)/RectSize,(y-hDig.top)/RectSize, ZEROS);
   else if(PtInRect(&vDig,c)) vMakeNumb((x-vDig.left)/RectSize,(y-vDig.top)/RectSize, ZEROS);
}

void PAINTING::MouseMove(int x, int y, WORD flag)
{
   POINT a={x,y}, b={(x-WorkFild.x)/RectSize, (y-WorkFild.y)/RectSize};
   char Vol[]={"Знач.: "};
   char *buf=new char[sizeof(Vol)+17];
   strcpy(buf, Vol);
   int c;
   if(PtInRect(&Fild,a))
     {
     if((OldLightingPos.x!=b.x) || (OldLightingPos.y!=b.y))
     		{
       	LightPos(b, true);
       	OldLightingPos=b;
       	}
  	  if(flag & (MK_MBUTTON | MK_LBUTTON | MK_RBUTTON))
     		PaintControl((x-Fild.left)/RectSize,(y-Fild.top)/RectSize);
     }
   else
     {
     if(LightingErase)
       {
       LightPos(b, false);
       OldLightingPos.x=OldLightingPos.y=MAXSIZE+1;
       LightingErase=false;
       }
     if(GLOBAL::StatusBox)
       {
       if(PtInRect(&hDig,a))
          {
          c=(int)*(hData->Digits+(hExtSize-((y-hDig.top)/RectSize)-1)+((x-hDig.left)/RectSize)*hExtSize);
          if(c) itoa (c, buf+strlen(buf), 10);
          else strcpy(buf,"");
          }
       else if(PtInRect(&vDig,a))
          {
          c=(int)*(vData->Digits+((y-vDig.top)/RectSize)*vExtSize+(vExtSize-((x-vDig.left)/RectSize)-1));
          if(c) itoa (c, buf+strlen(buf), 10);
          else strcpy(buf,"");
          }
       else strcpy(buf,"");
       StatusBox->SetText(SB_CURSOR, buf);
       strcpy(buf,"");
       StatusBox->SetText(SB_CURSOR_BACK, buf);
       }
     }
   return;
}

void PAINTING::LightPos(POINT c, bool Lighting)
{
  char buf[17];

// Стираем старую подсветку
  if(LightingErase)
    {
    for(int y=0; y<hExtSize; y++)
       if(!Lighting || OldLightingPos.x!=c.x)
          ViewDigit(OldLightingPos.x, y, true, NULL, false);
    for(int x=0; x<vExtSize; x++)
       if(!Lighting || OldLightingPos.y!=c.y)
          ViewDigit(x, OldLightingPos.y, false, NULL, false);
    }
// Выходим если курсор вне рабочего поля
  if(!Lighting || MouseInput==CDIGITAL)
    {
    LightingErase = false;
    return;
    }
// Рисуем подсветку
  for(int y=0; y<hExtSize; y++)
    ViewDigit(c.x, y, true, NULL, true);
  for(int x=0; x<vExtSize; x++)
    ViewDigit(x, c.y, false, NULL, true);
  LightingErase = true;

// Обновляем в строке статуса текущую позицию курсора
  if(GLOBAL::StatusBox){
     itoa (c.x+1, buf, 10);
     strcpy(buf+strlen(buf)," x ");
     itoa (c.y+1, buf+strlen(buf), 10);
     StatusBox->SetText(SB_CURSOR, buf);

     itoa (c.x-hSize, buf, 10);
     strcpy(buf+strlen(buf)," x ");
     itoa (c.y-vSize, buf+strlen(buf), 10);
     StatusBox->SetText(SB_CURSOR_BACK, buf);
     }
}

void PAINTING::MakeElement(int x, int y, byte Color)
{
   int addr = y*hSize+x;
   if(MouseInput==CDIGITAL || LastCell == addr) return;
   if(!ButtonDown)
     {
     ButtonDown=true;
     DownButton();
     }

   byte a, *b = BitMap + addr;
   a = *b;
   if(Color == ZEROS){
   	if(*b!=ZEROS) *b = ZEROS;
      else *b = MARKER;
      }
   else if(Color == MARKER) *b = ZEROS;
   else if(*b != CurrentColor) *b = CurrentColor;
//   else *b = BACK;
   if(a != *b)
   	{
      ChangesInFile(false);
      MakeRect(x, y, false);
      if(TestFull()) Congratulation();
      }
   LastCell = addr;
}

void PAINTING::PaintControl(int x, int y)
{
	if(MouseInput == CDIGITAL) return;
   switch(MouseInput)
   	{
   	case CPEN:
   		switch(MOldButtonStatus){
//      		case MK_MBUTTON:
//         		MakeElement(x,y,MARKER);
//	            break;
   	   	case MK_RBUTTON:
      	   	MakeElement(x,y,ZEROS);
         	   break;
	      	case MK_LBUTTON:
   	      	MakeElement(x,y,CurrentColor);
      	      break;
         	}
      	break;
      case CRECT:
   		switch(MOldButtonStatus){
//      		case MK_MBUTTON:
//         		FillFieldRect(x,y,ZEROS);
//	            break;
   	   	case MK_RBUTTON:
      	   	FillFieldRect(x,y,ZEROS);
         	   break;
	      	case MK_LBUTTON:
   	      	FillFieldRect(x,y,CurrentColor);
      	      break;
         	}
      	break;
      case CEYE:
      	CurrentColor = BitMap[y*hSize+x];
         UpdatePalette();
         RestoreOldButtonState();
         int i;
   		for(i=0; i<hSize*vSize; i++) SaveMaps[i]=BitMap[i];
      	break;
      case CFILLED:
   		switch(MOldButtonStatus){
//      		case MK_MBUTTON:
//         		FilledColorRegions(x,y,MARKER);
//	            break;
   	   	case MK_RBUTTON:
      	   	FilledColorRegions(x,y,ZEROS);
         	   break;
	      	case MK_LBUTTON:
   	      	FilledColorRegions(x,y,CurrentColor);
      	      break;
         	}
      	break;
      }
}

void PAINTING::FillFieldRect(int x, int y, byte Color)   //Заливка полей
{
   if(MouseInput==CDIGITAL || LastCell == y*hSize+x || LastCell<0) return;
   int oldX = LastCell%hSize,
       oldY = LastCell/hSize;
   int StX = MIN(oldX, x), EnX = MAX(oldX, x);
   int StY = MIN(oldY, y), EnY = MAX(oldY, y);
   bool *ch = new bool[hSize*vSize];
   for(int i=0; i<hSize*vSize; i++)
   	if(BitMap[i]!=SaveMaps[i]){BitMap[i]=SaveMaps[i]; ch[i]=true;}
      else ch[i]=false;
   for(int Y = StY; Y <= EnY; Y++)
   	for(int X = StX; X <= EnX; X++)
         if(Y >= 0 && X >=0 && Y < vSize && X < hSize)
      		if(BitMap[Y*hSize+X]!=Color)
         		{
               BitMap[Y*hSize+X]=Color;
               ch[Y*hSize+X] = !ch[Y*hSize+X];
               ChangesInFile(false);
               }
	for(int i=0; i<hSize*vSize; i++)
   	if(ch[i]) MakeRect(i%hSize, i/hSize, false);
   delete[] ch;
   HDC hDC= GetDC(hWnd);
   PaintIconic(hDC);
   ReleaseDC(hWnd, hDC);
}

void PAINTING::FilledColorRegions(int x, int y, byte fillcolor)
{
   if(RegFilled) return;
	RegFilled = true;
	PatternColor = BitMap[y*hSize+x];
   if(PatternColor==fillcolor) return;

   PrepareUndo();
   byte svcolor = CurrentColor;
   CurrentColor = fillcolor;
   RegCompleteCount = RegCount = 0;
   RegAddr = new int[hSize*vSize];
   CheckCross(y*hSize+x);
   while(RegCount > RegCompleteCount)
      CheckCross(RegAddr[RegCompleteCount++]);
   CurrentColor = svcolor;
   ChangesInFile(false);
	delete[] RegAddr;
}

void PAINTING::CheckCross(int addr)
{
   int x=addr%hSize, y=addr/hSize;
   BitMap[addr]=CurrentColor; MakeRect(x, y, false);
   if(y>0 && BitMap[addr-hSize]==PatternColor)
   	{BitMap[addr-hSize]=CurrentColor; MakeRect(x, y-1, false); RegAddr[RegCount++]=addr-hSize;}
   if(y<vSize-1 && BitMap[addr+hSize]==PatternColor)
   	{BitMap[addr+hSize]=CurrentColor; MakeRect(x, y+1, false); RegAddr[RegCount++]=addr+hSize;}
   if(x>0 && BitMap[addr-1]==PatternColor)
   	{BitMap[addr-1]=CurrentColor; MakeRect(x-1, y, false); RegAddr[RegCount++]=addr-1;}
   if(x<hSize-1 && BitMap[addr+1]==PatternColor)
   	{BitMap[addr+1]=CurrentColor; MakeRect(x+1, y, false); RegAddr[RegCount++]=addr+1;}
   if(!DiagonalPotting) return; // заливка соседних по диагонали ячеек
   if(y>0 && x>0 && BitMap[addr-hSize-1]==PatternColor)
   	{BitMap[addr-hSize-1]=CurrentColor; MakeRect(x-1, y-1, false); RegAddr[RegCount++]=addr-hSize-1;}
   if(y>0 && x<hSize-1 && BitMap[addr-hSize+1]==PatternColor)
   	{BitMap[addr-hSize+1]=CurrentColor; MakeRect(x+1, y-1, false); RegAddr[RegCount++]=addr-hSize+1;}
   if(y<vSize-1 && x>0 && BitMap[addr+hSize-1]==PatternColor)
   	{BitMap[addr+hSize-1]=CurrentColor; MakeRect(x-1, y+1, false); RegAddr[RegCount++]=addr+hSize-1;}
   if(y<vSize-1 && x<hSize-1 && BitMap[addr+hSize+1]==PatternColor)
   	{BitMap[addr+hSize+1]=CurrentColor; MakeRect(x+1, y+1, false); RegAddr[RegCount++]=addr+hSize+1;}
}

void PAINTING::hMakeNumb  (int x, int y, byte ColFl)
{
   byte *Digit = hData->Digits+(hExtSize-y-1)+x*hExtSize,
   *DigitColor = hData->DigitColors+(hExtSize-y-1)+x*hExtSize;
   bool *DigitMarker = hData->DigitMarkers+(hExtSize-y-1)+x*hExtSize;
   int *PozKeyIn,CurrPoz;
   if(MouseInput==CDIGITAL){
//Находим оперируемый байт
      if(!ButtonDown)
        {
        ButtonDown=true;
        DownButton();
        }
      if(ColFl==ZEROS)   //Обнуление или установка цвета цифры, если нажата средняя кнопка мыши
         {
         if(Containers == COLORS)
         	if(*DigitColor != CurrentColor && *Digit) *DigitColor = CurrentColor;
         	else {*Digit=0; *DigitColor = BACK;}
         else *Digit=0;
         }/*
      else if(ColFl==BLACK)         //Увеличение, если нажата левая кнопка
         {
         if(*Digit<99) Digit[0]++;
         else *Digit=0;
         }
      else if(ColFl==MARKER)     //Уменьшение, если нажата правая кнопка
         {
         if(Digit[0]>0) Digit[0]--;
         else Digit[0]=99;
         }
      else return;*/
      if(Containers == COLORS)
         if(*Digit) *DigitColor = CurrentColor;
         else       *DigitColor = BACK;
      PozKeyIn = GetCurrPoz();
      CurrPoz = *PozKeyIn;
      ViewDigit((CurrPoz < hSize*hExtSize) ? CurrPoz/hExtSize : (CurrPoz-hSize*hExtSize)%vExtSize,
        (CurrPoz < hSize*hExtSize) ? CurrPoz%hExtSize : (CurrPoz-hSize*hExtSize)/vExtSize,
        (bool)(CurrPoz < hSize*hExtSize));
      *PozKeyIn = x*hExtSize+y;
      ViewDigit(x, y, true, NULL, true);

      ReleaseDC(hWnd,PaintIconic(GetDC(hWnd)));
      ChangeInTable(false, false);
      ChangesInFile(true);
      }
   else {
     	if(*Digit){
     		if(ColFl==BLACK)
         	if(MouseInput!=CEYE) *DigitMarker=!(*DigitMarker);//Инверсия маркера, если нажата левая кнопка мыши
        		else if(Containers==COLORS){
					CurrentColor = *DigitColor;
					UpdatePalette();
					RestoreOldButtonState();
   				}
     		ViewDigit(x, y, true);
        	}
		}
}

void PAINTING::vMakeNumb  (int x, int y, byte ColFl)
{
   byte *Digit = vData->Digits+y*vExtSize+(vExtSize-x-1),
   *DigitColor = vData->DigitColors+y*vExtSize+(vExtSize-x-1);
   bool *DigitMarker = vData->DigitMarkers+y*vExtSize+(vExtSize-x-1);
   int *PozKeyIn, CurrPoz;
   if(MouseInput==CDIGITAL){
      if(!ButtonDown)
        {
        ButtonDown=true;
        DownButton();
        }
      if(ColFl==ZEROS) //Обнуление или установка цвета цифры, если нажата средняя кнопка мыши
         {
         if(Containers == COLORS)
	         if(*DigitColor != CurrentColor && *Digit) *DigitColor = CurrentColor;
   	      else {*Digit=0; *DigitColor = BACK;}
         else *Digit=0;
         }/*
      else if(ColFl==BLACK)         //Увеличение, если нажата левая кнопка
        {
        if(Digit[0]<99) Digit[0]++;
        else Digit[0]=0;
        }
      else if(ColFl==MARKER)     //Уменьшение, если нажата правая кнопка
        {
        if(Digit[0]>0) Digit[0]--;
        else Digit[0]=99;
        }
      else return;*/
      if(Containers == COLORS)
         if(*Digit) *DigitColor = CurrentColor;
         else       *DigitColor = BACK;
      PozKeyIn = GetCurrPoz();
      CurrPoz = *PozKeyIn;
      ViewDigit((CurrPoz < hSize*hExtSize) ? CurrPoz/hExtSize : (CurrPoz-hSize*hExtSize)%vExtSize,
        (CurrPoz < hSize*hExtSize) ? CurrPoz%hExtSize : (CurrPoz-hSize*hExtSize)/vExtSize,
        (bool)(CurrPoz < hSize*hExtSize));
      *PozKeyIn = x+vExtSize*y+hExtSize*hSize;
      ViewDigit(x, y, false, NULL, true);

      ReleaseDC(hWnd,PaintIconic(GetDC(hWnd)));
      ChangeInTable(false, false);
      ChangesInFile(true);
      }
   else  {
     	if(*Digit){
     		if(ColFl==BLACK)
         	if(MouseInput!=CEYE) *DigitMarker=!(*DigitMarker);//Инверсия маркера, если нажата левая кнопка мыши
        		else if(Containers==COLORS){
					CurrentColor = *DigitColor;
					UpdatePalette();
					RestoreOldButtonState();
   				}
     		ViewDigit(x, y, false);
        	}
      }
}

void PAINTING::UpButton (int, int)
{
   RegFilled=ButtonDown=false;
   WaitUpdate=WAITUPDATE;
   ButtonUp=true;
   LastCell = -1;
   HDC hDC = GetDC(hWnd);
   PaintIconic(hDC);
   ReleaseDC(hWnd, hDC);
}

void PAINTING:: DownButton(void)
{
   ButtonUp=false;
   PrepareUndo();
}

void PAINTING::Timer(void)
{
   char Time[10];
   if(GLOBAL::StatusBox)
     if(!(WaitUpdateTime--))
       {
       WaitUpdateTime = WAITUPDATETIME;
       GetTime(Time);
       StatusBox->SetText(SB_TIME, Time);
       }
   if(!ButtonUp) return;
   if(--WaitUpdate) return;
   ButtonUp=false;
   if(!KeyIn())ChangeInTable(true, true);
}

void PAINTING::GetTime(char *Time)
{
   SYSTEMTIME LTime;
   GetLocalTime(&LTime);
   itoa((int)LTime.wHour, Time, 10);
   strcpy(Time+strlen(Time),":");
   if(LTime.wMinute<10) strcpy(Time+strlen(Time),"0");
   itoa((int)LTime.wMinute, Time+strlen(Time), 10);
   strcpy(Time+strlen(Time),":");
   if(LTime.wSecond<10) strcpy(Time+strlen(Time),"0");
   itoa((int)LTime.wSecond, Time+strlen(Time), 10);
}

void PAINTING::Normalize    (void)
{
   Multyple=0;
   SetScroll();
   EnableMenuItem(GetMenu(hWnd), IDM_MIN, MF_GRAYED);
   EnableMenuItem(GetMenu(hWnd), IDM_MAX, MF_ENABLED);
   InvalidateRect(hWnd,&Client,true);
}

#define STEPSIZING 3

void PAINTING::Maximize(void)
{
   if(Multyple==50) return;
   Multyple+=STEPSIZING;
   if(Multyple>50)
     {
     Multyple = 50;
     EnableMenuItem(GetMenu(hWnd), IDM_MAX, MF_GRAYED);
     }
   else EnableMenuItem(GetMenu(hWnd), IDM_MIN, MF_ENABLED);
   SetScroll();
   InvalidateRect(hWnd,&Client,TRUE);
}

void PAINTING::Minimize(void)
{
   if(!Multyple) return;
   Multyple-=STEPSIZING;
   if(Multyple<0){
     Multyple=0;
	  EnableMenuItem(GetMenu(hWnd), IDM_MIN, MF_GRAYED);
     }
   else EnableMenuItem(GetMenu(hWnd), IDM_MAX, MF_ENABLED);
   SetScroll();
   InvalidateRect(hWnd,&Client,true);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////// Члены-функции скроллинга ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void PAINTING::CreateScrollBar(void)
{
   RECT c;
   hHScroll = CreateWindowEx(0L, "SCROLLBAR", NULL, WS_CHILD|SBS_HORZ|SBS_BOTTOMALIGN,//|SBS_SIZEBOXBOTTOMRIGHTALIGN|SBS_SIZEGRIP,
                             Client.left, Client.top,
                             Client.right, Client.bottom,
                             hWnd, (HMENU) NULL, hinst,
                             (LPVOID) NULL);

   vHScroll = CreateWindowEx(0L, "SCROLLBAR", NULL, WS_CHILD|SBS_VERT|SBS_RIGHTALIGN,//|SBS_SIZEBOXBOTTOMRIGHTALIGN|SBS_SIZEGRIP,
                             Client.left, Client.top,
                             Client.right, Client.bottom,
                             hWnd, (HMENU) NULL, hinst,
                             (LPVOID) NULL);
   GetWindowRect(hHScroll, &c);
   SBV = c.bottom-c.top;
   GetWindowRect(vHScroll, &c);
   SBH = c.right-c.left;
   ClipY = ClipX = 0;
}

void PAINTING::SetScroll(void)
{
   bool swhscr = false;
   Calculate();

   si.cbSize=sizeof(SCROLLINFO);
   si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
   si.nMin=0;
   si.nMax=vTotalSize*RectSize;
   si.nPage=(UINT) (Client.bottom-Client.top);
   si.nPos=vSbPoz;
   vSbLim = vTotalSize*RectSize-(Client.bottom-Client.top)+1;
   SetScrollInfo(vHScroll,SB_CTL,&si,TRUE);
   MoveWindow(vHScroll,Client.right, Client.top, SBH, Client.bottom-Client.top, true);
   if((si.nMax-si.nMin-(int)si.nPage) > 0){
      swhscr = true;
      ShowScrollBar(vHScroll, SB_CTL, SW_SHOW);
      vScrollFlag=true;
      }
   else {
      ShowScrollBar(vHScroll, SB_CTL, SW_HIDE);
      vScrollFlag=false;
      }

//   si.cbSize=sizeof(SCROLLINFO);
//   si.fMask=SIF_RANGE | SIF_PAGE | SIF_POS;
//   si.nMin=0;
   si.nMax=hTotalSize*RectSize;
   si.nPage=(UINT) (Client.right - Client.left);
   si.nPos=hSbPoz;
   hSbLim = hTotalSize*RectSize-(Client.right-Client.left)+1;
   SetScrollInfo(hHScroll,SB_CTL,&si,TRUE);
   MoveWindow(hHScroll,Client.left, Client.bottom, Client.right-Client.left+(swhscr ? SBH : 0), SBV, true);
   if((si.nMax-si.nMin-(int)si.nPage) > 0) {
      ShowScrollBar(hHScroll, SB_CTL, SW_SHOW);
      hScrollFlag=true;
      }
   else {
      ShowScrollBar(hHScroll, SB_CTL, SW_HIDE);
      hScrollFlag=false;
      }
}

void PAINTING::hScroll(int sCode, short int nPos, bool wheel)
{
   int delta, OldPoz=hSbPoz;
   RECT movs, invalid;
	if(wheel) delta = nPos*RectSize;
   else switch(sCode)
     {
     case SB_LINELEFT:
        delta=-RectSize;
        break;
     case SB_LINERIGHT:
        delta=RectSize;
        break;
     case SB_PAGELEFT:
        delta=-(Client.right-Client.left);
        break;
     case SB_PAGERIGHT:
        delta=(Client.right-Client.left);
        break;
     case SB_THUMBPOSITION:
     case SB_THUMBTRACK:
        delta=0;
        hSbPoz=(int)nPos;
        break;
     default:
        return;
     }
   if(!hScrollFlag){
     hSbPoz -= delta;
     if(hSbPoz>0) hSbPoz=0;
     if(hSbPoz<hTotalSize*RectSize-(Client.right-Client.left)) hSbPoz=hTotalSize*RectSize-(Client.right-Client.left);
     movs=Border;
     Calculate();
     delta = Border.left-movs.left;
     ScrollRect(&movs, delta, 0); //movs - источник (delta полож - вверх)
     invalid = movs;
     if(delta < 0) {invalid.left=Border.right-1; invalid.right=movs.right;}
     else          {invalid.left=movs.left; invalid.right=Border.left+1;}
     InvalidateRect(hWnd,&invalid, true);
     return;
     }
   GetScrollInfo(hHScroll,SB_CTL,&si);
   hSbPoz  += delta;
   hSbPoz   = hSbPoz < 0 ? 0 : hSbPoz;
   hSbPoz   = hSbPoz > hSbLim ? hSbLim : hSbPoz;
   si.nPos  = hSbPoz;
   if(OldPoz!=hSbPoz){
      SetScrollInfo(hHScroll,SB_CTL,&si,TRUE);
      Calculate();
      invalid = movs = Client;
      delta=hSbPoz-OldPoz;
      if(delta>0){
//сдвигаем влево left+delta
        movs.left += delta;
        invalid.left = movs.right-delta;
        }
      else if(delta<0){
//сдвигаем вправо right+delta
        movs.right += delta;
        invalid.right = movs.left-delta;
        }
      else return;
      if(delta < Client.right) ScrollRect(&movs, -delta, 0);
      else invalid = Client;
      InvalidateRect(hWnd,&invalid,false);
      PaintProc();
      }
}

void PAINTING::vScroll(int sCode, short int nPos, bool wheel)
{
   int delta, OldPoz=vSbPoz;
   RECT movs, invalid;
   if(wheel) delta = nPos*RectSize;
   else switch(sCode)
     {
     case SB_LINEDOWN:
        delta=RectSize;
        break;
     case SB_LINEUP:
        delta=-RectSize;
        break;
     case SB_PAGEDOWN:
        delta=(Client.bottom-Client.top);
        break;
     case SB_PAGEUP:
        delta=-(Client.bottom-Client.top);
        break;
     case SB_THUMBPOSITION:
     case SB_THUMBTRACK:
        delta=0;
        vSbPoz=(int)nPos;
        break;
     default:
        return;
     }
   if(!vScrollFlag){
     vSbPoz -= delta;
     if(vSbPoz>0) vSbPoz=0;
     if(vSbPoz<vTotalSize*RectSize-(Client.bottom-Client.top)) vSbPoz=vTotalSize*RectSize-(Client.bottom-Client.top);
     movs=Border;
     Calculate();
     delta = Border.top-movs.top;
     ScrollRect(&movs, 0, delta); //movs - источник (delta полож - вверх)
     invalid = movs;
     if(delta < 0) {invalid.top=Border.bottom-1; invalid.bottom=movs.bottom;}
     else          {invalid.top=movs.top; invalid.bottom=Border.top+1;}
     InvalidateRect(hWnd,&invalid, true);
     return;
     }
   GetScrollInfo(vHScroll,SB_CTL,&si);
   vSbPoz  += delta;
   vSbPoz   = vSbPoz < 0 ? 0 : vSbPoz;
   vSbPoz   = vSbPoz > vSbLim ? vSbLim : vSbPoz;
   si.nPos  = vSbPoz;
   if(OldPoz!=vSbPoz){
      SetScrollInfo(vHScroll,SB_CTL,&si,TRUE);
      Calculate();
      invalid = movs = Client;
      delta=vSbPoz-OldPoz;
      if(delta>0){
//сдвигаем вверх top+delta
        movs.top += delta;
        invalid.top = movs.bottom-delta;
        }
      else if(delta<0){
//сдвигаем вниз bottom+delta
        movs.bottom += delta;
        invalid.bottom = movs.top-delta;
        }
      else return;
      if(delta < Client.bottom) ScrollRect(&movs, 0, -delta);
      else invalid = Client;
      InvalidateRect(hWnd,&invalid,false);
      PaintProc();
      }
}

void PAINTING::ScrollRect(RECT *Source, int x, int y)
{
   HDC hDC=GetDC(hWnd);
   BitBlt(hDC,Source->left+x,Source->top+y,Source->right-Source->left,
                                           Source->bottom-Source->top,
                             hDC,Source->left,Source->top,SRCCOPY);
   ReleaseDC(hWnd,hDC);
}

//////////////////////////////////////////////////////////////////////////////

