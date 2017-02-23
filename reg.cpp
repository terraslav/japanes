#include "features.h"
#include "reg.h"
#include <commctrl.h>

const DIAGONALPOTTING = 0x00000001;
const CLEARCALCFLAG   = 0x00000002;
const VIEWPICTFLAG    = 0x00000004;
const VIEWPROTFLAG    = 0x00000008;
const VIEWWEBFLAG     = 0x00000010;
const VIEWWEB5x5FLAG  = 0x00000020;
const VIEWSTATUSFLAG  = 0x00000040;
const DEMOFLAG			 = 0x00000080;
const TOOLFLAG        = 0x00000100;
const MULTYINSTANCE   = 0x00000200;

//GLOBAL* GLOBAL::This;
CRITICAL_SECTION GLOBAL::sinc;
/*
void GLOBAL::Registration()
{
	MessageBox(hWnd,"Демо-версия программы!\rОсновные Функции записи отключены.\rВы можете сохранить образ\r\
кроссворда только в формате BMP.\rДля приобретения полнофункциональной\rверсии обращайтесь к автору.", "Demo Version", MB_OK|MB_ICONINFORMATION);
} */

GLOBAL::GLOBAL()
{
	FileRemark = new char[MAX_REMARK_LEN];
	InitializeCriticalSection(&sinc);
  	hToolWnd = NULL;
  	LibraryLoaded = false;
  	PanelWidth = StatusHight = WindowToolHeight = NULL;
  	This = this;
  	MenuItemData = new MENUITEMDATA[NumbMenuItem];
  	ZeroMemory((void *)MenuItemData, sizeof(MENUITEMDATA)*NumbMenuItem);
  	MenuDataEndPointer = 0;
  	ExecDemo = false;
  	for(int i=0; i<MAXCOLOR; i++) ColorTable[i+FIRST] = VOIDCOLOR;
  	ColorTable[BACK]      = BACKCOLOR;
  	ColorTable[BLACK]     = RGB_BLACK;                   // Set colors
  	ColorTable[GRAY]      = RGB_GRAY;
  	ColorTable[LIGHTGRAY] = RGB_SILVER;

   DWORD type, size=MAX_REMARK_LEN;

  	SubKey = new char[strlen(FEATURES_KEY)+1];
  	strcpy (SubKey, FEATURES_KEY);
  	DWORD dwDisposition;
  	FileTitle=new char[FILETITLE];
  	*FileTitle=NULL;
  	LastFile=new char[FILEPATHNAME];
  	SearchPath(NULL,EXEFILENAME,NULL,FILEPATHNAME,FileRemark,NULL);
   GetShortPathName(FileRemark, LastFile,FILEPATHNAME);

  	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, SubKey, 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_ALL_ACCESS, NULL,&hKey,&dwDisposition)!=ERROR_SUCCESS) dwDisposition = REG_CREATED_NEW_KEY;
   else if(dwDisposition == REG_OPENED_EXISTING_KEY){
		RegQueryValueEx(hKey, "Folder Setup", 0, &type, (LPBYTE) FileRemark, &size);
      if(strcmp(FileRemark,LastFile)) dwDisposition = REG_CREATED_NEW_KEY;
   	}
  	RegCloseKey(hKey);

   *FileRemark=0;

  	if(dwDisposition == REG_CREATED_NEW_KEY)
     	{
      WindowPosX=GetSystemMetrics(SM_CXSCREEN)/2-WINDOW_SIZE_X/2;
     	WindowPosY=GetSystemMetrics(SM_CYSCREEN)/2-WINDOW_SIZE_Y/2;
     	WindowSizeX=WINDOW_SIZE_X; WindowSizeY=WINDOW_SIZE_Y;
  	  	ClearPicturesFromCalc = ToolBar = StatusBox = ViewProtocol = Demo = true;
  	  	ViewPictures = MultyInstance = DiagonalPotting = ViewWeb = ViewWeb5x5 = false;

//      SearchPath(NULL,EXEFILENAME,NULL,FILEPATHNAME,LastFile,NULL);
      strcat(PathToFolder(LastFile), "\\demo\\demo (1).jcr");

      RegistryExtentions();
      CreateDesktopLink("Mаtrix.lnk", "Японские кроссворды");
      bool t=creates;
      creates = false;
     	StoreValue();
      creates = t;
//     	StartSecurity();

     	}
   else
   	{
      GetValue();
      }
         //	Secure();
}

