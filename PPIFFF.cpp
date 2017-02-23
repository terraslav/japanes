// плагин для работы с PPIFF форматом
#include "PPIFF.h"

PPIFF::~PPIFF() //деструктор
{
	DELETE(Column);
   DELETE(Line);
//	DELETE(Remark);
   DELETE(FilePtr);
   delete[] IndexColorTable;
}

char *PPIFF::DigitToStringA(char *DString, byte n)
{
	if(!n) return DString;
   if(n>99)
   	{
      *DString++=(char)(n/100+0x30);
      n=(byte)(n%100);
      *DString++=(char)(n/10+0x30);
      n=(byte)(n%10);
      }
   else if(n>9)
   	{
      *DString++=(char)(n/10+0x30);
      n=(byte)(n%10);
      }
   *DString++=(char)(n+0x30);
   *DString='\0';
   return DString;
}

char *PPIFF::DigitToString(char *DString, byte n)
{
	if(!n) return DString;
   if(n>99)
   	{
      *DString++=DIGIT_3;
      *DString++=(char)(n/100+0x30);
      n=(byte)(n%100);
      *DString++=(char)(n/10+0x30);
      n=(byte)(n%10);
      }
   else if(n>9)
   	{
      *DString++=DIGIT_2;
      *DString++=(char)(n/10+0x30);
      n=(byte)(n%10);
      }
   *DString++=(char)(n+0x30);
   *DString='\0';
   return DString;
}

char *PPIFF::RGBtoString(char *ptr, COLORDATA *cd)
{
   byte x,y;

  	*ptr++=(byte)(cd->NumberColor+'0'-1);
  	*ptr++='=';
   y=GetRValue(cd->Color);
   x=(byte)(y>>4); y&=0xf;
   x=(byte)(x>9?x+0x37:x+'0');
   y=(byte)(y>9?y+0x37:y+'0');
	*ptr++=x;
	*ptr++=y;
   y=GetGValue(cd->Color);
   x=(byte)(y>>4); y&=0xf;
   x=(byte)(x>9?x+0x37:x+'0');
   y=(byte)(y>9?y+0x37:y+'0');
	*ptr++=x;
	*ptr++=y;
   y=GetBValue(cd->Color);
   x=(byte)(y>>4); y&=0xf;
   x=(byte)(x>9?x+0x37:x+'0');
   y=(byte)(y>9?y+0x37:y+'0');
 	*ptr++=x;
	*ptr++=y;
	return ptr;
}

