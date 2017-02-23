//io.cpp - определение класса работы с файлами
#include "io.h"

void FILE_ROUTINE::SetAppData()
{
   if(!(*LastFile)) SelectNewFileName();
   ofn.hwndOwner=hWnd;
   ofn.hInstance=hinst;
}

byte *FILE_ROUTINE::SetData  (byte *data, int datalen)
{
   creates = true;
   DELETE(FileData);
   FileData = new byte[datalen];
   FileDataLen = datalen;
   CopyMemory((LPVOID)FileData,(LPVOID)data,datalen);
   return FileData;
}

void FILE_ROUTINE::SelectNewFileName (char *name)
{
   strcpy(LastFile, name);
   GetFileTitle(LastFile,FileTitle,FILETITLE);
   SetCaptionWindow();
	DELETE(FileData);	// удаляем буфер данных файла
   FileDataLen = 0;
}

bool FILE_ROUTINE::GetSaveFileName (bool open)
{
   int i;
   FILE_TYPES ft=GetExtension();;
   static char DefExt[]={"pp"};
   static char DefExtColor[]={"ppc"};
   static char FiltersOpen[]={"\
Все поддерживаемые файлы\0*.pp*;*.jcr;*.bmp;*.ico;*.jcc\0\
Японские кроссворды (PPIFF)\0*.pp*\0\
Японские кроссворды (JCR)\0*.jcr\0\
Японские кроссворды (JCC)\0*.jcc\0\
Файлы точечных рисунков (BMP)\0*.bmp\0\
Файлы значков (ICO)\0*.ico\0Все файлы (*.*)\0*.*\0\0"};
   static char FiltersSave[]={"\
Японский кроссворд (PPIFF)\0*.pp\0\
Японский кроссворд (JCR)\0*.jcr\0\
Японский кроссворд (JCC)\0*.jcc\0\
файл отложенной игры (PPIFF)\0*.pps\0\
Точечных рисунок (*.BMP)\0*.bmp\0\0"};
   static char FiltersSaveColor[]={"\
Японский кроссворд (PPIFF)\0*.ppc\0\
Японский кроссворд (JCR)\0*.jcr\0\
Японский кроссворд (JCC)\0*.jcc\0\
файл отложенной игры (PPIFF)\0*.pps\0\
Точечных рисунок (*.BMP)\0*.bmp\0\0"};
   static char CaptionS[]={"Сохранить кроссворд как..."};
   static char CaptionO[]={"Открытие файлов с кроссвордами."};
	if(!open){
   	strcpy(LastFile,FileTitle);
      for(i=0; i<FILEPATHNAME; i++) if(LastFile[i] == '.') break;
      LastFile[i]=0;
      ofn.lpstrFile=LastFile;
      }
   ofn.lpstrTitle=open ? CaptionO : CaptionS;
   ofn.lpstrFileTitle=FileTitle;
   ofn.lpstrFilter=open ? FiltersOpen : (GLOBAL::Colors>1 ? FiltersSaveColor : FiltersSave);
   if(open) ofn.nFilterIndex = 1;
#ifdef DEMOVER
   else ofn.nFilterIndex = 5;
#else
   else if(ft == PPIFF1_FILE || ft == PPIFF2_FILE)
   												 	ofn.nFilterIndex = 1;
   else if(ft == JCR_FILE) 	ofn.nFilterIndex = 2;
   else if(ft == JCC_FILE) 	ofn.nFilterIndex = 3;
   else if(ft == PPIFFS_FILE) ofn.nFilterIndex = 4;
   else if(ft == BMP_FILE) 	ofn.nFilterIndex = 5;
   else if(ft == ICO_FILE)		ofn.nFilterIndex = 3; //ico предлагаем сохранить в jcc-формате
#endif
   ofn.nFileExtension=NULL;
   ofn.lStructSize=sizeof(ofn);
   ofn.lpstrCustomFilter=NULL;
   ofn.nMaxCustFilter=NULL;
   ofn.nMaxFile=FILEPATHNAME;
   ofn.nMaxFileTitle=FILETITLE;
   ofn.lpstrInitialDir=DirBuffer;
   ofn.nFileOffset=NULL;
   ofn.lpstrDefExt=(GLOBAL::Colors>1 ? DefExtColor : DefExt);
   ofn.Flags=OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   if(!open) ofn.Flags |= OFN_OVERWRITEPROMPT;
   ofn.lCustData=NULL;
   ofn.lpfnHook=NULL;
   ofn.lpTemplateName=NULL;
	GetCurrentDirectory(FILEPATHNAME,(LPTSTR)ofn.lpstrInitialDir);
   if(open) return ::GetOpenFileName(&ofn);
   else if(::GetSaveFileName(&ofn)) {creates = false; return true;}
   strcpy(LastFile,FileTitle);
	return false;
}

bool FILE_ROUTINE::GetOpenFileName(char *name)
{
   ofn.lpstrFile = name;
   strcpy(name, FileTitle);

   bool res = GetSaveFileName(true);
   ofn.lpstrFile = LastFile;
   return res;
}

