// io.h - header files from file routines
#ifndef _IO_
#define _IO_
#include "reg.h"
#include "resource.h"
/*
typedef struct FILE_HEADERtag {
  WORD   FileId;
  byte   Flags;
  byte   NumbColors;
  DWORD  OffsetBitmap;
  byte   hSize;
  byte   vSize;
  byte   hNumbSize;
  byte   vNumbSize;
  byte   Data[1];
  } FILE_HEADER;
*/

//////////////////////////структуры для формата JCC/////////////////////////////
typedef struct JCC_DD_tag {
	byte Digit;
   byte Color;
   } JCC_DD;

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
////////////////////////////////////////////////////////////////////////////////

typedef struct JCR_FILE_HEADERtag {
  byte hSize;
  byte vSize;
  byte vNumbSize;
  byte hNumbSize;
  byte Digits[1];
} JCR_FILE_HEADER;

class FILE_ROUTINE : public GLOBAL {
   OPENFILENAME ofn;
   char *DirBuffer;
public:
   byte *FileData;	// указатель на данные прочитанные из файла
   int FileDataLen;	// длина данных файла

   FILE_ROUTINE *This;
   FILE_ROUTINE()	 // Constructor
	{
      creates = true;
   	This = this;
	   FileData=NULL;
   	FileDataLen=0;
	   DirBuffer=new char[FILEPATHNAME];
   	GetCurrentDirectory (FILEPATHNAME, DirBuffer);
	}

   ~FILE_ROUTINE()	// Destructor
   {
	   delete[] DirBuffer;	// удаляем буфер пути
   	DELETE(FileData);	// удаляем буфер данных файла
   }

   void NotifyCreatesFile(void) {creates = true;}
   bool GetCreatesFile   (void) {if(creates) return GetSaveFileName(); return true;}
	void SelectNewFileName(char *name="Японец.jpn");
   bool GetSaveFileName (bool open = false);
   bool GetOpenFileName(char *name);
   bool Read_from_File(bool Quest=true);
   bool LoadesFromLybrary(int numberfile=0);
   bool SaveDataToFile(void);
   void SetAppData();
   void SetCaptionWindow(void);
   FILE_TYPES GetExtension(void);
   byte *GetData (void){return FileData;}
   byte *SetData  (byte *data, int datalen);

   int  GetDataLen (void){return FileDataLen;}
	void ChangeDataLen(int len, bool reset = false)
	{
		if(len <= FileDataLen && !reset) {FileDataLen=len; return;}
		DELETE(FileData);
   	FileData = new byte[len];
   	FileDataLen = len;
	}
};
#endif