void PPIFF::PrepareSaveData()
{
#define SPACE {*ptr++=' ';}
#define CarRet {*ptr++=0xd; *ptr++=0xa;}
#define REMLEN 80
//#define REMARK "File is created by program \"Matrix\". By Terraslav()<http:\/\/www.asgrem.narod.ru>"
   char Remark[]={0x13,0x3C,0x39,0x30,0x75,0x3C,0x26,0x75,0x36,0x27,0x30,0x34,
   0x21,0x30,0x31,0x75,0x37,0x2C,0x75,0x25,0x27,0x3A,0x32,0x27,0x34,0x38,0x75,
   0x77,0x18,0x34,0x21,0x27,0x3C,0x2D,0x77,0x7B, // "Matrix".
   0x75,0x17,0x2C,0x75,0x01,0x30,0x27,0x27,0x34,0x26,0x39,0x34,0x23,0x7D,0x7C, //Terraslav()
   0x69,0x3D,0x21,0x21,0x25,0x6F,0x7A,0x7A,0x22,0x22,0x22,0x7B,0x34,0x26,0x32,0x27,0x30,0x38,0x7B,0x3B,0x34,0x27,0x3A,0x31,0x7B,0x27,0x20,0x6B,0x55}; //<http://www.asgrem.narod.ru>"
   for(int i=0; i<REMLEN; i++) Remark[i]^=0x55;
   char *a=Remark;
	FileLen = (KrossData->hDigitSize+KrossData->vDigitSize)*8+12+REMLEN+\
   				KrossData->hSize*2+KrossData->vSize*4+KrossData->hSize*KrossData->vSize+128; //128 - резерв для определения палитры
   FilePtr = new byte[FileLen];
   memset(FilePtr, 0, FileLen);
   char *ptr = DigitToStringA((char *)FilePtr, (byte)KrossData->hSize);
	SPACE
   ptr = DigitToStringA(ptr, (byte)KrossData->vSize);
  	SPACE
   while(*a)*ptr++=*a++;
   if(KrossData->Containers==COLORS){
   	COLORDATA cd={0,0};										// сохраняем измененные цвета
  		while(KrossData->Global->GetDifferenceColor(&cd)<12){
      	SPACE
			ptr=RGBtoString(ptr,&cd);
   		}
      }

// сохраняем символы матрицы
//   char Subscribe[]={"BY_LOMASTER_MATRIX"};
//   int offs=KrossData->hSize>=(int)strlen(Subscribe)?0:12;

   if(KrossData->StartLetter)
//   	KrossData->StartLetter=new Char_In_Matrix((byte)(KrossData->hSize-strlen(Subscribe+offs)),(byte)KrossData->vSize,'@');
//  	if(!KrossData->StartLetter->findLetter(Subscribe[offs],
//     													(byte)(KrossData->hSize-strlen(Subscribe+offs)+1),
//     												  	(byte)KrossData->vSize))
//	   for(int i=0; i<(int)strlen(Subscribe+offs); i++)
//     		KrossData->StartLetter->AddLetter((byte)(i+KrossData->hSize-strlen(Subscribe+offs)+1),(byte)KrossData->vSize,Subscribe[i+offs]);
  		KrossData->StartLetter->Serialize(ptr);// сохраняем символы матрицы
   ptr+=strlen(ptr);


   int i, x=KrossData->hNumbSize, y=KrossData->vNumbSize; // преобразуем оцифровку
  	for(i=0; i<KrossData->hSize; i++)
      {
      if (!(i%5)) CarRet
      else SPACE
  		for(int j=0; j<x; j++)
         {
        	ptr = DigitToString(ptr, (byte)KrossData->hData->Digits[i*x+x-1-j]);
         if(j+1==x && !KrossData->hData->Digits[i*x])
         	*ptr++='-';
         }
      }
   CarRet
  	for(i=0; i<KrossData->vSize; i++)
      {
      if (!(i%5)) CarRet
      else SPACE
  		for(int j=0; j<y; j++)
         {
        	ptr = DigitToString(ptr, (byte)KrossData->vData->Digits[i*y+y-1-j]);
         if(j+1==y && !KrossData->vData->Digits[i*y])
         	*ptr++='-';
         }
      }
////////////////////////////////////////////////////////////////////////////////
	if(KrossData->Containers==COLORS)							   // преобразуем цвета
   	{
      CarRet
      char tmp;
		char IndexToChar[]={"00brmgcy789nultiaop"};
      *IndexToChar=0;
   	x=KrossData->hNumbSize, y=KrossData->vNumbSize;
	  	for(i=0; i<KrossData->hSize; i++)
   	   {
      	if (!(i%5)) CarRet
	      else SPACE
  			for(int j=0; j<x; j++)
      	   {
            tmp = KrossData->hData->DigitColors[i*x+x-1-j];
        		tmp = IndexToChar[tmp];
            if(tmp)*ptr++ = tmp;
	         if(j+1==x && !KrossData->hData->DigitColors[i*x])// проверка на пустоту строки
   	      	*ptr++='-';
      	   }
	      }
   	CarRet
	  	for(i=0; i<KrossData->vSize; i++)
   	   {
      	if (!(i%5)) CarRet
	      else SPACE
  			for(int j=0; j<y; j++)
      	   {
        		tmp = IndexToChar[KrossData->vData->DigitColors[i*y+y-1-j]];
            if(tmp)*ptr++ = tmp;
	         if(j+1==y && !KrossData->vData->DigitColors[i*y])
   	      	*ptr++='-';
      	   }
	      }
      }

   PPFileLen=(int)((byte *)ptr-(byte *)FilePtr);			// Сохраняем матрицу
   char IndexToSimbol[MARKER+1]={"?0brmgcy789nultiaop"}; //табл. перекодировки матрицы в спец. символы(?.)
   IndexToSimbol[MARKER]='.';

   for(i=0; i<KrossData->MapLen; i++) if(KrossData->BitMap[i]) break;
	if(i!=KrossData->MapLen)
   	{
      CarRet
      *ptr++='=';
   	for(y=0; y<KrossData->vSize; y++)
      	{
   		CarRet
      	for(x=0; x<KrossData->hSize; x++)
   			*ptr++ = IndexToSimbol[KrossData->BitMap[y*KrossData->hSize+x]];
	      }
      }
   FileLen = (int)((byte *)ptr-(byte *)FilePtr);
#undef SPACE
#undef CarRet
}

