#ifndef _CALC_
#define _CALC_

#include "features.h"
#include "paint.h"

typedef enum REStag{ALL_OK,CALCERROR,OVERFLOW,USERSTOPED,NOLOGIK} calcres;
typedef unsigned __int64 UINT64;
const Overflow=0x7fffffff;

struct VAR_SAVE{	// структура хранения вариантов подстановки с результатом NOLOGIC
	UINT64 NoLogVar; // служит для пропуска этих вариантов если они приводят к CALKERROR
	int line;
	bool that;
   VAR_SAVE *next;
   VAR_SAVE(UINT64 a, int b, bool c){next=NULL;NoLogVar=a;line=b;that=c;};
   ~VAR_SAVE() 							{if(next) delete next;}
   };

class CStopwatch {

public:
    CStopwatch() { QueryPerformanceFrequency(&m_liPerfFreq), Start(); }
    void Start() { QueryPerformanceCounter(&m_liPerfStart); }
    UINT64 Now() const
    { // возвращает число миллисекунд после вызова Start
        LARGE_INTEGER liPerfNow;
        QueryPerformanceCounter(&liPerfNow);
        return(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) /
               m_liPerfFreq.QuadPart);
    }
private:
    LARGE_INTEGER m_liPerfFreq;
    // количество отсчетов в секунду
    LARGE_INTEGER m_liPerfStart;
    // начальный отсчет
};

typedef struct CALCDATATAG
{
	bool Runing;		// Идикатор работы нити true - нить выполняется
	UINT  Count;			// Счетчик обработанных вариантов
   UINT TotalCicle;	// Количество прогонов программы анализа
   int  CurrLine;		// Строка или столбец обрабат. на данный момент
   UINT64 CurrLineVar;// Приоритет строки или столбца обрабат. на данный момент
	bool That;			// Переменная-флаг, что в данн. момент обрабат. строка или столбец
   int  Total;			// Количество цифр оцифровки
   bool Target;      // Переменные для параллельной обработки
   int *Complete;		// Буфер хранящий индексы отгаданых ячеек.
   int CompleteCounter; // Счетчик буфера индексов (указывает на первую
} CALCDATA;

class CALCULATE : public PAINTING
{
	bool Semaphore;	// Семафор прерывания нити
   bool Stopped;		// Флаг признака принудительного завершения
   bool Runing;      // Идикатор работы нити true - нить выполняется
	HANDLE hThread;	// Хэдер нити
   char *ExitMessage;// Указатель на строку рапорта при принудительном выходе из нити
   bool Cleared;		// Хранение факта чистый или нет рисунок при старте.

   bool Target;         // Переменные для параллельной обработки
   int *Complete;			// Буфер хранящий индексы отгаданых ячеек.
   int CompleteCounter; // Счетчик буфера индексов (указывает на первую
								// свободную ячейку в буфере
	int *Variants;	// Буфер счетчиков элементов.
	int Variant;	// Счетчик верных вариантов.

	int BCount;		// Количество блок-элементов в строке.
	byte *Original;// Буфер строки оригинала.
	byte *Buffer;	// Буфер строки варианта.
	int *Block;		// Буфер оцифровки строки.
	int *Pos;		// Текущии позиции элементов-блоков.

	bool That;	// Переменная-флаг, что в данн. момент обрабат. строка или столбец
	int Number;	// Переменная содержащая номер обрабат. строки или столбца
   int infinity; // Переменная содержащая номер обрабат. строки или столбца при нелог. кроссв.

	bool Res,	// Булева переменная результата для головной программы.
   calcerror;  // Фгаг ошибки
   int  Count;	// Счетчик обработанных строк и столбцов
   int  Total;	// Количество цифр оцифровки
   int  TotalCicle;	// Количество прогонов программы анализа

							// Переменные времени (для протокола)
   DWORD Tic;
   CStopwatch *Stopwatch;
	int LineLen;	// Размер текущей строки или столбца.

