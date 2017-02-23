#include "icon.h"
#include "dialog.h"

MICON::MICON(void *filedata, KROSSFEATURES *feat)
{
   MICON::feat = feat;
	ifh = (ICONFILEHEADER *)filedata;
   numb = 0;
   error = false;
   Image = new WORD[NUMBUNDO];
   if(ifh->ifhImageCount>NUMBUNDO) 	{error = true; return;}
//   for(WORD i=0; i<ifh->ifhImageCount; i++)	// поиск в каталоге ICO файла подходящей картинки
//   	if(ifh->ifhIconInfo[i].iiClrUsed <= MAXCOLOR) Image[numb++]=i;
   numb = ifh->ifhImageCount;
   if(!numb)			{error = true; return;} // выход если подходящих картинок нет
}

bool MICON::GetImage(WORD numb)
{
   ICONHEADER *ih = (ICONHEADER *)((LPBYTE)ifh + ifh->ifhIconInfo[numb].iiDataOffset);
	if(numb > MICON::numb || ih->ihBitCount>4) return false;
   int hSize = feat->hSize = (int)ifh->ifhIconInfo[numb].iiWidth;
   int vSize = feat->vSize = (int)ifh->ifhIconInfo[numb].iiHeight;
   RGBQUAD *ColorTab = (RGBQUAD *)((LPBYTE)ih + ih->ihSize);
   feat->Colors = (int)((WORD)0x1 << ih->ihBitCount);
   LPBYTE Matrix  = (LPBYTE)ColorTab + feat->Colors*sizeof(RGBQUAD);
   LPBYTE AndMask = Matrix+((ih->ihBitCount*ih->ihBitPlanes*ifh->ifhIconInfo[numb].iiWidth+31)/32)*4*
   									ifh->ifhIconInfo[numb].iiHeight;
// копируем палитру
	byte VoidColorIndex = (byte)((ifh->ifhIconInfo[numb].iiClrUsed <= 2) ? 1 : 15);
   if(feat->Colors <= MAXCOLOR)
		for(int i=0; i<feat->Colors; i++)
      	{
	      COLORREF t = feat->Global->ColorTable[FIRST+i]=RGBConvert(ColorTab[i]);
   		if(t == BACKCOLOR) VoidColorIndex = (byte)i;
      	}
   else  {
   		feat->Global->ResetPalette(false);
         }
// заполняем матрицу
   feat->BitMap = new byte[hSize*vSize];
//   COLORSMAPPED cm(feat->Global->ColorTable+FIRST, feat->Colors);
	byte akkum, mask = (byte)(((byte)0x1 << ih->ihBitCount)-1);
	for(int vert = 0; vert < vSize; vert++) // высота
   	for(int hdword = 0; hdword<((hSize*ih->ihBitCount+31)/32); hdword++) // ширина в байтах
         {
         for(int hbyte = hdword*4; hbyte < (hdword+1)*4; hbyte++)
         	{
         	akkum = Matrix[hbyte+((hSize*ih->ihBitCount+31)/32*4)*vert];
	      	for(int i=0; i<(8/ih->ihBitCount); i++)	// размер байта в пикселах
               if(8/ih->ihBitCount == 1) // если 256 цветов
               	{
                  feat->BitMap[(vSize-vert-1)*hSize+hbyte] =
                  				 feat->GetNearestColorIndex(ColorTab[akkum]);
//                  				 (byte)cm.GetNearestColorIndex(ColorTab[akkum]);
                  continue;
                  }
               else if((hbyte*(8/ih->ihBitCount)+i)<hSize)
	   	         {
   	   	      byte bt = (byte)(akkum >> (8-ih->ihBitCount)) & mask;// отделяем нужные биты
      	   	   if(ifh->ifhIconInfo[numb].iiClrUsed <= 2) bt ^= 1; // инверсия 1го бита если рисунок монохромный
         	   	else if(bt == VoidColorIndex)             bt = ZEROS; // если индекс белого цвета делаем его прозрачным
	         	   else													bt+=(byte)FIRST;//
   	         	feat->BitMap[(vSize-vert-1)*hSize+hbyte*(8/ih->ihBitCount)+i] = bt;
	      	      akkum <<= ih->ihBitCount;
   	      	   }
               else break;
            }
         }
// накрываем AND маской
   for(int vert = 0; vert < vSize; vert++) // высота
      for(int hdword = 0; hdword<(hSize+31)/32; hdword++)
			for(int hbyte = hdword*4; hbyte<(hdword+1)*4; hbyte++) // ширина в байтах
   	      {
      	   akkum = AndMask[vert*(((hSize+31)/32)*4)+hbyte];
      		for(int bits = 0; bits<8; bits++)
               if((hbyte*8+bits) < hSize)
	         		{
		            int addr = (vSize-vert-1)*hSize+hbyte*8+bits;
   		         if((bool)(akkum & 0x80))
      		      	if(feat->BitMap[addr]==BLACK) feat->BitMap[addr] = ZEROS;// прозрачность
         		      else feat->BitMap[addr]=MARKER;								   // инверсия
            		akkum <<= 1;
		            }
               else break;
         	}
   if(feat->Colors == 2) feat->Colors--;
   return true;
}