int PPIFF::LoadMatrix(int i)
{
	char temp;
   int x,y;
   i=GetNextCharAddres(i);
  	for(y=0; y<vsize; y++)
   	{
     	for(x=0; x<hsize; x++)
     		{
         if(i>=FileLen)
         	{
            strcat(KrossData->FileRemark,"\r\rНеверный формат PPIFF отложенной игры:\rРисунок сохранен не полностью!");
            return i;
            }
         temp = FilePtr[i++];
         if(temp=='?') temp=0;
         else if(temp=='.') temp=MARKER;
         else temp=IndexColorTable[temp];
//  			if(KrossData->ViewPictures)
         KrossData->BitMap[y*hsize+x]=temp;
      	}
      while(i<FileLen && (FilePtr[i]==0xa||FilePtr[i]==0xd)) i++;
      }
	return i;
}

int PPIFF::GetNextCharAddres(int i) //процедура пропуска незначящих символов (\r\n пробел)
{
   byte f;
   for(; i<FileLen; i++) // Получение ширины и высоты
      {
      f=FilePtr[i];
      if(f==0xd || f==0xa) Stroka++;
      if(f!=' ' && f!=0xd && f!=0xa)
      	break;
      }
	return i;
}

char *PPIFF::GetItem(char *item, char *ptr)
{
	while((bool)(*ptr) && *ptr==' ') ptr++; // space remove
   if(!*ptr) return NULL;
   int i=0;
   while(i<10 && (bool)(*ptr) && *ptr!=' ') item[i++]=*ptr++;
   if(i<10) item[i]=0;
   else *item=0;
   return ptr;
}

void PPIFF::SetupExtData(char *ptr)
{
	char item[10]; //255,255=K 9=FFFFFF
   bool extA=false, extC=false, errA=false, errC=false;
   while((bool)(ptr = GetItem(item, ptr))){
      if(item[1]=='=' && *item>='0' && *item<='9') // 9=FFFFFF  описание цвета
      	{
         extC = true;
         byte rgb[3];
         for(int i=0; i<3; i++)
         	{
            int a=(int)item[i*2+2];
            int b=(int)item[i*2+3];
            if(a>='0' && a<='9') 		a=(a-'0')&0xf;
            else if(a>='A' && a<='F') 	a=(a-'A'+10)&0xf;
            else if(a>='a' && a<='f') 	a=(a-'a'+10)&0xf;
            else {errC=true; break;}
            if(b>='0' && b<='9') 		b=(b-'0')&0xf;
            else if(b>='A' && b<='F') 	b=(b-'A'+10)&0xf;
            else if(b>='a' && b<='f') 	b=(b-'a'+10)&0xf;
            else {errC=true; break;}
            rgb[i]=(byte)((a<<4)+b);
            }
         KrossData->ColorTable[*item-'0'+FIRST]=RGB(rgb[0], rgb[1], rgb[2]);
         }
      else {                                         //255,255=K Описание буквы
			byte a=0, b=0;
         char c;
         for(byte i=0; i<10; i++) if(item[i]==',') b=i; else if(item[i]=='=') a=i;
         if(!a || !b){continue;}
         c=item[a+1];
         item[a]=item[b]=0;
         a=(byte)atoi(item);
         b=(byte)atoi(item+b+1);
         if(a>hsize || b>vsize){errA=true; continue;}
         if(!KrossData->StartLetter)
         	  KrossData->StartLetter=new Char_In_Matrix(a,b,c);
         else KrossData->StartLetter->AddLetter(a,b,c);
         extA=true;
      	}
   	}
   if(extA) strcat(KrossData->FileRemark, "\rА - Японские кроссворды с буквами.");
   if(extC) strcat(KrossData->FileRemark, "\rC - определение точных цветов.");
   if(errA) strcat(KrossData->FileRemark, "\rОписание ресширения А содержит ошибки!");
   if(errC) strcat(KrossData->FileRemark, "\rОписание ресширения C содержит ошибки!");
}

