//kdata.cpp - определение класса работы с файлами
#include "kdata.h"
#include "bitmaps.h"
#include "print.h"
#include "icon.h"
//#include "clrmapp.h"
#include "PPIFF.h"

KROSSWORD::KROSSWORD (HINSTANCE hInstance, LPSTR lpCmLn, int CmdShow)
: CALCULATE (hInstance, lpCmLn, CmdShow)
{
   StartLetter=NULL;
   InputDigitRange=0;
	Containers=EMPTY;
   RedoFlag = UndoFlag = false;
   EndRedo = CurUndo = EndUndo = 0;
   Undo = new UNDOTAG[NUMBUNDO];
   Status=NOCHANGE;
   CurrPoz=0;
   WINMAIN::MessageProc();
}

void KROSSWORD::CreateKrossword()
   {
   MSG Msg;         // Удаляем из очереди сообщения нажатий кнопок мыши
/* WM_LBUTTONDOWN 513  // чтобы не портить рисунок (после открытия файла)
   WM_LBUTTONUP   514
   WM_RBUTTONDOWN 516
   WM_RBUTTONUP   517
   WM_MBUTTONDOWN 519
   WM_MBUTTONUP   520*/
   while(PeekMessage(&Msg, hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE));
   if(LibraryLoaded)
	   LoadesFromLybrary(NumberLibraryEntry);
  	if(GetDataLen())
     	FillData();
  	else ZeroData();
   if(Containers!=EMPTY)
   	{
     	EnableMenuItem(Menu,IDM_DELDIG,    MF_ENABLED);
     	EnableMenuItem(Menu,IDM_CLEARFIELD,MF_ENABLED);
     	EnableMenuItem(Menu,IDM_CORRECTED, MF_ENABLED);
     	EnableMenuItem(Menu,IDM_CALCULATE, MF_ENABLED);
      EnableMenuItem(Menu,IDM_MAKEOVERLAPED, MF_ENABLED);
     	EnableMenuItem(Menu,IDM_NORM      ,MF_ENABLED);
     	EnableMenuItem(Menu,IDM_MAX       ,MF_ENABLED);
     	EnableMenuItem(Menu,IDM_MIN       ,MF_GRAYED);
		EnableMenuItem(Menu,IDM_CLEARMASK ,MF_ENABLED);
     	EnableMenuItem(Menu,IDM_CLEARFIELD,MF_ENABLED);
     	EnableMenuItem(Menu,IDM_DELDIG    ,MF_ENABLED);
     	EnableMenuItem(Menu,IDM_STATUSBOX ,MF_ENABLED);
      }
   else
   	{
     	EnableMenuItem(Menu,IDM_DELDIG,    MF_GRAYED);
     	EnableMenuItem(Menu,IDM_CLEARFIELD,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_CORRECTED, MF_GRAYED);
     	EnableMenuItem(Menu,IDM_CALCULATE ,MF_GRAYED);
      EnableMenuItem(Menu,IDM_MAKEOVERLAPED, MF_GRAYED);
     	EnableMenuItem(Menu,IDM_NORM      ,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_MAX       ,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_MIN       ,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_CLEARMASK ,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_CLEARFIELD,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_DELDIG    ,MF_GRAYED);
     	EnableMenuItem(Menu,IDM_STATUSBOX ,MF_GRAYED);
      }
//   InvalidateRect(hWnd, NULL, true);
}

byte *KROSSWORD::InitializeMatrix(int len)
{
	byte *ptr = new byte[len];
   memset(ptr, 0, len);
   return ptr;
}

int *KROSSWORD::InitializeMatrixInt(int len)
{
	int *ptr = new int[len];
   memset(ptr, 0, len*sizeof(int));
   return ptr;
}

char *KROSSWORD::InitializeMatrixChar(int len)
{
	char *ptr = new char[len];
   memset(ptr, 0, len);
   return ptr;
}

void KROSSWORD::ZeroData (void)
{
  if(Containers!=EMPTY){
  	StopThread();
  	DelData();
   }
  Containers = (KROSSFEATURES::Colors > 1) ? COLORS : MONOCHROME;

// Every time is no changing
  SetCaptionWindow();
  MapLen=hSize*vSize;

  hDigitSize = hNumbSize*hSize;
  vDigitSize = vNumbSize*vSize;

  BitMap = InitializeMatrix(MapLen);// Allocate memory for Bit Maps
//  LetterMap = InitializeMatrix(MapLen);
  hData->Digits = InitializeMatrix(hDigitSize); // Allocate memory for
  vData->Digits = InitializeMatrix(vDigitSize); // digits arrays
  hData->DigitColors = InitializeMatrix(hDigitSize);
  vData->DigitColors = InitializeMatrix(vDigitSize);
  hData->DigitMarkers = new bool[hDigitSize];
  memset(hData->DigitMarkers, false, hDigitSize*sizeof(bool));
  vData->DigitMarkers = new bool[vDigitSize];
  memset(vData->DigitMarkers, false, vDigitSize*sizeof(bool));
  hData->Number = InitializeMatrixInt(hSize);// Allokate memory for listing numbers
  vData->Number = InitializeMatrixInt(vSize);// digits in line
  ScreenInitialize();
  CALCULATE::ResetData();
  NotifyCreatesFile();
  Status=CHANGES;
  RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
}

void KROSSWORD::DelData (bool delundo) //Release memory from data
{
  if(delundo)	KillUndo();
  *FileRemark=0;
  Containers = EMPTY;
  Status = NOCHANGE;
  DELETE(BitMap);
  DELETE(hData->Digits);
  DELETE(vData->Digits);
  DELETE(hData->DigitColors);
  DELETE(vData->DigitColors);
  DELETE(hData->DigitMarkers);
  DELETE(vData->DigitMarkers);
  DELETE(hData->Number);
  DELETE(vData->Number);
  delete StartLetter;
  StartLetter=NULL;
  DELETE (LetterMap);
}

void KROSSWORD::PasteBitmap(byte *FileData)
{
   if(Containers!=EMPTY){ StopThread(); PrepareUndo();}
   FilledBmpFile(FileData, false);
   CALCULATE::ResetData();
   InvalidateRect(hWnd, NULL, true);
}

void KROSSWORD::FillData (void)
{
  	static char ErrNosup[]={"Неподдерживаемый формат"}, ErrICO[]={"Неподходящая глубина цвета"};
   static char Err[]={"Файл кроссворда поврежден или имеет неверный формат"};
   static char Smile[]={"Решайте, бог Вам в помощь ;)"}, sOK[]={"OK"};
	StopThread();
  	byte *FileData=FILE_ROUTINE::GetData();
   int tmp;
  	if(!FileData) return;
  	FILE_TYPES ext = GetExtension();
  	if(ext==BMP_FILE)
  		{
   	if(Containers!=EMPTY)	DelData();
   	FilledBmpFile(FileData);
  		if(!ViewPictures){
      	ZeroMemory(BitMap, hSize*vSize);
      	STATUS(Smile);
         }
      else {STATUS(sOK);}
  		RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
      return;
   	}
  	else if(ext==JCR_FILE)
		{
  		if(FilledJcrFile(FileData))
   		{
		  	CALCULATE::ResetData();
  			SetCurrentInput(CPEN);
	   	RectSize=-1;
	   	ScreenInitialize();
			RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
  			if(ViewPictures || ExecDemo) {
         	Calc();
      		STATUS(sOK);
         	}
      	else {STATUS(Smile);}
		   return;
   	   }
	   if(!ExecDemo) errhandler(Err, hWnd);
      else STATUS(Err);
   	}
	else if(ext==JCC_FILE){
  		if((bool)(tmp=FilledJccFile(FileData)))
   		{
		  	CALCULATE::ResetData();
  			SetCurrentInput(CPEN);
	   	RectSize=-1;
	   	ScreenInitialize();
			RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
  			if(tmp==1 && (ViewPictures || ExecDemo)) {
         	Calc();
            STATUS(sOK);
            }
         else STATUS(Smile);
		   return;
   	   }
	   if(!ExecDemo) errhandler(Err, hWnd);
      else STATUS(Err);
      }
  	else if(ext==ICO_FILE)
  	{
   if(Containers!=EMPTY) StopThread();
   if(!FilledIcoFile(FileData))
  		{
     	if(!ExecDemo) errhandler(ErrICO, hWnd);
      else STATUS(ErrICO);
      }
   else {
	  	CALCULATE::ResetData();
  		SetCurrentInput(CPEN);
	   RectSize=-1;
   	ScreenInitialize();
		RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
      STATUS(sOK);
      return;
   	}
   }
  else if(ext==PPIFF1_FILE || GetExtension()==PPIFFS_FILE || GetExtension()==PPIFF2_FILE)
  	{
   if(Containers!=EMPTY) StopThread();
   if(Filled_PPIFF1_File(FileData))
  		{
      STATUS(sOK);
   	if((ViewPictures || ExecDemo) && (ext!=PPIFFS_FILE))
         Calc();
      else if(!ExecDemo) {STATUS(Smile);}
   	return;
      }
   else {
   	if(!ExecDemo) errhandler(Err, hWnd);
      else STATUS(Err);
		}
   }
  else {
  		if(!ExecDemo) errhandler(ErrNosup, hWnd);
      else STATUS(Err);
      }
  	RectSize = 10;
	hNumbSize = vNumbSize = 4;
	hSize = vSize = 20;
	ZeroData();
  	CALCULATE::ResetData();
  	SetCurrentInput(CPEN);
  	ScreenInitialize();
  	RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
}

bool KROSSWORD::FilledIcoFile(byte *FileData)
{
	if(Containers!=EMPTY) PrepareUndo();
   MICON mi((LPVOID)FileData, KROSSFEATURES::This);
   if(mi.error)
   	{
      if(Containers!=EMPTY) {Undo_(true); return true;}
      else return false;
      }
	if(Containers!=EMPTY) KillUndo();
   for(int image=(int)mi.numb-1; image >=0; image--)
     	{
	  	if(Containers!=EMPTY) DelData(false);
		Status=NOCHANGE;          // Every time is no changing
   	if(!mi.GetImage((WORD)image)) return false;
	  	Containers = KROSSFEATURES::Colors > 1 ? COLORS : MONOCHROME;
		MapLen = vSize*hSize;

		hData->Number = new int[hSize];
  		vData->Number = new int[vSize];
  		hData->DigitMarkers = vData->DigitMarkers = NULL;
      hData->DigitColors = vData->DigitColors = hData->Digits = vData->Digits = NULL;

  		Corrected(false);
	   if((!hData->Digits) || (!vData->Digits)) continue;

  		if(!ViewPictures)
   		FillMemory(BitMap, hSize*vSize, ZEROS);
  	   if(image) PrepareUndo();
     	}
  	if(mi.numb > 1) STATUS("Воспользуйтесь командой ОТМЕНИТЬ.");
   return true;
}

bool KROSSWORD::Filled_PPIFF1_File (byte *pointer)
{
   if(Containers!=EMPTY) DelData();
	PPIFF ppf(this, pointer, FILE_ROUTINE::GetDataLen());
   if(ppf.faithfully) return false;

   GLOBAL::Colors=Containers==MONOCHROME ? 1 : MAXCOLOR;
   Status=NOCHANGE;
  	ChangeInTable(true, false);

  	CALCULATE::ResetData();
  	SetCurrentInput(CPEN);
   RectSize=-1;
   ScreenInitialize();
	RedrawWindow(hWnd,NULL,NULL,RDW_ERASE | RDW_INVALIDATE);
//   if(FileRemark) MessageBox(hWnd, FileRemark, "Комментарий.", MB_OK | MB_ICONINFORMATION);
   return true;
}

