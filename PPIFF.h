#include "defines.h"
#include "kdata.h"

#define ERROR_LEN 500
#define DIGIT_3	0x2c
#define ZERO_LINE 0x2d
#define DIGIT_2	0x2e
#define ZERO_MARKER 0xff // Маркер символа '-' иначе ошибка с числом 45
#define ERR_INDEX_COLOR 0xff //код ошибочного индекса цветовой таблицы
#define MAX_NUMBER_IN_LINE 100
class PPIFF {
	KROSSWORD 	*KrossData;
   int			RemarkCount;	// Счетчик указатель на конец строки комментария
   char			*ErrorMessage; // Указатель на буфер сообщения об ошибке
   byte			*Column;			// Столбцы
   byte			*Line;			// Строки
   byte			*ColumnColor, *LineColor;// цвета оцифровки
   int ColumnCount, LineCount;// Счетчики столбцов и строк
   int ColumnSize, LineSize;	// Размер верт и гориз оцифровки
//	char DString[5];				// Буфер стринга цифры
	char *DigitToString(char *,byte);	// Конверт. цифры в стринг с префиксом (.,)
	char *DigitToStringA(char *,byte);	// Конверт. цифры в стринг без префиксов
	char *RGBtoString(char *ptr, COLORDATA *cd);
   void PrepareSaveData();		// Подготовка данных для сохранения
	int  GetNextCharAddres(int i);//процедура пропуска незначящих символов (\r\n пробел)
   char *IndexColorTable;		// таблица трансляции цветов
   void InitializeIndexColorTable(); // Иниц. таблица трансляции
   void SetupExtData(char *);			// Применение расширений формата
	char *GetItem(char *item, char *ptr);
   int  LoadMatrix(int);		// Заполнение матрицы рисунка
   int  hsize, vsize;		  	// размеры кроссворда
   int  Stroka;					// счетчик строк(\cr \lf)

public:
   byte 			*FilePtr;		// Указатель на буфер файла
   int			FileLen;			// Длина файла
   int			PPFileLen;		// Длина файла без матрицы
//   char			*Remark;			// Комментарий
   bool GetDataFromText ();	// перевод текстового файла в цифры
   bool faithfully;				// флаг верности формата файла
	PPIFF (KROSSWORD *, byte *, int); //конструктор
   ~PPIFF(); 						//деструктор
};