bool PPIFF::GetDataFromText()
{
	char *ExternData = NULL;
   Stroka=0;
   memset(KrossData->FileRemark, 0, MAX_REMARK_LEN);
   RemarkCount = 0;
   ColumnCount = LineCount = 0;
	int CLS, tmp, i;
   byte la,lb,lc;
   hsize=0;
   vsize=0;
   bool who=false;
   KrossData->Containers=MONOCHROME;
   i=GetNextCharAddres(0);
   for(i; i<FileLen; i++) // Получение ширины и высоты
		if(FilePtr[i]>=0x30 && FilePtr[i]<=0x39)
      	{
         if(!who) hsize=hsize*10+FilePtr[i]-0x30;
         else     vsize=vsize*10+FilePtr[i]-0x30;
         }
      else if(FilePtr[i]==' ')
      	{
         if(who) {i=GetNextCharAddres(i); break;}
         who=true;
         i=GetNextCharAddres(i)-1;
         }
      else if(FilePtr[i]==0xd || FilePtr[i]==0xa)
     		{
         GetNextCharAddres(i);
         break;
         }
   if(!vsize || !hsize) return false;
   DELETE(KrossData->BitMap);	// Сразу выделяем память для матрицы
  	KrossData->MapLen=hsize*vsize;
  	KrossData->BitMap = new byte[hsize*vsize];// Allocate memory for Bit Maps
  	FillMemory(KrossData->BitMap, (DWORD)KrossData->MapLen, ZEROS);//Zero BitMap

	int ColumnLimit = hsize*MAX_NUMBER_IN_LINE; // задаем ограничители буферов
   int LineLimit   = vsize*MAX_NUMBER_IN_LINE;
   Column = new byte [ColumnLimit];
   ColumnColor = new byte [ColumnLimit];
   memset(Column, 0, ColumnLimit);
   memset(ColumnColor, 0, ColumnLimit);

	Line      = new byte [LineLimit];
	LineColor = new byte [LineLimit];
   memset(Line, 0, LineLimit);
   memset(LineColor, 0, LineLimit);

   if((!(bool)Stroka) || ((FilePtr[i]<0x30 || FilePtr[i]>0x39) && (FilePtr[i]!=DIGIT_2 && FilePtr[i]!=DIGIT_3)))// копируем комментарий
   	for(; i<FileLen; i++)
      	if(FilePtr[i] == 0xd || FilePtr[i] == 0xa)
            {
         	i=GetNextCharAddres(i);
            break;
            }
         else if(FilePtr[i] == '=')
         	{
            while ((bool)RemarkCount && FilePtr[i] != ' ' && i) {i--; RemarkCount--;}
            KrossData->FileRemark[RemarkCount]='\0';
            int save=i;
            while (FilePtr[i] != 0xd && FilePtr[i] != 0xa) i++;
            ExternData = new char[i-save+1];
            memcpy(ExternData, FilePtr+save, i-save);
            ExternData[i-save]='\0';
            i=GetNextCharAddres(i);
            if(RemarkCount)strcat(KrossData->FileRemark, "\r");
            strcat(KrossData->FileRemark, "Файл содержит расширения формата PPIFF:");
            break;
            }
         else {if(RemarkCount<MAX_REMARK_LEN) KrossData->FileRemark[RemarkCount++]=FilePtr[i]; else break;}
	if((RemarkCount<MAX_REMARK_LEN-2) && *KrossData->FileRemark) strcat (KrossData->FileRemark+RemarkCount," ");

   bool CR=false;		// переводим оцифровку
   bool errDigit=false;// ошибка ввода двухзначных и трехзначных чисел
   byte LS=0;			// хранит последний символ
   who=false;
   for(; i<FileLen; i++)
   	if((ColumnCount>=ColumnLimit) || (LineCount>=LineLimit)) break;
      else if(FilePtr[i]==0xd || FilePtr[i]==0xa)
      	{
         LS = FilePtr[i];
         if(CR)
        		if(who) break;
         	else {
            	who = true;
               for(;i<FileLen; i++)
               	if(FilePtr[i]>=0x30 && FilePtr[i]<=0x39 ||\
                  	FilePtr[i]==ZERO_LINE || FilePtr[i]==DIGIT_2 || FilePtr[i]==DIGIT_3) break;
               i--;
               continue;
               }
         CR = true;
         if(!who) {if((ColumnCount<ColumnLimit) && ColumnCount && Column[ColumnCount-1]) Column[ColumnCount++]=0;}
         else		{if((LineCount<LineLimit) && LineCount && Line[LineCount-1]) Line[LineCount++]=0;}
      	if(FilePtr[i+1]==0xd || FilePtr[i+1]==0xa)
            if(FilePtr[i+1]!=LS) i++;
         }
		else if(FilePtr[i]=='=') break;	// Если отложенное решение
		else if(FilePtr[i] == ' ') //разделитель строк
      	{
         if(LS==FilePtr[i]) continue;
         LS=FilePtr[i];
         if(!who) {if((ColumnCount<ColumnLimit) && ColumnCount && Column[ColumnCount-1]) Column[ColumnCount++]=0;}
         else		{if((LineCount<LineLimit) && LineCount && Line[LineCount-1]) Line[LineCount++]=0;}
         }
		else if(FilePtr[i] == ZERO_LINE) //пустая строка
      	{
         LS=FilePtr[i];
         if(!who) {if(ColumnCount<ColumnLimit) Column[ColumnCount++]=ZERO_MARKER;}
         else		{if(LineCount<LineLimit) Line[LineCount++]=ZERO_MARKER;}
         CR = false;
         }
		else if(FilePtr[i] == DIGIT_2 || FilePtr[i] == DIGIT_3) //спецсимвол .,-или/
      	{
         if(FilePtr[i]==DIGIT_2)
         	{
            if(i+2>FileLen) return false;
            la = (byte)(FilePtr[++i]-(byte)'0');
            lb = (byte)(FilePtr[++i]-(byte)'0');
            if(la>9) {errDigit=true; la=0;}
            if(lb>9) {errDigit=true; lb=0;}
            LS = (byte)(la*10+lb);
            if(!who) {if(ColumnCount<ColumnLimit) Column[ColumnCount++]=LS; else break;}
	         else		{if(LineCount<LineLimit)Line [LineCount++]=LS; else break;}
            }
         else {
            if(i+3>FileLen) return false;
            la = (byte)(FilePtr[++i]-'0');
            lb = (byte)(FilePtr[++i]-'0');
            lc = (byte)(FilePtr[++i]-'0');
            if(la>9) {errDigit=true; la=0;}
            if(lb>9) {errDigit=true; lb=0;}
            if(lc>9) {errDigit=true; lc=0;}
            LS = (byte)(la*100+lb*10+lc);
            if(!who) {if(ColumnCount<ColumnLimit) Column[ColumnCount++]=LS; else break;}
	         else		{if(LineCount<LineLimit) Line[LineCount++]=LS; else break;}
         	}
         LS=FilePtr[i];
         CR = false;
         }
		else if(FilePtr[i]>=0x30 && FilePtr[i]<=0x39) // цифра
      	{
         if(!who) {if(ColumnCount<ColumnLimit) Column[ColumnCount++]=(byte)(FilePtr[i]-(byte)0x30); else break;}
         else		{if(LineCount<LineLimit) Line[LineCount++]=(byte)(FilePtr[i]-(byte)0x30); else break;}
         LS=FilePtr[i];
         CR = false;
         }
   	else {
      	errDigit=true;
			break;
	      }
   if(errDigit) strcat(KrossData->FileRemark, "\r\rВнимание!!! Неверный формат PPIFF:\rописание оцифровки содержит ошибки!");
   if(i<FileLen)
      {
      i=GetNextCharAddres(i);//Проверка на цвет или отложенность игры
		if(FilePtr[i]=='=') i=LoadMatrix(++i);
      }


   CR=false;	// переводим цвета оцифровки
   LS=0;			// хранит последний символ
   who=false;  // заполняем промежуточный буффер 0-разделитель,1-Черный,2-Синий...ZERO_MARKER-Пустая строка
   int CColumnCount = 0, CLineCount = 0;
   for(; i<FileLen; i++)
      if((CColumnCount>=ColumnLimit) || (CLineCount>=LineLimit)) break;
      else if(FilePtr[i]==0xd || FilePtr[i]==0xa)
      	{
         LS = FilePtr[i];
         if(CR)
        		if(who) break;
         	else {
            	who = true;
               i=GetNextCharAddres(i)-1;
//               for(;i<FileLen; i++) // пропускаем ненужные символы
//               	if(FilePtr[i]!=0xd && FilePtr[i]!=0xa) break;
//               i--;
               continue;
               }
         CR = true;
         if(!who) {if((CColumnCount<ColumnLimit) && CColumnCount && ColumnColor[CColumnCount-1]) ColumnColor[CColumnCount++]=0;}
         else		{if((CLineCount<LineLimit) && CLineCount && LineColor[CLineCount-1])	LineColor[CLineCount++]=0;}
      	if(FilePtr[i+1]==0xd || FilePtr[i+1]==0xa)
            if(FilePtr[i+1]!=LS) i++;
         }
		else if(FilePtr[i]=='=') break;	// Если отложенное решение
		else if(FilePtr[i] == ' ') //разделитель строк
      	{
         if(LS==FilePtr[i]) continue;
         LS=FilePtr[i];
         if(!who) {if((CColumnCount<ColumnLimit) && CColumnCount && ColumnColor[CColumnCount-1]) ColumnColor[CColumnCount++]=0;}
         else		{if((CLineCount<LineLimit) && CLineCount && LineColor[CLineCount-1]) LineColor[CLineCount++]=0;}
         }
		else if(FilePtr[i] == ZERO_LINE) //пустая строка
      	{
         LS=FilePtr[i];
         if(!who) {if(CColumnCount<ColumnLimit) ColumnColor[CColumnCount++]=ZERO_MARKER; else break;}
         else		{if(CLineCount<LineLimit) LineColor[CLineCount++]=ZERO_MARKER; else break;}
         CR = false;
         }
		else if(IndexColorTable[FilePtr[i]]!=ERR_INDEX_COLOR) // цвет
      	{
         KrossData->Containers=COLORS; // Отмечаем, что кроссворд цветной
         if(!who) {if(CColumnCount<ColumnLimit) ColumnColor[CColumnCount++]=(byte)IndexColorTable[FilePtr[i]]; else break;}
         else		{if(CLineCount<LineLimit) LineColor[CLineCount++]=(byte)IndexColorTable[FilePtr[i]]; else break;}
         LS=FilePtr[i];
         CR = false;
         }
   	else {
      	strcat(KrossData->FileRemark, "\r\rВнимание!!! Неверный формат PPIFF:\rтаблица цвета оцифровки содержит ошибки!");
			break;
         }

   if(i<FileLen)
      {
      i=GetNextCharAddres(i);//Проверка на цвет или отложенность игры
		if(FilePtr[i]=='=') i=LoadMatrix(++i);
      }
   i=GetNextCharAddres(i);
   if(i!=FileLen)
      strcat(KrossData->FileRemark, "\r\rВнимание!!! Неверный формат PPIFF:\rдлина файла не верна!");


   tmp = ColumnSize = LineSize = 0;	// расчитываем ширину оцифровок
   for(i=0; i<=ColumnCount; i++)
      {
   	if(!Column[i])
      	{
         if(tmp>ColumnSize) ColumnSize=tmp;
         tmp=0;
         }
      else tmp++;
      }
   for(i=0; i<=LineCount; i++)
      {
   	if(!Line[i])
      	{
         if(tmp>LineSize) LineSize=tmp;
         tmp=0;
         }
      else tmp++;
      }

  	KrossData->hSize = hsize;         // Fill size
  	KrossData->vSize = vsize;
  	KrossData->hNumbSize = ColumnSize;
   KrossData->vNumbSize = LineSize;
//   KrossData->Containers=MONOCHROME;
   KrossData->Status=NOCHANGE;          // Every time is no changing
  	KrossData->ResetPalette(KrossData->Containers==MONOCHROME);

//Показатели количества цифр оцифровки в строке
  	KrossData->hData->Number=new int[hsize];// Allokate memory for listing numbers
  	KrossData->vData->Number=new int[vsize];// digits in line

//Выделение памяти под массивы оцифровки кроссворда
  	KrossData->hDigitSize = ColumnSize*hsize;
  	KrossData->vDigitSize = LineSize*vsize;
  	KrossData->hData->Digits = new byte[KrossData->hDigitSize]; // Allocate memory for
  	KrossData->vData->Digits = new byte[KrossData->vDigitSize]; // digits
	KrossData->hData->DigitColors = new byte[KrossData->hDigitSize]; // Allocate memory for
	KrossData->vData->DigitColors = new byte[KrossData->vDigitSize]; // digits color
	KrossData->hData->DigitMarkers = new bool[KrossData->hDigitSize]; // Allocate memory for
	KrossData->vData->DigitMarkers = new bool[KrossData->vDigitSize]; // digits color
	ZeroMemory((LPVOID)KrossData->hData->Number, (DWORD)(hsize*sizeof(int)));
	ZeroMemory((LPVOID)KrossData->vData->Number, (DWORD)(vsize*sizeof(int)));
	ZeroMemory((LPVOID)KrossData->hData->Digits, (DWORD)KrossData->hDigitSize);
	ZeroMemory((LPVOID)KrossData->vData->Digits, (DWORD)KrossData->vDigitSize);
	ZeroMemory((LPVOID)KrossData->hData->DigitColors, (DWORD)KrossData->hDigitSize);
	ZeroMemory((LPVOID)KrossData->vData->DigitColors, (DWORD)KrossData->vDigitSize);
 	memset(KrossData->hData->DigitMarkers, false, KrossData->hDigitSize*sizeof(bool));
 	memset(KrossData->vData->DigitMarkers, false, KrossData->vDigitSize*sizeof(bool));
   int ptr = 0;
  	for(i=0; i<hsize; i++)
  		for(int j=0; j<ColumnSize; j++)
      	{
         LS = Column[ptr];
         CLS = ColumnColor[ptr++];
         if(LS==ZERO_MARKER)
         	{if(!Column[ptr]) ptr++;break;}
         if(!LS) break;
  	   	else {
         	KrossData->hData->Digits[i*ColumnSize+ColumnSize-1-j]=LS;
         	KrossData->hData->DigitColors[i*ColumnSize+ColumnSize-1-j]=(char)CLS;
            }
         if(j+1==ColumnSize) ptr++;
  	   	}
	ptr = 0;
  	for(i=0; i<vsize; i++)
  		for(int j=0; j<LineSize; j++)
     	   {
			LS = Line[ptr];
         CLS = LineColor[ptr++];
         if(LS==ZERO_MARKER)
         	{if(!Line[ptr]) ptr++;break;}
         if(!LS) break;
  	   	else {
         	KrossData->vData->Digits[i*LineSize+LineSize-1-j]=LS;
    	  		KrossData->vData->DigitColors[i*(int)LineSize+(int)LineSize-1-j]=(char)CLS;
            }
         if(j+1==LineSize) ptr++;
  	      }
   if(ExternData) {SetupExtData(ExternData); delete[] ExternData;}
																// Заполняем матрицу латтералов
	if(KrossData->StartLetter)
   	{
      DELETE(KrossData->LetterMap);
   	KrossData->LetterMap = new byte[hsize*vsize];
      memset(KrossData->LetterMap, 0, hsize*vsize);
      Char_In_Matrix *tmp=KrossData->StartLetter;
      do{
        KrossData->LetterMap[(tmp->Line-1)*hsize+tmp->Column-1] = tmp->Letter;
        } while((bool)(tmp=tmp->Next));
      }

   FilePtr = NULL; // запрещаем деструктору удалять массив класса FILE_ROUTINE
   return true;
}                           