void GLOBAL::RegistryExtentions(void)
{
  	DWORD dwDisposition;
  	HKEY hkey, hskey;
   char *directory = new char[FILEPATHNAME],
   	  *buf       = new char[FILEPATHNAME];
   SearchPath(NULL,EXEFILENAME,NULL,FILEPATHNAME,buf,NULL);
   GetShortPathName(buf, directory,FILEPATHNAME);

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".pp", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"PP.Document", 12);
      RegCloseKey(hkey);
      }
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PP.Document", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"japanes crossword", 18);
		if(RegCreateKeyEx(hkey, "shell\\open\\command", 0,
      	(LPTSTR) NULL,     // address of class string
	      REG_OPTION_NON_VOLATILE,
   	   KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
	      strcpy(buf, directory);
   	   strcat(buf," \"%1\"");
      	RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
	      RegCloseKey(hskey);
         }
		if(RegCreateKeyEx(hkey, "shell\\DefaultIcon", 0,
      	(LPTSTR) NULL,     // address of class string
	      REG_OPTION_NON_VOLATILE,
   	   KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
	      strcpy(buf, directory);
   	   strcat(buf, ", 2");
      	RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
	      RegCloseKey(hskey);
   	   }
      RegCloseKey(hkey);
      }

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".pps", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"PPS.Document", 12);
      RegCloseKey(hkey);
      }
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PPS.Document", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"remitted japanes crossword", 18);
		if(RegCreateKeyEx(hkey, "shell\\open\\command", 0,
      	(LPTSTR) NULL,     // address of class string
	      REG_OPTION_NON_VOLATILE,
   	   KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
	      strcpy(buf, directory);
   	   strcat(buf," \"%1\"");
      	RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
	      RegCloseKey(hskey);
         }
		if(RegCreateKeyEx(hkey, "shell\\DefaultIcon", 0,
      	(LPTSTR) NULL,     // address of class string
	      REG_OPTION_NON_VOLATILE,
   	   KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
	      strcpy(buf, directory);
   	   strcat(buf, ", 2");
      	RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
	      RegCloseKey(hskey);
   	   }
      RegCloseKey(hkey);
      }

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".ppc", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"PPC.Document", 12);
      RegCloseKey(hkey);
      }
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "PPC.Document", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"Color japanes crossword", 18);
		if(RegCreateKeyEx(hkey, "shell\\open\\command", 0,
      	(LPTSTR) NULL,     // address of class string
	      REG_OPTION_NON_VOLATILE,
   	   KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
	      strcpy(buf, directory);
   	   strcat(buf," \"%1\"");
      	RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
	      RegCloseKey(hskey);
         }
		if(RegCreateKeyEx(hkey, "shell\\DefaultIcon", 0,
      	(LPTSTR) NULL,     // address of class string
	      REG_OPTION_NON_VOLATILE,
   	   KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
	      strcpy(buf, directory);
   	   strcat(buf, ", 2");
      	RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
	      RegCloseKey(hskey);
   	   }
      RegCloseKey(hkey);
      }
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, ".jcr", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"JC.Document", 11);
      RegCloseKey(hkey);
      }
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "JC.Document", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)"japanes crossword", 18);
		if(RegCreateKeyEx(hkey, "shell\\open\\command", 0,
   	   (LPTSTR) NULL,     // address of class string
      	REG_OPTION_NON_VOLATILE,
	      KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
      	strcpy(buf, directory);
	      strcat(buf," \"%1\"");
   	   RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
      	RegCloseKey(hskey);
	      }
		if(RegCreateKeyEx(hkey, "shell\\DefaultIcon", 0,
      	(LPTSTR) NULL,     // address of class string
      	REG_OPTION_NON_VOLATILE,
	      KEY_WRITE, NULL,&hskey,&dwDisposition)==ERROR_SUCCESS)
  			{
      	strcpy(buf, directory);
	      strcat(buf, ", 3");
   	   RegSetValueEx(hskey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
      	RegCloseKey(hskey);
	      }
      RegCloseKey(hkey);
      }

	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, "icofile\\shell\\open\\command", 0,
      (LPTSTR) NULL,     // address of class string
      REG_OPTION_NON_VOLATILE,
      KEY_WRITE, NULL,&hkey,&dwDisposition)==ERROR_SUCCESS)
  		{
      strcpy(buf, directory);
      strcpy(buf+strlen(buf)," \"%1\"");
      RegSetValueEx(hkey, NULL, 0, REG_SZ, (byte *)buf, strlen(buf));
      RegCloseKey(hkey);
      }
   delete[] buf;
   delete[] directory;
}
/*
void GLOBAL::Secure(void)
{
   DWORD contrsumm;
   if(GetSetupValue((LPBYTE)&contrsumm, PASSWORD_KEY, sizeof(DWORD)) && contrsumm == PASSWORD) return;
   bool exist = CopyDeleteFile(NULL, TRUE);
	DWORD currsumm = ControlSumm();
   if(!currsumm) ExitProcess(0);
   contrsumm = 0xffffffff;
   if(!GetSetupValue((LPBYTE)&contrsumm, CONTROLSUMM, sizeof(DWORD))) ProgrammCreked();
   if(currsumm != contrsumm) ProgrammCreked();
   if(!GetSetupValue((LPBYTE)&contrsumm, NUMBSTART, sizeof(DWORD))) ProgrammCreked();
   contrsumm++;
   SetSetupValue((LPBYTE)&contrsumm, NUMBSTART, sizeof(DWORD));
   if(contrsumm > SHAREWARE_LIMIT) MessageCaution();
	if(contrsumm <= SHAREWARE_LIMIT && exist) ProgrammCreked();
}

bool GLOBAL::CopyDeleteFile(char *directory, bool test)
{// копирует файл-стиратель данного приложения из ресурсов в системный директорий
   HANDLE CurrentFile;
   bool arrayerase = false;
   if(!directory) {directory = new char[MAX_PATH*4]; arrayerase=true;}
   if(!GetSystemDirectory(directory, MAX_PATH*4)) ExitProcess(0);
   strcpy(directory+strlen(directory),"\\Spool31l.exe");
   CurrentFile=CreateFile(directory,GENERIC_WRITE,FILE_SHARE_READ,
                                         NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, NULL);
   if(test || CurrentFile!=INVALID_HANDLE_VALUE)
   	{ // если это проверка сущ. файла или файл уже существует
      if(arrayerase) delete[] directory;
      if(CurrentFile==INVALID_HANDLE_VALUE) return false;
      CloseHandle(CurrentFile);
      return true;
      }
   CurrentFile=CreateFile(directory,GENERIC_WRITE,FILE_SHARE_READ,
                                         NULL, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, NULL);
  	HRSRC hResInfo = FindResource(NULL, (char *)DEL_FILE_RES, RT_RCDATA);
   DWORD FileLen = SizeofResource(NULL, hResInfo);
	byte *FileData = (byte *)LoadResource(NULL, hResInfo);
   FileData = (byte *)LockResource((void *)FileData);
   SetFilePointer(CurrentFile,NULL,NULL,FILE_BEGIN);// Set file pointer
   DWORD numb_write = 0;
   if(!WriteFile(CurrentFile, FileData, FileLen, &numb_write, NULL) ||
   					numb_write!=FileLen) ExitProcess(0);
   CloseHandle(CurrentFile);
   return true;
}

void GLOBAL::ProgrammCreked()
{
   char *path = new char[MAX_PATH*4];
   CopyDeleteFile(path, FALSE);
   WinExec(path, SW_SHOWDEFAULT);
//   CreateProcess(path,NULL,NULL,NULL,FALSE,
//   					CREATE_NEW_PROCESS_GROUP|HIGH_PRIORITY_CLASS,NULL,NULL,NULL,NULL);
   delete[] path;
	ExitProcess(0);
}

void GLOBAL::MessageCaution()
{
   CopyDeleteFile(NULL, FALSE);
   char  *mess = new char [5000];
  	LoadString(hinst, STRINGTABLE_101, mess, 5000);
   MessageBox(NULL, mess, "", MB_OK);
	ExitProcess(0);
}

void GLOBAL::StartSecurity()
{
	DWORD res;
	if(CopyDeleteFile(NULL, TRUE))
   	{
      res = 0xffffffff;
   	SetSetupValue((LPBYTE)&res, NUMBSTART, sizeof(DWORD));
      ProgrammCreked();
      }
	res = ControlSumm();
   if(!res) ExitProcess(0);
	SetSetupValue((LPBYTE)&res, CONTROLSUMM, sizeof(DWORD));
   res = 0;
   SetSetupValue((LPBYTE)&res, NUMBSTART, sizeof(DWORD));
}

DWORD GLOBAL::ControlSumm(void)
{
   char *directory = new char[FILEPATHNAME];
   SearchPath(NULL,EXEFILENAME,NULL,FILEPATHNAME,directory,NULL);
   HANDLE CurrentFile=CreateFile(directory,GENERIC_READ,FILE_SHARE_READ,
                                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(CurrentFile==INVALID_HANDLE_VALUE) return 0;
   DWORD c=0, reads,
   FileDataLen=GetFileSize(CurrentFile, NULL);
   byte *filedata=new byte[FileDataLen];
	SetFilePointer(CurrentFile,(LONG)0,NULL,FILE_BEGIN);//Обнуляем файловый указатель
//Считаем контрольную сумму файла
  	bool bResult=ReadFile(CurrentFile,(void *)filedata, FileDataLen, &reads, NULL);
   CloseHandle(CurrentFile);
   if(!bResult || reads!=FileDataLen) return 0;
   for(DWORD i=0; i<FileDataLen; i++)
      c+=(DWORD)filedata[i];
   return c;
}
*/
void GLOBAL::SetMenuItem(HWND hwnd)
{
   HMENU Menu = GetMenu(hwnd);
   CheckMenuItem(Menu,IDM_AUTOCLEAR, ClearPicturesFromCalc ? MF_CHECKED : MF_UNCHECKED);
   CheckMenuItem(Menu,IDM_VIEWPICTURES, ViewPictures ? MF_CHECKED : MF_UNCHECKED);
   CheckMenuItem(Menu,IDM_VIEWWEB, ViewWeb ? MF_CHECKED : MF_UNCHECKED);
   if(!ViewWeb)
   	{
      ViewWeb5x5 = false;
      EnableMenuItem(Menu,IDM_WEB5x5,MF_GRAYED);
      }
   CheckMenuItem(Menu,IDM_WEB5x5, ViewWeb5x5 ? MF_CHECKED : MF_UNCHECKED);
   CheckMenuItem(Menu,IDM_VIEWLOG, ViewProtocol ? MF_CHECKED : MF_UNCHECKED);
   CheckMenuItem(Menu,IDM_DEMO, !Demo ? MF_CHECKED : MF_UNCHECKED);
   CheckMenuItem(Menu,IDM_DIAGONALPOTTING, DiagonalPotting ? MF_CHECKED : MF_UNCHECKED);
   CheckMenuItem(Menu,IDM_MULTYINSTANCE, MultyInstance ? MF_CHECKED : MF_UNCHECKED);
}

