#ifndef _FEATURES_
#define _FEATURES_

#include "defines.h"
#include "resource.h"
#include "io.h"

typedef struct FILEBITMAPINFOtag{
  BITMAPFILEHEADER Fh;
  BITMAPINFOHEADER Bh;
  RGBQUAD Colors[1];
  } FILEBITMAPINFO, *LPFILEBITMAPINFO;

struct DATATAG {
   byte *Digits;             //Указатель на массив оцифровки
   byte *DigitColors;        //Указатель на массив цветов оцифровки
   bool *DigitMarkers;       //Маркеры определенной оцифровки
   int  *Number;             //Указатель на массив содержащий количество
   };                        //       действительных цифр в строке оцифровки

struct UNDOTAG{
   CONTAINERS Containers;    // Type class data
   STATUS     Status;        // Data status
   DATATAG    hData;     // Horizontal digitals array
   DATATAG    vData;     // Vertical digitals array
   COLORREF   Palette[MAXCOLOR];// палитра
   int        hSize;         // Horizontal size
   int        vSize;         // Vertical size
   int hNumbSize, vNumbSize; // Size table numbers
   int        MapLen;        // Size BitMap
	int 		  Colors;		  // Number colors
   byte       *Maps;         // Pointer to BitMaps
   };

class KROSSFEATURES : public FILE_ROUTINE {
public:
   KROSSFEATURES *This;
   CONTAINERS Containers;    // Type class data
   int SavehSize, SavevSize;
   int SavehNumbSize, SavevNumbSize;
   int hSize, vSize;         //Size kross table
   int hNumbSize, vNumbSize; //Size table numbers
   GLOBAL *Global;
   byte *BitMap;
   byte *LetterMap;
   Char_In_Matrix *StartLetter;
   DATATAG vConstData, hConstData;
   DATATAG *hData, *vData;
   CTOOL MouseInput;
   byte Flags;

   KROSSFEATURES()
   {
   This = this;
   MouseInput = CVOID;
   Global = GLOBAL::This;
   BitMap = NULL;
   hData = &hConstData;
   vData = &vConstData;
  	hData->Digits=hData->DigitColors=vData->Digits=vData->DigitColors=NULL;
  	hData->Number=vData->Number=NULL;
   LetterMap = BitMap = NULL;
   hSize=vSize=hNumbSize=vNumbSize=Colors=NULL;
   Containers = EMPTY;
   }
};

#endif