PPIFF::PPIFF (KROSSWORD *k, byte *ptr, int filelen=0) //конструктор
{
	KrossData = k;
	FilePtr = ptr;
	Column = Line = NULL;
//	Remark = NULL;
   FileLen = filelen;
   InitializeIndexColorTable();

	if(!filelen) {PrepareSaveData(); return;}
   faithfully = !GetDataFromText();
   if(faithfully) return;
}

void PPIFF::InitializeIndexColorTable()
{
#define INDEX 256
  	IndexColorTable = new char[INDEX];
	memset((void *)IndexColorTable, ERR_INDEX_COLOR, INDEX);
   IndexColorTable[0]=IndexColorTable['0'] = 1;
   IndexColorTable[1]=IndexColorTable['1']=IndexColorTable['b']=IndexColorTable['B']=2;
   IndexColorTable[2]=IndexColorTable['2']=IndexColorTable['r']=IndexColorTable['R']=3;
   IndexColorTable[3]=IndexColorTable['3']=IndexColorTable['m']=IndexColorTable['M']=4;
   IndexColorTable[4]=IndexColorTable['4']=IndexColorTable['g']=IndexColorTable['G']=5;
   IndexColorTable[5]=IndexColorTable['5']=IndexColorTable['c']=IndexColorTable['C']=6;
   IndexColorTable[6]=IndexColorTable['6']=IndexColorTable['y']=IndexColorTable['Y']=7;
   IndexColorTable[7]=IndexColorTable['7']=8;
   IndexColorTable[8]=IndexColorTable['8']=9;
   IndexColorTable[9]=IndexColorTable['9']=0xa;
   IndexColorTable['n']=IndexColorTable['N']=0xb;
   IndexColorTable['u']=IndexColorTable['U']=0xc;
   IndexColorTable['l']=IndexColorTable['L']=0xd;
   IndexColorTable['t']=IndexColorTable['T']=0xe;
   IndexColorTable['i']=IndexColorTable['I']=0xf;
   IndexColorTable['a']=IndexColorTable['A']=0x10;
   IndexColorTable['o']=IndexColorTable['O']=0x11;
   IndexColorTable['p']=IndexColorTable['P']=0x12;
#undef INDEX
}