bool GLOBAL::GetSetupValue(LPBYTE dist, char *KeyName, int Size)
{
	DWORD size = (DWORD) Size;
	if(RegQueryValueEx(hKey, KeyName, 0, NULL, dist, &size)==ERROR_SUCCESS) return true;
   return false;
}

void GLOBAL::SetSetupValue(LPBYTE dist, char *KeyName, int size)
{
   RegSetValueEx(hKey, KeyName, NULL, REG_BINARY, dist, (DWORD)size);
}

void GLOBAL::GetValue(void)
{
  char wX[]={"WindowX"};
  char wY[]={"WindowY"};
  char wSizeX[]={"Window height"};
  char wSizeY[]={"Window width"};
  char Cp[]={"ControlPanel"};
  char Lf[]={"Last Opened File"};
  DWORD type, data, size=sizeof(DWORD);

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, SubKey, 0,
      KEY_ALL_ACCESS, &hKey);

  RegQueryValueEx(hKey, wX, 0, &type, (LPBYTE) &WindowPosX, &size);
  RegQueryValueEx(hKey, wY, 0, &type, (LPBYTE) &WindowPosY, &size);
  RegQueryValueEx(hKey, wSizeX, 0, &type, (LPBYTE) &WindowSizeX, &size);
  RegQueryValueEx(hKey, wSizeY, 0, &type, (LPBYTE) &WindowSizeY, &size);
  RegQueryValueEx(hKey, Cp, 0, &type, (LPBYTE) &data, &size);

  DiagonalPotting = (bool)(data & DIAGONALPOTTING);
  ViewPictures = (bool)(data & VIEWPICTFLAG);
  ViewProtocol = (bool)(data & VIEWPROTFLAG);
  ClearPicturesFromCalc = (bool)(data & CLEARCALCFLAG);
  ViewWeb      = (bool)(data & VIEWWEBFLAG);
  ViewWeb5x5   = (bool)(data & VIEWWEB5x5FLAG);
  StatusBox    = (bool)(data & VIEWSTATUSFLAG);
  Demo         = (bool)(data & DEMOFLAG);
  ToolBar      = (bool)(data & TOOLFLAG);
  MultyInstance= (bool)(data & MULTYINSTANCE);

  size = FILEPATHNAME;
  RegQueryValueEx(hKey, Lf, 0, &type, (LPBYTE) LastFile, &size);
  if(strlen(LastFile))
     {
     GetFileTitle(LastFile, FileTitle, FILETITLE);
     char *c = new char[FILEPATHNAME];
     int i = 0;
     while(LastFile[i] != '.' && (bool)(LastFile[i])) {c[i] = LastFile[i];i++;}
     while(c[--i] != '\\');
     c[i] = 0;
     SetCurrentDirectory(c);
     delete[] c;
     }
  RegCloseKey(hKey);
}