	void Execute       ();	// Выполнение расчета
	calcres CalcLogikMain ();     // Новая управляющая программа решения кроссворда.

	void Protocol  (int);	// Вывод отчета о проделанной работе
   bool CheckComplete();	// Функция проверки матрицы на полноту

   bool TestFull();			// Управляющая процедура проверки на окончание решения и отметка марктровки
   								// Проверка строки и отметка маркировки при точном определении располож. элемента
	bool TestLine_MakeMarkers(char *line, int line_len, byte *digits, int numb_digits, bool *markers);
	bool TestLine_MakeMarkers_color(char *line, int line_len, byte *digits, int numb_digits, byte *colors, bool *markers);

	bool NewScanLine(bool);
   bool MakeStartPosition(void);	// определение стартовы-стоповых позиций
   bool MakeOverlapping(bool that);	// безбуферная отметка перкрытых ячеек
   bool MakeVoid(bool that);	// отметка пустых клеток

   static void StartRoutine  	(void);
   bool CreateVariant			(UINT64 number, int line, bool that); // процедура создания варианта по его номеру
   UINT64 CalcNumbVariants		(int module, int range);
	UINT64 SetPriority(bool = false);	// Функция установки приоритетов
   UINT64 *hNumberVariants, *vNumberVariants,// Массивы количества возможн вариантов
          VarTotal;								  			// Хранит глобальное количество возможных вариантов

	calcres CalcColorLogikMain(); // Управляющая программа для цветного кроссворда.
	calcres MakeAccessColor (void);// чл-ф определения доступных цветов ячейки
   bool    BeginAccessColor(void);
	calcres RecalcPlace     (void);
	bool 	  CheckColorCell(int adr, byte color);
   bool    CheckCell (int adr, int clrnumb);
   bool 	  ScanColorLine(bool that);
	calcres FindDecisionColor(void *sts = NULL);   // Решение нелогических цветных матриц
	void 	  SearchSequenceColor(void *sts);
   bool 	  CheckCompleteColor();

//.........................................................................
protected:
   VAR_SAVE *vs;
//	bool FindErrorVariants(UINT64, int, bool); // поиск вариантов NOLOGIC приведших к ошибке
// bool CheckFull(void);		// проверка строку на заполнение

   int Result;	// Хранит результат предыдущего анализа
   char *ltoa(UINT64 value, char *string, int radix);
   void MakeOverlappingCells();
//	void MakeDigitizationMask(int addr);
////////////////////////////////////////////////////////////////////////////////
	int DeepRecurce, Recurce;
	calcres FindDecision(void *sts = NULL);
	void 	  SearchSequence(void *);

////////////////////////////////////////////////////////////////////////////////

public:
// члены анализа цветного кроссворда ......................................
	byte *CBlock;	// Буфер цвета оцифровки.
   byte *AccessColor;// Буфер доступных ячейкам цветов
   int  CCount;// Количество цветов кроссворда

   bool NoClassics;  // Флаг указатель, что кросс. неклассический и запущ. проц. поиска вар. реш.
   int MapLen;
   int *StPos;		// буфера для хранения стартовых и конечных позиций первых
   int *EnPos; 	// правильных вариантов это сокращает циклы обработки строки
   bool *Hstop;			// Массивы блокировки обработки строк и
   bool *Vstop;			//                                      столбцов
   byte *Maps;       //	Указатель на текущую матрицу рисунка
   int  StEndPosLen; // длина буферов стартовых и стоповых позиций

	void ResetData();
	void Run (bool target = false);	// Инициализация потока анализа
	void StopThread(void);	// Принудительное прерывание потока
	void GetData (CALCDATA *c);	// Функция получения информ. о ходе обработки

	CALCULATE(HINSTANCE hInstance, char *lpCmLn, int CmdShow)	// Конструктор
   : PAINTING (hInstance, lpCmLn, CmdShow)

