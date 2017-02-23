// paint.h - header files from painting
#ifndef _PAINT_
#define _PAINT_
#include "winmain.h"

class PAINTING : public WINMAIN{                //Класс прорисовки экрана и ввода данных
   HWND vHScroll,hHScroll; //Хэдеры окон (главного и полос прокрутки)
   HBITMAP hIconBmp,hTrancBmp;  //Bitmap миниатюры
   HDC hMemDC,hTrancDC,hPaintDC;//Контексты устройсв памяти
   HFONT NormalFont, BigFont;   //Хэдеры шрифтов
   DWORD *Colors;               //Указатель на массив цветов
   int NumbColors;              //Количество цветов
   HPEN SolidPen, BoldPen[FIRST+MAXCOLOR+EXTCOLOR], DotPen, CrossPen;//Хэдеры ручек
   int LastDigit;               //Последняя обрабатываемая цифра (для Iconic)

   POINT VPort,                 //Стартовая точка вывода на экран
                WorkFild;       //  и прямоугольник рабочего поля

   bool InitScreen;             //Флаг, показывающий, что параметры вывода
                                //на экран установлены.
   int ClipX,ClipY;             //Показатели усечения области клиента
                                //  полосами прокрутки с строкой статуса
   bool LightingErase;          //Флаг-семафор стирания старой позиции подсветки
   POINT OldLightingPos;        //Устаревшая позиция подсветки

   bool ButtonDown, ButtonUp;   //Индикаторы нажатия на кнопки мыши
   int WaitUpdateTime;          //Счетчик обновления времени
   int WaitUpdate;              //Счетчик таймера обновления оциф. при отп.
                                //кнопки мыши

   SCROLLINFO si;               //Переменные скроллинга
   int hSbPoz, vSbPoz,          //Позиции полос прокрутки
       hSbLim, vSbLim;          //Лимит скроллинга
   int SBV,SBH;                 //Высота и ширина полос прокрутки
   bool hScrollFlag,vScrollFlag;//флаги видимости полос прокрутки

   int RegCount,RegCompleteCount,//Переменные функции заливки цв. полей
   	 *RegAddr;		  			  //
   byte PatternColor;           //
   bool RegFilled;				  //

	void SetIconFeat(void);		  //Установка объектов иконки	
   void Calculate    (void);    //Член-Функция расчета экранных переменных
   void MakeLogFont  (int Size);//Член-Функция заполнения структуры LOGFONT
   HFONT CreateFonts (int Size);//Член-Функция создания большого шрифта
   void PaintControl(int x, int y);
   void MakeElement  (int x, int y, byte Color);   //Выделение прямоуг.
   void FillFieldRect(int x, int y, byte Color);   //Заливка полей
	void FilledColorRegions(int x, int y, byte fillcolor);//Заливка цветовых регионов
   void CheckCross(int addr);                      //Подпр. заливки полей
   void hMakeNumb    (int x, int y, byte ColFl);   //Ввод
   void vMakeNumb    (int x, int y, byte ColFl);   //     оцифровки
   void LightPos     (POINT c, bool Lighting = false);// Подсветка позиции
   void DelObject    (bool All=true);//Удаление всех графических объектов
   void SetScroll    (void);    //Установка параметров полос проокрутки

protected: //Наследуемые элементы
   HBRUSH *Brushes;             //Указатель на массив кистей
   int RectSize, hExtSize, hTotalSize, vExtSize, vTotalSize;
   byte *SaveMaps;				  //Массив временного сохранения матрицы для функц. рисования
   int Multyple;                // Переменная показатель увеличения
   bool ExecPaint;				  //Флаг обработки WM_PAINT
   bool group;						// Флажок нажатия SHIFT для мыши
   WPARAM MOldButtonStatus;	// статус кнопок мыши для 'области'
   int LastCell;					  //Адрес последней изменяемой ячейки

//Виртуальные функции
   virtual void ChangesInFile(bool)=0;
   virtual void ChangeInTable(bool,bool)=0;
   virtual void PrepareUndo  (void)=0;
   virtual int *GetCurrPoz   (void)=0;
   virtual bool KeyIn        (void)=0;
   virtual bool CheckSumm (void)	= 0;
   virtual void Congratulation(void) = 0;
   virtual bool TestFull(void) = 0;

   void ScreenInitialize     (void);//KROSSFEATURES *TF);
   void Paint        		  (HDC hDC); //Перерисовка экрана
   void ViewTable            (HDC hDC=NULL);
   void GetTime              (char *buf);
   HDC PaintIconic  (HDC hDC); //Рисует иконку
   void ViewDigit    (int x, int y, bool Whu,
                 HDC hdc=NULL, bool Lights=false); //Вывод оцифровки
   void PaintCell (int *cells, int number); // перерисовка ячеек при параллельной работе
	void DrawCell (HDC hDC, int x, int y);
	POINT *GetMapViewPort(){return &WorkFild;}// функция для класса PALETTE
   void WindowSizing();
   void SetCurrentInput (CTOOL control);

public:
   RECT Client, Border, hDig, vDig, Fild, Icon;

	void ChooseFonts(void);
//   void InputPaint           (bool update = true);
//   void InputDigit           (bool update = true);
   void MakeRect     (int x, int y, bool updateicons=false, HDC hdc=NULL); //Прорисовка прямоугольника
   void RButtonDown  (int x, int y, WORD flag);
   void LButtonDown  (int x, int y, WORD flag);
   void MButtonDown  (int x, int y, WORD flag);
   void MouseMove    (int x, int y, WORD flag);
   void Timer        (void);
   void UpButton     (int x, int y);
   void DownButton   (void);
   void Maximize     (void);
   void Minimize     (void);
   void Normalize    (void);
   LRESULT PaintProc (void);
   void hScroll(int sCode, short int nPos, bool wheel=false);
   void vScroll(int sCode, short int nPos, bool wheel=false);
   void ScrollRect   (RECT *Source, int x, int y);
   void CreateScrollBar(void);

   PAINTING (HINSTANCE hInstance, LPSTR lpCmLn, int CmdShow)
   : WINMAIN (hInstance, lpCmLn, CmdShow)
     {               //Constructor
     ExecPaint = true;
     ButtonUp=ButtonDown=false;
     WaitUpdate=WAITUPDATE;
     InitScreen=false;
//     StatusVisible=false;
     OldLightingPos.x=MAXSIZE; OldLightingPos.y=MAXSIZE;
     LightingErase=false;
     WaitUpdateTime = WAITUPDATETIME;
     hIconBmp = NULL;
     hMemDC = NULL;
     LastDigit = 0;
     LastCell = -1;
     SaveMaps = NULL;
     }
   ~PAINTING();
   };
#endif