void GLOBAL::StoreValue()
{
  char wX[]={"WindowX"};

  char wY[]={"WindowY"};
  char wSizeX[]={"Window height"};
  char wSizeY[]={"Window width"};
  char Cp[]={"ControlPanel"};
  char Lf[]={"Last Opened File"};
  char Path[]={"Folder Setup"};
  DWORD a;

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, SubKey, 0,
      KEY_ALL_ACCESS, &hKey);

  RegSetValueEx(hKey, wX, 0, REG_DWORD, (CONST BYTE *) &WindowPosX, sizeof(DWORD));
  RegSetValueEx(hKey, wY, 0, REG_DWORD, (CONST BYTE *) &WindowPosY, sizeof(DWORD));
  RegSetValueEx(hKey, wSizeX, 0, REG_DWORD, (CONST BYTE *) &WindowSizeX, sizeof(DWORD));
  RegSetValueEx(hKey, wSizeY, 0, REG_DWORD, (CONST BYTE *) &WindowSizeY, sizeof(DWORD));
  a=0;
  a |= DiagonalPotting ? DIAGONALPOTTING : 0;
  a |= ViewWeb ? VIEWWEBFLAG : 0;
  a |= ViewPictures ? VIEWPICTFLAG : 0;
  a |= ViewProtocol ? VIEWPROTFLAG : 0;
  a |= ClearPicturesFromCalc ? CLEARCALCFLAG : 0;
  a |= ViewWeb5x5 ? VIEWWEB5x5FLAG : 0;
  a |= StatusBox ? VIEWSTATUSFLAG : 0;
  a |= Demo ? DEMOFLAG : 0;
  a |= ToolBar ? TOOLFLAG : 0;
  a |= MultyInstance ? MULTYINSTANCE : 0;

  RegSetValueEx(hKey, Cp, 0, REG_DWORD, (CONST BYTE *) &a, sizeof(DWORD));
  int i=strlen(LastFile);
  while(i && LastFile[i]!='.') i--;
  if(!i || creates) return;