bool KROSSWORD::FilledJcrFile (byte *pointer)
{
  	if(Containers!=EMPTY) DelData();
	JCR_FILE_HEADER *file = (JCR_FILE_HEADER *) pointer;
  	hSize=(int)file->hSize;         // Fill size
  	vSize=(int)file->vSize;
  	hNumbSize = (int)(file->hNumbSize & 0x7f);
   vNumbSize = (int)(file->vNumbSize & 0x7f);

//   bool newformat =  (bool)(file->hNumbSize & 0x80);
   bool colors = (file->hNumbSize & 0x80);
   Containers=colors?COLORS:MONOCHROME;

  	Status=NOCHANGE;          // Every time is no changing
  	MapLen=hSize*vSize;
  	int i;

  	LPBYTE ptr = file->Digits;	// Указатель на оцифровку

//Заполнение палитры
  	ResetPalette(Containers==MONOCHROME);

//Показатели количества цифр оцифровки в строке
  	hData->Number=InitializeMatrixInt(hSize);// Allokate memory for listing numbers
  	vData->Number=InitializeMatrixInt(vSize);// digits in line

//Выделение памяти под массивы оцифровки кроссворда
  	hDigitSize = hNumbSize*hSize;
  	vDigitSize = vNumbSize*vSize;
  	hData->Digits = InitializeMatrix(hDigitSize); // Allocate memory
  	vData->Digits = InitializeMatrix(vDigitSize);
  	hData->DigitColors = InitializeMatrix(hDigitSize);
  	vData->DigitColors = InitializeMatrix(vDigitSize);

  	hData->DigitMarkers = new bool[hDigitSize];
  	vData->DigitMarkers = new bool[vDigitSize];
	memset(hData->DigitMarkers, false, hDigitSize*sizeof(bool));
	memset(vData->DigitMarkers, false, vDigitSize*sizeof(bool));

//Заполнение массивов оцифровки данными из файла
   if(!colors)
   	{
	  	for(i=0; i<hSize; i++)
   		for(int j=0; j<hNumbSize; j++)
				hData->Digits[i*hNumbSize+j]=*(ptr++);
	  	for(i=0; i<vSize; i++)
   		for(int j=0; j<vNumbSize; j++)
      	   vData->Digits[vNumbSize*vSize-((i+1)*vNumbSize)+j]=*(ptr++);
		}
	else  						// заливка цветного формата
   	{  						// заливаем оцифровку
	  	for(i=0; i<hSize; i++)                                // счетчик столбцов
  			for(int j=hData->Number[i]=(int)*(ptr++); j>0; j--)
            {
            if(j>hNumbSize) return false;
        	   hData->Digits[(i+1)*hNumbSize-j]=*(ptr++);
            }
	  	for(i=0; i<vSize; i++)                                // счетчик строк
  			for(int j=vData->Number[i]=(int)*(ptr++); j>0; j--)
            {
            if(j>vNumbSize) return false;
        	   vData->Digits[(i+1)*vNumbSize-j]=*(ptr++);
            }
									// заливаем цвета оцифровки
	  	for(i=0; i<hSize; i++)                                // счетчик столбцов
  			for(int j=hData->Number[i]; j>0; j--)
            {
            if(j>hNumbSize) return false;
         	hData->DigitColors[(i+1)*hNumbSize-j] = (byte)((*ptr++)+FIRST);
            }
	  	for(i=0; i<vSize; i++)                                // счетчик строк
  			for(int j=vData->Number[i]; j>0; j--)
            {
            if(j>vNumbSize) return false;
            vData->DigitColors[(i+1)*vNumbSize-j] = (byte)((*ptr++)+FIRST);
            }
		}
//   LetterMap = InitializeMatrix(MapLen);
  	BitMap = InitializeMatrix(MapLen);// Allocate memory for Bit Maps
  	ChangeInTable(true, false);
   return true;
}

int KROSSWORD::FilledJccFile (byte *pointer)
{
  	if(Containers!=EMPTY) DelData();
	JCC_FILE_DATA *file = (JCC_FILE_DATA *) pointer;
  	hSize			= (int)file->JCC_hSize;
  	vSize			= (int)file->JCC_vSize;
  	hNumbSize 	= (int)file->JCC_hNumbSize;
   vNumbSize 	= (int)file->JCC_vNumbSize;
   bool colors = (file->JCC_Colors > 2 ? true : false);
   Containers 	= colors ? COLORS : MONOCHROME;
  	Status=NOCHANGE;          // Every time is no changing
  	MapLen=hSize*vSize;
  	int i, j;

  	JCC_DD *ptr = file->JCC_Digit;	// Указатель на оцифровку

//Заполнение палитры
  	ResetPalette(Containers==MONOCHROME);
   byte ConvertTable[16];
   ConvertTable[0] = BACK;
   ConvertTable[1] = BLACK;
   if(Containers==COLORS){
	   for(i=1; i<16; i++){
   		ConvertTable[i]=GetNearestColorIndex(file->JCC_ColorTable[i]);
         if(ConvertTable[i]==BACK) ConvertTable[i]=18;
         }
      }
//Показатели количества цифр оцифровки в строке
  	hData->Number=InitializeMatrixInt(hSize);// Allokate memory for listing numbers
  	vData->Number=InitializeMatrixInt(vSize);// digits in line

//Выделение памяти под массивы оцифровки кроссворда
  	hDigitSize = hNumbSize*hSize;
  	vDigitSize = vNumbSize*vSize;
  	hData->Digits = InitializeMatrix(hDigitSize); // Allocate memory
  	vData->Digits = InitializeMatrix(vDigitSize);
  	hData->DigitColors = InitializeMatrix(hDigitSize);
  	vData->DigitColors = InitializeMatrix(vDigitSize);

  	hData->DigitMarkers = new bool[hDigitSize];
  	vData->DigitMarkers = new bool[vDigitSize];
	memset(hData->DigitMarkers, false, hDigitSize*sizeof(bool));
	memset(vData->DigitMarkers, false, vDigitSize*sizeof(bool));
   byte tmp; int c;
//Заполнение массивов оцифровки данными из файла
	for(j=0; j<hSize; j++)
	  	for(i=0; i<hNumbSize; i++){
			if(ConvertTable[ptr[i+hNumbSize*j].Color]!=18){
	         c = (hNumbSize-i-1)+hNumbSize*j;
      	   tmp = ptr[i+hNumbSize*j].Digit;
   	      hData->DigitColors[c]	= ConvertTable[ptr[i+hNumbSize*j].Color];
         	hData->Digits[c] 			= (byte)(tmp&0x7f);
		      hData->DigitMarkers[c] 	= (bool)(tmp&0x80);
   	      }
         }
   ptr += (hSize*hNumbSize);
	for(j=0; j<vSize; j++)
	  	for(i=0; i<vNumbSize; i++){
			if(ConvertTable[ptr[i+vNumbSize*j].Color]!=18){
	         c = (vNumbSize-i-1)+vNumbSize*j;
   	      tmp = ptr[i+vNumbSize*j].Digit;
      	   vData->DigitColors[c]	= ConvertTable[ptr[i+vNumbSize*j].Color];
         	vData->Digits[c] 			= (byte)(tmp&0x7f);
		      vData->DigitMarkers[c] 	= (bool)(tmp&0x80);
            }
         }
   byte *bptr = (byte *)(ptr+(vSize*vNumbSize));
  	BitMap = InitializeMatrix(MapLen);// Allocate memory for Bit Maps
  	ChangeInTable(true, false);
   if(!file->JCC_Containers)
      for(i=0; i<MapLen; i++)
         if(bptr[i] == 0xff) BitMap[i]=MARKER;
      	else if(ConvertTable[bptr[i]]==18) BitMap[i]=ZEROS;
         else BitMap[i]=ConvertTable[bptr[i]];
   else return 1;
   return 2;
}

/*
void KROSSWORD::FilledJpnFile (byte *pointer)
{
  if(Containers!=EMPTY) DelData();
  FILE_HEADER *file = (FILE_HEADER *) pointer;
  if(file->FileId != 0x504a) return; //ASCII 'JP'
  Containers = (file->NumbColors > 1) ? COLORS : MONOCHROME;
  hSize=file->hSize;         // Fill size
  vSize=file->vSize;
  hNumbSize=file->hNumbSize;
  vNumbSize=file->vNumbSize;
  Status=NOCHANGE;          // Every time is no changing
  MapLen=hSize*vSize;
  int i;
  DWORD Offset=0;

//Показатели количества цифр оцифровки в строке
  hData->Number=new int[hSize];// Allokate memory for listing numbers
  vData->Number=new int[vSize];// digits in line

//Выделение памяти под массивы оцифровки кроссворда
  hDigitSize = hNumbSize*hSize;
  vDigitSize = vNumbSize*vSize;
  hData->Digits = new byte[hDigitSize]; // Allocate memory for
  vData->Digits = new byte[vDigitSize]; // digits
  hData->DigitColors = new byte[hDigitSize]; // Allocate memory for
  vData->DigitColors = new byte[vDigitSize]; // digit color arrays
  ZeroMemory((LPVOID)hData->DigitColors, (DWORD)hDigitSize); //Zeros colors array
  ZeroMemory((LPVOID)vData->DigitColors, (DWORD)vDigitSize); //

  ResetPalette(true);
  KROSSFEATURES::Colors=(file->NumbColors > 1) ? file->NumbColors : 0;
  if(Containers == COLORS)
    {
    for(i=0; i<MAXCOLOR; i++) //Восстанавливаем цвета
      {
      ColorTable[i+FIRST] = *(LPDWORD)(file->Data+Offset);
      Offset += (DWORD)sizeof(DWORD);
      }
    }

//Заполнение массивов оцифровки данными из файла
  for(i=0; i<hSize; i++)
     for(int j=0; j<hNumbSize; j++)
        {
        hData->Digits[i*hNumbSize+j]=file->Data[Offset++];
        hData->DigitColors[i*hNumbSize+j]=file->Data[Offset++];
        }
  for(i=0; i<vSize; i++)
     for(int j=0; j<vNumbSize; j++)
        {
		  vData->Digits[i*vNumbSize+j]=file->Data[Offset++];
		  vData->DigitColors[i*vNumbSize+j]=file->Data[Offset++];
		  }

// Заполнение битовой карты
  BitMap = new byte[MapLen];// Allocate memory for Bit Maps
  Offset = (DWORD)(file->OffsetBitmap-(sizeof(FILE_HEADER)-1));
  if(file->OffsetBitmap && ViewPictures)
	  for(i=0; i<MapLen; i++) BitMap[i]=file->Data[Offset++];//Copy BitMap
  else FillMemory(BitMap, (DWORD)MapLen, ZEROS);           //Zero BitMap

  SetCurrentInput((file->Flags & PAINTINGFLAG) ? CPEN : CDIGITAL);
  ChangeInTable(true, false);
} */