	{

	vs=NULL;
	ThisPointer = this;
	Semaphore = true;	// блокир. процедуру принуд. остановки
   Runing = false;	// При старте нить не занята
   ExitMessage = NULL;
   Result = ALL_OK;

	Complete = NULL;
   Hstop = Vstop = NULL;
   Original = Buffer = NULL;
   Block = Pos = Variants = StPos = EnPos = NULL;
   AccessColor = CBlock = NULL;
   Number = NULL;
   hNumberVariants = vNumberVariants = NULL;
	}

   ~CALCULATE()						// Деструктор
   {
   ResetData();
   if(vs) delete vs;
   delete[] ExitMessage;
   }

   static CALCULATE *ThisPointer;
};

struct SAVE_CALC_STATE {
	byte *SaveMaps, *SaveAccessColor, *orig;

   int *Sequence, *StPos, *EnPos;
   int LinePtr, Line, RecurceSkip, Elem, Len, pos, ocount, ColCount, ErrColCount;
   int *BCount;
   bool *SaveHstop, *SaveVstop;
   CALCULATE* calc;
   void RestoreData(){
      memcpy(calc->Maps,  SaveMaps,  calc->MapLen);
      memcpy(calc->Hstop, SaveHstop, calc->hSize*sizeof(bool));
      memcpy(calc->Vstop, SaveVstop, calc->vSize*sizeof(bool));
      memcpy(calc->StPos, StPos, (calc->hNumbSize*calc->hSize+calc->vNumbSize*calc->vSize)*sizeof(int));
      memcpy(calc->EnPos, EnPos, (calc->hNumbSize*calc->hSize+calc->vNumbSize*calc->vSize)*sizeof(int));
      GAC();
   	}
   void SaveStop(){
      memcpy(StPos, calc->StPos, (calc->hNumbSize*calc->hSize+calc->vNumbSize*calc->vSize)*sizeof(int));
      memcpy(EnPos, calc->EnPos, (calc->hNumbSize*calc->hSize+calc->vNumbSize*calc->vSize)*sizeof(int));
   	}
   void SaveAttr(){
      memcpy(SaveHstop, calc->Hstop, calc->hSize*sizeof(bool));
      memcpy(SaveVstop, calc->Vstop, calc->vSize*sizeof(bool));
      SaveStop();
      SAC();
   	}
   void SAC(){
  		if(calc->Containers==COLORS)
			memcpy(SaveAccessColor, calc->AccessColor, calc->MapLen*calc->CCount);
      }
   void GAC(){
  		if(calc->Containers==COLORS)
			memcpy(calc->AccessColor, SaveAccessColor, calc->MapLen*calc->CCount);
      }
   void SaveMap(){memcpy(SaveMaps,  calc->Maps,  calc->MapLen);}
   SAVE_CALC_STATE(CALCULATE* c){
   	calc      =c;
   	SaveMaps  =new byte[calc->MapLen];
      SaveHstop =new bool[calc->hSize];
      SaveVstop =new bool[calc->vSize];
      Sequence  =new int [calc->hSize+calc->vSize];
      StPos     =new int [calc->hNumbSize*calc->hSize+calc->vNumbSize*calc->vSize];
      EnPos     =new int [calc->hNumbSize*calc->hSize+calc->vNumbSize*calc->vSize];
		BCount    =new int [calc->hSize+calc->vSize];
  		if(calc->Containers==COLORS){
         SaveAccessColor =new byte[calc->MapLen*calc->CCount];
	      SAC();
         }
      memcpy(SaveMaps,  calc->Maps,  calc->MapLen);
		SaveAttr();
      }
   ~SAVE_CALC_STATE(){
   	delete[] SaveMaps;
   	delete[] SaveHstop;
   	delete[] SaveVstop;
      delete[] Sequence;
      delete[] StPos;
      delete[] EnPos;
      delete[] BCount;
  		if(calc->Containers==COLORS)
      	delete[] SaveAccessColor;
      }
   };

#endif