//  if(strlen(LastFile) && !Demo)
  	RegSetValueEx(hKey, Lf, 0, REG_SZ, (CONST BYTE *) LastFile, strlen(LastFile));

   char pth[MAX_PATH], ptl[MAX_PATH];
  	SearchPath(NULL,EXEFILENAME,NULL,FILEPATHNAME,pth,NULL);
   GetShortPathName(pth, ptl,FILEPATHNAME);
  	RegSetValueEx(hKey, Path, 0, REG_SZ, (CONST BYTE *) ptl, strlen(ptl));
  	RegCloseKey(hKey);
}

void GLOBAL::CommandProcessing (WORD Command)
{
	HMENU Menu = GetMenu(hWnd);
   DWORD tmp;
	switch(Command)
   	{
      case IDM_AUTOCLEAR:
         ClearPicturesFromCalc = !ClearPicturesFromCalc;
         CheckMenuItem(Menu, IDM_AUTOCLEAR,
						ClearPicturesFromCalc ? MF_CHECKED : MF_UNCHECKED);
         break;

      case IDM_DIAGONALPOTTING:
         DiagonalPotting = !DiagonalPotting;
         CheckMenuItem(Menu, IDM_DIAGONALPOTTING,
						DiagonalPotting ? MF_CHECKED : MF_UNCHECKED);
         break;

      case IDM_VIEWPICTURES:
      	ViewPictures = !ViewPictures;
         CheckMenuItem(Menu, IDM_VIEWPICTURES,ViewPictures ? MF_CHECKED : MF_UNCHECKED);
         break;

      case IDM_VIEWWEB:
         ViewWeb = !ViewWeb;
         if(!ViewWeb)	ViewWeb5x5 = false;
			else {
         	tmp = CheckMenuItem(Menu, IDM_WEB5x5, MF_UNCHECKED);
            CheckMenuItem(Menu, IDM_WEB5x5, tmp);
            ViewWeb5x5 = tmp == MF_CHECKED ? true : false;
         	}
         EnableMenuItem(Menu, IDM_WEB5x5, ViewWeb ? MF_ENABLED : MF_GRAYED);
         CheckMenuItem(Menu, IDM_VIEWWEB, ViewWeb ? MF_CHECKED : MF_UNCHECKED);
         InvalidateRect(hWnd,NULL,true);
         break;

      case IDM_WEB5x5:
          ViewWeb5x5 = !ViewWeb5x5;
          CheckMenuItem(Menu, IDM_WEB5x5, ViewWeb5x5 ? MF_CHECKED : MF_UNCHECKED);
          InvalidateRect(hWnd,NULL,true);
          break;

      case IDM_VIEWLOG:
          ViewProtocol = !ViewProtocol;
          CheckMenuItem(Menu, IDM_VIEWLOG, ViewProtocol ? MF_CHECKED : MF_UNCHECKED);
          break;

      case IDM_DEMO:
          Demo = !Demo;
          CheckMenuItem(Menu, IDM_DEMO, Demo ? MF_UNCHECKED : MF_CHECKED);
          break;

      case IDM_MULTYINSTANCE:
          MultyInstance = !MultyInstance;
          CheckMenuItem(Menu, IDM_MULTYINSTANCE, MultyInstance ? MF_CHECKED : MF_UNCHECKED);
          break;
      }
   StoreValue();
}

int GLOBAL::FindRegItem(UINT id)
{
   int i;
   for(i=0; i<MenuDataEndPointer; i++)// поиск регистрации пункта меню
   	if(MenuItemData[i].id == id) return i;
   i = MenuDataEndPointer++;
	MenuItemData[i].id = id;
   return i;
}

