// Kdata.h - header files from data routines
#ifndef _KDATA_
#define _KDATA_
#include <stdlib.h>
#include "calc.h"

class KROSSWORD : public CALCULATE{
   int InputDigitRange;			// Флаг разрядности вводимого числа
   bool UndoFlag, RedoFlag;  	// Флаги наличия отмен / повторов
   int CurUndo,           	  	// Указатель на текущюю отмену
   EndUndo,				  			// Указатель на последнюю отмену
   EndRedo;							// Указатель на последнее REDO
   UNDOTAG *Undo;            	//Структура отмены

   void RestoreCaption(void);	//Восстановление заголовка главного окна при отмене изменений
   void KillUndo      (void);	//Удаление отмены

   void ZeroData   (void);
   void FillData  (void);
   bool FilledIcoFile   (byte *pointer);
//   void FilledJpnFile   (byte *pointer);
	bool FilledJcrFile	(byte *pointer);
	int  FilledJccFile (byte *pointer);
   bool FilledBmpFile   (byte *pointer, bool killundo=true);
   bool Filled_PPIFF1_File (byte *pointer);

	bool Save       (bool Quest=true, bool SaveAs=false);
	bool SaveBmpFiles(bool saveas);
//   bool SaveJpnFile(void);
	bool SaveJcrFile(void);
   bool SavePPIFFFile(void);
   bool SaveJccFile(void);

   void PasteBitmap(byte *FileData);

   void PrepareUndo(void);
   void ChangesInFile(bool digit);
	void ChangesInFileA();
   void ChangeInTable(bool Press=false, bool update = false);

   int CompleteCounter;	// Локальный счетчик найд. ячеек для параллельной работы
   int CurrLine;        // переменная для ограничения обн. стр. статуса при пар. раб.
//   Обработка сообщеший клавиатурного ввода
   bool keyIn;                       //Флаг обработки клавиатурного ввода
   int  CurrPoz;                     //Позиция курсора
   int  LastKey;                     //Последняя нажатая клавиша
   void DigKeyIn(int Dig);           //Ввод цифр
   void MoveCarret(int Command);     //Перемещение курсора
   bool CheckSumm (void);
   int *GetCurrPoz(void) {return &CurrPoz;}
   bool KeyIn     (void) {return keyIn;}
   void DefPaint (void);
#ifdef _DEBUG_
   void DebugMessage (int First, int Slave);
#endif
	void CreateKrossword();
   void DeleteUndoTag(UNDOTAG *Undo);
   void MoveCurrentData(UNDOTAG *Undo);
   void CopyCurrentData(UNDOTAG *Undo);
   void RestoreData(UNDOTAG *Undo);

	void LookCriticalMenuItems(bool enable);

public:

   STATUS     Status;        	// Data status
   int        MapLen;        	// Size BitMap
   int hDigitSize,vDigitSize;	// Размер массивов оцифровки
   KROSSWORD (HINSTANCE hInstance, LPSTR lpCmLn, int CmdShow);
   ~KROSSWORD ()                     //Destructor
     {
     StopThread();
     DelData();
     }
   void ClearMask            (void){ //Обнуление таблицы масок
      PrepareUndo();
      memset(hData->DigitMarkers, 0, hSize*hNumbSize*sizeof(bool));
      memset(vData->DigitMarkers, 0, vSize*vNumbSize*sizeof(bool));
      ViewTable();
      }
   void CommandProcessing(UINT Msg, WPARAM WParam, LPARAM LParam);
	void Print(void);
   void ClearMark(void);
   void KeyIn(int vKeyCode, bool Ext);         //Обработка нажатий на клавиатуру
   STATUS GetStatus (){ return Status;}
   void ScrollingData(WORD Command);
   void Characteristics (void);					// Вывод характеристик кроссворда
   void Congratulation(void);						// Поздравления при полном решении кроссворда
   void DelData    (bool delundo = true);    //Function memory release from data
	void SetParam   (void);
   void Undo_      (bool undos);
   bool Corrected  (bool prepun = true);
	void Calc       (void);
	void MakeOverlapping(void);
	void EndCalc  (bool res = true);
	void Timer 		 (void);
   void DelDig     (void);
   void ClearField(void);
	byte *InitializeMatrix(int); //Create and zero memory
	char *InitializeMatrixChar(int); //Create and zero memory
	int  *InitializeMatrixInt(int); //Create and zero memory
   };
#endif
