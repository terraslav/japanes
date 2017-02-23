#ifndef _DEFINES_
#define _DEFINES_
#define STRICT
#include <windows.h>
#include <shlobj.h>
#include <string.h>

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

//#define DEMOVER // Для домо версии программы

//#define WM_USER_OPEN WM_USER+1
#define CLASS_NAME "Matrix"
#define EXEFILENAME "Matrix.exe"
#define CAPTION_TEXT "Lomaster Matrix"
#define NEW_FILE_NAME "nameless.pp"
#define FEATURES_KEY "SOFTWARE\\Lomaster\\Matrix\\Features"

#define WINDOW_SIZE_X 640
#define WINDOW_SIZE_Y 480

#define DEMOWAIT 300
#define WAITUPDATE 500   // ожидание до сброса оцифровки
#define WAITUPDATETIME 5 // обновление часов в статусе
#define SPEEDCLICKS 50	 // скорость автоизменения оцифровки
#define DELTASPEED 3     // порог наращивания скорости автоизменения оцифровки
#define MAXSPEED 20      // минимальное значение равное максимальной скорости

#define MAX_REMARK_LEN 2048 // Длина буфера комментария

#define SB_STATUSTEXT  0
#define SB_SIZE        1
#define SB_CURSOR      2
#define SB_CURSOR_BACK 3
#define SB_TIME        4

#define NUMBBUTTON  21

// константы
const FILEPATHNAME = MAX_PATH;
const FILETITLE    = MAX_PATH;
const NUMBUNDO 	 = 40; // максимальное количество отмен


const MAXSIZE 				= 254;   // Максимальная длина и ширина
const MAXCOLOR 			= 18;   	// Максимальное количество цветов
const BMP_BIT_PER_PIXEL = 8; 		// Количество бит на пиксел в BMP файлах
//const JCRCLRTBLEN       = 3*15;	// Колич. байт в цветовой табл. JCR файла

// стандартные цвета
#define VOIDCOLOR  RGB_GRAY			// пустой цвет
#define BACKCOLOR  RGB_WHITE			// цвет фона
#define PENSWIDTH  2						// Толщина кистей зачеркивания чисел

const ZEROS = 0;
const BLACK = 1;
const BACK  = ZEROS;

const FIRST	= BLACK;
const MARKER = 0x3f;
const LIGHTGRAY = MAXCOLOR+FIRST;
const GRAY      = MAXCOLOR+FIRST+1;
const EXTCOLOR  = 2;	// количество служебных цветов

// определение основных цветов формата PPIFF2
const RGB_WHITE	  = RGB(0xff,0xff,0xff);
const RGB_BLACK     = RGB(   0,   0,   0);
const RGB_BLUE      = RGB(   0,   0,0xff);
const RGB_RED       = RGB(0xff,   0,   0);
const RGB_MAGENTA   = RGB(0xff,   0,0xff);
const RGB_GREEN     = RGB(   0,0xb5,   0);
const RGB_CYAN      = RGB(   0,0xff,0xff);
const RGB_YELLOW    = RGB(0xff,0xff,   0);
const RGB_SILVER    = RGB(0xc0,0xc0,0xc0);
const RGB_GRAY      = RGB(0x80,0x80,0x80);
const RGB_MAROON    = RGB(0xb5,   0,   0);
const RGB_NAVY      = RGB(   0,   0,0xb5);
const RGB_PURPLE    = RGB(0xb5,   0,0xb5);
const RGB_LIME      = RGB(   0,0xff,   0);
const RGB_TEAL      = RGB(   0,0xb5,0xb5);
const RGB_OLIVE     = RGB(0x80,0x80,   0);
const RGB_AQUAMARINE= RGB(0x7f,0xff,0xd4);
const RGB_ORANGE    = RGB(0xff,0xa5,   0);
const RGB_PINK      = RGB(0xff,0xc0,0xcb);

const SHOWMASK  = 0;
const HIDEMASK  = BLACK;

// Макросы
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#ifdef DELETE
#undef DELETE
#endif
#define DELETE(a) if(a)delete[](a);(a)=NULL
#define MOVEDATA(a,b) if(a)delete[](a);(a)=(b);(b)=NULL
#define errhandler(a,b) MessageBox(b,a,"Ошибка!",MB_OK|MB_ICONERROR)
#define RGBConvert(a) (COLORREF)((DWORD)((a).rgbRed)|(((DWORD)(a).rgbGreen)<<8)|(((DWORD)(a).rgbBlue)<<16))
#define STATUS(a) if(WINMAIN::StatusBox)StatusBox->SetText(SB_STATUSTEXT,a)
#define GetMidleColorValue(a) ((52*GetRValue(a)+196*GetGValue(a)+8*GetBValue(a))/256)

typedef enum FILE_TYPES_TAG {JCR_FILE, BMP_FILE, ICO_FILE, PPIFF1_FILE, PPIFF2_FILE, PPIFFS_FILE, JCC_FILE, UNKNOW_FILE} FILE_TYPES;
typedef enum CONTAINERStag {EMPTY,MONOCHROME,COLORS} CONTAINERS;
typedef enum STATUStag {NOCHANGE,CHANGES} STATUS;
typedef enum CTOOLtag {CVOID, CPEN, CRECT, CEYE, CFILLED, CDIGITAL} CTOOL;

typedef struct Char_In_Matrix_tag {	// структура для PPIFF расширения А
   byte Column;                     // хранит букву выводимую в матрице
   byte Line;
   char Letter;
   Char_In_Matrix_tag *Next;
   Char_In_Matrix_tag(byte a, byte b, char c) {Column=a; Line=b; Letter=c; Next = NULL;}
   Char_In_Matrix_tag() {Column=Line=0xff; Letter=0; Next = NULL;}
   ~Char_In_Matrix_tag() {delete Next;}
   void Serialize(char *ptr); // члены описаны в файле reg.cpp
   void AddLetter(byte a,byte b,char c);
   Char_In_Matrix_tag *findLetter(char c, byte a=0xff,byte b=0xff);
   void UpdateMatrix(byte *Matrix, int hsize, int vsize);
   int GetLen(int a=0){if(Next) return Next->GetLen(++a); else return ++a;}
   void GetData(char *ptr, int *Cptr, int *Lptr){
		*ptr++=Letter; *Cptr++=Column; *Lptr++=Line;
      if(Next) Next->GetData(ptr, Cptr, Lptr);}
} Char_In_Matrix;

typedef struct COLORDATAtag{ // Структура для обмена парам. цвета между классами PPIFF и PALETTE
	int NumberColor;	// номер цвета
   COLORREF Color;	// RGB цвета
   } COLORDATA;

#endif //_DEFINES_