bool KROSSWORD::FilledBmpFile (byte *pointer, bool killundo)
{
   static char ErrBig[]={"Ширина и высота рисунка не должна превышать 254 пикселей"}, ErrZip[]={"Сжатие BMP-формата неподдержано программой"};
  	if(Containers!=EMPTY) DelData(killundo);
  	FILEBITMAPINFO *BMP = (FILEBITMAPINFO *)pointer;
  	Status=NOCHANGE;          // Every time is no changing
  	bool UpToDown = BMP->Bh.biHeight>0 ? false : true;
  	int x,y,i;
  	byte pixel;

  	hNumbSize = 0;
  	vNumbSize = 0;
  	hSize = (int)BMP->Bh.biWidth;
  	vSize = (int)(BMP->Bh.biHeight>0 ? BMP->Bh.biHeight : -(BMP->Bh.biHeight));
  	MapLen = vSize*hSize;

  	int CountBits = BMP->Bh.biBitCount;// Количество бит на пиксел
   int NumbColor;
   if(!(int)BMP->Bh.biClrUsed)
  			NumbColor = ((byte)0x01 << CountBits);
   else 	NumbColor = (int)BMP->Bh.biClrUsed;
  	if(NumbColor == 2) NumbColor--;
  	Containers = (CountBits > 1) ? COLORS : MONOCHROME;

  	if((BMP->Bh.biCompression!=BI_RGB)||
		BMP->Bh.biWidth>MAXSIZE || vSize>MAXSIZE)
	{
		Containers = EMPTY;
      ResetPalette(true);
   	RectSize = 10;
	   hNumbSize = vNumbSize = 4;
   	hSize = vSize = 20;
		ZeroData();
   	InvalidateRect(hWnd,NULL,true);
      if(BMP->Bh.biCompression!=BI_RGB)errhandler(ErrZip, hWnd);
		else 										errhandler(ErrBig, hWnd);
		return false;
    }

 	hData->Number = new int[hSize];
	vData->Number = new int[vSize];
	hData->DigitColors = vData->DigitColors = hData->Digits = vData->Digits = NULL;
   ResetPalette(false);// обнуляем палитру
   BitMap = InitializeMatrix(MapLen);
	int NumbPixelInByte = 8/CountBits;
							// выравниваем данные на DWORD
	int hSizeLnInLong = (hSize*CountBits+31)/32;
	byte *MapPointer = InitializeMatrix((hSize+7)*vSize);

  	if(BMP->Bh.biBitCount >= 24)
   	{
      byte *Data = pointer+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
      for(int v=vSize-1; v>=0; v--)
      	for(int h=0; h<hSize; h++)
	      	{
      	   RGBQUAD rq;
            if(BMP->Bh.biBitCount == 24){
            		int i=(vSize-v-1)*((hSize*3+3)/4*4)+h*3;
            		rq.rgbRed = Data[i+2];
                  rq.rgbGreen = Data[i+1];
                  rq.rgbBlue = Data[i];
                  }
            else {int i=(vSize-v-1)*(hSize*4)+h*4;
            		rq.rgbRed = Data[i+2];
                  rq.rgbGreen = Data[i+1];
                  rq.rgbBlue = Data[i];
                  }
         	MapPointer[v*hSize+h] = GetNearestColorIndex(rq);
	         }
      }
   else if(BMP->Bh.biBitCount == 8)
      {
      RGBQUAD *colortable = (RGBQUAD *)(pointer+sizeof(BITMAPFILEHEADER)
      									+sizeof(BITMAPINFOHEADER));
      byte *Data = pointer+sizeof(BITMAPFILEHEADER)
      									+sizeof(BITMAPINFOHEADER)
                                 +NumbColor*sizeof(RGBQUAD);
      for(int v=vSize-1; v>=0; v--)
      	for(int h=0; h<hSize; h++)
	      	{
   	      int i=(vSize-v-1)*((hSize+3)/4*4)+h;
         	MapPointer[v*hSize+h] = GetNearestColorIndex(colortable[Data[i]]);
	         }
      }
	else if(BMP->Bh.biBitCount <= 4)
   	{
		// Заполняем цветовую палитру
  		KROSSFEATURES::Colors = (CountBits>1 ? MAXCOLOR : 1);
      ResetPalette(CountBits > 1 ? false : true);// обнуляем палитру
		// Заполняем матрицу
							// соотношение бит на пиксел в байте
  		byte rotsave, operand, mask = (byte)((CountBits>1) ? 0xf : 0x1);
  		byte *oper = ((byte *)pointer)+BMP->Fh.bfOffBits;// Начало таблицы индексов

  		for(y=0; y<vSize; y++)				// строки
		for(x=0; x<hSizeLnInLong; x++)   // размер в LONG
	 	for(int l=0; l<4 ;l++)          // байт в LONG
			{
		 	rotsave = *oper++;
		 	operand = 0;
		 	if(CountBits>1)
				{
				operand = (byte)(rotsave & 0xf);
				rotsave >>= 4;
				operand <<= 4;
				operand |= (byte)(rotsave & 0xf);
				}
		 	else for(int n=0; n<8; n++)	//переворачиваем байт
				{
				operand <<= 1;
				if(rotsave & 0x1) operand|=0x1;
				rotsave >>= 1;
				}
		 	for(i=0; i<NumbPixelInByte; i++)// пикселей в байте
				{
				pixel = (operand & mask);
            MapPointer[y*hSize+x*4*NumbPixelInByte+l*NumbPixelInByte+i] =
            		GetNearestColorIndex(BMP->Colors[pixel]);
				operand >>= CountBits;
				}
		 	}
      UpToDown=!UpToDown;
		}
	// переворачиваем, если требуется, картинку
	if(!UpToDown) memcpy(BitMap, MapPointer, MapLen);
//		for(i=0; i<vSize; i++)
//			for(x=0; x<hSize; x++)
//				BitMap[i*hSize+x]=MapPointer[i*hSize+x];
	else for(i=0; i<vSize; i++)
		for(x=0; x<hSize; x++)
			BitMap[(vSize-i-1)*hSize+x]=MapPointer[i*hSize+x];
	delete[]MapPointer;
  	Corrected(false);
  	return true;
}