void GLOBAL::EnableItem (UINT id, bool state)
{
	int NumbItem = FindRegItem(id);
  	MenuItemData[NumbItem].enable = state ? MF_ENABLED : MF_GRAYED;
  	if(MenuItemData[NumbItem].looks)
     	{                          // если пункт заблокирован
      ::EnableMenuItem(Menu, id, MF_GRAYED);
   	if(hToolWnd) SendMessage(hToolWnd,TB_ENABLEBUTTON,id,NULL);
      }
   else {
   	::EnableMenuItem(Menu, id, state ? MF_ENABLED : MF_GRAYED);
   	if(hToolWnd) SendMessage(hToolWnd,TB_ENABLEBUTTON,id,(LPARAM)MAKELONG(state,0));
      }
}

void GLOBAL::EnableMenuItem (HMENU Menu, UINT id, UINT state)
{
	int NumbItem = FindRegItem(id);
  	MenuItemData[NumbItem].enable = state;
  	if(MenuItemData[NumbItem].looks)
     	{                          // если пункт заблокирован
      ::EnableMenuItem(Menu, id, MF_GRAYED);
   	if(hToolWnd) SendMessage(hToolWnd,TB_ENABLEBUTTON,id,NULL);
      }
   else {
   	::EnableMenuItem(Menu, id, state);
   	if(hToolWnd) SendMessage(hToolWnd,TB_ENABLEBUTTON,id,(LPARAM)MAKELONG(state == MF_ENABLED ? true : false,0));
      }
}

void GLOBAL::LookMenuItem(UINT id, bool looks)
{
	int NumbItem = FindRegItem(id);
   if(!MenuItemData[NumbItem].looks && looks)
   	MenuItemData[NumbItem].enable = ::EnableMenuItem(Menu, id, MF_GRAYED);
   MenuItemData[NumbItem].looks = looks;
   if(looks) {
   	if(hToolWnd) SendMessage(hToolWnd,TB_ENABLEBUTTON,id,NULL);
		}
   else {
   	::EnableMenuItem(Menu, id, MenuItemData[NumbItem].enable);
   	if(hToolWnd) SendMessage(hToolWnd,TB_ENABLEBUTTON,id,
      	(LPARAM)MAKELONG(MenuItemData[NumbItem].enable == MF_ENABLED ? true : false,0));
      }
}

bool GLOBAL::GetMenuItemState (UINT id)
{
	return MenuItemData[FindRegItem(id)].looks;
}

void GLOBAL::ModifyMenu(UINT item, char *name)
{
   MENUITEMINFO mii;
   int numbitem = FindRegItem(item);
   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_TYPE;
   mii.fType = MFT_STRING;
   if(MenuItemData[numbitem].looks) mii.fState = MFS_GRAYED;
   else mii.fState = MenuItemData[numbitem].enable ? MFS_ENABLED : MFS_GRAYED;
   mii.wID = item;
   mii.hSubMenu = NULL;
   mii.hbmpChecked = NULL;
   mii.hbmpUnchecked = NULL;
   mii.dwItemData = NULL;
   mii.dwTypeData = name;
   mii.cch = strlen(name);
   SetMenuItemInfo(Menu, item, false, &mii);
}

DWORD GLOBAL::CheckMenuItem(HMENU Menu, UINT id, UINT checked)
{
   MenuItemData[FindRegItem(id)].check = (checked == MF_CHECKED);
   if(hToolWnd) SendMessage(hToolWnd,TB_CHECKBUTTON,id,(LPARAM)MAKELONG(checked == MF_CHECKED ? true : false,0));
   return ::CheckMenuItem(Menu, id, checked);
}
/*
int GLOBAL::GetNearestColorIndex(RGBQUAD color)
{
	int index = -1;
   int min = 0x300;
   int akkum, t;
   for(int i=0; i<MAXCOLOR; i++)
   	{
      t = GetRValue(ColorTable[i+FIRST]) - color.rgbRed;
      akkum = (t > 0 ? t : -t);
      t = GetGValue(ColorTable[i+FIRST]) - color.rgbGreen;
      akkum += (t > 0 ? t : -t);
      t = GetBValue(ColorTable[i+FIRST]) - color.rgbBlue;
      akkum += (t > 0 ? t : -t);
      if(akkum < 	min) {index = i+FIRST; min = akkum;}
      }
   if(ColorTable[index] == BACKCOLOR) index = ZEROS;
   return index;
}
*/
void GLOBAL::ResetPalette(bool that, bool s)
{// обнуление палитры
	COLORREF *CT=s?SaveColorTable:ColorTable;
  	CT[GRAY]      = RGB_GRAY;
  	CT[LIGHTGRAY] = RGB_SILVER;
	CT[BACK] = BACKCOLOR;
	CT[BLACK] = RGB_BLACK;
   This->Colors = 1; // для монохромии
   This->CurrentColor = BLACK;				  // номер текущего цвета
   if(that)
      for(int i=FIRST+1; i<FIRST+MAXCOLOR; i++) CT[i] = VOIDCOLOR;
   else // начальные цвета
      {
      int volatile i=FIRST;
      CT[i++]=RGB_BLACK;
      CT[i++]=RGB_BLUE; //синий
      CT[i++]=RGB_RED; //красный
      CT[i++]=RGB_MAGENTA;
      CT[i++]=RGB_GREEN; //зеленый
      CT[i++]=RGB_CYAN;
      CT[i++]=RGB_YELLOW;
      CT[i++]=RGB_SILVER;
      CT[i++]=RGB_GRAY;
      CT[i++]=RGB_MAROON; //светло синий
      CT[i++]=RGB_NAVY; //светло красный
      CT[i++]=RGB_PURPLE;
      CT[i++]=RGB_LIME; //светло зеленый
      CT[i++]=RGB_TEAL;
      CT[i++]=RGB_OLIVE;
      CT[i++]=RGB_AQUAMARINE;
      CT[i++]=RGB_ORANGE;
      CT[i++]=RGB_PINK;
      This->Colors = MAXCOLOR;
      }
}