bool FILE_ROUTINE::LoadesFromLybrary(int numberfile)
{
   UNREFERENCED_PARAMETER(numberfile);
	return true;
}

bool FILE_ROUTINE::Read_from_File(bool Quest)
{  // Перенос данных из файла в буфер в памяти
   static char temp[23+FILEPATHNAME]={"Не могу открыть файл: "};
	static char Error[]     ={"Ошибка файла!"};
	static char Faulure[]   ={"Ошибка при чтении файла."};
	DWORD ReadLen;
   BOOL bResult;
   HANDLE CurrentFile;

// диалоговое меню выбора файла
   if(Quest)
      if(!GetOpenFileName(ofn.lpstrFile)) return false;
   CurrentFile=CreateFile(LastFile,GENERIC_READ,FILE_SHARE_WRITE,
                                         NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(CurrentFile==INVALID_HANDLE_VALUE){
   	strcpy(temp+22, LastFile);                        // Preparing message
   	MessageBox(NULL,temp,"Файла не существует.",MB_OK | MB_ICONSTOP |
                    MB_SYSTEMMODAL);
      FileDataLen = 0;
   	return false;
      }
// Читаем длину файла
   FileDataLen=(int)GetFileSize(CurrentFile, &ReadLen);
// Ok opening
   if(!FileDataLen)
   	{
      CloseHandle(CurrentFile);
		MessageBox(NULL,"Файл имеет нулевую длину.","Ошибка открытия файла.",MB_OK | MB_ICONSTOP |
                    MB_SYSTEMMODAL);
      return false;
      }

	ChangeDataLen(FileDataLen, true);	// выделяем буфер под данные
	SetFilePointer(CurrentFile,(LONG)0,NULL,FILE_BEGIN);//Обнуляем файловый указатель
	bResult=ReadFile(CurrentFile,(void *)FileData, (DWORD)FileDataLen,&ReadLen, NULL);
	if(!bResult)
		{
		MessageBox(hWnd, Faulure, Error,MB_OK | MB_ICONSTOP);
		CloseHandle(CurrentFile);
      FileDataLen = 0;
		delete[] FileData;
      FileData = NULL;
		return false;
		}
   CloseHandle(CurrentFile);
   SetCaptionWindow();
   EnableItem(IDM_SAVE, false);
   EnableItem(IDM_SAVE_AS, true);
   EnableItem(IDM_FEATURES, true);
   creates = false;
   return true;
}

bool FILE_ROUTINE::SaveDataToFile(void)
{
   char ErrMes[] ={"Не могу записать в файл: "};
   char Message[100];
   DWORD numb_write;
   HANDLE CurrentFile;
   if(creates) if(!GetSaveFileName(false)) return false;
   CurrentFile=CreateFile(LastFile, GENERIC_WRITE | GENERIC_READ,FILE_SHARE_WRITE,
                                         NULL, OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
   if(CurrentFile==INVALID_HANDLE_VALUE) return false;

   SetFilePointer(CurrentFile,NULL,NULL,FILE_BEGIN);// Set file pointer
   if(!WriteFile(CurrentFile, FileData, (DWORD)FileDataLen, &numb_write, NULL) ||
   					numb_write!=(DWORD)FileDataLen)
     {
     strcpy(Message,ErrMes);
     strcpy(Message+strlen(Message), LastFile);
     MessageBox(hWnd, Message, "Ошибка записи в файл",MB_OK | MB_ICONSTOP);
     return false;
     }
   SetEndOfFile(CurrentFile);
   CloseHandle(CurrentFile);
	SetCaptionWindow();
   creates = false;
   return true;
}

FILE_TYPES FILE_ROUTINE::GetExtension(void)
{
   char test[4], Ext[4][4]={{"BMP"},{"JCR"},{"ICO"},{"JCC"}};
   int Count=strlen(LastFile);//FILEPATHNAME;
   char *temp=LastFile+Count-1;
   while((Count--) && (*(temp)!='.')) temp--;
   temp++;
   for(int i=0; i<3; i++) test[i]=(char)(temp[i]&0xDF);
   test[3]=0;
   if(!strcmp(Ext[0], test)) return BMP_FILE;
   if(!strcmp(Ext[1], test)) return JCR_FILE;
   if(!strcmp(Ext[2], test)) return ICO_FILE;
   if(!strcmp(Ext[3], test)) return JCC_FILE;
   if(test[0]=='P' && test[1]=='P')
   	if(test[2]=='C') return PPIFF2_FILE;
      else 	if(test[2]=='S') return PPIFFS_FILE;
      else 	if(!temp[2]) return PPIFF1_FILE;
   return UNKNOW_FILE;
}

void FILE_ROUTINE::SetCaptionWindow(void)
{
   //Формируем заголовок главного окна
   char Title[FILETITLE+sizeof(CAPTION_TEXT)+4];
   strcpy(Title, CAPTION_TEXT);
   strcpy(Title+strlen(Title)," - ");
   strcpy(Title+strlen(Title), FileTitle);
   SetWindowText(hWnd, Title);
}