bool KROSSWORD::SaveBmpFiles(bool saveas) //bool matrix
{
   HANDLE hf;
	DWORD dwTmp;

   if(!saveas)
   	{
      if(!GetCreatesFile()) return false;
      BITMAPS bmi(KROSSFEATURES::This, NULL);
      DWORD FileLen = ((BITMAPFILEHEADER *)bmi.FileInMemory)->bfSize;
	 	hf = CreateFile(LastFile, GENERIC_READ | GENERIC_WRITE,
						 (DWORD) 0, (LPSECURITY_ATTRIBUTES) NULL,
						 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	 	if (hf == INVALID_HANDLE_VALUE) {errhandler("CreateFile", hWnd); return false;}
	 	if(!WriteFile(hf, (LPVOID) bmi.FileInMemory, FileLen,
						(LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
		 				errhandler("WriteFile", hWnd);
   	if (!CloseHandle(hf))
			  			errhandler("CloseHandle", hWnd);
      return true;
      }

//   STATUS("Сохранение рисунка.");
	BITMAPS bmi(KROSSFEATURES::This, &lf);
   if(bmi.error) return false;
   if(bmi.FileInMemory)
   	{
      DWORD FileLen = ((BITMAPFILEHEADER *)bmi.FileInMemory)->bfSize;
	 	hf = CreateFile(LastFile, GENERIC_READ | GENERIC_WRITE,
						 (DWORD) 0, (LPSECURITY_ATTRIBUTES) NULL,
						 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	 	if (hf == INVALID_HANDLE_VALUE) {errhandler("CreateFile", hWnd); return false;}
	 	if(!WriteFile(hf, (LPVOID) bmi.FileInMemory, FileLen,
						(LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
		 				errhandler("WriteFile", hWnd);
   	if (!CloseHandle(hf))
			  			errhandler("CloseHandle", hWnd);
      return true;
      }

	WORD cClrBits = (WORD)(bmi.bmp.bmPlanes * bmi.bmp.bmBitsPixel);
	if (cClrBits == 1) 			cClrBits = 1;
	else if (cClrBits <= 4)		cClrBits = 4;
	else if (cClrBits <= 8)		cClrBits = 8;
	else if (cClrBits <= 16)  	cClrBits = 16;
	else if (cClrBits <= 24)  	cClrBits = 24;
	else                      	cClrBits = 32;

	DWORD FileSize = (DWORD)(sizeof(BITMAPFILEHEADER)
					  + sizeof(BITMAPINFOHEADER)+(2^cClrBits) * sizeof(RGBQUAD)
					  + ((bmi.bmp.bmWidth+7)/8 * bmi.bmp.bmHeight * cClrBits));

   BITMAPFILEHEADER *hdr = (BITMAPFILEHEADER *) LocalAlloc(LPTR, sizeof(BITMAPFILEHEADER));
	LPBITMAPINFO pbmi;
	LPBYTE lpBits;

// Заполняем BITMAPFILEHEADER структуру
	hdr->bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
	hdr->bfSize = FileSize;
	hdr->bfReserved1 = 0;
	hdr->bfReserved2 = 0;


//Распределите память для BITMAPINFO структуры. (Эта структура содержит структуру
//BITMAPINFOHEADER и массив RGBQUAD структур данных.)
	 if (cClrBits != 24)	pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (2^cClrBits));

//Нет RGBQUAD массива для 24-битовый-за-пиксель формата.
	 else 				pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

// Проинициализируйте области в BITMAPINFO структуре.
	 pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	 pbmi->bmiHeader.biWidth = bmi.bmp.bmWidth;
	 pbmi->bmiHeader.biHeight = bmi.bmp.bmHeight;
	 pbmi->bmiHeader.biPlanes = 1;
	 pbmi->bmiHeader.biBitCount = cClrBits;
	 if (cClrBits < 24)	pbmi->bmiHeader.biClrUsed = 2^cClrBits;

// Если побитовое отображение не сжимается, установите флаг BI_RGB.
	 pbmi->bmiHeader.biCompression = BI_RGB;

// Вычислите количество байтов в массиве цветных индексов
// и сохраните результат в biSizeImage.

	 pbmi->bmiHeader.biSizeImage = (((pbmi->bmiHeader.biWidth+7)/8+3)/4)*4
											 * pbmi->bmiHeader.biHeight
											 * cClrBits;

//	Установим biClrImportant в 0, указав,	что все цвета используются.
	 pbmi->bmiHeader.biClrImportant = pbmi->bmiHeader.biClrUsed;

	 DWORD cb;                   // incremental count of bytes
	 BYTE *hp;                   // byte pointer

	 lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbmi->bmiHeader.biSizeImage);
	 if (!lpBits)
		  {
   	  LocalFree((HLOCAL)hdr);
   	  LocalFree((HLOCAL)pbmi);
		  errhandler("GlobalAlloc", hWnd);
		  return false;
        }

// Извлеките цветную таблицу (RGBQUAD массив)
// и биты (массив индексов палитры) из DIB.

	 if (!GetDIBits(bmi.hMemDC, bmi.hbmp, 0, (WORD) bmi.bmp.bmHeight,
						 lpBits, pbmi, DIB_RGB_COLORS))
		  {
        GlobalFree((HGLOBAL)lpBits);
   	  LocalFree((HLOCAL)hdr);
   	  LocalFree((HLOCAL)pbmi);
		  errhandler("GetDIBits", hWnd);
		  return false;
        }

	 // Create the .BMP file.

	 hf = CreateFile(LastFile, GENERIC_READ | GENERIC_WRITE,
						 (DWORD) 0,
						 (LPSECURITY_ATTRIBUTES) NULL,
						 CREATE_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL,
						 (HANDLE) NULL);

	 if (hf == INVALID_HANDLE_VALUE)
		  {
        GlobalFree((HGLOBAL)lpBits);
   	  LocalFree((HLOCAL)hdr);
   	  LocalFree((HLOCAL)pbmi);
		  errhandler("CreateFile", hWnd);
		  return false;
        }
// Вычислите компенсацию в массиве цветных индексов.

	 hdr->bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
						  pbmi->bmiHeader.biSize + pbmi->bmiHeader.biClrUsed
						  * sizeof (RGBQUAD);

// Скопируйте BITMAPFILEHEADER в файл .BMP.

	 if (!WriteFile(hf, (LPVOID) hdr, sizeof(BITMAPFILEHEADER),
		 (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
		 errhandler("WriteFile", hWnd);

// Скопируйте BITMAPINFOHEADER и RGBQUAD массив в файл.

	 if (!WriteFile(hf, (LPVOID) pbmi, sizeof(BITMAPINFOHEADER)
						+ pbmi->bmiHeader.biClrUsed * sizeof (RGBQUAD),
						(LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
		  {
        GlobalFree((HGLOBAL)lpBits);
   	  LocalFree((HLOCAL)hdr);
   	  LocalFree((HLOCAL)pbmi);
        CloseHandle(hf);
		  errhandler("WriteFile", hWnd);
		  return false;
        }

// Скопируйте массив цветных индексов в файл .BMP.

    cb = pbmi->bmiHeader.biSizeImage;
	 hp = lpBits;
#define MAXWRITE 20000
	 while (cb > MAXWRITE) {
				if (!WriteFile(hf, (LPSTR) hp, (int) MAXWRITE,
								  (LPDWORD) &dwTmp, (LPOVERLAPPED) NULL))
					 errhandler("WriteFile", hWnd);
				cb -= MAXWRITE;
				hp += MAXWRITE;
	 }
	 if (!WriteFile(hf, (LPSTR) hp, (int) cb,
			(LPDWORD) &dwTmp, (LPOVERLAPPED) NULL)) errhandler("WriteFile", hWnd);

	 if (!CloseHandle(hf)) errhandler("CloseHandle", hWnd);

// Освобождаем память.
	GlobalFree((HGLOBAL)lpBits);
   LocalFree((HLOCAL)hdr);
   LocalFree((HLOCAL)pbmi);
   return true;
}

bool KROSSWORD::SavePPIFFFile()
{
   if(!GetCreatesFile()) return false;
	PPIFF ppf(this, FILE_ROUTINE::GetData(), 0);

   FILE_ROUTINE *fr = FILE_ROUTINE::This;
   DELETE(fr->FileData);
	FILE_TYPES type=GetExtension();
   if(type!=PPIFFS_FILE)
      {
   	if(Containers==COLORS && type!=PPIFF2_FILE)
      	strcat(LastFile,"c");
      ppf.FileLen=ppf.PPFileLen;
      }
   fr->FileData = new byte[ppf.FileLen];
   fr->FileDataLen = ppf.FileLen;
   memcpy((LPVOID)FileData,(LPVOID)ppf.FilePtr, ppf.FileLen);
   return true;
}

bool KROSSWORD::SaveJcrFile(void)
{
   if(!GetCreatesFile()) return false;
   int j, i, BuffSize=hSize*hNumbSize+vSize*vNumbSize+4;
   if(Containers==COLORS)
      BuffSize=BuffSize*2+hSize+vSize;
	ChangeDataLen(BuffSize);
  	JCR_FILE_HEADER *jcrfh = (JCR_FILE_HEADER *)FILE_ROUTINE::GetData();

//Записываем размер
  	jcrfh->hSize=(byte)hSize;
   jcrfh->vSize=(byte)vSize;
  	jcrfh->hNumbSize=(byte)hNumbSize;
   jcrfh->vNumbSize=(byte)vNumbSize;
	LPBYTE ptr = jcrfh->Digits;
   ChangeInTable(true, false);
   int offset = hSize*hNumbSize;
	if(Containers!=COLORS)
   	{
//Записываем оцифровку
	  	for(i=0; i<hSize; i++)
   	   for(j=0; j<hNumbSize; j++)
            jcrfh->Digits[i*hNumbSize+j]=hData->Digits[i*hNumbSize+j];
	  	for(i=0; i<vSize; i++)
   	   for(j=0; j<vNumbSize; j++)
            jcrfh->Digits[i*vNumbSize+j+offset]=
            vData->Digits[vNumbSize*vSize-((i+1)*vNumbSize)+j];
      }
	else
   	{
  		jcrfh->hNumbSize|=0x80;
      jcrfh->vNumbSize|=0x80;

// заливаем оцифровку
	  	for(i=0; i<hSize; i++){                               // счетчик столбцов
         *(ptr++) = (byte)hData->Number[i];
  			for(int j=0; j<hData->Number[i]; j++)
        	   *(ptr++) = hData->Digits[i*hNumbSize+j];
         }
	  	for(i=0; i<vSize; i++){                               // счетчик строк
         *(ptr++) = (byte)vData->Number[i];
  			for(int j=0; j<vData->Number[i]; j++)
        	   *(ptr++) = vData->Digits[i*vNumbSize+j];
         }

// заливаем цвета оцифровки
	  	for(i=0; i<hSize; i++)                                // счетчик столбцов
  			for(int j=0; j<hData->Number[i]; j++)
            *(ptr++) = (byte)(hData->DigitColors[i*hNumbSize+j]-FIRST);
	  	for(i=0; i<vSize; i++)                                // счетчик строк
  			for(int j=0; j<vData->Number[i]; j++)
				*(ptr++) = (byte)(vData->DigitColors[i*vNumbSize+j]-FIRST);
      int datalen = (int)(ptr - (LPBYTE)jcrfh);
      ChangeDataLen(datalen);
      }
	return true;
}

bool KROSSWORD::SaveJccFile(void)
{
   if(!GetCreatesFile()) return false;
   bool matrix=false;
   int j, i, c;
   for(i=0; i<MapLen; i++)
   	if(BitMap[i]!=ZEROS) {matrix=true; break;}
   int BuffSize = sizeof(JCC_FILE_DATA)-2+
   					hSize*hNumbSize*2+
                  vSize*vNumbSize*2+
                  (matrix ? MapLen : 0);
	ChangeDataLen(BuffSize);
  	JCC_FILE_DATA *jccfh = (JCC_FILE_DATA *)FILE_ROUTINE::GetData();
/*
typedef struct JCC_FILE_DATA_tag {
	char JCC_Header[5]; 				//JAPAN
   byte JCC_Colors;   				//2..16
   COLORREF JCC_ColorTable[16];  //16 colors
   byte JCC_hSize;               //HSize
   byte JCC_vSize;               //VSize
   byte JCC_vNumbSize;           //hNumbSize
   byte JCC_hNumbSize;           //vNumbSize
   byte JCC_Containers;          //0 Matrix is no empty
   JCC_DD JCC_Digit[1];            //Pointer to digit
   } JCC_FILE_DATA;
*/
//Записываем размер
   strcpy(jccfh->JCC_Header, "JAPAN");
   jccfh->JCC_Colors = (byte)(Containers==COLORS ? 16 : 2);
   for(i=0; i<16; i++)
   	jccfh->JCC_ColorTable[i]=ColorTable[i];
  	jccfh->JCC_hSize=(byte)hSize;
   jccfh->JCC_vSize=(byte)vSize;
  	jccfh->JCC_hNumbSize=(byte)hNumbSize;
   jccfh->JCC_vNumbSize=(byte)vNumbSize;
   jccfh->JCC_Containers=(byte)(matrix ? 0 : 1);
   ChangeInTable(true, false);
//Записываем оцифровку
	JCC_DD *ptr = jccfh->JCC_Digit;
   byte akk;
	for(j=0; j<hSize; j++)
	  	for(i=0; i<hNumbSize; i++){
         c = (hNumbSize-i-1)+hNumbSize*j;
         akk = hData->DigitColors[c];
         if(akk==17) akk=13;
         else if(akk==18) akk=8;
         else if(akk==16) akk=6;
         if(Containers!=COLORS && hData->Digits[c]) akk=1;
   	   ptr[i+hNumbSize*j].Color = akk;
         ptr[i+hNumbSize*j].Digit = (byte)(hData->Digits[c]|(hData->DigitMarkers[c] ? 0x80 : 0));
         }
   ptr += (hSize*hNumbSize);
	for(j=0; j<vSize; j++)
	  	for(i=0; i<vNumbSize; i++){
         c = (vNumbSize-i-1)+vNumbSize*j;
         akk = vData->DigitColors[c];
         if(akk==17) akk=13;
         else if(akk==18) akk=8;
         else if(akk==16) akk=6;
         if(Containers!=COLORS && vData->Digits[c]) akk=1;
   	   ptr[i+vNumbSize*j].Color = akk;
         ptr[i+vNumbSize*j].Digit = (byte)(vData->Digits[c]|(vData->DigitMarkers[c] ? 0x80 : 0));
         }
   byte *bptr = (byte *)(ptr + (vSize*vNumbSize));
   if(matrix)
		for(i=0; i<MapLen; i++)
      	bptr[i] = (byte)(BitMap[i]==MARKER ? 0xff : BitMap[i]);
	return true;
}

bool KROSSWORD::Save (bool Quest, bool SaveAs)//Save data to file
{
  if(Status==NOCHANGE && !SaveAs) return true;
  if(SaveAs) {if(!GetSaveFileName()) return false;}
  else if(Quest)
	  {
	  int j=MessageBox(hWnd,"Файл изменен.\rСохранить изменения?", FileTitle,
							MB_YESNOCANCEL | MB_ICONQUESTION);
	  if(j==IDCANCEL) return false; // return if canceled
	  else if(j==IDNO) return true;  // return if nosaves
	  }
   FILE_TYPES ft=GetExtension();
  	if(ft==ICO_FILE && !SaveAs && !GetSaveFileName()) return false;
   ft=GetExtension();
#ifdef DEMOVER
  	if(ft==BMP_FILE && SaveAs && SaveBmpFiles(true));
	else Registration();
#else
  	if(ft==BMP_FILE)
   	{if(!SaveBmpFiles(SaveAs)) return false;}
  	else if(ft==JCR_FILE)
      {if(!SaveJcrFile() || !SaveDataToFile()) return false;}
  	else if(ft==JCC_FILE)
      {if(!SaveJccFile() || !SaveDataToFile()) return false;}
  	else if(ft==PPIFF1_FILE || ft==PPIFF2_FILE || ft==PPIFFS_FILE)
      {if(!SavePPIFFFile() || !SaveDataToFile()) return false;}
  	else if(ft==UNKNOW_FILE)
     	{
     	errhandler("Незадан формат для сохранения", hWnd);
     	return false;
     	}
#endif
	RestoreCaption();
	KillUndo();
   return true;
}
/*
bool KROSSWORD::SaveJpnFile(void)
{
   if(!GetCreatesFile()) return false;

  	int j, i, BuffSize;
  	FILE_HEADER *fh;
  	hDigitSize = hSize*hNumbSize;
  	vDigitSize = vSize*vNumbSize;
   MapLen = hSize*vSize;

//Определяем размер буфера файла(7байт-заголовок + оцифровка + BitMap)
  	BuffSize=sizeof(FILE_HEADER)-1+hSize*hNumbSize*2+vSize*vNumbSize*2+hSize*vSize+MAXCOLOR*sizeof(DWORD);
	ChangeDataLen(BuffSize);
  	fh = (FILE_HEADER *)FILE_ROUTINE::GetData();
  	bool BMP = false;
  	DWORD Offset=0;
  	char Count;

//Заполняем заголовок
  fh->FileId = 0x504a; //ASCII 'JP'
  fh->NumbColors = 0;

//Сохраняем флаги --------------------------------------
  fh->Flags = 0;                                      //
                                                      //
//Флаг режима редактирования                          //
  fh->Flags &= MPAINTINGFLAG;                         //
  if(MouseInput != CDIGITAL) fh->Flags |= PAINTINGFLAG;//
// -----------------------------------------------------

//Определяем наличие битовой карты
  for (i=0; i<MapLen; i++) if(BitMap[i]!=ZEROS)
      {
      BMP = true;
      break;
      }

//Определяем тип(Цветной/Монохромный)
  if(Containers == COLORS)
    {
    fh->NumbColors=(byte)KROSSFEATURES::Colors;
    for(Count=0; Count<MAXCOLOR; Count++) //Сохраняем цвета
      {
      *(LPDWORD)(fh->Data+Offset)=ColorTable[Count+FIRST];
      Offset+=sizeof(DWORD);
      }
    }
  else fh->NumbColors = (byte) BMP;

//Записываем размер
  fh->hSize=(byte)hSize; fh->vSize=(byte)vSize;
  fh->hNumbSize=(byte)hNumbSize; fh->vNumbSize=(byte)vNumbSize;

//Записываем оцифровку
  for(i=0; i<hSize; i++)
      for(j=0; j<hNumbSize; j++)
         {
         fh->Data[Offset++]=hData->Digits[i*hNumbSize+j];
        	fh->Data[Offset++]=hData->DigitColors[i*hNumbSize+j];
         }
  for(i=0; i<vSize; i++)
      for(j=0; j<vNumbSize; j++)
         {
         fh->Data[Offset++]=vData->Digits[i*vNumbSize+j];
        	fh->Data[Offset++]=vData->DigitColors[i*vNumbSize+j];
         }

  if(BMP){
//Заполняем указатель на BitMap
     fh->OffsetBitmap=(DWORD)(Offset+(sizeof(FILE_HEADER)-1));
     for (i=0; i<MapLen; i++)
       fh->Data[Offset++]=BitMap[i];
     }
  else fh->OffsetBitmap = 0;

  ChangeDataLen(Offset+(sizeof(FILE_HEADER)-1));
  return true;
} */

void KROSSWORD::RestoreCaption(void)
{
  Status=NOCHANGE;
  SetCaptionWindow();
  EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_GRAYED);
}

void KROSSWORD::Print(void)
{
	PRINTS(KROSSFEATURES::This, &lf);
}

void KROSSWORD::DelDig     (void)
{
	PrepareUndo();
   ZeroMemory(hData->Digits, hNumbSize*hSize);
   ZeroMemory(vData->Digits, vNumbSize*vSize);
   ZeroMemory(hData->DigitMarkers, hNumbSize*hSize);
   ZeroMemory(vData->DigitMarkers, vNumbSize*vSize);
   ZeroMemory(hData->Number, hSize*sizeof(int));
   ZeroMemory(vData->Number, vSize*sizeof(int));
   ChangesInFile(true);
   ChangeInTable(true, true);
}

void KROSSWORD::ClearField(void)
{
   bool res = true;
	for(int i=0; i<MapLen; i++)
   	if(BitMap[i]!=ZEROS) res = false;
   if(res) return;
	PrepareUndo();
   memset(BitMap, ZEROS, MapLen);
   memset(hData->DigitMarkers, 0, hSize*hNumbSize*sizeof(bool));
   memset(vData->DigitMarkers, 0, vSize*vNumbSize*sizeof(bool));
//   ViewTable();
   STATUS("Поле очищено.");
   InvalidateRect(hWnd, NULL, false);
   ChangesInFile(false);
}


void KROSSWORD::KillUndo (void)
{
	if(!UndoFlag && !RedoFlag) return;
	while(EndRedo != EndUndo)
   	{
		DeleteUndoTag(&Undo[EndRedo]);
	   if(--EndRedo < 0) EndRedo = NUMBUNDO-1;
      }
   if(UndoFlag)DeleteUndoTag(&Undo[EndUndo]);
   RedoFlag = UndoFlag = false;
   EndRedo = CurUndo = EndUndo = 0;
   EnableMenuItem(Menu, IDM_UNDO, MF_GRAYED);
   EnableMenuItem(Menu, IDM_REDO, MF_GRAYED);
}

void KROSSWORD::ChangesInFile(bool digit)
{
  if(Status==CHANGES) return;
  if(GetExtension()==ICO_FILE && digit) return;
  if(GetExtension()==BMP_FILE && digit) return;
  if(GetExtension()==JCR_FILE && !digit) return;
  if(GetExtension()==PPIFF1_FILE && !digit) return;
  if(GetExtension()==PPIFF2_FILE && !digit) return;
  ChangesInFileA();
}

void KROSSWORD::ChangesInFileA()
{
  Status=CHANGES;
  char buf[FILEPATHNAME];
  GetWindowText(hWnd,buf,FILEPATHNAME);
  if(*(buf+strlen(buf)-1) != '*') strcpy(buf+strlen(buf),"*");
  SetWindowText(hWnd,buf);
  EnableMenuItem(GetMenu(hWnd),IDM_SAVE,MF_ENABLED);
}

void KROSSWORD::ChangeInTable(bool Press, bool update)//Пересчитываем количество элементов
{                                        //таблицы
  int Count, Source, Dist, Limit;
  if(!hData->Digits || !vData->Digits) return;
  for(int x=0; x<hSize; x++)
     {
     Count=0;
     for(int y=0; y<hNumbSize; y++)
        if(hData->Digits[x*hNumbSize+y]) Count++;
     hData->Number[x]=Count;
     }
  for(int y=0; y<vSize; y++)
     {
     Count=0;
     for(int x=0; x<vNumbSize; x++)
        if(vData->Digits[y*vNumbSize+x]) Count++;
     vData->Number[y]=Count;
     }
  if(!Press) return;
  for(int x=0; x<hSize; x++)
     {
     Dist=x*hNumbSize;
     Limit=Dist+hNumbSize;
     Count=hNumbSize;
     while(hData->Digits[Dist]) {Dist++; if(!--Count) break;}
     Source=Dist+1;
     while(Source<Limit)
        if(hData->Digits[Source])
           {
           hData->Digits[Dist]=hData->Digits[Source];
           hData->Digits[Source]=0;
			  hData->DigitMarkers[Dist]=hData->DigitMarkers[Source];
           hData->DigitMarkers[Source]=false;
           if(Containers == COLORS)
              {
              hData->DigitColors[Dist]=hData->DigitColors[Source];
              hData->DigitColors[Source]=BACK;
              }
           Dist++;
           Source++;
           }
        else Source++;
     }
  for(int x=0; x<vSize; x++)
     {
     Dist=x*vNumbSize;
     Limit=Dist+vNumbSize;
     Count=vNumbSize;
     while(vData->Digits[Dist]) {Dist++; if(!--Count) break;}
     Source=Dist+1;
     while(Source<Limit)
        if(vData->Digits[Source])
           {
           vData->Digits[Dist]=vData->Digits[Source];
           vData->Digits[Source]=0;
			  vData->DigitMarkers[Dist]=vData->DigitMarkers[Source];
           vData->DigitMarkers[Source]=false;
           if(Containers == COLORS)
              {
              vData->DigitColors[Dist]=vData->DigitColors[Source];
              vData->DigitColors[Source]=BACK;
              }
           Dist++;
           Source++;
           }
        else Source++;
      }
  if(update)
  	ViewTable(NULL);
}

void KROSSWORD::Undo_(bool undos)
{
	if(!UndoFlag && !RedoFlag) return;

   int cur = CurUndo;

   if(undos){                // если отмена
		if(CurUndo == EndUndo) // и последний элемент отмены
   		{
			UndoFlag = false;   // отключаем 'отмены' в меню и сбрасываем флаг
      	EnableMenuItem(Menu, IDM_UNDO, MF_GRAYED);
      	}
  		EnableMenuItem(Menu, IDM_REDO, MF_ENABLED);
      RedoFlag = true;       // включаем пункт меню и флаг повтора
      }
   else 	{                   // если повтор
    	if(CurUndo == EndRedo) // если элемент 'повторов' самый верхний
   		{
			RedoFlag = false;   // сбрасываем флаг и откючаем пункт меню 'повтор'
      	EnableMenuItem(Menu, IDM_REDO, MF_GRAYED);
         if(CurUndo != EndUndo) return;// выход если это не первый элемент 'отмен'
      	}
   	else                   // если элемент не самый верхний
      	{
         if(UndoFlag)        // наращиваем счетчик, если это не первый элемент 'отмен'
      		if(++CurUndo >= NUMBUNDO) CurUndo = 0;
    		if(CurUndo == EndRedo)// проверяем не стал ли он самым верхним
   			{                // сбрасываем флаг и откючаем пункт меню 'повтор'
				RedoFlag = false;// если текущий повтор последний
   	   	EnableMenuItem(Menu, IDM_REDO, MF_GRAYED);
      	   }
         }
		UndoFlag = true;       // включаем флажок и пункт меню 'отмена'
     	EnableMenuItem(Menu, IDM_UNDO, MF_ENABLED);
      }
   int volatile su = CurUndo;
	if(UndoFlag && undos)
   	if((--CurUndo) < 0) CurUndo = NUMBUNDO-1;

   UNDOTAG curset;
   MoveCurrentData(&curset); // сохраняем текушее состояние

	RestoreData(&Undo[su]);
   Undo[undos ? cur : CurUndo] = curset;
}

void KROSSWORD::PrepareUndo(void)
{
// сперва удаляем, при необходимости, все предыдущие Redo
	EnableMenuItem(Menu, IDM_REDO, MF_GRAYED);
   RedoFlag = false;
	while(EndRedo != CurUndo)
 		{
		DeleteUndoTag(Undo+EndRedo);
   	if(--EndRedo < 0) EndRedo = NUMBUNDO-1;
     	}
	if(UndoFlag)
   	{
		if(++CurUndo >= NUMBUNDO) CurUndo = 0;
   	if(CurUndo == EndUndo) // если переполнение буфера
   		{
			DeleteUndoTag(&Undo[EndUndo]); // удаляем последний элемент "хвоста"
   	   if(++EndUndo >= NUMBUNDO) EndUndo = 0;
      	}
      EndRedo = CurUndo;
		}
   else 	UndoFlag = true;

	CopyCurrentData(&Undo[CurUndo]);
   EnableMenuItem(Menu, IDM_UNDO, MF_ENABLED);
}

void KROSSWORD::DeleteUndoTag(UNDOTAG *Undo)
{
	DELETE(Undo->Maps);
	DELETE(Undo->hData.Digits);
   DELETE(Undo->vData.Digits);
  	DELETE(Undo->hData.DigitColors);
   DELETE(Undo->vData.DigitColors);
  	DELETE(Undo->hData.DigitMarkers);
   DELETE(Undo->vData.DigitMarkers);
   DELETE(Undo->hData.Number);
   DELETE(Undo->vData.Number);
}

void KROSSWORD::RestoreData(UNDOTAG *Undo)
{
   bool InitScr=false;
   KROSSFEATURES::Colors = Undo->Colors;
   if(Containers!=COLORS && Undo->Containers==COLORS)
      {
   	for(int i=0; i<MAXCOLOR; i++)
     		ColorTable[i+FIRST] = Undo->Palette[i];
      CreateColorPalette();
      }
   else if(Containers==COLORS && Undo->Containers!=COLORS)
   	{
      DestroyColorPalette();
      ResetPalette(true);
      }
   else if(Containers==COLORS && Undo->Containers==COLORS)
   	{
   	for(int i=0; i<MAXCOLOR; i++)
     		ColorTable[i+FIRST] = Undo->Palette[i];
      UpdatePalette();
      }
   Containers = Undo->Containers;
   Status     = Undo->Status;
   MOVEDATA(vData->Digits, Undo->vData.Digits);
   MOVEDATA(vData->DigitColors,Undo->vData.DigitColors);
   MOVEDATA(vData->Number, Undo->vData.Number);
   MOVEDATA(hData->Digits, Undo->hData.Digits);
   MOVEDATA(hData->DigitColors,Undo->hData.DigitColors);
   MOVEDATA(hData->DigitMarkers,Undo->hData.DigitMarkers);
   MOVEDATA(vData->DigitMarkers,Undo->vData.DigitMarkers);
   MOVEDATA(hData->Number, Undo->hData.Number);
   MOVEDATA(BitMap,        Undo->Maps);
   if((vNumbSize != Undo->vNumbSize) || (hNumbSize != Undo->hNumbSize))
     InitScr=true;
   if((hSize!=Undo->hSize) || (vSize!=Undo->vSize))
     InitScr=true;

   hNumbSize  = Undo->hNumbSize;
   vNumbSize  = Undo->vNumbSize;
   hSize=Undo->hSize;
   vSize=Undo->vSize;
   MapLen=hSize*vSize;

   if(Status == NOCHANGE) RestoreCaption();
   if(Status == CHANGES)  ChangesInFileA();

   UpButton(0,0);
   ScreenInitialize();
   if(InitScr)
      InvalidateRect(hWnd, &Client, true);
   else
   	{
      HDC hDC=GetDC(hWnd);
	  	Paint(hDC);
   	ReleaseDC(hWnd, hDC);
      }
}

void KROSSWORD::MoveCurrentData(UNDOTAG *Undo)
{
#define MovePointer(a,b) (a)=(b);(b)=NULL
   MovePointer(Undo->hData.Digits, hData->Digits);
   MovePointer(Undo->vData.Digits, vData->Digits);
   MovePointer(Undo->vData.DigitColors,vData->DigitColors);
   MovePointer(Undo->hData.DigitColors,hData->DigitColors);
   MovePointer(Undo->vData.DigitMarkers,vData->DigitMarkers);
   MovePointer(Undo->hData.DigitMarkers,hData->DigitMarkers);
   MovePointer(Undo->vData.Number, vData->Number);
   MovePointer(Undo->hData.Number, hData->Number);
   MovePointer(Undo->Maps, BitMap);
   Undo->Containers = Containers;
   Undo->Status     = Status;
   Undo->Colors	  = KROSSFEATURES::Colors;
   Undo->hSize = hSize;
   Undo->hNumbSize = hNumbSize;
	Undo->vSize = vSize;
   Undo->vNumbSize =  vNumbSize;
   Undo->MapLen = hSize*vSize;
   if(Containers==COLORS) for(int i=0; i<MAXCOLOR; i++)
     	Undo->Palette[i]=ColorTable[i+FIRST];
}

void KROSSWORD::CopyCurrentData(UNDOTAG *Undo)
{
	MapLen = hSize*vSize;
   Undo->hSize = hSize;
   Undo->hNumbSize = hNumbSize;
	Undo->vSize = vSize;
   Undo->vNumbSize =  vNumbSize;
   Undo->MapLen = MapLen;
   Undo->Containers = Containers;
   Undo->Status     = Status;
   Undo->Colors	  = KROSSFEATURES::Colors;

	Undo->Maps = new byte[MapLen];
	Undo->hData.Digits = new byte[hSize*hNumbSize];
   Undo->vData.Digits = new byte[vSize*vNumbSize];
   Undo->hData.DigitColors = new byte[hSize*hNumbSize];
   Undo->vData.DigitColors = new byte[vSize*vNumbSize];
   Undo->hData.DigitMarkers = new bool[hSize*hNumbSize];
   Undo->vData.DigitMarkers = new bool[vSize*vNumbSize];
   Undo->hData.Number = new int[hSize];
   Undo->vData.Number = new int[vSize];
   for(int i=0; i<MapLen; i++)
      Undo->Maps[i] = BitMap[i];

   for(int i=0; i<hSize*hNumbSize; i++)
   	{
      Undo->hData.Digits[i] 		= hData->Digits[i];
     	Undo->hData.DigitColors[i] = hData->DigitColors[i];
    	Undo->hData.DigitMarkers[i]= hData->DigitMarkers[i];
      }

   if(hData->Number)
	   for(int i=0; i<hSize; i++)
   	   Undo->hData.Number[i] = hData->Number[i];

   for(int i=0; i<vSize*vNumbSize; i++)
   	{
  	   Undo->vData.Digits[i] 		= vData->Digits[i];
    	Undo->vData.DigitColors[i] = vData->DigitColors[i];
    	Undo->vData.DigitMarkers[i]= vData->DigitMarkers[i];
      }

   if(vData->Number)
   	for(int i=0; i<vSize; i++)
      	Undo->vData.Number[i] = vData->Number[i];

   if(Containers==COLORS) for(int i=0; i<MAXCOLOR; i++)
     	Undo->Palette[i]=ColorTable[i+FIRST];
}

bool KROSSWORD::Corrected(bool prepun)
{                              //Расчитываем оцифровку
  	CALCDATA cd;
  	CALCULATE::GetData(&cd);
  	if(cd.Runing) return false;	// Выход если запущена программа анализа
  	int Count, hMax = 0, vMax = 0;
  	byte color;
  	bool z=true;
  	for(int i=0; i<MapLen; i++) if(BitMap[i] != ZEROS && BitMap[i] != MARKER) z=false;
  	if(z) return false;
  	if(prepun) PrepareUndo();
//Пересчитываем размеры буферов оцифровки
  	for(int x=0; x<hSize; x++)
     {
     z=false;
     Count = 0;
     for(int y=0; y<vSize; y++)
       if(BitMap[y*hSize+x] != ZEROS && BitMap[y*hSize+x] != MARKER)
         {
         if(!z) {color = BitMap[y*hSize+x]; z=true;}
         else if(color != BitMap[y*hSize+x])
         			{Count++; color = BitMap[y*hSize+x];}
         }
       else if(z)
         {
         z=false;
         Count++;
         }
     	if(z) Count++;
     	hMax = MAX(Count, hMax);
     	}
  	for(int y=0; y<vSize; y++)
     	{
     	z=false;
     	Count = 0;
     	for(int x=0; x<hSize; x++)
       	if(BitMap[y*hSize+x] != ZEROS && BitMap[y*hSize+x] != MARKER)
         	{
         	if(!z) {color = BitMap[y*hSize+x]; z=true;}
         	else if(color != BitMap[y*hSize+x])
         			{Count++; color = BitMap[y*hSize+x];}
         	}
         else if(z)
         	{
         	z=false;
         	Count++;
         	}
     	if(z) Count++;
     	vMax = MAX(Count, vMax);
   	}

  	hNumbSize = hMax;
  	vNumbSize = vMax;
  	DELETE(hData->Digits);
  	DELETE(hData->DigitColors);
  	DELETE(hData->DigitMarkers);
  	hData->Digits = InitializeMatrix(hNumbSize*hSize);
  	hData->DigitColors = InitializeMatrix(hNumbSize*hSize);
   hData->DigitMarkers= new bool[hNumbSize*hSize];
   memset(hData->DigitMarkers, false, hNumbSize*hSize*sizeof(bool));
  	DELETE(vData->Digits);
  	DELETE(vData->DigitColors);
  	DELETE(vData->DigitMarkers);
  	vData->Digits = InitializeMatrix(vNumbSize*vSize);
  	vData->DigitColors = InitializeMatrix(vNumbSize*vSize);
   vData->DigitMarkers= new bool[vNumbSize*vSize];
   memset(vData->DigitMarkers, false, vNumbSize*vSize*sizeof(bool));

//Обсчитываем оцифровку
	for(int x=0; x<hSize; x++)
   	{
     	z=false;
     	hData->Number[x]=0;
     	Count = 0;
     	color = ZEROS;
		for(int y=vSize-1; y>=0; y--)
     		if(BitMap[y*hSize+x] != ZEROS && BitMap[y*hSize+x] != MARKER)
      		{
         	if(color!=BitMap[y*hSize+x])
         		if(!z) {z=true; color=BitMap[y*hSize+x];}
            	else {
            		hData->Digits[x*hNumbSize+hData->Number[x]] = (byte)Count;
               	if(Containers == COLORS)
                  	hData->DigitColors[x*hNumbSize+hData->Number[x]] = color;
               	hData->Number[x]++;
               	color=BitMap[y*hSize+x];
                  Count=0;
               	}
         	Count++;
         	}
      	else if(z)
         	{
        		hData->Digits[x*hNumbSize+hData->Number[x]] = (byte)Count;
         	if(Containers == COLORS)
            	hData->DigitColors[x*hNumbSize+hData->Number[x]] = color;
         	hData->Number[x]++;
         	z=false;
         	Count=0;
     			color = ZEROS;
         	}
     	if(z)
     		{
        	hData->Digits[x*hNumbSize+hData->Number[x]] = (byte)Count;
         if(Containers == COLORS)
         	hData->DigitColors[x*hNumbSize+hData->Number[x]] = color;
         hData->Number[x]++;
         }
     	}

	for(int y=0; y<vSize; y++)
     	{
     	z=false;
     	vData->Number[y]=0;
     	Count = 0;
   	color = ZEROS;
     	for(int x=hSize-1; x>=0; x--)
       	if(BitMap[y*hSize+x] != ZEROS && BitMap[y*hSize+x] != MARKER)
         	{
         	if(color!=BitMap[y*hSize+x])
         		if(!z) {z=true; color=BitMap[y*hSize+x];}
            	else {
            		vData->Digits[y*vNumbSize+vData->Number[y]] = (byte)Count;
               	if(Containers == COLORS)
                  	vData->DigitColors[y*vNumbSize+vData->Number[y]] = color;
               	vData->Number[y]++;
               	color=BitMap[y*hSize+x];
                  Count=0;
               	}
         	Count++;
         	}
       	else if(z)
         	{
         	vData->Digits[y*vNumbSize+vData->Number[y]] = (byte)Count;
         	if(Containers == COLORS)
            	vData->DigitColors[y*vNumbSize+vData->Number[y]] = color;
         	vData->Number[y]++;
         	z=false;
         	Count=0;
     			color = ZEROS;
         	}
     	if(z)
      	{
      	vData->Digits[y*vNumbSize+vData->Number[y]] = (byte)Count;
         if(Containers == COLORS)
         	vData->DigitColors[y*vNumbSize+vData->Number[y]] = color;
         vData->Number[y]++;
         }
     	}

  	ScreenInitialize();
	InvalidateRect(hWnd, &Client, true);
  	if(prepun)
   	ChangesInFile(true);
   return true;
}

void KROSSWORD::ScrollingData(WORD Command)
{
  int x/*=hSize*/, y=vSize;
  byte data;
  byte *source=BitMap, *dist=BitMap;
  RECT Source=Fild,invalid=Fild;
  int ScX=0, ScY=0;
  switch(Command){
      case IDM_SCLEFT:
         ScX=-RectSize;
         Source.left-=ScX;
         invalid.left=invalid.right+ScX-1;
         while(y--){
           x=hSize-1;
           data = *source++;
           while(x--) *dist++ = *source++;
           *dist++=data;
           }
         break;

      case IDM_SCRIGHT:
         ScX=RectSize;
         Source.right-=ScX;
         invalid.right=invalid.left+ScX+1;
         dist=source+=MapLen-1;
         while(y--){
           x=hSize-1;
           data = *source--;
           while(x--) *dist-- = *source--;
           *dist--=data;
           }
         break;

      case IDM_SCUP:
         ScY=-RectSize;
         Source.top-=ScY;
         invalid.top=invalid.bottom+ScY-1;
         for(x=0; x<hSize; x++){
           data = source[x];
           for(y=0; y<(vSize-1)*hSize; y+=hSize)
              source[x+y]=source[x+y+hSize];
           source[x+hSize*vSize-hSize] = data;
           }
         break;

      case IDM_SCDOWN:
         ScY=RectSize;
         Source.bottom-=ScY;
         invalid.bottom=invalid.top+ScY+1;
         for(x=0; x<hSize; x++){
           data = source[x+hSize*vSize-hSize];
           for(y=(vSize-1)*hSize; y>0; y-=hSize)
              source[x+y]=source[x+y-hSize];
           source[x]=data;
           }
         break;

      default:
        return;
      }
  ScrollRect(&Source, ScX, ScY);
  InvalidateRect(hWnd,&Fild,false);
  ReleaseDC(hWnd,PaintIconic(GetDC(hWnd)));
}

void KROSSWORD::DigKeyIn(int Dig)
{ // на входе '0' '9' '.' ',' '\r' '-''
  	if(MouseInput!=CDIGITAL) return;

  	int x,y,vOffSet=hNumbSize*hSize;  //Находим координаты и адрес оперируемой цифры
  	bool whu = (bool)(CurrPoz < hSize*hNumbSize);

   x = whu ? CurrPoz/hNumbSize : (CurrPoz-vOffSet)%vNumbSize;
 	y = whu ? CurrPoz%hNumbSize : (CurrPoz-vOffSet)/vNumbSize;

  	byte *Digit = whu ? hData->Digits+(x+1)*hNumbSize-y-1: vData->Digits+(y+1)*vNumbSize-x-1;

	if		 (Dig=='.')	{InputDigitRange=2; *Digit=0; return;}
   else if(Dig==',') {InputDigitRange=3; *Digit=0; return;}

  	ViewDigit(x,y,whu);                                     //сброс пред. отметки

  	if(Dig == VK_RETURN)  // пробел или ентер
		{
      if(whu) 	{x++; y=0; if(x>=hSize) {whu=false; x=0;}}
      else		{y++; x=0; if(y>=vSize) {whu=true;  y=0;}}
     	ChangeInTable(true, true);
      ViewDigit(x,y,whu,NULL,true);
      }

   else if(Dig == '-'){		// пустая строка
      if(whu){
         y=0;
         memset(hData->Digits+x*hNumbSize, 0, hNumbSize);
         x++; if(x>=hSize) {whu=false; x=0;}
         }
		else {
         x=0;
         memset(vData->Digits+y*vNumbSize, 0, vNumbSize);
         y++; if(y>=vSize) {whu=true; y=0;}
         }
     	ChangeInTable(true, true);
      ViewDigit(x,y,whu,NULL,true);
      }
   else if(Dig >= 0x30 && Dig <= 0x39){
   	Dig=Dig&0xf;
      if(InputDigitRange>1) {InputDigitRange--; *Digit = (byte)(*Digit*10+Dig);}
      else {
      	if(InputDigitRange){
            InputDigitRange=0;
            int a=*Digit*10+Dig;
            *Digit = (byte)(a<256?a:255);
         	}
         else *Digit=(byte)Dig;
         ViewDigit(x,y,whu);
      	if(whu) 	{if(y<hNumbSize-1) y++;}
      	else		{if(x<vNumbSize-1) x++;}
      	}
   	}
   CurrPoz = whu ? x*hNumbSize+y : y*vNumbSize+x+vOffSet;
	ChangesInFile(true);
  	ViewDigit(x,y,whu,NULL,true);
  	ReleaseDC(hWnd,PaintIconic(GetDC(hWnd)));
}

void KROSSWORD::MoveCarret(int Command){
  if(MouseInput!=CDIGITAL) return;
   int comp = hSize*hNumbSize;
   bool whu = (bool)(CurrPoz < comp);
   int addr = whu ? CurrPoz : CurrPoz - comp;
   int x = whu ? addr/hNumbSize : addr%vNumbSize,
       y = whu ? addr%hNumbSize : addr/vNumbSize,
       LimX = whu ? hSize : vNumbSize,
       LimY = whu ? hNumbSize : vSize;
   ViewDigit(x, y, whu, NULL, false);
   switch(Command){
   case VK_RIGHT:
     if(++x >= LimX) x=0;
     break;
   case VK_LEFT:
     if(--x < 0) x=LimX-1;
     break;
   case VK_UP:
     if(--y < 0) y=LimY-1;
     break;
   case VK_DOWN:
     if(++y >= LimY) y=0;
     break;
   case VK_TAB:
     whu = !whu;
     x = y = 0;
     break;
   }
   if(whu) CurrPoz = x*hNumbSize+y;
   else    CurrPoz = y*vNumbSize+x+comp;
   ViewDigit(x, y, whu, NULL, true);
   ReleaseDC(hWnd,PaintIconic(GetDC(hWnd)));
}

void KROSSWORD::KeyIn(int vKeyCode, bool Ext)
{     //0x6e 0xbe - точка  0110
		//0xbc - запятая     1011
      //0xbd - '-'
   if(vKeyCode==0x6b) DigKeyIn(0x2c);
   if(vKeyCode==0x6e || vKeyCode==0xbe || vKeyCode==0xbc || vKeyCode==0xbd || vKeyCode==0x6d)
      DigKeyIn((vKeyCode&0xf)|0x20);
   if(vKeyCode==VK_RETURN || vKeyCode==VK_SPACE)
		DigKeyIn(VK_RETURN);
   if((vKeyCode >= 0x60 && vKeyCode <=0x69) || (vKeyCode>=0x30 && vKeyCode<=0x39)) // преобразование цифровой клавиатуры
   	{
      DigKeyIn((vKeyCode-0x30)|0x30);
      return;
      };

   switch(vKeyCode){
   case VK_DOWN:
     if(MouseInput==CDIGITAL) MoveCarret(vKeyCode);
     else if(!Ext) vScroll((int)LOWORD((WPARAM)SB_LINEDOWN),(short int)HIWORD((WPARAM)SB_LINEDOWN));
     else ScrollingData(IDM_SCDOWN);
     break;
   case VK_UP:
     if(MouseInput==CDIGITAL) MoveCarret(vKeyCode);
     else if(!Ext) vScroll((int)LOWORD((WPARAM)SB_LINEUP),(short int)HIWORD((WPARAM)SB_LINEUP));
     else ScrollingData(IDM_SCUP);
     break;
   case VK_LEFT:
     if(MouseInput==CDIGITAL) MoveCarret(vKeyCode);
     else if(!Ext) hScroll((int)LOWORD((WPARAM)SB_LINELEFT),(short int)HIWORD((WPARAM)SB_LINERIGHT));
     else ScrollingData(IDM_SCLEFT);
     break;
   case VK_RIGHT:
     if(MouseInput==CDIGITAL) MoveCarret(vKeyCode);
     else if(!Ext) hScroll((int)LOWORD((WPARAM)SB_LINERIGHT),(short int)HIWORD((WPARAM)SB_LINERIGHT));
     else ScrollingData(IDM_SCRIGHT);
     break;
   case VK_NEXT:
     if(MouseInput==CDIGITAL){
        ViewDigit((CurrPoz < hSize*hNumbSize) ? CurrPoz/hNumbSize : (CurrPoz-hSize*hNumbSize)%vNumbSize,
        (CurrPoz < hSize*hNumbSize) ? CurrPoz%hNumbSize : (CurrPoz-hSize*hNumbSize)/vNumbSize,
        (bool)(CurrPoz < hSize*hNumbSize));
        CurrPoz = hSize*hNumbSize;
        ViewDigit(0,0,false, NULL, true);
        }
     else vScroll((int)LOWORD((WPARAM)SB_PAGEDOWN),(short int)HIWORD((WPARAM)SB_PAGEDOWN));
     break;
   case VK_PRIOR:
     if(MouseInput==CDIGITAL){
        ViewDigit((CurrPoz < hSize*hNumbSize) ? CurrPoz/hNumbSize : (CurrPoz-hSize*hNumbSize)%vNumbSize,
        (CurrPoz < hSize*hNumbSize) ? CurrPoz%hNumbSize : (CurrPoz-hSize*hNumbSize)/vNumbSize,
        (bool)(CurrPoz < hSize*hNumbSize));
        CurrPoz = 0;
        ViewDigit(0,0,true, NULL, true);
        }
     else vScroll((int)LOWORD((WPARAM)SB_PAGEUP),(short int)HIWORD((WPARAM)SB_PAGEUP));
     break;
   case VK_TAB:
     if(MouseInput==CDIGITAL) MoveCarret(vKeyCode);
     break;
   case VK_INSERT:
     if(MouseInput!=CDIGITAL) SetCurrentInput(CDIGITAL);
     else SetCurrentInput(CPEN);
     break;
   }
}

bool KROSSWORD::CheckSumm(void){
  	char mes[]={"Ошибка контрольных сумм."};
   char warn[]={"Внимание, оцифровка несбалансирована!\r\r"};
  	char hsize[]= { "Сумма по горизонтали = "};
  	char vsize[]={"\rСумма по вертикали = "};
  	char checks[]={"\r\rРешение кроссворда невозможно.\rПроверьте правильность ввода оцифровки."};
  	if(Containers == MONOCHROME)
		{
  		int hSumm=0, vSumm=0;
  		for(int i=0; i<hSize*hNumbSize; i++) hSumm += hData->Digits[i];
  		for(int i=0; i<vSize*vNumbSize; i++) vSumm += vData->Digits[i];
  		if(hSumm!=vSumm){
			char *buf = new char[sizeof(warn)+sizeof(hsize)+sizeof(vsize)+sizeof(checks)+36];
         strcpy(buf, warn);
    		strcat(buf,hsize);
    		itoa(hSumm,buf+strlen(buf),10);
    		strcat(buf,vsize);
    		itoa(vSumm,buf+strlen(buf),10);
    		strcat(buf,checks);
    		if(!ExecDemo) MessageBox(hWnd,buf,mes,MB_OK|MB_ICONSTOP);
    		delete[] buf;
    		return false;
    		}
  		}
  	else if(Containers == COLORS)
  		{
      int hSumm[MAXCOLOR], vSumm[MAXCOLOR];// массивы счетчиков каждого цвета
      bool equival = true;
		char *buf = new char[2000];
      strcpy(buf, warn);
      for(int i=0; i<MAXCOLOR; i++) hSumm[i] = vSumm[i] = 0;
      for(int i=0; i<hSize*hNumbSize; i++)
      	if(hData->Digits[i])
         	hSumm[hData->DigitColors[i]-FIRST]+=hData->Digits[i];
      for(int i=0; i<vSize*vNumbSize; i++)
      	if(vData->Digits[i])
         	vSumm[vData->DigitColors[i]-FIRST]+=vData->Digits[i];
      for(int i=0; i<MAXCOLOR; i++)// проверка счетчиков цветов
      	if(hSumm[i] != vSumm[i])  // если кокй-либо цвет неуравновешен
         	{
            equival = false;
          	strcpy(NumberColorToString(buf+strlen(buf),i),",\rпо горизонтали = ");
            itoa(hSumm[i],buf+strlen(buf),10);
            strcpy(buf+strlen(buf),", по вертикали = ");
            itoa(vSumm[i],buf+strlen(buf),10);
            strcpy(buf+strlen(buf),".\r");
            }

         if(!equival && !ExecDemo)
         	{
            strcpy(buf+strlen(buf)-1,checks);
            MessageBox(hWnd,buf,mes,MB_OK|MB_ICONSTOP);
            }
         delete[] buf;
         return equival;
  		}
   else return false;
  return true;
}

void KROSSWORD::SetParam(void) //Член-функция изменения параметров
{
  byte *buf, *cbuf, *sour, *dist;
  bool *mbuf, *msour, *mdist;
  int hsize=0, vsize=0, hnumbsize, vnumbsize, x, y, *Numb;
  bool colors = KROSSFEATURES::Colors > 1 ? true : false;
  bool corr = false;
  if((Containers == MONOCHROME) && colors)
  	 {
    hnumbsize = MAX(SavehSize, hSize)*MAX(SavehNumbSize, hNumbSize);
	 vnumbsize = MAX(SavevSize, vSize)*MAX(SavevNumbSize, vNumbSize);
	 hData->DigitColors = new byte[hnumbsize];
	 vData->DigitColors = new byte[vnumbsize];
    FillMemory((LPVOID)hData->DigitColors,hnumbsize,BACK);
    FillMemory((LPVOID)vData->DigitColors,vnumbsize,BACK);
    Containers=colors ? COLORS : MONOCHROME;
    corr = true;
    }
  else if((Containers == COLORS) && !colors)
    {
    DELETE(hData->DigitColors);
    DELETE(vData->DigitColors);
    hData->DigitColors = vData->DigitColors = NULL;
    Containers=colors ? COLORS : MONOCHROME;
    for(x=0; x<SavehSize*SavevSize; x++)
    	if(BitMap[x] != ZEROS && BitMap[x] != MARKER) BitMap[x] = BLACK;
    corr = true;
    }
  else Containers=colors ? COLORS : MONOCHROME;
// Изменяем, если требуется, размер картинки
  if(SavehSize!=hSize || SavevSize!=vSize)
    {
    MapLen = hSize*vSize;
    buf = new byte[MapLen];
    memset(buf, ZEROS, MapLen);
    hsize = MIN(SavehSize, hSize);
    vsize = MIN(SavevSize, vSize);
//Копируем, если требуется, старую картинку
    for(y=0; y<vsize; y++)
       for(x=0; x<hsize; x++)
          buf[y*hSize+x]=BitMap[y*SavehSize+x];
    DELETE(BitMap);
    BitMap = buf;
    if(LetterMap){ // Обновляем карту Литералов
	    DELETE(LetterMap);
   	 LetterMap = new byte[MapLen];
	    memset(LetterMap, 0, MapLen);
       if(StartLetter)
   	 	StartLetter->UpdateMatrix(LetterMap, hSize, vSize);
       }
    }
//Копируем, если требуется, горизонтальную оцифровку
  if(SavehNumbSize!=hNumbSize || (bool)hsize)
    {
//    colors = KROSSFEATURES::Colors > 1 ? true : false;
    if(!hsize) hsize = hSize;

    buf = new byte[hSize*hNumbSize];
    ZeroMemory((LPVOID)buf,hSize*hNumbSize);
    cbuf = new byte[hSize*hNumbSize];
    ZeroMemory((LPVOID)cbuf,hSize*hNumbSize);
    mbuf = new bool[hSize*hNumbSize];
    ZeroMemory((LPVOID)cbuf,hSize*hNumbSize*sizeof(bool));

    hnumbsize = MIN(SavehNumbSize, hNumbSize);
    for(x=0; x<hsize; x++){
    	y=hnumbsize;
      sour=hData->Digits+x*SavehNumbSize;
      dist=buf+x*hNumbSize;
      msour=hData->DigitMarkers+x*SavehNumbSize;
      mdist=mbuf+x*hNumbSize;
      while(y--) {*dist++ = *sour++; *mdist++ = *msour++;}
      y=hnumbsize;
      dist=cbuf+x*hNumbSize;
  	 	sour=hData->DigitColors+x*SavehNumbSize;
      while(y--) *dist++ = *sour++;
      }
  	 DELETE(hData->Digits);
    hData->Digits = buf;
    DELETE(hData->DigitColors);
    hData->DigitColors = cbuf;
    DELETE(hData->DigitMarkers);
    hData->DigitMarkers=mbuf;
    Numb = new int[hSize];
    ZeroMemory((LPVOID) Numb, hSize*sizeof(int));
    for(x=0; x<hsize; x++)
       Numb[x] = hData->Number[x] > hnumbsize ? hnumbsize : hData->Number[x];
    DELETE(hData->Number);
    hData->Number = Numb;
    hDigitSize = hNumbSize * hSize;
    }
//Копируем, если требуется, вертикальную оцифровку
  if(SavevNumbSize!=vNumbSize || (bool)vsize)
    {
    if(!vsize) vsize = vSize;
    buf = new byte[vSize*vNumbSize];
    ZeroMemory((LPVOID)buf,vSize*vNumbSize);
    cbuf = new byte[vSize*vNumbSize];
    ZeroMemory((LPVOID)cbuf,vSize*vNumbSize);
    mbuf = new bool[vSize*vNumbSize];
    ZeroMemory((LPVOID)cbuf,vSize*vNumbSize*sizeof(bool));

    vnumbsize = MIN(SavevNumbSize, vNumbSize);
    for(y=0; y<vsize; y++){
       x=vnumbsize;
       sour=vData->Digits+y*SavevNumbSize;
       dist=buf+y*vNumbSize;
       msour=vData->DigitMarkers+y*SavevNumbSize;
       mdist=mbuf+y*vNumbSize;
       while(x--) {*dist++ = *sour++; *mdist++ = *msour++;}
       x=vnumbsize;
       dist=cbuf+y*vNumbSize;
  	 	 sour=vData->DigitColors+y*SavevNumbSize;
       while(x--) *dist++ = *sour++;
       }
    DELETE(vData->Digits);
    vData->Digits = buf;
    DELETE(vData->DigitColors);
    vData->DigitColors = cbuf;
    DELETE(vData->DigitMarkers);
    vData->DigitMarkers = mbuf;
    Numb = new int[vSize];
    ZeroMemory((LPVOID) Numb, vSize*sizeof(int));
    for(x=0; x<vsize; x++)
       Numb[x] = vData->Number[x] > vnumbsize ? vnumbsize : vData->Number[x];
    DELETE(vData->Number);
    vData->Number = Numb;
    vDigitSize = vNumbSize * vSize;
    }
  	ScreenInitialize();
   if(corr) {Corrected(false); ChangesInFile(false);}
}

void KROSSWORD::ClearMark(void){
  PrepareUndo();
  for(int i=0; i<MapLen; i++)
     if(BitMap[i] == MARKER) BitMap[i] = ZEROS;
  InvalidateRect(hWnd, &Fild, false);
}

void KROSSWORD::Congratulation(void)
{
	STATUS("Кроссворд решен! ;)");
	MessageBox(hWnd, "Примите мои поздравления!!!", "У Вас получилось!", MB_ICONINFORMATION|MB_OK);
	STATUS("");
}

void KROSSWORD::Characteristics (void)	// Вывод характеристик кроссворда
{
// Создание строки характеристик
   int i, cells=0, full=0;//, hsumm=0, vsumm=0;
   char *buff = new char[1024];
   char caption[]={"Свойства кроссворда."};
   hDigitSize = hSize*hNumbSize;
   vDigitSize = vSize*vNumbSize;

// имя
   strcpy(buff, "Имя файла:\t\t\t");
   strcat(buff, FileTitle);
// тип
   strcat(buff, "\rТип кроссворда:\t\t\t");
   FILE_TYPES ext = GetExtension();
	if		 (ext==PPIFF1_FILE) strcat(buff, "PPIFF");
   else if(ext==PPIFF2_FILE) strcat(buff, "PPIFF2(цветной)");
   else if(ext==PPIFFS_FILE) strcat(buff, "файл отложенный игры PPIFF");
   else strcat(buff, Containers == COLORS ? "Цветной" : "Монохромный");
   if(*FileRemark){
	   strcat(buff, "\rКомментарий:\r");
   	strcat(buff, FileRemark);
      }
// размеры
   strcat(buff, "\r\rРазмер по вертикали:\t\t");
   itoa(vSize, buff+strlen(buff), 10);
   strcat(buff, "\rРазмер по горизонтали:\t\t");
   itoa(hSize, buff+strlen(buff), 10);
//	Матрица
   strcat(buff, "\rКлеток в матрице:\t\t\t");
   itoa(hSize*vSize, buff+strlen(buff), 10);
//	Уравновешенность и сумма цифр в оцифровке
	for(i=0; i<hDigitSize; i++) if(hData->Digits[i]) {cells++; /*hsumm+=hData->Digits[i];*/}
	for(i=0; i<vDigitSize; i++) if(vData->Digits[i]) {cells++; /*vsumm+=vData->Digits[i];*/}
   strcat(buff, "\r\rОцифровка ");
   strcat(buff, CheckSumm() ? "уравновешена." : "неверна !!!\r\
Несоответствие сумм горизонтальной\rи вертикальной оцифровки.\rПроверьте правильность ввода.");
/*   strcpy(buff+strlen(buff), "\rСумма по вертикали:\t\t");
   itoa(vsumm, buff+strlen(buff), 10);
   strcpy(buff+strlen(buff), "\rСумма по горизонтали:\t\t");
   itoa(hsumm, buff+strlen(buff), 10);*/
//	Количество цифр в оцифровке
   strcat(buff, "\rКоличество цифр:\t\t\t");
	itoa(cells, buff+strlen(buff), 10);
// Количество и процент определенных клеток
	for(i=0; i<vSize*hSize; i++)
      if(BitMap[i] != ZEROS) full++;
   cells = (full*100)/(hSize*vSize);// расчитываем проценты
   strcat(buff, "\r\rОпределено клеток:\t\t");
	itoa(full, buff+strlen(buff), 10);
   strcat(buff, " \(");
	itoa(cells, buff+strlen(buff), 10);
   strcat(buff, "%\)");

   MessageBox(hWnd, buff, caption, MB_ICONINFORMATION);
   delete[] buff;
}

void KROSSWORD::LookCriticalMenuItems(bool enable)
{
	LookMenuItem(IDM_FEATURES, enable);
	LookMenuItem(IDM_REDO, enable);
	LookMenuItem(IDM_UNDO, enable);
 	LookMenuItem(IDM_SCLEFT, enable);
	LookMenuItem(IDM_SCRIGHT, enable);
	LookMenuItem(IDM_SCUP, enable);
	LookMenuItem(IDM_SCDOWN, enable);
	LookMenuItem(IDM_DELDIG, enable);
	LookMenuItem(IDM_CLEARFIELD, enable);
	LookMenuItem(IDM_CORRECTED, enable);
	LookMenuItem(IDM_MAKEOVERLAPED, enable);
}
void KROSSWORD::Calc  (void)
{
  if(!CheckSumm()) return;
  CALCDATA cd;
  CALCULATE::GetData(&cd);
  if(cd.Runing)
	{				// Если программа запущена
	STATUS("Остановка анализа рисунка");
	StopThread();	// Остановка нити анализа
   ModifyMenu(IDM_CALCULATE, "&Решить кроссворд\tF5");
   LookCriticalMenuItems(false);
	}
  else
	{				// Если программа свободна
	PrepareUndo();
   ModifyMenu(IDM_CALCULATE, "Остановить &решение\tF5");
	STATUS("Выполняется анализ рисунка");
	ChangeInTable(true, false);
	CurrLine = CompleteCounter = 0;	// обнуляем локальный счетчик найденных ячеек
   LookCriticalMenuItems(true);
	CALCULATE::Run();	// Запуск анализа
	}
}

void KROSSWORD::MakeOverlapping(void)
{
  if(!CheckSumm()) return;
  CALCDATA cd;
  CALCULATE::GetData(&cd);
  if(cd.Runing)
	return;				// Если программа запущена
  PrepareUndo();
  for(int i=0; i<hSize*vSize; i++) BitMap[i] = ZEROS;
  CALCULATE::MakeOverlappingCells();
  STATUS("Перекрытые ячейки отмечены.");
  InvalidateRect(hWnd, &Fild, false);
}

void KROSSWORD::EndCalc  (bool res)
{
  	CALCDATA cd;
  	CALCULATE::GetData(&cd);
   if(cd.CompleteCounter != CompleteCounter) // есть изменения
		PaintCell(cd.Complete+CompleteCounter,cd.CompleteCounter-CompleteCounter);

//	Timer();
	HDC hDC = GetDC(hWnd);
	PaintIconic(hDC);
	ReleaseDC(hWnd, hDC);
//   if(NoClassics)
//		RedrawWindow(hWnd, &Fild, NULL, RDW_INVALIDATE|RDW_NOERASE);
//   InvalidateRect(hWnd, &Fild, false);
   ModifyMenu(IDM_CALCULATE, "&Решить кроссворд\tF5");
   LookCriticalMenuItems(false);
  	if(res) STATUS("Анализ рисунка окончен.");
   else    STATUS("Рисунок отгадан не полностью.");
   if(Result==CALCERROR && UndoFlag) Undo_(true);
}

void KROSSWORD::Timer (void)
{
	PAINTING::Timer();
  	CALCDATA cd;
  	CALCULATE::GetData(&cd);
   if(!cd.Runing || cd.Target) return;	// Пропуск если программа стоит
													// или работает в монопольном реж.
/*	if(!CALCULATE::NoClassics && cd.CurrLine != CurrLine){
// Обновляем строку статуса
//	char status[]={"Обрабатывается "};
   	char line[]=  {"Строка: "};
	   char column[]={"Столбец:"};
   	char buf[100];
//   strcpy(buf, status);
	   strcpy(buf, cd.That ? column : line);
   	itoa(cd.CurrLine+1, buf+strlen(buf), 10);
	   strcat(buf, " (");
   	ltoa(cd.CurrLineVar, buf+strlen(buf), 10);
	   strcat(buf, " вариантов)");
   	STATUS(buf);
		CurrLine = cd.CurrLine;
   	}*/

// Если же программа работает в параллельном режиме необходимо обновлять
// экран соответствено найденым ячейкам
   if(cd.CompleteCounter == CompleteCounter) return;// нет изменений
   PaintCell(cd.Complete+CompleteCounter,cd.CompleteCounter-CompleteCounter);
// Обновляем локальный счетчик
   CompleteCounter = cd.CompleteCounter;
}

void KROSSWORD::CommandProcessing(UINT Msg,WPARAM WParam,LPARAM LParam)
{
	static bool buttonsdown = false;
   if(Containers == EMPTY)
   	{
      if(Msg == WM_PAINT) DefPaint();
      else DefWindowProc (hWnd, Msg, WParam, LParam);
      return;
      }

   switch (Msg) {	// перехват всех сообщений мыши
      case WM_MBUTTONUP:
      case WM_LBUTTONUP:
		case WM_RBUTTONUP:
         UpButton((int)WParam, (int)LParam);
         if(buttonsdown)
            {
         	buttonsdown = false;
				MOldButtonStatus = ((WParam & MK_LBUTTON)	|
         		(WParam & MK_RBUTTON) | (WParam & MK_MBUTTON));
            }
			return;
      case WM_MBUTTONDOWN:
      case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
         if(!buttonsdown)
           	{
				MOldButtonStatus = ((WParam & MK_LBUTTON)	|
           		(WParam & MK_RBUTTON) | (WParam & MK_MBUTTON));
           	if(MouseInput == CRECT)
              	{
            	LastCell = ((HIWORD(LParam)-Fild.top)/RectSize)*hSize+
            		  			(LOWORD(LParam)-Fild.left)/RectSize;
               memcpy(SaveMaps, BitMap, hSize*vSize);
//               for(int i=0; i<hSize*vSize; i++) SaveMaps[i] = BitMap[i];
               PrepareUndo();
               }
            buttonsdown = true;
            }
      }

   switch (Msg) {
		case WM_COMMAND:           // обработка сообщений меню
      	switch(LOWORD(WParam))  // обработка команд
         	{
            case IDM_SAVE:
            	Save(false);
               return;

            case IDM_SAVE_AS:
               Save(false, true);
               return;

            case IDM_FEATURES:
              	SetParam();
               InvalidateRect(hWnd,NULL,true);
               return;

            case IDM_PRINT:
              	Print();
              	return;

            case IDM_UNDO:
               Undo_(true);
               return;

            case IDM_REDO:
               Undo_(false);
               return;

            case IDM_DELDIG:
               DelDig();
               return;

            case IDM_CLEARMASK:
               ClearMask();
               return;

            case IDM_CLEARFIELD:
               ClearField();
               return;

            case IDM_CLEARMARK:
               ClearMark();
               return;

            case IDM_SCLEFT:
            case IDM_SCRIGHT:
            case IDM_SCUP:
            case IDM_SCDOWN:
               ScrollingData(LOWORD(WParam));
               return;

            case IDM_CORRECTED:
               {
                CALCDATA cd;
  					 CALCULATE::GetData(&cd);
            	 if(!Corrected() && !cd.Runing) MessageBox(hWnd,"Нет смысла, матрица пустая.", "Расчет оцифровки",MB_OK);
                return;
               } 

            case IDM_CALCULATE:
					Calc();
					return;

				case IDM_MAKEOVERLAPED:
					MakeOverlapping();
					return;

            case IDM_CHARACTERISTICS:
               Characteristics();
               return;

            case IDM_MAX:
               Maximize();
               return;

            case IDM_MIN:
               Minimize();
               return;

            case IDM_NORM:
               Normalize();
               return;

            case IDM_CHOOSEFONT:
              	ChooseFonts();
               return;

            case IDM_STATUSBOX:
               StatusCheck();
               return;

            case IDC_WORKNOLOGIC:	// нить приступила к поику вариантов решения некласс. кросс.
               if(ExecDemo) return;
            	PrepareUndo();
               return;
      		}

        case WM_MBUTTONDOWN:
           MButtonDown((int)LOWORD(LParam),(int)HIWORD(LParam), (WORD)WParam);
           return;

        case WM_LBUTTONDOWN:
           LButtonDown((int)LOWORD(LParam),(int)HIWORD(LParam), (WORD)WParam);
           return;

		  case WM_RBUTTONDOWN:
           RButtonDown((int)LOWORD(LParam), (int)HIWORD(LParam), (WORD)WParam);
           return;

        case WM_MOUSEMOVE:
           MouseMove((int)LOWORD(LParam),(int)HIWORD(LParam), (WORD)WParam);
           return;

        case WM_TIMER:
			  Timer();
			  return;

        case WM_SIZE:
           WindowSizing();
			  return;

        case WM_VSCROLL:
           vScroll((int)LOWORD(WParam),(short int)HIWORD(WParam));
           return;

        case WM_HSCROLL:
           hScroll((int)LOWORD(WParam),(short int)HIWORD(WParam));
           return;

        case WM_MOUSEWHEEL:
           short int i;
           i=(short int)(-((short int)HIWORD(WParam))/120);
           if(LOWORD(WParam)==MK_SHIFT || LOWORD(WParam)==MK_MBUTTON) //Horizontal scroll
                hScroll(NULL, i, true);
           else vScroll(NULL, i, true);   //Vertical scroll
           return;

        case WM_KEYDOWN:
           KeyIn((int)WParam, (bool)(GetKeyState(VK_CONTROL) & 0x80));
           return;

         case WM_PAINT:
			  PaintProc();
			  return;

         case WM_DRAWITEM:
           PaintColorPal((LPDRAWITEMSTRUCT)LParam);
           return;
      }
}

void KROSSWORD::DefPaint (void)
{
   RECT Client;
   PAINTSTRUCT ps;
   HDC hDC=BeginPaint(hWnd,&ps);
   GetClientRect(hWnd,&Client);
   HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC, CreateSolidBrush(RGB(128,128,128)));
   PatBlt(hDC,Client.left,Client.top,Client.right, Client.bottom,PATCOPY);
   DeleteObject(SelectObject(hDC,hOldBrush));
   EndPaint(hWnd,&ps);
   ValidateRect(hWnd,NULL);
}