int GLOBAL::GetDifferenceColor(COLORDATA *cd)
{
   ResetPalette(false, true);
   int i=++cd->NumberColor;
   for(;i<MAXCOLOR;i++)
   	if(SaveColorTable[i]!=ColorTable[i]) break;
   cd->Color = ColorTable[i];
   return cd->NumberColor=i;
}

char *GLOBAL::NumberColorToString(char *buf, int numb)
{
   COLORREF color = ColorTable[numb+FIRST];
   switch(color){
   	case RGB_BLACK: 		strcat(buf,"Black")		; break;
   	case RGB_BLUE:  		strcat(buf,"Blue")		; break;
   	case RGB_RED:   		strcat(buf,"Red")			; break;
   	case RGB_MAGENTA: 	strcat(buf,"Magenta")	; break;
   	case RGB_GREEN:   	strcat(buf,"Green")		; break;
   	case RGB_CYAN:    	strcat(buf,"Cyan")		; break;
   	case RGB_YELLOW:  	strcat(buf,"Yellow")		; break;
   	case RGB_SILVER:  	strcat(buf,"Silver")		; break;
   	case RGB_GRAY:    	strcat(buf,"Gray")		; break;
   	case RGB_MAROON:  	strcat(buf,"Maroon")		; break;
   	case RGB_NAVY:    	strcat(buf,"Navy")		; break;
   	case RGB_PURPLE:  	strcat(buf,"Purple")		; break;
   	case RGB_LIME:    	strcat(buf,"Lime")		; break;
   	case RGB_TEAL:    	strcat(buf,"Teal")		; break;
   	case RGB_OLIVE:   	strcat(buf,"Olive")		; break;
   	case RGB_AQUAMARINE: strcat(buf,"Aquamarine"); break;
   	case RGB_ORANGE:  	strcat(buf,"Orange")		; break;
   	case RGB_PINK:    	strcat(buf,"Pink")		; break;
      case RGB_WHITE:   	strcat(buf,"White")		; break;
		default:
		   strcat(buf," (RGB: ");
		   itoa(GetRValue(color),buf+strlen(buf),10);
   		strcat(buf,", ");
		   itoa(GetGValue(color),buf+strlen(buf),10);
   		strcat(buf,", ");
	   	itoa(GetBValue(color),buf+strlen(buf),10);
	   	strcat(buf,")");
		}
   return buf+strlen(buf);
}

void GLOBAL::SavePalette (bool That)
{// сохранение / восстановление палитры
	COLORREF *source = That ? ColorTable : SaveColorTable;
   COLORREF *dist   = That ? SaveColorTable : ColorTable;
   for(int i=0; i<FIRST+MAXCOLOR+EXTCOLOR; i++)
      dist[i]=source[i];
   if(That) {
   	colorsave = Colors;
      }
   else {
   	Colors = colorsave;
      }
}

