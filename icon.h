#ifndef _MY_ICON_
#define _MY_ICON_
#include "features.h"

typedef struct MICONINFOtag{
   BYTE iiWidth;   // ширина
   BYTE iiHeight;  // высота
	DWORD iiClrUsed;// колич цветов
   WORD iiUnknow1; //= 0
   DWORD iiDataLen; //= 0x02e8
   DWORD iiDataOffset; //смещение от начала файла
   						  //до данных иконки
} MICONINFO;

typedef struct ICONFILEHEADERtag{
	WORD ifhUnknow1;
   WORD ifhUnknow2;		// == 1
   WORD ifhImageCount;	// количество иконок
   MICONINFO ifhIconInfo[1];
} ICONFILEHEADER;

typedef struct ICONHEADERtag {
	DWORD ihSize; // = sizeof(this structure)
   DWORD ihRectSize;// ??? = 0x20
   DWORD ihUnknow2; // ???(ihRectSize*2)=  0x40
   WORD  ihBitPlanes;  // = 1
   WORD  ihBitCount; // = 4
   DWORD ihUnknow3; // = 0
   DWORD ihBitsLen; //общий размер матрицы индексов и AND маски = 0X280
   DWORD ihUnknow4; // = 0
   DWORD ihUnknow5; // = 0
   DWORD ihUnknow6; // = 0
   DWORD ihUnknow7; // = 0
} ICONHEADER;

class MICON{
	ICONFILEHEADER *ifh;
   KROSSFEATURES *feat;
public:
	MICON(void *filedata, KROSSFEATURES *feat);
   ~MICON() {DELETE(Image);}
	bool GetImage(WORD numb);
	COLORREF Convert(RGBQUAD rgbq);
	bool error;
   WORD numb;
   WORD *Image;
};
#endif //_MY_ICON_
