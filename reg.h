#ifndef _REGS_
#define _REGS_
#include "defines.h"

#define CONTROLSUMM "Parent"
#define NUMBSTART "Start"
#define PASSWORD_KEY "Key"
#define PASSWORD 0x000b4868
#define SHAREWARE_LIMIT 40 // количество запусков демо версии программы

typedef struct MENUITEMDATAtag{
	UINT id;
   UINT enable;
   UINT check;
   bool looks;
   } MENUITEMDATA;

class GLOBAL {
   HKEY hKey;
   void StoreValue(void);
   void GetValue  (void);
	void RegistryExtentions(void);
//   void StartSecurity(void);
//   DWORD ControlSumm(void);
//   void Secure(void);
//	void MessageCaution();
//	void ProgrammCreked();
//   bool CopyDeleteFile(char *path, bool test = false);
   MENUITEMDATA *MenuItemData;
   int MenuDataEndPointer;

   int FindRegItem(UINT id);
protected:
   void Registration();

public:
   static CRITICAL_SECTION sinc;
	HWND hWnd, hStatusWnd, hToolWnd, hWndToolTip;
   HACCEL hAccel;
   HMENU Menu;
   HINSTANCE hinst;
   LOGFONT lf;
   char *SubKey;
	GLOBAL *This;

	bool creates;	// флаг указатель, что файл был создан а не открыт
   bool DiagonalPotting;
   bool Demo;
   bool ExecDemo;
	bool ViewWeb;
   bool ViewWeb5x5;
	bool ViewPictures;
   bool ViewProtocol;
   bool ClearPicturesFromCalc;
   bool StatusBox;
   bool ToolBar;
   bool MultyInstance;
   bool LibraryLoaded;				// Флаг признак открытия библиотеки
   int NumberLibraryEntry;       // Номер файла в библ. для загрузки
   int WindowToolHeight; 			// Высота панели инструментов
   int StatusHight;             	// Высота окна строки статуса
   int PanelWidth;               // Ширина панели рисования
   DWORD  WindowPosX, WindowPosY;
   DWORD  WindowSizeX, WindowSizeY;
   char *FileTitle, *LastFile, *FileRemark;
	COLORREF ColorTable[FIRST+MAXCOLOR+EXTCOLOR];//глабальная индексная таблица цветов
	COLORREF SaveColorTable[FIRST+MAXCOLOR+EXTCOLOR];
   int Colors;               		//Numbers of colors
   byte CurrentColor;				  // номер текущего цвета
   int colorsave;
   byte GetNearestColorIndex(RGBQUAD c); // Получение ближайшего цвета
	byte GetNearestColorIndex(COLORREF c);

   GLOBAL();
   ~GLOBAL(){
     StoreValue();
     if(LastFile) delete[] LastFile;
     if(FileTitle) delete[] FileTitle;
     if(FileRemark) delete[] FileRemark;
     RegCloseKey(hKey);
     delete[] MenuItemData;
     delete[] SubKey;
     DeleteCriticalSection(&sinc);
     }
	void EnableMenuItem (HMENU Menu, UINT id, UINT state);
   void EnableItem (UINT id, bool state);
	void LookMenuItem(UINT id, bool looks);
   bool GetMenuItemState (UINT id);
   void ModifyMenu(UINT item, char *name);
	void SetMenuItem(HWND hwnd);
   DWORD CheckMenuItem(HMENU Menu, UINT id, UINT checked);
	void CommandProcessing (WORD Command);
//   int GetNearestColorIndex(RGBQUAD color);
   void SavePalette (bool That);
	void ResetPalette(bool that, bool s=false);
   int  GetDifferenceColor(COLORDATA *cd);
   char *NumberColorToString(char *buf, int numb);
	bool GetSetupValue(LPBYTE dist, char *KeyName, int Size);
	void SetSetupValue(LPBYTE dist, char *KeyName, int size);
	bool CreateDesktopLink(LPSTR, LPSTR);			// Создает ярлык
   char *PathToFolder(char *ptr); //Удаляет имя файла из полного пути
   };
#endif //_REGS_