bool GLOBAL::CreateDesktopLink(LPSTR Name, LPSTR Desc)			// Создает ярлык
{
   HRESULT hres;
   char szPathLink[MAX_PATH];
   CoInitialize(NULL);
   IShellLink* psl;
   ITEMIDLIST *pidl;

   char lpszPathObj[FILEPATHNAME];
   SearchPath(NULL,EXEFILENAME,NULL,FILEPATHNAME,lpszPathObj,NULL);

   if(SHGetSpecialFolderLocation(NULL,CSIDL_DESKTOPDIRECTORY,&pidl)!=NOERROR) return false;
	if(!SHGetPathFromIDList(pidl, szPathLink)) return false;
	strcat(szPathLink,"\\");
	strcat(szPathLink,Name);

// Get a pointer to the IShellLink interface.
 	hres = CoCreateInstance(CLSID_ShellLink, NULL,
        CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *)&psl);
  	if (SUCCEEDED(hres)) {
      IPersistFile* ppf;

        // Set the path to the shortcut target, and add the
        // description.
      psl->SetPath(lpszPathObj);

      psl->SetDescription(Desc);

       // Query IShellLink for the IPersistFile interface for saving the
       // shortcut in persistent storage.
      hres = psl->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf);

      if (SUCCEEDED(hres)) {
         wchar_t wsz[MAX_PATH];

            // Ensure that the string is ANSI.
         MultiByteToWideChar(CP_ACP, 0, szPathLink, -1,
             wsz, MAX_PATH);


            // Save the link by calling IPersistFile::Save.
         ppf->Save(wsz, TRUE);
         ppf->Release();
      	}
      psl->Release();
     	}
    CoUninitialize();
    return true;
}

char *GLOBAL::PathToFolder(char *ptr) //Обрезает имя файла из полного пути
{
	int i=strlen(ptr);
   if(i<=1) return ptr;
   while(--i && ptr[i]!='\\');
   ptr[i]=0;
   return ptr;
}

byte GLOBAL::GetNearestColorIndex(RGBQUAD c)
{
	int Separ[MAXCOLOR+FIRST];
	int R, G, B;

   for(int i=0; i<MAXCOLOR+FIRST; i++){
		R= (int)c.rgbRed - (int)GetRValue(ColorTable[i]);
      if(R<0) R=-R;
		G= (int)c.rgbGreen - (int)GetGValue(ColorTable[i]);
      if(G<0) G=-G;
		B= (int)c.rgbBlue - (int)GetBValue(ColorTable[i]);
      if(B<0) B=-B;
      if(R<G) R=G;
      if(R<B) R=B;
      Separ[i]=R;
      }

   int index=-1, akk = 0x7fffffff, tmp;
   for(int i=0; i<MAXCOLOR+FIRST; i++){
   	tmp = Separ[i];
      if(tmp < akk){
      	index=i;
         akk=tmp;
         }
      }
   return (byte) index;
}

byte GLOBAL::GetNearestColorIndex(COLORREF c)
{
	int Separ[MAXCOLOR+FIRST];
	int R, G, B;

   for(int i=0; i<MAXCOLOR+FIRST; i++){
		R= (int)GetRValue(c) - (int)GetRValue(ColorTable[i]);
      if(R<0) R=-R;
		G= (int)GetGValue(c) - (int)GetGValue(ColorTable[i]);
      if(G<0) G=-G;
		B= (int)GetBValue(c) - (int)GetBValue(ColorTable[i]);
      if(B<0) B=-B;
      if(R<G) R=G;
      if(R<B) R=B;
      Separ[i]=R;
      }

   int index=-1, akk = 0x7fffffff, tmp;
   for(int i=0; i<MAXCOLOR+FIRST; i++){
   	tmp = Separ[i];
      if(tmp < akk){
      	index=i;
         akk=tmp;
         }
      }
   return (byte) index;
}

/// Описание членов структуры Char_In_Matrix_tag, заголовок в файле defines.h///
void Char_In_Matrix_tag::Serialize(char *ptr){
	*ptr++=' ';
	strcat(itoa(Column, ptr, 10),",");
	ptr += strlen(ptr);
	strcat(itoa(Line, ptr, 10),"=");
	ptr += strlen(ptr);
	*ptr++=Letter;
	*ptr=0;
	if(Next) Next->Serialize(ptr);
}

void Char_In_Matrix_tag::AddLetter(byte a,byte b,char c){
   if(!c || c==' ' || c==0xd || c==0xa) return;
	if(Next) Next->AddLetter(a,b,c);
   else Next=new Char_In_Matrix_tag(a,b,c);
}

Char_In_Matrix_tag *Char_In_Matrix_tag::findLetter(char c, byte a,byte b){
   if(!c || c==' ' || c==0xd || c==0xa) return NULL;
	if(Letter==c){
      if(Column==0xff || Line==0xff) return this;
      else if(Column==a && Line==b) return this;
      }
   if(Next) return Next->findLetter(c,a,b);
   return NULL;
}

void Char_In_Matrix_tag::UpdateMatrix(byte *Matrix, int hsize, int vsize)
{
	if(Column<=hsize && Line<=vsize)
		Matrix[(Line-1)*hsize+(Column-1)]=Letter;
   if(Next) Next->UpdateMatrix(Matrix, hsize, vsize);
}
////////////////////////////////////////////////////////////////////////////////
