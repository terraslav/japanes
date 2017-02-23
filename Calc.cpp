/*

Проект: Программа "Японское рисование"
Задача: Расчет рисунка по оцифровке
        (Данный трактат и коды были написаны автором 14.04.01 в ночь на
        святую пасху /вместо того чтобы, как и весь часной народ славно
        пить водку, закусывая куличем/.)
Подзадача:	Расчет логически доступных ходов кроссворда.
Идея:	Перебор и сравнение всех возможных вариантов с оригиналом. Создается
счетчик всех непротиворечащих оригиналу вариантов. Также для каждого элемента
строки создается счетчик, увеличивающийся при непустом элементе варианта,
непротиворечащем оригиналу.
	Элемент строки оригинала может иметь значения неивестно, "однозначно
пусто" и непустой (однозначно закрашеный).
	Сравнение производится по принципу: вариант считается неверным если
элемент в оригинале помечен, как "однозначно пусто", а соответствующий элемент
варианта непустой. Вариант также считается неверным если элемент оригинала
непустой, а элемент варианта имеет значение "пусто".
	По окончании цикла перебора производится проверка всех счетчиков
элементов. При совпадении их со счетчиком правильных вариантов выполняется
установка соответствующих элементов в оригинальном варианте. При нулевом
значении этих счетчиков соответствующие элементы оригинала маркируются как
однозначно пустые.
	Подпрограмма при старте должна создавать булеву переменную, которая
устанавливается при каком-либо изменении в строке оригинала. Значение этой
переменной должно возращаться вызывающей программе, для определения конца
обработки логически доступных ходов кроссворда (воизбежание зацикливания).
	Вызывающая программа также должна содержать булеву переменную,
обнуляемую при старте, и при каждом вызове данной подпрограммы
последовательно для каждой строки и столбца кроссворда, присваивать этой
переменной значение равное логическому ИЛИ текущего значения переменной и
значения возвращаемого подпрограммой. По окончании цикла вызова подпрограммы
для всех строк и столбцов, программа должна анализировать данную переменную
и при ее нулевом значении возвращать управление (конец обработки). При
ненулевом значении этой переменной цикл повторяется.

P.S. 16.04.01
	После отладки и прогона вышенаписанного, пришел к выводу, что алгоритм
в силу своей примитивности (хотя я и не расчитывал, что "отгадываемые"
кроссворды он будет прогонять до точки) работает недостаточно эффективно.
Понаблюдав за его работой заметил, что он довольно надолго зовисает на
строках, которые содержат большое количество малозначимой (1,2,3...)
оцифровки. Т.е. при этом возникает огромное количество возможных вариантов
в то время, как результат их прогона близок к нулю. В итоге решил немного
модернизировать программу разделив строки на 3 приоритета. Изначально
алгоритм прогоняется на отказ обрабатывая только строки с самым высоким
приоритетом. Когда дело запахнет тупиком, подключаем строки с более
низким приоритетом и т.д. В итоге "трудные" строки начнут обрабатываться
уже имея в себе какую-то определенность, что значительно ускорит их
обработку. Приоритет надумал вычислять по формуле: (умножаем количество
цифр в оцифровке на количество незаполняемых клеток строки) - позднее переработал.
Вроде все, завтра расскажу, что из этого получилось...

18.04.01
	Ура результаты превзошли все ожидания!!! Вышеописанный прибамбас я
скомбинировал с ограничением перемещений элементов в соответствии с их
стартовыми и конечными позициями первого и последнего подходящего варианта.
В результате несколько замедлились процедуры проверки одного варианта но
значительно сократилось количество проверяемых вариантов и программа
стала работать в десятки раз быстрее своего первоначального варианта
(для примера кроссворд, на котором я ее проверял, вначале обрабатывался
более 20 минут, а теперь 43 сек.). Пока новых идей нет, если появятся
напишу.

18.06.01
	Все, что я изобрел до этого не стоит и выеденого яйца в сравнении с новым алгоритмом
разработанным и отлаженным мной накануне. Теперь кроссворд решается не проверкой
всех вариантов а глубоким анализом самой строки на основаниии оцифровки и содержимого,
так, что все придуманое ранее пора выкидывать в мусорку, обидно-то как!
В итоге программа решает любой однозначно решаемый кроссворд за доли секунды.
В дополнение к этому при отсутствии прямого логического решения запускается программа
подставляющая значения в строки с наименьшим количеством вариантов. После этого она
полностю выполняет анализ и при ошибке подставляет следующий вариант и т.д.
В итоге большинство 'неклассических' кроссвордов решаются довольно быстро, но
некоторые требуют уйму времени. Ну что-ж, буду пытаться что-либо сделать в этом плане.
Также мною были написаны процедуры подсчета количества возможных вариантов строки
и построения варианта по его номеру, математику уже конкретно подзабыл поэтому с
ними пришлось повозиться доходя до всего своими умом.
*/

#include <process.h>

#include "calc.h"

CALCULATE *CALCULATE::ThisPointer;

void CALCULATE::GetData (CALCDATA *c)	// Функция получения информ. о ходе обработки
{
   EnterCriticalSection(&sinc);
	c->Runing = Runing;
	c->Count = Count;
	c->TotalCicle = TotalCicle;
/*   if(NoClassics){
      c->That = (infinity < hSize ?  true : false);
      c->CurrLine = (c->That ? infinity : infinity-hSize);
	   if(Runing) {c->CurrLineVar = c->That ? hNumberVariants[infinity] : vNumberVariants[infinity-hSize];}
   	else c->CurrLineVar = NULL;
   	}
   else {
		c->CurrLine = Number;
		c->That = That;
	   if(Runing) c->CurrLineVar = c->That ? hNumberVariants[Number] : vNumberVariants[Number];
   	else c->CurrLineVar = NULL;
      }*/
	c->Total = Total;
   c->Target = Target;
   c->Complete = Complete;
   c->CompleteCounter = CompleteCounter;
   LeaveCriticalSection(&sinc);
}

void CALCULATE::ResetData()
{
	CompleteCounter = -1;
	DELETE(Original);	// удаляем буфера
	DELETE(Buffer);
	DELETE(Variants);
	DELETE(Block);
   DELETE(CBlock);
	DELETE(Pos);
	DELETE(StPos);
	DELETE(EnPos);
   DELETE(Complete);
   DELETE(Hstop);
   DELETE(Vstop);
	DELETE(hNumberVariants);
   DELETE(vNumberVariants);
}

void CALCULATE::Run ( bool target )	// Target - цель параллельно или
{                                   // монопольно обрабатывать рисунок
	if(Runing) return;	// проверяем не выполняется ли поток
	ExitMessage = new char[1024];
	Semaphore = false;	// Сброс флага остановки потока
	Target = target;
   if(target){
		StartRoutine();
      return;
      }
   else {
		hThread=(HANDLE)_beginthread((void (cdecl *)(void *))CALCULATE::StartRoutine, NULL, (void *)NULL);
//	hThread = CreateThread(NULL , NULL, (LPTHREAD_START_ROUTINE) CALCULATE::StartRoutine,
//	 (LPVOID) NULL, 0, &ThreadId);
		SetThreadPriority(hThread, /*target ? THREAD_PRIORITY_NORMAL : */THREAD_PRIORITY_BELOW_NORMAL);
      }
}

void CALCULATE::StopThread(void)	// функ. прерывания выполнения потока
{
   if(!Runing) return;
	if(Semaphore) return;	// проверка флага, дабы не зависнуть
	Semaphore = true;	// устанавливаем флаг окончания потока и ожидаем
	Stopped = true;	// устанавливаем признак принудительного завершения программы
   WaitForSingleObject(hThread,10000);// пока поток закончит свою работу и сбросит флаг
	if(Runing)
   	{
      TerminateThread(hThread,0);
   	WaitForSingleObject(hThread,INFINITE);
      CloseHandle(hThread);
      Runing = false;
      Result = CALCERROR;
      ResetData();
      }
	Semaphore = true;	// блокир. чтобы не зависнуть если вызовут повторно
	SendMessage(hWnd, WM_COMMAND, IDC_ENDCALC, NULL);// сообщаем головной
													// программе, что поток остановлен
	if(!ExecDemo && ViewProtocol)
   	MessageBox(hWnd, ExitMessage, "Прерывание выполнения программы.", MB_OK|MB_ICONERROR);
}

void CALCULATE::StartRoutine (void)
{
	ThisPointer->Stopped = false;	// сбрасываем признак принудительного завершения
	ThisPointer->Execute();// собственно вызываем функцию анализа
   EnterCriticalSection(&sinc);
	ThisPointer->Runing = false;	// сбрасываем индикатор работы потока
   ThisPointer->ResetData();
   LeaveCriticalSection(&sinc);
	ThisPointer->Semaphore = ThisPointer->Stopped ? false : true;	// сообщ. главной потока, закончено выполнение
   return;       // завершаем поток анализа
//	_endthread();
}

void CALCULATE::Execute ()
{
	EnterCriticalSection(&sinc);
	int i;
   										// Сохраняем момент старта
	Tic = GetTickCount();
	Stopwatch = new CStopwatch;
	NoClassics = false;
	int size = MAX(vSize, hSize);	// задаем размер текущих буферов
	size = MAX(size, 5);		// Страхуемся от ошибки описаной ниже
//	int count = MAX(vNumbSize, hNumbSize);
// По идее в двух следующих операторах New размер выделяемой памяти должен
// определяться переменной count, но при ее значении менее 3 библиотеки
// начинают глючить и вызывают ошибку "нарушение доступа к памяти"
	Original = new byte[size];	// создаем буфер оригинала
	Buffer   = new byte[size];	// создаем буфер вариантов
	Variants = new int [size];	// создаем массив счетчиков элементов
	Block    = new int [size];	// выделяем буфер текущей оцифровки
   if(Containers == COLORS){
   	CBlock      = new byte[size]; // буфер цвета оцифровки
      AccessColor = new byte[hSize*vSize*KROSSFEATURES::Colors];
      }                          // буфер доступных каждой ячейке цветов
   else {
   	CBlock      = NULL;
      AccessColor = NULL;
      }
	Pos      = new int [size];	// выделяем буфер текущих позиций блоков
	StEndPosLen=hSize*hNumbSize+vSize*vNumbSize;
   MapLen=hSize*vSize;
	StPos 	= new int[StEndPosLen];							 // выделяем буфер для хранения стартовых
	EnPos 	= new int[StEndPosLen];							 // конечных и временных
	Complete = new int[MapLen*4];						 		 // выделяем двойнойбуфер отгад. ячеек
   																	 // на случ. когда юзер рисует при решении
	Hstop 	= new bool[hSize];								 // и буфера блокировки
	Vstop 	= new bool[vSize];                         //
	hNumberVariants = new UINT64[hSize];         		 // Буфера количества
   vNumberVariants = new UINT64[vSize];         		 // возможных вариантов
	Runing = true;													 // это сокращает циклы обработки строки
	Total = 0;                 // расчет количества цифр оцифровки (для протокола)
	Cleared = true;				// и проверки на чистоту рисунка
	for(i=0; i<hSize*hNumbSize; i++) if(hData->Digits[i]) Total++;
	for(i=0; i<vSize*vNumbSize; i++) if(vData->Digits[i]) Total++;
	for(i=0; i<MapLen; i++) 			if(BitMap[i] != ZEROS) Cleared = false;
   Semaphore = false;
   LeaveCriticalSection(&sinc);
   if(Containers == COLORS)
        Result = CalcColorLogikMain();
   else Result = CalcLogikMain();	// вызов управляющей функции анализа
	if(Result != USERSTOPED) SendMessage(hWnd, WM_COMMAND, IDC_ENDCALC, (LPARAM)(Result == ALL_OK));
	if(!ExecDemo)Protocol(Result);		// создание протокола работы
}

void CALCULATE::MakeOverlappingCells()
{
	Buffer   = new byte[MAX(MAX(vSize, hSize),5)];	// создаем буфер вариантов
	StPos 	= new int[hSize*hNumbSize+vSize*vNumbSize];// выделяем буфер для хранения стартовых
	EnPos 	= new int[hSize*hNumbSize+vSize*vNumbSize];// конечных и временных
	Complete = new int[MapLen];							 	 // выделяем буфер отгад. ячеек
   CompleteCounter=0;

   MakeStartPosition(); // определяем стартовые и конечные позиции элементов
	MakeOverlapping(true); // закрашиваем перекрываемые элементами ячейки
	MakeOverlapping(false);
	MakeVoid(true);
	MakeVoid(false);

   DELETE(Buffer);
   DELETE(StPos);
   DELETE(EnPos);
   DELETE(Complete);
	return;
}

bool CALCULATE::MakeOverlapping(bool that)
{
// Быстрое заполнение "перекрытых" ячеек
	int end, ptr, i, j;
	bool res = false;
	if(that)
		{
		for (j=0; j<hSize; j++) // счетчик столбцов
			for(i=0; i<hData->Number[j]; i++) // счетчик цифр в строчке
					// если    старт + длина > конец
				if(StPos[j*hNumbSize+i]+hData->Digits[j*hNumbSize+hData->Number[j]-i-1]>EnPos[j*hNumbSize+i])
					// то заполняем ячейки: конец - (старт+длина)
					for(end=EnPos[j*hNumbSize+i]; end<(StPos[j*hNumbSize+i]+hData->Digits[j*hNumbSize+hData->Number[j]-i-1]);end++)
						{
						ptr = end*hSize+j;
						if(BitMap[ptr]!=BLACK)
							{
							BitMap[Complete[CompleteCounter++]=ptr] = BLACK;
							res = true;
							}
						}
		}
	else
		{
		for (j=0; j<vSize; j++) // счетчик строк
			for(i=0; i<vData->Number[j]; i++) // счетчик цифр в строчке
					// если    старт + длина > конец
				if(StPos[j*vNumbSize+i+hSize*hNumbSize]+vData->Digits[j*vNumbSize+vData->Number[j]-i-1]>EnPos[j*vNumbSize+i+hSize*hNumbSize])
					// то заполняем ячейки: конец - (старт+длина)
					for(end=EnPos[j*vNumbSize+i+hSize*hNumbSize]; end<(StPos[j*vNumbSize+i+hSize*hNumbSize]+vData->Digits[j*vNumbSize+vData->Number[j]-i-1]);end++)
						{
						ptr = end+hSize*j;
						if(BitMap[ptr]!=BLACK)
							{
							BitMap[Complete[CompleteCounter++]=ptr] = BLACK;
							res = true;
							}
						}
		}
	return res;
}

bool CALCULATE::MakeVoid(bool that)
{  // отмечаем все однозначно пустые клетки
	int PosPtr;
	int i,j, cells;
	bool res = false;
	if(that)
		{
		for(j=0; j<hSize; j++)	// счетчик столбцов
			{
			PosPtr = j*hNumbSize;
			for(i=0; i<vSize; Buffer[i++]=ZEROS);	// очищаем буфер
			for(i=0; i<hData->Number[j]; i++)		// счетчик элементов
				{
				for (cells = StPos[PosPtr+i];	// заливаем все возм. ячейки строки
					  cells < (EnPos[PosPtr+i]+hData->Digits[j*hNumbSize+hData->Number[j]-i-1]);
					  cells++)
					Buffer[cells] = BLACK;
				}
			for(cells=0; cells<vSize; cells++)	// отмечаем ячейки оставшиеся незалитыми
				if(Buffer[cells]==ZEROS)
					if(BitMap[j+hSize*cells]!=MARKER)
						{
						Complete[CompleteCounter++]=j+hSize*cells;
						BitMap[j+hSize*cells]=MARKER;
						res = true;
						}
			}
		}
	else
		{
		for(j=0; j<vSize; j++)	// счетчик строк
			{
			PosPtr = j*vNumbSize+hSize*hNumbSize;
         for(i=0; i<hSize; Buffer[i++]=ZEROS);	// очищаем буфер
         for(i=0; i<vData->Number[j]; i++)		// счетчик элементов
         	{
            for (cells = StPos[PosPtr+i];	// заливаем все возм. ячейки строки
            	  cells < (EnPos[PosPtr+i]+vData->Digits[j*vNumbSize+vData->Number[j]-i-1]);
                 cells++)
            	Buffer[cells] = BLACK;
            }
	      for(cells=0; cells<hSize; cells++)	// отмечаем ячейки оставшиеся незалитыми
         	if(Buffer[cells]==ZEROS && BitMap[j*hSize+cells]!=MARKER)
            	{
               BitMap[Complete[CompleteCounter++]=j*hSize+cells]=MARKER;
               res = true;
               }
         }
      }
	return res;
}

void CALCULATE::Protocol( int res )// создаем стринг протокола решения
{
	int mbtype;

	char *buff = ExitMessage;
   char NoClassicsString[] ={"\
Поиск решения завершен.\r\
Рисунок головоломки не имеет прямого\r\
логического решения. Был найден первый\r\
подходящий вариант, поэтому рисунок\r\
может содержать искажения. Для того,\r\
чтобы вернуться к моменту, где закончились,\r\
логические действия, выберите пункт меню\r\
ОТМЕНИТЬ. После этого вы можете закрасить\r\
несколько клеток и повторить анализ кроссворда."};
/*	char nologik[] ={"\
Внимание! Головоломка не имеет прямого логического решения.\
 Дальнейшие действия должны основываться на субъективном\
 восприятии рисунка. В принципе вы можете закрасить одну\
 или несколько клеток и еще раз выполнить анализ рисунка."};*/
	char caption[] ={"Результаты анализа головоломки."};
	char overflow[]={"Ошибка !!!\rПереполнение в "};
	char errcalc[] ={"Ошибка !!!\rНесоответствие вертикальной и горизонтальной\
 оцифровки. Обнаружена в "};
	char line[]    ={" строке (сверху)."};
	char column[]  ={" стобце (слева)."};
	char digit[]   ={"\rПроверьте правильность ввода оцифровки."};
	char repaint[] ={"\rВозможно также, что рисунок содержал неточности,\
 удалите рисунок и повторите попытку."};

//	if(res == NOLOGIK)
//		{
//		strcpy(buff, nologik);
//			"Предупреждение.",
//		mbtype = MB_OK|MB_ICONINFORMATION;
//		}
//	else
   if(res == OVERFLOW)
		{
		strcpy(buff, overflow);
		itoa(Number+1, buff+strlen(buff), 10);
		strcpy(buff+strlen(buff), That ? column : line);
		strcpy(buff+strlen(buff), digit);
		mbtype = MB_OK|MB_ICONERROR;
		}
	else if(res == CALCERROR) // ошибка оцифровки или стартового рисунка
		{
		strcpy(buff, errcalc);
		itoa(Number+1, buff+strlen(buff), 10);
		strcpy(buff+strlen(buff), That ? column : line);
		strcpy(buff+strlen(buff), digit);
		if(!Cleared)strcpy(buff+strlen(buff), repaint);
		mbtype = MB_OK|MB_ICONERROR;
		}
	else if(res == USERSTOPED)	// выход при остановке потока пользователем
		{
		strcpy(buff, "Анализ головоломки прерван.");
//		mbtype = MB_OK|MB_ICONERROR|MB_TASKMODAL;
		}
	else // нормальный отчет
		{
      if(NoClassics) strcpy(buff, NoClassicsString);
		else strcpy(buff, "Анализ окончен со следующим результатом:");
		mbtype = MB_OK|MB_ICONINFORMATION;
		}

// Создание строки протокола
	int i, cells=0, procent, full=0, hour, min, sec, msec;

// расчитываем отгаданные и заполненые ячейки
	for(i=0; i<vSize*hSize; i++)
		{
		if(BitMap[i] != ZEROS) cells++;
		if(BitMap[i] != ZEROS && BitMap[i] != MARKER) full++;
		}
	full    = (full*100)/(MapLen);// грубо расчитываем проценты
	procent = (cells*100)/(MapLen);

// количество ячеек
	strcpy(buff+strlen(buff), "\r\rМатрица содержит ячеек:\t\t");
	itoa(MapLen, buff+strlen(buff), 10);

// колич. цифр оцифровки
	strcpy(buff+strlen(buff), "\rОцифровка содержит цифр:\t\t");
	itoa(Total, buff+strlen(buff), 10);

// Процент заполнения рисунка
	if(res == ALL_OK)
		{
		strcpy(buff+strlen(buff), "\rЗаполнение матрицы на:\t\t");
		itoa(full, buff+strlen(buff), 10);
		strcpy(buff+strlen(buff), "%");
		}

// количество проходов
//	strcpy(buff+strlen(buff), "\r\rОтработано циклов:\t\t");
//	itoa(TotalCicle, buff+strlen(buff), 10);

// количество возможных вариантов
	strcpy(buff+strlen(buff), "\rВариантов подстановки:\t\t");
	ltoa(VarTotal, buff+strlen(buff), 10);

// Уровень сложности
//	strcpy(buff+strlen(buff), "\rУровень сложности:\t\t");
//	itoa(TotalCicle/3, buff+strlen(buff), 10);

// количество и процент найденых ячеек
	strcpy(buff+strlen(buff), "\rОпределено ячеек:\t\t");
	itoa(cells, buff+strlen(buff), 10);
	strcpy(buff+strlen(buff), "  \(");
	itoa(procent, buff+strlen(buff), 10);
	strcpy(buff+strlen(buff), "%\).");

// пройдено времени
/*	DWORD Ticur = GetTickCount();
	Ticur -= Tic;
	i = (int)(Ticur/1000);
  	msec = (Ticur/10)%100;
	hour = i/3600;
	min = (i%3600)/60;
	sec = i%60;
*/
   __int64 volatile qwElapsedTime = Stopwatch->Now();
   delete Stopwatch;

	i = (int)(qwElapsedTime/1000);
  	msec = (int)((qwElapsedTime/10)%100);
	hour = i/3600;
	min = (i%3600)/60;
	sec = i%60;

	strcpy(buff+strlen(buff), "\r\rЗатрачено времени (ms):\t\t");
	if(hour){itoa(hour,buff+strlen(buff),10); strcpy(buff+strlen(buff),":");}
	if(min<10) strcpy(buff+strlen(buff),"0");
	itoa(min,buff+strlen(buff),10);
	strcpy(buff+strlen(buff),":");
	if(sec<10) strcpy(buff+strlen(buff),"0");
	itoa(sec,buff+strlen(buff),10);
	strcpy(buff+strlen(buff),".");
	if(msec<10) strcpy(buff+strlen(buff),"0");
	itoa(msec,buff+strlen(buff),10);

	if(res == USERSTOPED)
		return;

	if(res!=ALL_OK || ViewProtocol)
   	MessageBox(hWnd, buff, caption, mbtype);
}

bool CALCULATE::TestLine_MakeMarkers(char *line, int line_len, byte *digits, int numb_digits, bool *markers)
{

//*Часть 1 - расчет начальных-конечных позиций элементов и маркировка оцифровки*

   if(!numb_digits){						// Выход, если пустая строка
   	for(int i=0; i<line_len; i++)
      	if(line[i]!=ZEROS && line[i]!=MARKER) return false;
      return true;
      }
   int var, numb_var, v1;
   int *start = new int[MAX(numb_digits*2, 5)];
   int *end   = start+numb_digits;
   bool res, cur_place;
   int space = line_len-numb_digits+1, count = 0;
   for(int i=0; i<numb_digits; i++){                  // Расчитываем начальные стартовые позиции
   	space-=digits[i];
      start[i]=count++;
      count+=digits[i];
      }
   for(int i=0; i<numb_digits; i++)                  // начальные стоповые позиции
      end[i]=start[i]+space;
                                                     // прогоняем все элементы подстановкой и обн.старт.поз
   for(int el=0; el<numb_digits; el++){              // счетчик элементов
      numb_var=0;
   	for(int pos=start[el]; pos<=end[el]; pos++){   // счетчик свободного пространства подстановки
         cur_place=true;
      	for(int ptr=pos; ptr<pos+digits[el]; ptr++){// счетчик тела элемента
         	if(line[ptr]==MARKER) cur_place=false;
            }
         if(cur_place){                              // если место подходящее
            if(!numb_var){ 								  // обновляем стартовые позиции, если это первое подх.место
            	var=pos-start[el];
            	for(int i=el; i<numb_digits; i++)
            		start[i]+=var;
               }
         	numb_var++;
            }
         }
      }
                                                     // теперь то-же самое для стоповых позиций
   for(int el=numb_digits-1; el>=0; el--){           // счетчик элементов
      numb_var=0;
   	for(int pos=end[el]; pos>=start[el]; pos--){   // счетчик свободного пространства подстановки
         cur_place=true;
      	for(int ptr=pos; ptr<pos+digits[el]; ptr++) // счетчик тела элемента
         	if(line[ptr]==MARKER) cur_place=false;
         if(cur_place){                              // если место подходящее
            if(!numb_var){ 								  // обновляем стартовые позиции, если это первое подх.место
            	var=end[el]-pos;
            	for(int i=el; i>=0; i--)
            		end[i]-=var;
               }
         	numb_var++;
            }
         }
      }

   res=true;
   for(var=0; var<numb_digits; var++)
   	if(start[var]==end[var]){  // при нахождении точного месторасположения
         cur_place=true;         // проверяем прорисован-ли элемент
         for(int i=start[var]; i<start[var]+digits[var]; i++)
         	if(line[i]!=BLACK) res=cur_place=false;
         markers[var]=cur_place;
         }
      else
      	res=false;

//****** Шаг 2 - проверка строки на правильную прорисовку всех элементов *******

   cur_place=true;
   var=0;								// счетчик ячеек строки
   for(numb_var=0; numb_var<numb_digits; numb_var++){// счетчик элементов
   	while(line[var]!=BLACK)    // поиск первой залитой ячейки
   		if(++var>=line_len) break;// прерывание если конец строки
      if(var>=line_len) break;
      v1=var+digits[numb_var];
      while(var<v1)					// проверка верности прорисовки
      	if(line[var++]!=BLACK){
         	cur_place=false;
            break;
            }
      if(!cur_place) break;      // прерывание если несоответсвие прорисовки элементу
      if(line[var++]==BLACK){		// поправка на межелементный разрыв
        	cur_place=false;
         break;
      	}

      }
   while(var<line_len)           // проверка остатка строки на отсутствие зарисовки
   	if(line[var++]==BLACK) cur_place=false;
   if(cur_place && (numb_var==numb_digits)){
   	for(v1=0; v1<numb_digits; v1++)// отмечаем всю оцифровку
      	markers[v1]=true;
      delete[] start;
   	return true;    // выход если строка прорисована верно
      }
   else {
   	for(v1=0; v1<numb_digits; v1++){// обновляем маркировку оцифровки
         cur_place=false;
         if(start[v1]==end[v1]){
            cur_place=true;
         	for(var=start[v1]; var<(start[v1]+digits[v1]); var++)
            	if(line[var]!=BLACK) cur_place=false;
            }
      	markers[v1]=cur_place;
         }
      }
//***************** шаг 3 - проверка отмеченых от краев элементов **************

   int s=0, e=line_len-1;

  	for(numb_var=0; numb_var<numb_digits; numb_var++){// с начала строки
      while((line[s]==MARKER) && (s<line_len)) s++;  // подгонка к первой ячейке,
		if((s<line_len)&&(line[s]==BLACK)){            // не отмеченной маркером
 			cur_place=true;
     		for(v1=s; v1<s+digits[numb_var]; v1++)
     			if(line[v1]!=BLACK) cur_place=false;
	      if(cur_place && (line[v1++]!=BLACK))
  		   	markers[numb_var]=true;
         else break;
         s=v1;
     		}
      else break;
     	}
  	for(numb_var=numb_digits-1; numb_var>=0; numb_var--){// с конца строки
      while((line[e]==MARKER) && (e>=0)) e--;
		if((e>=0) && (line[e]==BLACK)){
   		cur_place=true;
      	for(v1=e; v1>e-digits[numb_var]; v1--)
      		if(line[v1]!=BLACK) cur_place=false;
	      if(cur_place && (line[v1--]!=BLACK))
   	   	markers[numb_var]=true;
         else break;
         e=v1;
      	}
      else break;
      }

//*********** Шаг 4 поиск единственного самого длинного элемента ***************
   int save;
	numb_var=var=0;
   for(v1=0; v1<numb_digits; v1++) // ищем самый длинный элемент
   	if(digits[v1]>var) var=digits[v1];
   for(v1=0; v1<numb_digits; v1++) // проверяем его единственность
   	if(digits[v1]==var)
      	{numb_var++; save=v1;}
   if(numb_var==1){                // выполняем поиск его отметки в строке
      markers[save]=false;
      s=0;
      while(s<line_len){
      	while((line[s]!=BLACK) && (s<line_len)) s++;
         if(s+var>line_len) break;// брек если позиция+длина элем. вылазит за пределы строки
   	   cur_place=true;
	      for(e=s; e<s+var; e++)
      		if(line[e]!=BLACK)
      	   	{s=e; cur_place=false; break;}
   	   if(cur_place)
         	if(line[e]!=BLACK)
	      		{markers[save]=true; break;}
         	else break;
      	}
   	}
   delete[] start;
	return res;
}

bool CALCULATE::TestLine_MakeMarkers_color(char *line, int line_len, byte *digits, int numb_digits, byte *color, bool *markers)
{

//*Часть 1 - расчет начальных-конечных позиций элементов и маркировка оцифровки*

   if(!numb_digits){						// Выход, если пустая строка
   	for(int i=0; i<line_len; i++)
      	if(line[i]!=ZEROS && line[i]!=MARKER) return false;
      return true;
      }
   int var, numb_var, v1;
   int *start = new int[MAX(numb_digits*2, 5)];
   int *end   = start+numb_digits;
   bool res, cur_place;
   int space = line_len-numb_digits+1, count = 0;
   for(int i=0; i<numb_digits; i++){                  // Расчитываем начальные стартовые позиции
   	space-=digits[i];
      start[i]=count++;
      count+=digits[i];
      }
   for(int i=0; i<numb_digits; i++)                  // начальные стоповые позиции
      end[i]=start[i]+space;
                                                     // прогоняем все элементы подстановкой и обн.старт.поз
   for(int el=0; el<numb_digits; el++){              // счетчик элементов
      numb_var=0;
   	for(int pos=start[el]; pos<=end[el]; pos++){   // счетчик свободного пространства подстановки
         cur_place=true;
      	for(int ptr=pos; ptr<pos+digits[el]; ptr++){// счетчик тела элемента
         	if(line[ptr]==MARKER) cur_place=false;
            }
         if(cur_place){                              // если место подходящее
            if(!numb_var){ 								  // обновляем стартовые позиции, если это первое подх.место
            	var=pos-start[el];
            	for(int i=el; i<numb_digits; i++)
            		start[i]+=var;
               }
         	numb_var++;
            }
         }
      }
                                                     // теперь то-же самое для стоповых позиций
   for(int el=numb_digits-1; el>=0; el--){           // счетчик элементов
      numb_var=0;
   	for(int pos=end[el]; pos>=start[el]; pos--){   // счетчик свободного пространства подстановки
         cur_place=true;
      	for(int ptr=pos; ptr<pos+digits[el]; ptr++) // счетчик тела элемента
         	if(line[ptr]==MARKER) cur_place=false;
         if(cur_place){                              // если место подходящее
            if(!numb_var){ 								  // обновляем стартовые позиции, если это первое подх.место
            	var=end[el]-pos;
            	for(int i=el; i>=0; i--)
            		end[i]-=var;
               }
         	numb_var++;
            }
         }
      }

   res=true;
   for(var=0; var<numb_digits; var++)
   	if(start[var]==end[var]){  // при нахождении точного месторасположения
         cur_place=true;         // проверяем прорисован-ли элемент
         for(int i=start[var]; i<start[var]+digits[var]; i++)
         	if(line[i]!=BLACK) res=cur_place=false;
         markers[var]=cur_place;
         }
      else
      	res=false;

//****** Шаг 2 - проверка строки на правильную прорисовку всех элементов *******

   cur_place=true;
   var=0;								// счетчик ячеек строки
   for(numb_var=0; numb_var<numb_digits; numb_var++){// счетчик элементов
   	while(line[var]!=BLACK)    // поиск первой залитой ячейки
   		if(++var>=line_len) break;// прерывание если конец строки
      if(var>=line_len) break;
      v1=var+digits[numb_var];
      while(var<v1)					// проверка верности прорисовки
      	if(line[var++]!=BLACK){
         	cur_place=false;
            break;
            }
      if(!cur_place) break;      // прерывание если несоответсвие прорисовки элементу
      if(line[var++]==BLACK){		// поправка на межелементный разрыв
        	cur_place=false;
         break;
      	}

      }
   while(var<line_len)           // проверка остатка строки на отсутствие зарисовки
   	if(line[var++]==BLACK) cur_place=false;
   if(cur_place && (numb_var==numb_digits)){
   	for(v1=0; v1<numb_digits; v1++)// отмечаем всю оцифровку
      	markers[v1]=true;
      delete[] start;
   	return true;    // выход если строка прорисована верно
      }
   else {
   	for(v1=0; v1<numb_digits; v1++){// обновляем маркировку оцифровки
         cur_place=false;
         if(start[v1]==end[v1]){
            cur_place=true;
         	for(var=start[v1]; var<(start[v1]+digits[v1]); var++)
            	if(line[var]!=BLACK) cur_place=false;
            }
      	markers[v1]=cur_place;
         }
      }
//***************** шаг 3 - проверка отмеченых от краев элементов **************

   int s=0, e=line_len-1;

  	for(numb_var=0; numb_var<numb_digits; numb_var++){// с начала строки
      while((line[s]==MARKER) && (s<line_len)) s++;  // подгонка к первой ячейке,
		if((s<line_len)&&(line[s]==BLACK)){            // не отмеченной маркером
 			cur_place=true;
     		for(v1=s; v1<s+digits[numb_var]; v1++)
     			if(line[v1]!=BLACK) cur_place=false;
	      if(cur_place && (line[v1++]!=BLACK))
  		   	markers[numb_var]=true;
         else break;
         s=v1;
     		}
      else break;
     	}
  	for(numb_var=numb_digits-1; numb_var>=0; numb_var--){// с конца строки
      while((line[e]==MARKER) && (e>=0)) e--;
		if((e>=0) && (line[e]==BLACK)){
   		cur_place=true;
      	for(v1=e; v1>e-digits[numb_var]; v1--)
      		if(line[v1]!=BLACK) cur_place=false;
	      if(cur_place && (line[v1--]!=BLACK))
   	   	markers[numb_var]=true;
         else break;
         e=v1;
      	}
      else break;
      }

//*********** Шаг 4 поиск единственного самого длинного элемента ***************
   int save;
	numb_var=var=0;
   for(v1=0; v1<numb_digits; v1++) // ищем самый длинный элемент
   	if(digits[v1]>var) var=digits[v1];
   for(v1=0; v1<numb_digits; v1++) // проверяем его единственность
   	if(digits[v1]==var)
      	{numb_var++; save=v1;}
   if(numb_var==1){                // выполняем поиск его отметки в строке
      markers[save]=false;
      s=0;
      while(s<line_len){
      	while((line[s]!=BLACK) && (s<line_len)) s++;
         if(s+var>line_len) break;// брек если позиция+длина элем. вылазит за пределы строки
   	   cur_place=true;
	      for(e=s; e<s+var; e++)
      		if(line[e]!=BLACK)
      	   	{s=e; cur_place=false; break;}
   	   if(cur_place)
         	if(line[e]!=BLACK)
	      		{markers[save]=true; break;}
         	else break;
      	}
   	}
   delete[] start;
	return res;
}

bool CALCULATE::TestFull(void)
{
	bool res=true;
   char *buf=new char[MAX(hSize, vSize)];
   int h, count;
   for(h=0; h<hSize; h++){
   	for(count=0; count<vSize; count++)
      	buf[vSize-count-1]=BitMap[h+hSize*count];
      if(Containers != COLORS)
      	res &= TestLine_MakeMarkers(buf, vSize, hData->Digits+h*hNumbSize, hData->Number[h], hData->DigitMarkers+h*hNumbSize);
      else
      	res &= TestLine_MakeMarkers_color(buf, vSize, hData->Digits+h*hNumbSize, hData->Number[h], hData->DigitColors+h*hNumbSize, hData->DigitMarkers+h*hNumbSize);
		}
   for(h=0; h<vSize; h++){
   	for(count=0; count<hSize; count++)
      	buf[hSize-count-1]=BitMap[h*hSize+count];
      if(Containers != COLORS)
      	res &= TestLine_MakeMarkers(buf, hSize, vData->Digits+h*vNumbSize, vData->Number[h], vData->DigitMarkers+h*vNumbSize);
      else
			res &= TestLine_MakeMarkers_color(buf, hSize, vData->Digits+h*vNumbSize, vData->Number[h], vData->DigitColors+h*vNumbSize, vData->DigitMarkers+h*vNumbSize);		}
   delete[] buf;
   return res;
}

bool CALCULATE::CheckComplete()
{
// Проверяем полноту заполнения кроссворда
// сравнивая сумму заполненных ячеек таблицы и сумму оцифровки
// (только для монохромных рисунков)
	int a=0, b=0, i, Colors = KROSSFEATURES::Colors;
   if(Containers != COLORS){
		for(i=0; i<MapLen; i++ )
			if(Maps[i] == BLACK) a++;
		for(i=0; i<hNumbSize*hSize; i++)
			b+=hData->Digits[i];
		return (a == b);
      }
   else {// для цветного рисунка подсчитывается количество заполненых ячеек
   		// каждого цвета и сравнивается с суммой оцифровки
   	int *buf = new int[Colors];
      ZeroMemory((LPVOID) buf, sizeof(int) * Colors);
      for(i=0; i<MapLen; i++)
      	if(Maps[i]!=ZEROS && Maps[i]!=MARKER)
         	buf[Maps[i]-FIRST]++;
      for(i=0; i<hSize*hNumbSize; i++)
      	if(hData->Digits[i])
         	buf[hData->DigitColors[i]-FIRST]-=hData->Digits[i];
      int res = 0;
      for(i=0; i<Colors; i++)
      	res += buf[i];
      delete[] buf;
      return !((bool)res);
      }
}

bool CALCULATE::MakeStartPosition(void)
{
	int end, i, j;
   That = true;
	for (j=0; j<hSize; j++) // счетчик столбцов
		{
      Number = j;
      int len = 0;
      for(i=0; i<hData->Number[j]; i++)
      	len += hData->Digits[j*hNumbSize+hData->Number[j]-i-1]+1;
      if(len-1>vSize) return false;
		end = 0;
		for(i=0; i<hData->Number[j]; i++) // счетчик цифр в строчке
			{
			StPos[j*hNumbSize+i] = end;
			end += hData->Digits[j*hNumbSize+hData->Number[j]-i-1]+1;
			}
		}

   That = false;
	for (j=0; j<vSize; j++) // счетчик строк
		{
		Number = j;
      int len = 0;
      for(i=0; i<vData->Number[j]; i++)
      	len += vData->Digits[j*vNumbSize+vData->Number[j]-i-1]+1;
      if(len-1>hSize) return false;
      end = 0;
		for(i=0; i<vData->Number[j]; i++) // счетчик цифр в строчке
			{
			StPos[j*vNumbSize+i+hSize*hNumbSize] = end;
			end += vData->Digits[j*vNumbSize+vData->Number[j]-i-1]+1;
			}
		}

// заполняем таблицу конечных позиций элементов
	for (int j=0; j<hSize; j++)	// счетчик столбцов
		{
      Number = j;
		end = vSize;
		for (i=0; i<hData->Number[j]; i++)	// счетчик цифр в строчке
			{
			end -= hData->Digits[j*hNumbSize+i]; // end = начало последнего элемента.
			EnPos[j*hNumbSize+hData->Number[j]-i-1] = end;// переносим в позицию последнего элемента
			end--;	// отступ в 1 клетку
			}
		}

	for (int j=0; j<vSize; j++)	// счетчик строк
		{
      Number = j;
		end = hSize;
		for (i=0; i<vData->Number[j]; i++)	// счетчик цифр в строчке
			{
			end -= vData->Digits[j*vNumbSize+i]; // end = начало последнего элемента.
			EnPos[(j*vNumbSize+vData->Number[j]-i-1)+(hNumbSize*hSize)] = end;// переносим в позицию последнего элемента
			end--;	// отступ в 1 клетку
			}
		}
   return true;
}

bool CALCULATE::NewScanLine(bool that)
{
// Новая процедура сканирования строк: вначале обновление стартовых позиций
// элементов методом последовательной подстановки и проверки на верность.
// Далее выполняется тоже самое со стоповыми позициями.
// Следующий этап - сканирование всех ячеек строки. При нахождении заполненной,
// определяется ее длина и происходит поиск элементов, которые могут ее перекрыть
// (по длине и местоположению), если оных всего один, то обновляются стартовые
// и стоповые индексы этого элемента, а затем и всех остальных.
// Затем для каждого элемента отмечаются ячейки, которые могут быть заполнены
// только этим элементом. Проверяем эти ячейки на заполнение, при этом первая-же
// заполненая ячейка сравнивается со стоповой позицией и если она меньше то
// обновляется местоположением этой ячейки. Аналогично для последней заполненной
// ячейки диапазона действия этого элемента.
// Далее отмечаются все перекрытые ячейки, а затем создается пустая строка, заново
// прогоняются все варианты от стартовой до стоповой ползиции для всех элементов
// и при правильном варианте маркируются соответствующие элементы этой строки.
// Те ячейки, которые останутся неотмечеными маркируются в оригинале как пустые.

// Результат = false если не отмечено не одной ячейки(нет изменений).
// Входн. перем.: Number(глобальная) - номер строки или столбца, that - строка или столбец.

// Определяем указатели (для простоты)

   int volatile i, pos, cells, x;

  	int LineLen = that ? vSize : hSize;			// длина строки

   if(that)
   	for(i = 0; i<LineLen; i++)						// копируем строку в буфер (для простоты)
   		Original[i] = Maps[i*hSize+Number];
   else for(i = 0; i<LineLen; i++)
   		Original[i] = Maps[i+hSize*Number];

	byte *pointer = that ? hData->Digits+Number*hNumbSize : vData->Digits+Number*vNumbSize;

	cells = that ? hNumbSize : vNumbSize;	// находим первый ненулевой блок
	while (cells) if(pointer[cells-1]) break; // копируем оцифровку в буфер Block
   				  else cells--;
	BCount = cells;									// заполняем переменную количества блоков

	for (i=0; i<BCount; i++)						// копируем текущую оцифровку в буфер
		Block[i] = pointer[--cells];

// Original - копия строки
// LineLen	- длина строки
// Block		- копия оцифровки строки
// BCoun		- колич. цифр оцифровки

//   for(i=0; i<BCount; i++) {StPos[i] = 0; EnPos[i] = LineLen;}
   int *StPos = CALCULATE::StPos;
   int *EnPos = CALCULATE::EnPos;
   StPos += that ? Number*hNumbSize : Number*vNumbSize+hSize*hNumbSize;
	EnPos += that ? Number*hNumbSize : Number*vNumbSize+hSize*hNumbSize;

   bool compl=true;
   int volatile StartCurrent = 0;
// определяем начальные стартовые позиции
   for(i=0; i<BCount; i++)					// счетчик элементов
      	{										// проверяем от стартовой до стоповой позиции
         int a;
         byte b, c;
         calcerror = true;
         for(pos=StartCurrent; pos<LineLen; pos++)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// a   = указ. начало + длина
                                    	// b   = значение первой ячейки за элементом
            a = pos+Block[i];  			// c   = значение первой ячейки перед элементом
            if(a>LineLen || pos<0) {calcerror = true; break;}
            b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            int d = 0;
            if((b == MARKER || b == ZEROS) && (c == MARKER || c == ZEROS))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
                  {
         			if(Original[cells] == MARKER) cells = LineLen+1;
                  else if(Original[cells] == BLACK) d++;
                  }
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{								// стартовые значения в массиве
               if(d == Block[i] && compl)
               	StPos[i] = EnPos[i] = pos;
               else compl = false;
               StartCurrent = pos + Block[i] + 1;
					for(x=i; x<BCount; x++)	// счетчик оставшихся элем.
               	{
               	if(StPos[x] < pos) StPos[x] = pos;
                  else pos = StPos[x];
               	pos += (i<BCount-1 ? (int)Block[x]+1 : 0);
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// определяем начальные стоповые позиции
   StartCurrent = LineLen - Block[BCount-1];
   compl = true;
   for(i = BCount-1; i >= 0; i--)		// обратный счетчик элементов
      	{										// проверяем от стоповой до стартовой позиции
         calcerror = true;
         for(pos=StartCurrent; pos>=0; pos--)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// c   = значение первой ячейки перед элементом
                                    	// b   = значение первой ячейки за элементом
            int a = pos+(int)Block[i]; // a   = указ. начало + длина
            if(a>LineLen || pos<0) {calcerror = true; break;}
            byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            int d = 0;
            if((b == MARKER || b == ZEROS) && (c == MARKER || c == ZEROS))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
         			if(Original[cells] == MARKER) cells = LineLen+1;
                  else if(Original[cells] == BLACK) d++;
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{
               if(d == Block[i] && compl)
               	StPos[i] = EnPos[i] = pos;
               else compl = false;
               StartCurrent = (i>0 ? pos - Block[i-1] - 1 : StartCurrent);
					for(x=i; x >= 0; x--)	// счетчик оставшихся элем.
               	{
               	if(EnPos[x] > pos) EnPos[x] = pos;
                  else pos = EnPos[x];
               	pos -= (x > 0 ? ((int)Block[x-1]+1) : 0);
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// небольшое добавление ограничивающее диапазон действия элемента от конца
// диапазона предыдущего - первой непустой ячейкой
	for(i=0; i<BCount; i++)
      {
      int a = i>0 ? EnPos[i-1]+Block[i-1] : 0;	// a - граница диапазона пред. элемента
   	for(pos=a; pos<LineLen; pos++)
      	if(Original[pos] == BLACK)
         	if(EnPos[i] > pos)
               { EnPos[i] = pos; break; }
      }

	for(i=BCount-1; i>=0; i--)
      {
      int a = i<BCount-1 ? StPos[i+1]-1 : LineLen-1;	// a - граница диапазона пред. элемента
   	for(pos=a; pos > 0; pos--)
      	if(Original[pos] == BLACK)
         	if(StPos[i]+Block[i]-1 < pos)
               { StPos[i] = pos-Block[i]+1; break; }
      }

// Следующий этап - сканирование всех ячеек строки. При нахождении заполненной,
// определяется ее длина и происходит поиск элементов, которые могут ее перекрыть
// (по длине и местоположению), если оных всего один, то обновляются стартовые
// и стоповые индексы этого элемента, а затем и всех остальных.
// Если же длина совпадает со всеми перекрывающими элементами, то маркируем ее
// границы.
// P.S. Параллельно вышеописаному данный блок при нахождении подходящего элемента
// методом подстановки ищет непротиворечащее оригиналу положение этого элемента,
// с включением этой ячейки (или ячеек).
// При нахождении он увеличивает счетчики соответствующие каждой заполняемой ячейки
// и увеличиват общий счетчик вариантов. В итоге если к концу проверки величина
// какого-либо из счетчиков ячеек совпадает с общим счетчиком значит однозначно
// заливаем эту ячейку.

   int NumbElem, Element, ElemLen=0;
   bool repeat;//, skips = false;
   do	{
      repeat = false;
		for(i=0; i<LineLen; i++)      // счетчик ячеек
   		if(Original[i] != ZEROS && Original[i] != MARKER)	// если ячейка окрашена
      		{
         	NumbElem = 0;
	         x = i;                  // определяем длину
   	      while(x<LineLen && Original[++x] == BLACK);
      	   x -= i;                 // х = длина
// { здесь и далее отмечено то, что описано в P.S.
				int counter=0;		                                 		// обнуляем общий счетчик
				for(cells=0; cells<LineLen; cells++) Variants[cells] = 0;// обнуляем счетчики ячеек
// }
         	for(cells=0; cells<BCount; cells++)	// счетчик элементов
	         	{  // если элем. длинее,   старт. раньше       и  кончается позже то он подходит
   	         if((Block[cells] >= x) && (StPos[cells] <= i) && (EnPos[cells]+Block[cells] >= i+x))
      	      	{
         	      NumbElem++;       	// увеличиваем счетчик подходящих элементов
            	   Element = cells;  	// сохраняем номер элемента
               	if(NumbElem > 1 && ElemLen != Block[cells]) ElemLen = LineLen+1;
	               else ElemLen = Block[cells];
//                  if(skips) continue;
// {
                  int d = MIN(MIN(i, LineLen-Block[cells]),EnPos[cells]);
                  int e = MAX(MAX(i+x-Block[cells], 0), StPos[cells]);
                  for(int a = e; a <= d; a++)// проверяем положение
                  	{
                     bool r = false;
                     if((a > 0) && (Original[a-1]==BLACK)) r = true;
                     if((a+Block[cells] < LineLen) && (Original[a+Block[cells]]==BLACK)) r = true;
//                     bool r = ((a > 0) ? (Original[a-1]==BLACK) : false);
//                     r != ((a+Block[cells] < LineLen) ? (Original[a+Block[cells]]==BLACK) : false);
                     if(r) continue; 	// прокрутка цикла если сбоку закрашеная ячейка

                     for(int b=a; b<a+Block[cells]; b++)
                     	{if(Original[b] == MARKER) {r = true; break;}}
                     if(r) continue; 	// прокрутка цикла если внутри попался маркер
                     // Ok - увеличиваем счетчики
                     counter++;
                     for(int b=a; b<a+Block[cells]; b++)
                     	Variants[b]++;
                     }
// }
   	            }
      	      }
// {
//            if(!skips)
            	{	// сравниваем счетчики
            	if(!counter) {calcerror = true; return false;}
            	for(cells=0; cells<LineLen; cells++)
            		if(Variants[cells] == counter) Original[cells] = BLACK;
               }
// }
         	if(ElemLen == x)       // если длина совпадает со всеми доступными элементами
         		{
	            if(i>0) Original[i-1] = MARKER;// то маркируем границы
   	         if((i+x)<LineLen)Original[i+x] = MARKER;
      	      }
         	if(NumbElem == 1)			// если элемент единственный
         		{                    // обновляем стартовые и стоповые позиции
	            if(StPos[Element] < (i+x-Block[Element])) {StPos[Element] = (i+x-Block[Element]); repeat = true;}
   	         if(EnPos[Element] > i) {EnPos[Element] = i; repeat = true;}
      	      // обновляем стартовые и стоповые позиции остальных элементов
         	   for(cells=Element-1;cells>=0;cells--)
               	if(EnPos[cells]+Block[cells]+1>EnPos[cells+1])
                  	{EnPos[cells]=EnPos[cells+1]-Block[cells]-1; repeat = true;}
            	for(cells=Element+1;cells<BCount;cells++)
               	if(StPos[cells]<StPos[cells-1]+Block[cells-1]+1)
                  	{StPos[cells]=StPos[cells-1]+Block[cells-1]+1; repeat = true;}
	            }
            else if(!NumbElem) {calcerror = true; return false;}
   	      i += x;
      	   }
//      if(repeat)  skips = true;
      } while(repeat);
// Данный блок проверяет количество одинаковых по длине блоков и сканирует
// строку в поиске однозначно определеных блоков данной длины. Если количество
// блоков в оцифровке соответствует количеству в строке то присваиваются и
// отождествляются их стартовые и конечные индексы
   int LastElemLen = 0;
	for(x=0; x<BCount; x++)
   	{
      NumbElem = 0;        				// подсчитываем количество одинаковых блоков в оцифровке
      if(LastElemLen == Block[x]) continue;
      else LastElemLen = Block[x];
      for(i=0; i<BCount; i++) if(Block[i] == Block[x]) NumbElem++;
      for(i=0; i<LineLen; i++)			// сканируем строку в поиска заполненых ячеек
      	if(Original[i] != ZEROS && Original[i] != MARKER)	// если ячейка окрашена
         	{
         	int a = i;                 // определяем длину
            byte c = (byte)(i>0 ? Original[i-1] : MARKER);		// c = значение ячейки перед элем.
         	while(a<LineLen && Original[++a] == BLACK);
            byte b = (byte)(a<LineLen ? Original[a] : MARKER);	// b = значение ячейки за элем.
         	a -= i;                 	// a = длина
            if((b == MARKER) && (c == MARKER) && (a == Block[x])) NumbElem --;
            i += a;                    // проверяем на совпадение длины
            }
      if(!NumbElem)							// Если количество совпадает
      	{
         pos = 0;			// pos = позиция в строке
         int a = 0;   	// a = позиция в оцифровке
         while(a<BCount && Block[a]!=Block[x]) a++;
         while(1)
         	{
            for(;pos<LineLen;pos++)
      			if(Original[pos] != ZEROS && Original[pos] != MARKER)	// если ячейка окрашена
         			{
         			int d = pos;                 // определяем длину
            		byte c = (byte)(pos>0 ? Original[pos-1] : MARKER);		// c = значение ячейки перед элем.
         			while(d<LineLen && Original[++d] == BLACK);
            		byte b = (byte)(d<LineLen ? Original[d] : MARKER);	// b = значение ячейки за элем.
         			d -= pos;                 	// a = длина
            		if((b == MARKER) && (c == MARKER) && (d == Block[x]))
                  	{
                     StPos[a] = EnPos[a] = pos;
                     while(a<BCount && Block[++a]!=Block[x]);
                     if(a == BCount) break;
                     }
                  pos += d;
            		}
            if(a == BCount) break;
            }
         }
      }

// Теперь производится поиск элементов прижатых к маркеровке, но
// неизвесной длины и если она меньше наименьшего значения оцифровки, то элемент
// удлиняется.
   int MinElemLen = 255;               // определяем длину минимального элемента
   for(i=0; i<BCount; i++) if(MinElemLen > Block[i]) MinElemLen = Block[i];
   for(i=0; i<LineLen; i++)				// сканируем строку в поиска заполненых ячеек
    	if(Original[i] != ZEROS && Original[i] != MARKER)	// если ячейка окрашена
       	{
        	int a = i;                 	// определяем длину
         byte c = (byte)(i>0 ? Original[i-1] : MARKER);		// c = значение ячейки перед элем.
        	while(a<LineLen && Original[++a] == BLACK);
         byte b = (byte)(a<LineLen ? Original[a] : MARKER);	// b = значение ячейки за элем.
        	a -= i;                 		// a = длина
         if((b == MARKER && c == ZEROS) || (c == MARKER && b == ZEROS) && (a < MinElemLen))
         	{
            if(b == MARKER) x=i+a-MinElemLen;
            else            x=i;
            for(pos = 0; pos<MinElemLen; pos++) if(pos+x < LineLen) Original[pos+x] = BLACK;
            												else {calcerror = true; return false;}
            }
         i += a;                    	// проверяем на совпадение длины
         }

// Теперь обновляем старт - стоп позиции методом подстановки
   StartCurrent = StPos[0];
// определяем начальные стартовые позиции
   for(i=0; i<BCount; i++)					// счетчик элементов
      	{										// проверяем от стартовой до стоповой позиции
         calcerror = true;
         for(pos=StartCurrent; pos<LineLen; pos++)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// a   = указ. начало + длина
                                    	// b   = значение первой ячейки за элементом
            int a = pos+Block[i];  		// c   = значение первой ячейки перед элементом
            byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            if((b == MARKER || b == ZEROS) && (c == MARKER || c == ZEROS))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
         			if(Original[cells] == MARKER) cells = LineLen+1;
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{								// стартовые значения в массиве
               StartCurrent = pos + Block[i] + 1;
					for(x=i; x<BCount; x++)	// счетчик оставшихся элем.
               	{
               	if(StPos[x] < pos) StPos[x] = pos;
                  else pos = StPos[x];
               	pos += (i<BCount-1 ? (int)Block[x]+1 : 0);
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// определяем начальные стоповые позиции
   StartCurrent = EnPos[BCount-1];
   for(i = BCount-1; i >= 0; i--)		// обратный счетчик элементов
      	{										// проверяем от стоповой до стартовой позиции
         calcerror = true;
         for(pos=StartCurrent; pos>=0; pos--)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// c   = значение первой ячейки перед элементом
                                    	// b   = значение первой ячейки за элементом
            int a = pos+(int)Block[i]; // a   = указ. начало + длина
            byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            if((b == MARKER || b == ZEROS) && (c == MARKER || c == ZEROS))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
         			if(Original[cells] == MARKER) cells = LineLen+1;
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{
               StartCurrent = (i>0 ? pos - Block[i-1] - 1 : StartCurrent);
					for(x=i; x >= 0; x--)	// счетчик оставшихся элем.
               	{
               	if(EnPos[x] > pos) EnPos[x] = pos;
                  else pos = EnPos[x];
               	pos -= (x > 0 ? ((int)Block[x-1]+1) : 0);
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// Теперь для каждого элемента отмечаются ячейки, которые могут быть заполнены
// только этим элементом. Проверяем эти ячейки на заполнение, при этом первая-же
// заполненая ячейка сравнивается со стоповой позицией и если она меньше то
// обновляется местоположением этой ячейки. Аналогично для последней заполненной
// ячейки диапазона действия этого элемента.

	int StartPrivate, EndPrivate;	// индексы начала и конца "частной зоны"
   for(i=0; i<BCount; i++)       // счетчик элементов
   	{  	                    	// StartPrivate = конечная позиция предыдущ. элем.
     	StartPrivate = ((i>0) ? (EnPos[i-1] + Block[i-1]) : StPos[0]);
      EndPrivate   = ((i<(BCount-1)) ? StPos[i+1]-1 : (EnPos[i]+Block[i]-1));
      	                       	// EndPrivate   = стартовая позиция след. ячейки
      for(x=StartPrivate; x<=EndPrivate; x++)// Проверяем ячейки на заполнение
         {
         if(Original[x]!= ZEROS && Original[x]!= MARKER)
            {
            if(EnPos[i] > x) EnPos[i] = x;
            x = LineLen+1;
            break;
            }
         }
      for(x=EndPrivate; x>=StartPrivate; x--)// Проверяем ячейки на заполнение
         {
         if(Original[x]!= ZEROS && Original[x]!= MARKER)
            {
            if(StPos[i] < (x-Block[i])) StPos[i] = x-Block[i];
            x = LineLen+1;
            break;
            }
         }
      }

// Теперь обобщим полученные данные прогоном от стартовой до стоповой позиции и
// если подходящий вариант один однозначно определим старт и стоп индексы
	for(i=0; i<BCount; i++)
		{
      Element = ElemLen = 0;
   	for(pos = StPos[i]; pos <=EnPos[i]; pos++)
			{                       	// i	 = номер элемента
           									// pos = указ. на начало
                                   	// a   = указ. начало + длина
                                   	// b   = значение первой ячейки за элементом
         int a = pos+Block[i];  		// c   = значение первой ячейки перед элементом
         byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
         byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
         cells = -1;
         if((b == MARKER || b == ZEROS) && (c == MARKER || c == ZEROS))
         	{
            for(cells = pos; cells < a; cells++) // от старта до конца
                                		// вынуждаем цыкл окончится, если внутри маркер
        			if(Original[cells] == MARKER) cells = LineLen+1;
            }
         else cells = LineLen+1;

         if(cells <= LineLen)			//
           	{								//
            Element = pos;
            ElemLen++;
            }
         }
      if(ElemLen == 1) StPos[i] = EnPos[i] = Element;
      else if(!ElemLen) {calcerror=true; return false;}
      }

// Далее отмечаются все перекрытые ячейки, а затем создается пустая строка, заново
// прогоняются все варианты от стартовой до стоповой ползиции для всех элементов
// и при правильном варианте маркируются соответствующие элементы этой строки.
// Те ячейки, которые останутся неотмечеными маркируются в оригинале как пустые.

	for(i=0; i<BCount; i++)       // отмечаем перекрытые ячейки
   	for(x=EnPos[i]; x<(StPos[i]+Block[i]); x++)
      	Original[x] = BLACK;
                                 // обнуляем буфер (пустая строка)
   for(i=0; i<LineLen; i++) Buffer[i] = ZEROS;
   for(i=0; i<BCount; i++)			// счетчик элементов
   	for(pos=StPos[i]; pos<=EnPos[i]; pos++)	// счетчик доступных позиций
            {
            int a = pos+(int)Block[i]; // a   = указ. начало + длина
            byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            if((b == MARKER || b == ZEROS) && (c == MARKER || c == ZEROS))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                 // вынуждаем цыкл окончится, если внутри маркер
         			if(Original[cells] == MARKER) cells = LineLen+1;
               }
            else cells = LineLen+1;
            if(cells<=LineLen)   // если позиция непротиворечащая - заливаем ячейки строки
            	for(cells = pos; cells < a; cells++) Buffer[cells] = BLACK;
            }                    // маркируем пустые ячейки в строке оригинала
	for(i=0; i<LineLen; i++) if(Buffer[i] == ZEROS) Original[i] = MARKER;
   bool res = false;             // копируем полученную строку назад в матрицу
   pos = 0;
   for(i=0; i<LineLen; i++)
      {
      if(Original[i] != ZEROS) pos ++;
      x = that ? (i*hSize+Number) : (i+hSize*Number);
   	if((Maps[x]) != Original[i])
      	{
         if(that) Vstop[i]=true;
         else		Hstop[i]=true;
         Maps[x] = Original[i];
         if(Maps==BitMap) Complete[CompleteCounter++] = x;
         res = true;
         }
      }                          // сбрасываем флажки для дальнейшего игнорирования
   if(!res || pos == LineLen) 	if(that) Hstop[Number] = false;	// отгаданых строк
                        			else     Vstop[Number] = false;
	return res;
}

calcres CALCULATE::CalcLogikMain ()
{
// Управляющая программа логического решения кроссворда.
// Величина возврата зависит от результата решения
// ALL_OK - рисунок решен
// NOLOGIK - нет логического решения кроссворда
// USERSTOPED - остановка пользователем
// CALCERROR - ошибка оцифровки или стартового рисунка
// OVERFLOW - переполнение строки суммой цифр оцифровки
	TotalCicle = Count = 0;	// обнуление счетчиков строк и прогонов
   CompleteCounter = 0;	// сбрасываем счетчик отгаданных ячеек
   bool cleared = false;
   Maps = BitMap;
   NoClassics = false;
   VarTotal = 0;
	// заполняем таблицу начальных позиций элементов
	if(!MakeStartPosition()) return OVERFLOW; // определяем стартовые и конечные позиции элементов
	VarTotal = SetPriority(true);

begin:
   TotalCicle = Count = 0;	// обнуление счетчиков строк и прогонов
   MakeStartPosition();
   for(int i=0; i<hSize; i++)	Hstop[i] = true;	// сброс отгаданых строк
   for(int i=0; i<vSize; i++)	Vstop[i] = true;

   calcerror = false;
   do { // головной цикл прогона всех строк
      Res = false;
		int X=0, Y=0;
      while(X<hSize || Y<vSize){
      	if(X<hSize && Hstop[Number=X]) Res |= NewScanLine(That = true);
         if(calcerror)	break;
         if(Y<vSize && Vstop[Number=Y]) Res |= NewScanLine(That = false);
         if(calcerror) break;
         X++; Y++;
			}
     	if(calcerror)	break;
      if(Semaphore) return USERSTOPED; // Выход, по требованию
      if(Res) TotalCicle++;	// наращиваем счетчик прогонов (для протокола)
      } while (Res);
  	if(calcerror) if(!cleared && ClearPicturesFromCalc)
      {
      memset(BitMap, ZEROS, MapLen);
      InvalidateRect(hWnd, &Fild, false);
      cleared = true;
		goto begin;
      }
  	else return CALCERROR;
	if(CheckComplete()) return ALL_OK;
   SendMessage(hWnd, WM_COMMAND, IDC_WORKNOLOGIC, 0);	// посылаем сообщение окну, что

   infinity = 0;
   NoClassics = true;											// наступила долгая рутина поиска
   calcres slv;                                       // подходящего варианта
   byte *SaveMatrix = new byte[MapLen];
   memcpy(SaveMatrix, BitMap, MapLen);
	STATUS("Идет поиск решения.");
   Maps=SaveMatrix;
   Recurce=1;
   DeepRecurce=150;
	while(1){
		slv=FindDecision();
      if(slv==ALL_OK || slv==USERSTOPED) break;
		int coun=0;
      for(int i=0; i<MapLen; i++)
      	if(BitMap[i]!=Maps[i]) {coun++; BitMap[i]=Maps[i]; Complete[CompleteCounter++]=i;}
      if(!coun) {DeepRecurce+=30; Recurce++;}
      else SendMessage(hWnd, WM_COMMAND, IDC_WORKNOLOGIC, 0);	// посылаем сообщение окну, что
      if(DeepRecurce>=MapLen)
      	DeepRecurce=MapLen;
		}
	if(slv!=CALCERROR)
      for(int i=0; i<MapLen; i++)
      	if(BitMap[i]!=Maps[i]) {BitMap[i]=Maps[i]; Complete[CompleteCounter++]=i;}
   Maps=NULL;
   delete[] SaveMatrix;

// выход если не найден вариант решения
	return slv;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CALCULATE::SearchSequence(void *sts)
{
#define ELEMENT
	SAVE_CALC_STATE *scs = (SAVE_CALC_STATE *)sts;
	int *Seq = scs->Sequence;
   int *ptr = new int[vSize];
   int volatile i, cells, x, HX;
#ifdef ELEMENT
   for(HX=0; HX<vSize; HX++){								// Расчитывем варианты строк
  		for(i = 0; i<hSize; i++)							// копируем строку в буфер
  			Original[i] = Maps[i+hSize*HX];
		byte *pointer = vData->Digits+HX*vNumbSize;
		cells = vNumbSize;									// находим первый ненулевой блок
		while (cells) 	if(pointer[cells-1]) break;	// копируем оцифровку в буфер Block
   				  		else cells--;
		scs->BCount[HX] = BCount = cells;	// заполняем переменную количества блоков
		for (i=0; i<BCount; i++)							// копируем текущую оцифровку в буфер
			Block[i] = pointer[--cells];              // Original - копия строки
   	int *StPos = CALCULATE::StPos + HX*vNumbSize+hSize*hNumbSize;
   	int *EnPos = CALCULATE::EnPos + HX*vNumbSize+hSize*hNumbSize;
      int Counter=0;
      for(i=0; i<BCount; i++){							// Цикл элементов
         if(StPos[i]==EnPos[i]) continue;
      	for(x=StPos[i]; x<=EnPos[i]; x++){			// Цикл "от старт до стоп"
         	Counter++;
            for(cells=x; cells<x+Block[i]; cells++)// Цикл проверки ячеек элемента
            	if(Original[cells]==MARKER){
                  x=cells;
                  Counter--;
               	break;
                  }
            }
         }
      ptr[HX]=Counter ? Counter : Overflow-1;
		}
#else
	for(HX=0; HX<vSize; HX++){
      int Counter=0;
		for(i = 0; i<hSize; i++){
         if(Maps[HX*hSize+i]==ZEROS) Counter++;
			}
      ptr[HX]=Counter ? Counter : Overflow-1;
      }
#endif // ELEMENT
   for(i=0; i<vSize; i++){                        // Построение последовательности строк
      int tmp=Overflow;
      for(x=0; x<vSize; x++)
      	if(ptr[x]<tmp){
				tmp=ptr[x];
            Seq[i]=x;
            }
      ptr[Seq[i]]=Overflow;
      }
   delete[] ptr;
}

calcres CALCULATE::FindDecision(void *sts)
{
   Recurce--;
   bool res;
	int X, Y;
   if(!sts) sts = (void *)new SAVE_CALC_STATE(this);
	SAVE_CALC_STATE *scs = (SAVE_CALC_STATE *)sts;
   scs->RecurceSkip=0;
	SearchSequence(scs);
   for(scs->LinePtr=0; scs->LinePtr<vSize; scs->LinePtr++){// Цикл строк
   	scs->Line = scs->Sequence[scs->LinePtr];
      for(scs->Elem=0; scs->Elem<hSize; scs->Elem++){		// Цикл ячеек
        	scs->pos = scs->Line*hSize+scs->Elem;
         if(Maps[scs->pos]!=ZEROS) continue;
         Maps[scs->pos]=BLACK;
         Hstop[scs->Elem]=true;
         Vstop[scs->Line]=true;

  			res = false;
         Number=scs->Elem;
			res |= NewScanLine(That = true);
         Number=scs->Line;
	   	res |= NewScanLine(That = false);
			if(res)
	  			do { // цикл прогона всех строк
   	  			res = false;
					X=0; Y=0;
  					while(X<hSize || Y<vSize){
            	   calcerror = false;
  						if(Hstop[Number=X] && X<hSize) res |= NewScanLine(That = true);
	      			if(calcerror)	break;
   		   		if(Vstop[Number=Y] && Y<vSize) res |= NewScanLine(That = false);
  	   				if(calcerror) break;
         	      X++; Y++;
						}
	   			if(Semaphore) {
   	         	scs->RestoreData();
      	        	delete scs;
         	      Recurce++;
            	   return USERSTOPED;} // Выход, по требованию
	  	   		} while (res);
//	      memcpy(BitMap, Maps, MapLen);
//   	   RedrawWindow(hWnd, &Fild, NULL, RDW_INVALIDATE);
		   if(calcerror){
         	scs->SaveMaps[scs->pos]=MARKER;
         	scs->SaveHstop[scs->Elem]=true;
         	scs->SaveVstop[scs->Line]=true;
				scs->RestoreData();
     			res = false;
            Number=scs->pos%hSize;
  				res |= NewScanLine(That = true);
            Number=scs->pos/hSize;
   	   	res |= NewScanLine(That = false);
            if(res) {
	            scs->SaveAttr();
   	         scs->SaveMap();
               }
            }
			else if(CheckComplete()) {
           	delete (SAVE_CALC_STATE *)scs;
            Recurce++;
            return ALL_OK; // если анализ окончен
            }
  			else {
            int volatile Acount=0, Bcount=0;
            for(int i=0; i<MapLen; i++)
            	if(Maps[i]==ZEROS) Acount++;
               else if(Maps[i]!=scs->SaveMaps[i]) Bcount++;
            if(Acount<DeepRecurce){
//         	if(Acount<DeepRecurce && (bool)Recurce){
					calcres rfd = FindDecision();
            	scs = (SAVE_CALC_STATE *)sts;
        			if(rfd==ALL_OK){
               	delete scs;
                  Recurce++;
                  return rfd;}
               else if(rfd==USERSTOPED){
                  scs->RestoreData();
               	delete scs;
                  Recurce++;
                  return rfd;}
               else if(rfd==CALCERROR)
	               scs->SaveMaps[scs->pos]=MARKER;
         			scs->SaveHstop[scs->Elem]=true;
         			scs->SaveVstop[scs->Line]=true;
     					res = false;
	      	      Number=scs->pos%hSize;
  						res |= NewScanLine(That = true);
	   	         Number=scs->pos/hSize;
   	   			res |= NewScanLine(That = false);
         		   if(res) {
	   	      	   scs->SaveAttr();
   		         	scs->SaveMap();
		               }
   	         }
				else scs->RecurceSkip++;
            scs->RestoreData();
				}
         }
      }
   delete scs;
	Recurce++;
   if(scs->RecurceSkip)
   	return NOLOGIK;
   return CALCERROR;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

UINT64 CALCULATE::CalcNumbVariants(int module, int range)
{	// Для создания этой таблицы и маленькой процедурки я потратил целый день !!!
						// ...но оно того стоило.
/*
Следующая процедура расчитывает количество вариантов подстановки строки.
Искомое равно сумме значений чисел снизу и слева от искомого плюс 1
(см. таблицу)
6,		27,	83,	209,	461,	923,
5,		20,	55,	125,	251,	461,
4,		14,	34,	69,	125,	209,	Модуль /амплитуда/
3,		9,		19,	34,	55,	83,   (количество пространства строки для перемещения элементов
2,		5,		9,		14,	20,	27,    = длина стр. - суммарн. длина всех эл. - колич.эл. + 1)
1,		2,		3,		4,		5,		6,
			разрядность (количество элементов в строке)
*/
	if(!module || !range) return 0;
	UINT64 *mass = new UINT64[module];	// массив хранения промежуточных результатов
   int i,m;
   for(i=0; i<module; i++)
   	mass[i]=((UINT64)i)+1;			// заполняем массив начальными данными
   for(i=1; i<range; i++)
   	{
      (*mass)++;
   	for(m=1; m<module; m++)
      	mass[m] += mass[m-1]+1;
      }
   UINT64 res = mass[module-1];
   delete[] mass;
   return res;
}

UINT64 CALCULATE::SetPriority(bool fulls) // расчет количества возможных вариантов (приоритетов)
{
   EnterCriticalSection(&sinc);
	int sum, numb;             // подготавливаем массивы
	for(int i=0; i<hSize; i++) Hstop[i] = true;
	for(int i=0; i<vSize; i++) Vstop[i] = true;
   UINT64 total = 0;
   for(int i=0; i<hSize; i++)	// расчитываем приоритеты столбцов
   	{
		sum = numb = 0;
      for(int j=0; j<hNumbSize; j++)
      	{
      	if(hData->Digits[i*hNumbSize+j]) numb++;
      	sum += hData->Digits[i*hNumbSize+j];
         if((bool)j && (bool)hData->Digits[i*hNumbSize+j])
         	if(Containers == COLORS && hData->DigitColors[i*hNumbSize+j]!=hData->DigitColors[i*hNumbSize+j-1]);
         	else sum++;
         }
      sum = vSize - sum;	// подсчитываем незаполняемые ячейки
      if(fulls) total += hNumberVariants[i] = CalcNumbVariants(sum,numb);
      else total += CalcNumbVariants(sum,numb);
      }
   for(int i=0; i<vSize; i++)	// расчитываем приоритеты строк
   	{
		sum = numb = 0;
      for(int j=0; j<vNumbSize; j++)
      	{
      	if(vData->Digits[i*vNumbSize+j]) numb++;
      	sum += vData->Digits[i*vNumbSize+j];
         if((bool)j && (bool)vData->Digits[i*vNumbSize+j])
         	if(Containers == COLORS && vData->DigitColors[i*vNumbSize+j]!=vData->DigitColors[i*vNumbSize+j-1]);
         	else sum++;
         }
      sum = hSize - sum;	// подсчитываем незаполняемые ячейки
      if(fulls) total += vNumberVariants[i] = CalcNumbVariants(sum,numb);
      else total += CalcNumbVariants(sum,numb);
      }
   LeaveCriticalSection(&sinc);
   return total;
}

char *CALCULATE::ltoa(UINT64 value, char *string, int radix)
{	// преобразуем значение UINT64 в строку, разделенную запятыми
   char buf[33];
   _ui64toa(value, buf, radix);
   int c = (strlen(buf)+2)/3-1;
   char *st = buf+strlen(buf)-1;
   char *en = buf+strlen(buf)+c;
   *(en--) = 0;
   for(int i=0; i<c; i++){
   	for(int p=0 ; p<3; p++)
         *(en--) = *(st--);
      *(en--) = '.';
      }
   strcpy(string+strlen(string), buf);
   return string+strlen(string);
}
/*
calcres CALCULATE::SelectionLogicVariants()
{ // Программа поиска решения при отсутствии однозначного логического решния
	VarTotal = SetPriority(true); // определяем количество доступных вариантов для всех строк
   int size = hSize+vSize;
//   int MapLen = hSize*vSize;
   int *Sequence = new int[size]; // выделяем массив для перечисления последовательности
   UINT64 *mass = new UINT64[size];
   Maps = new byte[MapLen];
   UINT64 a, max = 0;
   int i, secCount = 0;

// копируем массивы в буфер
   for(i=0; i<hSize; i++) mass[i] = hNumberVariants[i];
   for(i=0; i<vSize; i++) mass[i+hSize] = vNumberVariants[i];

// находим максимальное значение
   for(i=0; i<size; i++) if(max < mass[i]) max = mass[i];

// перечисляем последовательность строк для подстановки
   for(int c=0; c<size; c++)
   	{
	   a=max;                     // сканируем буфер в поисках наименьшего значения
		for(i=0; i<size; i++) if(a > mass[i]) a = mass[i];
                                 // определяем номер стр. с наименьшим кол. вар. подстановки
	   for(int volatile i=0; i<size; i++) if(a == mass[i])
      	{mass[i]=max+1; Sequence[secCount++]=i; i=size; break;}
      }

// теперь исключаем из списка строки, которые полностью отгаданы
	for(i=0; i<size; i++)
   	{
      bool that = (Sequence[i]<hSize ? true : false);
      int line = (that ? Sequence[i] : Sequence[i]-hSize);
      if(that) if(!Hstop[line]) Sequence[i] = size+1;
      else 		if(!Vstop[line]) Sequence[i] = size+1;
      }

// снова копируем массивы в буфер
   for(i=0; i<hSize; i++) mass[i] = hNumberVariants[i];
   for(i=0; i<vSize; i++) mass[i+hSize] = vNumberVariants[i];

//
	calcres checkres = NOLOGIK;
   for(int infinity = 0; infinity<size; infinity++)//
		{															// цикл подстановки для всех строк и столбцов
      if(Sequence[infinity] >= size) continue;
      CALCULATE::infinity = Sequence[infinity];
      bool that = That = (Sequence[infinity]<hSize ? true : false);
      int line = Number = that ? Sequence[infinity] : Sequence[infinity]-hSize;
      if(!CheckFull()) continue;
      int linelen = that ? vSize : hSize;
      UINT64 NoLogVar;
      int NoLogCount=0;
      checkres = CALCERROR;
      for(UINT64 var = 0; var < mass[Sequence[infinity]]; var++)// цикл проверки всех вариантов строки
         {
//			if(checkres == CALCERROR)
         memcpy(Maps,BitMap,MapLen);// копир. рисунок в буфер
         if(!CreateVariant(var, line, that)) continue; // создаем и копируем в матрицу вариант
         checkres = ChecksVariants();
         if(Semaphore) break;
         if(checkres == ALL_OK) break;
         else if(checkres == CALCERROR) continue;
         else if(checkres == NOLOGIK)
         	{
            NoLogVar = var;
            NoLogCount++;
            }
         }
      if(Semaphore) break;                         // при нахождении правильного варианта
      if(checkres == ALL_OK)
      	{memcpy(BitMap, Maps, MapLen); break;}
      if(NoLogCount == 1)
      	{                                         // если вариант с результатом NOLOGIK 1 то он верный
         if(!FindErrorVariants(NoLogVar, line, that)) continue;
         CreateVariant(NoLogVar, line, that);      // создаем и копируем вариант в оригинал
         int saveCC = CompleteCounter;
         for(i=0; i<linelen; i++)
            {
         	int x = (that ? (i*hSize+line) : (i+hSize*line));
         	if(BitMap[x]!=Original[i]) {BitMap[x]=Original[i]; Complete[CompleteCounter++] = x;}
            }
         byte *save = Maps;
         Maps = BitMap;
         if((checkres=ChecksVariants())==ALL_OK)
         	{Maps = save; break;}
         Maps = save;
			// исключаем из списка строки, которые полностью отгаданы
			for(i=0; i<size; i++)
   			{
	   	   bool that = (Sequence[i]<hSize ? true : false);
		      int line = (that ? Sequence[i] : Sequence[i]-hSize);
      		if(that) if(!Hstop[line]) Sequence[i] = size+1;
   	   	else 		if(!Vstop[line]) Sequence[i] = size+1;
		      }
         if(saveCC!=CompleteCounter)
         	SendMessage(hWnd, WM_COMMAND, IDC_WORKNOLOGIC, 0);	// посылаем сообщение окну
         }
      }

	delete[]mass;	// освобождаем память (ее операционке вечно нехватает)
   delete[]Sequence;
   DELETE(Maps);
   if(Semaphore) return USERSTOPED;
   return checkres;
}

bool CALCULATE::FindErrorVariants(UINT64 NoLogVar, int line, bool that)
{
   VAR_SAVE *tmp, *prev;
   if (!vs)
   	{
      vs = new VAR_SAVE (NoLogVar, line, that);
      return true;
      }
	tmp = vs;
   do	{
      if(NoLogVar==tmp->NoLogVar && line==tmp->line && that==tmp->that) return false;
      prev=tmp;
   	} while((bool)(tmp=tmp->next));
   prev->next = new VAR_SAVE (NoLogVar, line, that);
   return true;
}

bool CALCULATE::CheckFull(void)
{
  	int LineLen = That ? vSize : hSize;			// длина строки
   for(int i = 0; i<LineLen; i++)						// копируем строку в буфер (для простоты)
   	if(Maps[That ? (i*hSize+Number) : (i+hSize*Number)]==ZEROS) return true;
   if(That) Hstop[Number] = false;
   else     Vstop[Number] = false;
   return false;
}

calcres CALCULATE::ChecksVariants()
{
	if(!MakeStartPosition()) return OVERFLOW; // определяем стартовые и конечные позиции элементов
   for(int i=0; i<hSize; i++)	Hstop[i] = true;	// сброс отгаданых строк
   for(int i=0; i<vSize; i++)	Vstop[i] = true;
   calcerror = false;
   do { // головной цикл прогона всех строк
      TotalCicle++;	// наращиваем счетчик прогонов (для протокола)
      Res = false;
      for(Number=0; Number<hSize; Number++)
     		{
         That = true;
	      if(Hstop[Number] && CheckFull()) Res |= NewScanLine(true);
         if(calcerror)	break;
         }
     	if(calcerror)	break;
		for(Number=0; Number<vSize; Number++)
      	{
         That = false;
         if(Vstop[Number] && CheckFull()) Res |= NewScanLine(false);
         if(calcerror) break;
         }
     	if(calcerror)	break;
      if(Semaphore) return USERSTOPED; // Выход, по требованию
      } while (Res);
  	if(calcerror) return CALCERROR;
	if(CheckComplete()) return ALL_OK;
   return NOLOGIK;
}

bool CALCULATE::CreateVariant(UINT64 number, int line, bool that)
{	// Эта процедура прямое следствие предыдущей, только с обратным результатом.
	// Создается вариант строки в буфере на основании заданного значения.
  	LineLen = that ? vSize : hSize;			// длина строки
   for(int i = 0; i<LineLen; i++)				// подготавливаем буфер
   	Original[i] = MARKER;

	byte *pointer = that ? hData->Digits+line*hNumbSize : vData->Digits+line*vNumbSize;

	int cells = that ? hNumbSize : vNumbSize;	// находим первый ненулевой блок
	while (cells) if(pointer[cells-1]) break; // копируем оцифровку в буфер Block
   				  else cells--;
	int NumbEl=cells;    							// колич. элем.
   int FreePlace = cells-1;                  // подсчитываем количество свободного места
	for (int i=0; i<NumbEl; i++)						// копируем текущую оцифровку в буфер
		{Block[i] = pointer[--cells]; FreePlace+=Block[i];}
   FreePlace = LineLen - FreePlace;
   int *ElPos = Pos;

// LineLen	 - длина строки
// Block		 - копия оцифровки строки
// NumbEl	 - колич. цифр оцифровки
// FreePlace - количество свободного места
// Original  - буфер получения варианта
// ElPos     - рабочий буфер позиций

   for(int i=0; i<NumbEl; i++) ElPos[i] = 0;
   if(NumbEl>1)
      {
      int numbelem = NumbEl-1;
		UINT64 *mass = new UINT64[numbelem];	// массив хранения промежуточных результатов
   	int i,m;
  		for(i=0; i<numbelem; i++)
   		mass[i]=(UINT64)(i+1);			// заполняем массив начальными данными
   	for(i=1; i<FreePlace; i++)
   		{
      	(*mass)++;
   		for(m=1; m<numbelem; m++)
      		mass[m] += mass[m-1]+1;
      	}
      int digit = numbelem-1;
      while(1)      							// считаем колич. перемещений для каждого элем.
         {
      	if(number >= mass[digit]+1) {number -= mass[digit]+1; ElPos[numbelem-digit-1]++;}
         else if(--digit < 0) break;	// вых. если скатились в самый низ.
         else continue;

      	for(i=numbelem-1; i>0; i--) mass[i] -= mass[i-1]+1;// ограничиваем амплитуду
         (*mass)--;
         }
      ElPos[numbelem] = (int) number;
      int akkum = ElPos[0];                       // сдвигаем позиции согласно предыдущ.
      for(int i=1; i<NumbEl; i++) {akkum+=Block[i-1]+ElPos[i]+1; ElPos[i] = akkum;}
      delete[] mass;
      }
   else ElPos[0] = (int) number;

// строим в буфере вариант
   for(int i=0; i<NumbEl; i++)
   	for(int cells=ElPos[i]; cells<ElPos[i]+Block[i]; cells++) Original[cells]=BLACK;

// копируем с проверкой вариант в матрицу
   for(int i=0; i<LineLen; i++)
   	{
      int x = (that ? (i*hSize+line) : (i+hSize*line));
      if((Maps[x]==BLACK && Original[i]==MARKER) || (Maps[x]==MARKER && Original[i]==BLACK))
      	return false;
      else Maps[x]=Original[i];
      }
   return true;
}
*/

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////Далее идут функции работы с цветными рисунками/////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool CALCULATE::ScanColorLine(bool that)
{
// Новая процедура сканирования строк: вначале обновление стартовых позиций
// элементов методом последовательной подстановки и проверки на верность.
// Далее выполняется тоже самое со стоповыми позициями.
// Следующий этап - сканирование всех ячеек строки. При нахождении заполненной,
// определяется ее длина и происходит поиск элементов, которые могут ее перекрыть
// (по длине и местоположению), если оных всего один, то обновляются стартовые
// и стоповые индексы этого элемента, а затем и всех остальных.
// Затем для каждого элемента отмечаются ячейки, которые могут быть заполнены
// только этим элементом. Проверяем эти ячейки на заполнение, при этом первая-же
// заполненая ячейка сравнивается со стоповой позицией и если она меньше то
// обновляется местоположением этой ячейки. Аналогично для последней заполненной
// ячейки диапазона действия этого элемента.
// Далее отмечаются все перекрытые ячейки, а затем создается пустая строка, заново
// прогоняются все варианты от стартовой до стоповой ползиции для всех элементов
// и при правильном варианте маркируются соответствующие элементы этой строки.
// Те ячейки, которые останутся неотмечеными маркируются в оригинале как пустые.

// Результат = false если не отмечено не одной ячейки(нет изменений).
// Входн. перем.: Number(глобальная) - номер строки или столбца, that - строка или столбец.

// Определяем указатели (для простоты)

   int volatile i, pos, cells, x;

  	int LineLen = that ? vSize : hSize;			// длина строки

   for(i = 0; i<LineLen; i++)						// копируем строку в буфер (для простоты)
   	Original[i] = Maps[that ? (i*hSize+Number) : (i+hSize*Number)];

	byte *pointer = that ? hData->Digits+Number*hNumbSize : vData->Digits+Number*vNumbSize;
   byte *cpointer = that ? hData->DigitColors+Number*hNumbSize : vData->DigitColors+Number*vNumbSize;

	cells = that ? hNumbSize : vNumbSize;	// находим первый ненулевой блок
	while (cells) if(pointer[cells-1]) break; // копируем оцифровку в буфер Block
   				  else cells--;
	BCount = cells;									// заполняем переменную количества блоков

	for (i=0; i<BCount; i++)						// копируем текущую оцифровку в буфер
      {
		Block[i] = pointer[--cells];
      CBlock[i] = cpointer[cells];
      }
// Original - копия строки
// LineLen	- длина строки
// Block		- копия оцифровки строки
// CBlock   - цвета оцифровки
// BCoun		- колич. цифр оцифровки

   int *StPos = CALCULATE::StPos;
   int *EnPos = CALCULATE::EnPos;
   StPos += that ? Number*hNumbSize : Number*vNumbSize+hSize*hNumbSize;
	EnPos += that ? Number*hNumbSize : Number*vNumbSize+hSize*hNumbSize;

   bool compl=true;
   int volatile StartCurrent = 0;
// определяем начальные стартовые позиции
   for(i=0; i<BCount; i++)					// счетчик элементов
      	{										// проверяем от стартовой до стоповой позиции
         int a;
         byte b, c;
         calcerror = true;
         for(pos=StartCurrent; pos<LineLen; pos++)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// a   = указ. начало + длина
                                    	// b   = значение первой ячейки за элементом
            a = pos+Block[i];  			// c   = значение первой ячейки перед элементом
            if(a>LineLen || pos<0) {calcerror = true; break;}
            b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            int d = 0;
            if((b != CBlock[i]) && (c != CBlock[i]))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
                  {
         			if(!CheckCell(cells, i)) cells = LineLen+1;
                  else if(Original[cells] == CBlock[i]) d++;
                  }
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{								// стартовые значения в массиве
               if(d == Block[i] && compl)
               	StPos[i] = EnPos[i] = pos;
               else compl = false;
               StartCurrent = pos + Block[i];
               if(i<BCount-1 && CBlock[i]==CBlock[i+1]) StartCurrent++;
					for(x=i; x<BCount; x++)	// счетчик оставшихся элем.
               	{
               	if(StPos[x] < pos) StPos[x] = pos;
                  else pos = StPos[x];
                  if(i<BCount-1)
                     {
               		pos += (int)Block[x];
                     if(CBlock[x]==CBlock[x+1]) pos++;
                     }
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// определяем начальные стоповые позиции
   StartCurrent = LineLen - Block[BCount-1];
   compl = true;
   for(i = BCount-1; i >= 0; i--)		// обратный счетчик элементов
      	{										// проверяем от стоповой до стартовой позиции
         calcerror = true;
         for(pos=StartCurrent; pos>=0; pos--)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// c   = значение первой ячейки перед элементом
                                    	// b   = значение первой ячейки за элементом
            int a = pos+(int)Block[i]; // a   = указ. начало + длина
            if(a>LineLen || pos<0) {calcerror = true; break;}
            byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            int d = 0;
            if((b != CBlock[i]) && (c != CBlock[i]))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
         			if(!CheckCell(cells, i)) cells = LineLen+1;
                  else if(Original[cells] == CBlock[i]) d++;
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{
               if(d == Block[i] && compl)
               	StPos[i] = EnPos[i] = pos;
               else compl = false;
               if(i>0){
               	StartCurrent = pos - Block[i-1];
               	if(CBlock[i]==CBlock[i-1]) StartCurrent--;
                  }
					for(x=i; x >= 0; x--)	// счетчик оставшихся элем.
               	{
               	if(EnPos[x] > pos) EnPos[x] = pos;
                  else pos = EnPos[x];
               	if(x>0){
               		pos -= (int)Block[x-1];
                     if(CBlock[x]==CBlock[x-1]) pos--;
                     }
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// небольшое добавление ограничивающее диапазон действия элемента от конца
// диапазона предыдущего - первой ячейкой своего цвета
	for(i=0; i<BCount; i++)
      {
      int a = i>0 ? EnPos[i-1]+Block[i-1] : 0;	// a - граница диапазона пред. элемента
   	for(pos=a; pos<LineLen; pos++)
      	if(Original[pos] == CBlock[i])
         	if(EnPos[i] > pos)
               { EnPos[i] = pos; break; }
      }

	for(i=BCount-1; i>=0; i--)
      {
      int a = LineLen-1;	// a - граница диапазона пред. элемента
      if(i<BCount-1)
      	{
         a=StPos[i+1];
         if(CBlock[i]==CBlock[i+1]) a--;
         }
   	for(pos=a; pos > 0; pos--)
      	if(Original[pos] == CBlock[i])
            {
            int f=0;
            if(i<BCount-1 && CBlock[i]==CBlock[i+1]) f++;
         	if(StPos[i]+Block[i]-f < pos)
               { StPos[i] = pos-Block[i]+f; break; }
            }
      }

// Следующий этап - сканирование всех ячеек строки. При нахождении заполненной,
// определяется ее длина и происходит поиск элементов, которые могут ее перекрыть
// (по длине и местоположению), если оных всего один, то обновляются стартовые
// и стоповые индексы этого элемента, а затем и всех остальных.
// Если же длина совпадает со всеми перекрывающими элементами, то маркируем ее
// границы.
// P.S. Параллельно вышеописаному данный блок при нахождении подходящего элемента
// методом подстановки ищет непротиворечащее оригиналу положение этого элемента,
// с включением этой ячейки (или ячеек).
// При нахождении он увеличивает счетчики соответствующие каждой заполняемой ячейки
// и увеличиват общий счетчик вариантов. В итоге если к концу проверки величина
// какого-либо из счетчиков ячеек совпадает с общим счетчиком значит однозначно
// заливаем эту ячейку.

   int NumbElem, Element, ElemLen=0;
   bool repeat;
   do	{
      repeat = false;
		for(i=0; i<LineLen; i++)      // счетчик ячеек
   		if(Original[i] != ZEROS && Original[i] != MARKER)	// если ячейка окрашена
      		{
            byte color = Original[i];
         	NumbElem = 0;
	         x = i;                  // определяем длину
   	      while(x<LineLen && Original[++x] == color);
      	   x -= i;                 // х = длина
// { здесь и далее отмечено то, что описано в P.S.
				int counter=0;		                                 		// обнуляем общий счетчик
				for(cells=0; cells<LineLen; cells++) Variants[cells] = 0;// обнуляем счетчики ячеек
// }
         	for(cells=0; cells<BCount; cells++)	// счетчик элементов
	         	{  // если элем. имеет тот-же цвет длинее, старт. раньше  и  кончается позже то он подходит
   	         if((CBlock[cells]==color) && (Block[cells] >= x) && (StPos[cells] <= i) && (EnPos[cells]+Block[cells] >= i+x))
      	      	{
         	      NumbElem++;       	// увеличиваем счетчик подходящих элементов
            	   Element = cells;  	// сохраняем номер элемента
               	if(NumbElem > 1 && ElemLen != Block[cells]) ElemLen = LineLen+1;
	               else ElemLen = Block[cells];
//                  if(skips) continue;
// {
                  int d = MIN(MIN(i, LineLen-Block[cells]),EnPos[cells]);
                  int e = MAX(MAX(i+x-Block[cells], 0), StPos[cells]);
                  for(int a = e; a <= d; a++)// проверяем положение
                  	{
                     bool r = false;
                     if((a > 0) && (Original[a-1]==color)) r = true;
                     if((a+Block[cells] < LineLen) && (Original[a+Block[cells]]==color)) r = true;
                     if(r) continue; 	// прокрутка цикла если сбоку закрашеная ячейка

                     for(int b=a; b<a+Block[cells]; b++)
                     	if(!CheckCell(b, cells)) {r = true; break;}
                     if(r) continue; 	// прокрутка цикла если внутри попалась ячейка неподходящего цвета
                     // Ok - увеличиваем счетчики
                     counter++;
                     for(int b=a; b<a+Block[cells]; b++)
                     	Variants[b]++;
                     }
// }
   	            }
      	      }
// {
//            if(!skips)
            	{	// сравниваем счетчики
            	if(!counter)
               	{calcerror = true; return false;}
            	for(cells=0; cells<LineLen; cells++)
            		if(Variants[cells] == counter) Original[cells] = color;
               }
// }
         	if(ElemLen == x)       // если длина и цвет совпадает со всеми элементами
         		{
               bool q=true;
               for(cells=0; cells<BCount; cells++) if(CBlock[cells]!=color) q=false;
               if(q){
	            	if(i>0) Original[i-1] = MARKER;// то маркируем границы
   	         	if((i+x)<LineLen)Original[i+x] = MARKER;
                  }
      	      }

         	if(NumbElem == 1)			// если элемент единственный
         		{                    // обновляем стартовые и стоповые позиции
	            if(StPos[Element] < (i+x-Block[Element])) {StPos[Element] = (i+x-Block[Element]); repeat = true;}
   	         if(EnPos[Element] > i) {EnPos[Element] = i; repeat = true;}
      	      // обновляем стартовые и стоповые позиции остальных элементов
         	   for(cells=Element-1;cells>=0;cells--)
               	if(EnPos[cells]+Block[cells]+(CBlock[cells]==CBlock[cells+1]?1:0)>EnPos[cells+1])
                  	{EnPos[cells]=EnPos[cells+1]-Block[cells]-(CBlock[cells]==CBlock[cells+1]?1:0); repeat = true;}
            	for(cells=Element+1;cells<BCount;cells++)
               	if(StPos[cells]<StPos[cells-1]+Block[cells-1]+(CBlock[cells]==CBlock[cells-1]?1:0))
                  	{StPos[cells]=StPos[cells-1]+Block[cells-1]+(CBlock[cells]==CBlock[cells-1]?1:0); repeat = true;}
	            }
            else if(!NumbElem)
            	{calcerror = true; return false;}
   	      i += x-1;
      	   }
      } while(repeat);

// Данный блок проверяет количество одинаковых по длине блоков и сканирует
// строку в поиске однозначно определеных блоков данной длины. Если количество
// блоков в оцифровке соответствует количеству в строке то присваиваются и
// отождествляются их стартовые и конечные индексы
   int LastElemLen = 0, LastColor=0;
	for(x=0; x<BCount; x++)
   	{
      NumbElem = 0;        				// подсчитываем количество одинаковых блоков в оцифровке
      if(LastElemLen == Block[x] || LastColor == CBlock[x]) continue;
      else {LastElemLen = Block[x]; LastColor = CBlock[x];}
      for(i=0; i<BCount; i++) if(Block[i] == Block[x] && CBlock[i] == CBlock[x]) NumbElem++;
      for(i=0; i<LineLen; i++)			// сканируем строку в поиска заполненых ячеек
      	if(Original[i] != ZEROS && Original[i] != MARKER)	// если ячейка окрашена
         	{
            byte color = Original[i];
         	int a = i;                 // определяем длину
            byte c = (byte)(i>0 ? Original[i-1] : MARKER);		// c = значение ячейки перед элем.
         	while(a<LineLen && Original[++a] == color);
            byte b = (byte)(a<LineLen ? Original[a] : MARKER);	// b = значение ячейки за элем.
         	a -= i;                 	// a = длина
            if((a==Block[x]) && (color==CBlock[x]) && (b!=ZEROS) && (c!=ZEROS)) NumbElem --;
            i += a-1;  // проверяем на совпадение длины и цвета
            }
      if(!NumbElem)							// Если количество совпадает
      	{
         pos = 0;			// pos = позиция в строке
         int a = 0;   	// a = позиция в оцифровке
         while(a<BCount && (Block[a]!=Block[x] || CBlock[a]!=CBlock[x])) a++;
         while(1)
         	{
            for(;pos<LineLen;pos++)
      			if(Original[pos] != ZEROS && Original[pos] != MARKER)	// если ячейка окрашена
         			{
                  byte color = Original[pos];
         			int d = pos;                 // определяем длину
            		byte c = (byte)(pos>0 ? Original[pos-1] : MARKER);		// c = значение ячейки перед элем.
         			while(d<LineLen && Original[++d] == color);
            		byte b = (byte)(d<LineLen ? Original[d] : MARKER);	// b = значение ячейки за элем.
         			d -= pos;                 	// a = длина
            		if((b!=ZEROS) && (c!=ZEROS) && (d==Block[x]) && (color==CBlock[x]))
                  	{
                     StPos[a] = EnPos[a] = pos;
                     do{a++;}while(a<BCount && (Block[a]!=Block[x] || CBlock[a]!=CBlock[x]));
                     if(a == BCount) break;
                     }
                  pos += d-1;
            		}
            if(a == BCount) break;
            }
         }
      }

// Теперь производится поиск элементов прижатых к маркеровке, но
// неизвесной длины и если она меньше наименьшего значения оцифровки, то элемент
// удлиняется.
   for(i=0; i<LineLen; i++)				// сканируем строку в поиска заполненых ячеек
    	if(Original[i] != ZEROS && Original[i] != MARKER)	// если ячейка окрашена
       	{
         byte color = Original[i];

   		int MinElemLen = LineLen+1;   // определяем длину минимального элемента
   		for(cells=0; cells<BCount; cells++)
         	if(CBlock[cells]==color && MinElemLen > Block[cells])
            	MinElemLen = Block[cells];
         if(MinElemLen > LineLen) {calcerror = true; return false;}

        	int a = i;                 	// определяем длину
         byte c = (byte)(i>0 ? Original[i-1] : MARKER);		// c = значение ячейки перед элем.
        	while(a<LineLen && Original[++a] == color);
         byte b = (byte)(a<LineLen ? Original[a] : MARKER);	// b = значение ячейки за элем.
        	a -= i;                 		// a = длина
         if((b!=ZEROS && c==ZEROS) || (c!=ZEROS && b==ZEROS) && (a < MinElemLen))
         	{
            if(b!=ZEROS) x=i+a-MinElemLen;
            else            x=i;
            for(pos = 0; pos<MinElemLen; pos++) if(pos+x < LineLen) Original[pos+x] = color;
            												else {calcerror = true; return false;}
            }
         i += a-1;
         }

// Теперь обновляем старт - стоп позиции методом подстановки
   StartCurrent = StPos[0];
// определяем начальные стартовые позиции
   for(i=0; i<BCount; i++)					// счетчик элементов
      	{										// проверяем от стартовой до стоповой позиции
         calcerror = true;
         for(pos=StartCurrent; pos<LineLen; pos++)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// a   = указ. начало + длина
                                    	// b   = значение первой ячейки за элементом
            int a = pos+Block[i];  		// c   = значение первой ячейки перед элементом
            byte b = (byte)((a < LineLen) ? Original[a] : MARKER);
            byte c = (byte)((pos > 0) ? Original[pos-1] : MARKER);
            if((b != CBlock[i]) && (c != CBlock[i]))
            	{
               for(cells=pos; cells<a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
         			if(!CheckCell(cells, i)) cells = LineLen+1;
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{								// стартовые значения в массиве
               StartCurrent = pos + Block[i];
               if(i<BCount-1 && CBlock[i]==CBlock[i+1]) StartCurrent++;
					for(x=i; x<BCount; x++)	// счетчик оставшихся элем.
               	{
               	if(StPos[x] < pos) StPos[x] = pos;
                  else pos = StPos[x];
                  if(i<BCount-1)
                     {
               		pos += (int)Block[x];
                     if(CBlock[x]==CBlock[x+1]) pos++;
                     }
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// определяем начальные стоповые позиции
   StartCurrent = EnPos[BCount-1];
   for(i = BCount-1; i >= 0; i--)		// обратный счетчик элементов
      	{										// проверяем от стоповой до стартовой позиции
         calcerror = true;
         for(pos=StartCurrent; pos>=0; pos--)
				{                       	// i	 = номер элемента
            									// pos = указ. на начало
                                    	// c   = значение первой ячейки перед элементом
                                    	// b   = значение первой ячейки за элементом
            int a = pos+(int)Block[i]; // a   = указ. начало + длина
            byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
            byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
            if((b != CBlock[i]) && (c != CBlock[i]))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                   		// вынуждаем цыкл окончится, если внутри маркер
         			if(Original[cells] == MARKER) cells = LineLen+1;
               }
            else cells = LineLen+1;

            if(cells <= LineLen)			// если вариант подходящий то обновляем
            	{
               if(i>0){
               	StartCurrent = pos - Block[i-1];
               	if(CBlock[i]==CBlock[i-1]) StartCurrent--;
                  }
					for(x=i; x >= 0; x--)	// счетчик оставшихся элем.
               	{
               	if(EnPos[x] > pos) EnPos[x] = pos;
                  else pos = EnPos[x];
               	if(x>0){
               		pos -= (int)Block[x-1];
                     if(CBlock[x]==CBlock[x-1]) pos--;
                     }
                  }
               pos = LineLen+1;
               calcerror = false;
               break;	// вынуждаем цыкл окончится
               }
            }
         if(calcerror) return false;
         }

// Теперь для каждого элемента отмечаются ячейки, которые могут быть заполнены
// только этим элементом. Проверяем эти ячейки на заполнение, при этом первая-же
// заполненая ячейка сравнивается со стоповой позицией и если она меньше то
// обновляется местоположением этой ячейки. Аналогично для последней заполненной
// ячейки диапазона действия этого элемента.

	int StartPrivate, EndPrivate;	// индексы начала и конца "частной зоны"
   for(i=0; i<BCount; i++)       // счетчик элементов
   	{  	                    	// StartPrivate = конечная позиция предыдущ. элем.
      int f=0;
      if(i<BCount-1 && CBlock[i]==CBlock[i+1]) f++;
     	StartPrivate = ((i>0) ? (EnPos[i-1] + Block[i-1]) : StPos[0]);
      EndPrivate   = ((i<(BCount-1)) ? StPos[i+1]-f : (EnPos[i]+Block[i]-1));
      	                       	// EndPrivate   = стартовая позиция след. ячейки
      for(x=StartPrivate; x<=EndPrivate; x++)// Проверяем ячейки на заполнение
         {
         if(Original[x] == CBlock[i])
            {
            if(EnPos[i] > x) EnPos[i] = x;
            x = LineLen+1;
            break;
            }
         }
      for(x=EndPrivate; x>=StartPrivate; x--)// Проверяем ячейки на заполнение
         {
         if(Original[x] == CBlock[i])
            {
            if(StPos[i] < (x-Block[i])) StPos[i] = x-Block[i];
            x = LineLen+1;
            break;
            }
         }
      }

// Теперь обобщим полученные данные прогоном от стартовой до стоповой позиции и
// если подходящий вариант один однозначно определим старт и стоп индексы
	for(i=0; i<BCount; i++)
		{
      Element = ElemLen = 0;
   	for(pos = StPos[i]; pos <=EnPos[i]; pos++)
			{                       	// i	 = номер элемента
           									// pos = указ. на начало
                                   	// a   = указ. начало + длина
                                   	// b   = значение первой ячейки за элементом
         int a = pos+Block[i];  		// c   = значение первой ячейки перед элементом
         byte b = (byte)((a < LineLen) ? Original[a] : ZEROS);
         byte c = (byte)((pos > 0) ? Original[pos-1] : ZEROS);
         if((b!=CBlock[i]) && (c!=CBlock[i]))
         	{
            for(cells = pos; cells < a; cells++) // от старта до конца
                                		// вынуждаем цыкл окончится, если внутри маркер
        			if(!CheckCell(cells, i)) cells = LineLen+1;
            }
         else cells = LineLen+1;

         if(cells <= LineLen)			//
           	{								//
            Element = pos;
            ElemLen++;
            }
         }
      if(ElemLen == 1) StPos[i] = EnPos[i] = Element;
      else if(!ElemLen) {calcerror=true; return false;}
      }

// Далее отмечаются все перекрытые ячейки, а затем создается пустая строка, заново
// прогоняются все варианты от стартовой до стоповой ползиции для всех элементов
// и при правильном варианте маркируются соответствующие элементы этой строки.
// Те ячейки, которые останутся неотмечеными маркируются в оригинале как пустые.

	for(i=0; i<BCount; i++)       // отмечаем перекрытые ячейки
   	for(x=EnPos[i]; x<(StPos[i]+Block[i]); x++)
      	Original[x] = CBlock[i];
                                 // обнуляем буфер (пустая строка)
   for(i=0; i<LineLen; i++) Buffer[i] = ZEROS;
   for(i=0; i<BCount; i++)			// счетчик элементов
   	for(pos=StPos[i]; pos<=EnPos[i]; pos++)	// счетчик доступных позиций
            {
            int a = pos+(int)Block[i]; // a   = указ. начало + длина
            byte b = (byte)((a < LineLen) ? Original[a] : MARKER);
            byte c = (byte)((pos > 0) ? Original[pos-1] : MARKER);
            if((b!=CBlock[i]) && (c!=CBlock[i]))
            	{
               for(cells = pos; cells < a; cells++) // от старта до конца
                                 // вынуждаем цыкл окончится, если внутри маркер
         			if(!CheckCell(cells, i)) cells = LineLen+1;
               }
            else cells = LineLen+1;
            if(cells<=LineLen)   // если позиция непротиворечащая - заливаем ячейки строки
            	for(cells = pos; cells < a; cells++) Buffer[cells] = CBlock[i];
            }                    // маркируем пустые ячейки в строке оригинала
	for(i=0; i<LineLen; i++) if(Buffer[i] == ZEROS) Original[i] = MARKER;
   bool res = false;             // копируем полученную строку назад в матрицу
   pos = 0;
   for(i=0; i<LineLen; i++)
      {
      if(Original[i] != ZEROS) pos ++;
      x = that ? (i*hSize+Number) : (i+hSize*Number);
   	if((Maps[x]) != Original[i]){
         if(that) Vstop[i]=true;
         else		Hstop[i]=true;
         Maps[x] = Original[i];
         if(Maps==BitMap) Complete[CompleteCounter++] = x;
         res = true;
         }
      }                          // сбрасываем флажки для дальнейшего игнорирования
   if(!res || pos == LineLen)
   	if(that) Hstop[Number] = false;	// отгаданых строк
      else     Vstop[Number] = false;
	return res;
}

calcres CALCULATE::MakeAccessColor(void)
{// чл-ф определения доступных цветов ячейки
   int ptr, elem, i, c;
   ZeroMemory(AccessColor, MapLen*CCount);

// обрабатываем столбцы
   for(int row = 0; row < hSize; row++)
   	{
      That = true;
      Number = row;
      c=hNumbSize;
      while (c) if(hData->Digits[(c-1)+row*hNumbSize]) break; // копируем оцифровку в буфер Block
   				  else c--;
		BCount = c;									// заполняем переменную количества блоков
		for (i=0; i<BCount; i++)						// копируем текущую оцифровку в буфер
         {
         Block[i] = (int)hData->Digits[(--c)+row*hNumbSize];
			CBlock[i] = hData->DigitColors[c+row*hNumbSize];
         }

// Подсчитываем количество свободного места в столбце
      int accessVoid = Block[0];
      for(i=1; i<BCount; i++)
			{
         accessVoid += Block[i];
         if(CBlock[i]==CBlock[i-1]) accessVoid++;
         }
      accessVoid = vSize - accessVoid;

// определяем стартовые и конечные позиции столбцов
      ptr=0;
      for(elem = 0; elem < BCount; elem++)
         {
         if(((bool)elem) && CBlock[elem]==CBlock[elem-1]) ptr++;
         StPos[row*hNumbSize+elem]=ptr;
         ptr+=Block[elem];
         if(ptr>vSize) return OVERFLOW;
         }
      ptr = vSize;
      for(elem = BCount-1; elem >=0; elem--)
         {
         if((elem<(BCount-1)) && CBlock[elem]==CBlock[elem+1]) ptr--;
         ptr-=Block[elem];
         if(ptr>=0)EnPos[row*hNumbSize+elem]=ptr;
         else return OVERFLOW;
         }

// Прогоняем подстановку от начала до конца строки
      for(int var = 0; var <= accessVoid; var++)
      	{
         ZeroMemory((LPVOID)Variants, sizeof(int)*vSize);
         ptr = var;
         for(elem = 0; elem < BCount; elem++)// создаем вариант в буфере
            {
            if(((bool)elem) && CBlock[elem]==CBlock[elem-1]) ptr++;
         	for(int elemlen=0; elemlen<Block[elem]; elemlen++)
            	Variants[ptr++]=(int)CBlock[elem];
            }
         for(int cells=0; cells<vSize; cells++)// опр. цвета ячеек
         	if(Variants[cells])
            	{	// указатель на строку доступных цветов
               byte *cptr = AccessColor+(cells*hSize+row)*CCount;
               for(i=0; i<CCount; i++)
                  {// если цвет уже доступен
               	if((int)cptr[i]==Variants[cells]) break;
                  else if(!cptr[i])
                  	{
                     cptr[i] = (byte)Variants[cells];
                     break;
                     }
                  }
               }
         }
      }

// обрабатываем строки
   byte *AccClr = new byte[hSize*CCount];
   for(int line = 0; line < vSize; line++)
   	{
      That = false;
      Number = line;
      c=vNumbSize;
      ZeroMemory((LPVOID)AccClr,hSize*CCount);
      while (c) if(vData->Digits[(c-1)+line*vNumbSize]) break; // копируем оцифровку в буфер Block
   				  else c--;
		BCount = c;											// заполняем переменную количества блоков
		for (i=0; i<BCount; i++)						// копируем текущую оцифровку в буфер
         {
         --c;
         Block[i] = (int)vData->Digits[c+line*vNumbSize];
			CBlock[i] = vData->DigitColors[c+line*vNumbSize];
         }

// Подсчитываем количество свободного места в строке
      int accessVoid = Block[0];
      for(i=1; i<BCount; i++)
			{
         accessVoid += Block[i];
         if(CBlock[i]==CBlock[i-1]) accessVoid++;
         }
      accessVoid = hSize - accessVoid;

// определяем стартовые и конечные позиции строк
      ptr=0;
      int offset = hSize*hNumbSize;
      for(elem = 0; elem < BCount; elem++)
         {
         if(((bool)elem) && CBlock[elem]==CBlock[elem-1]) ptr++;
         StPos[line*vNumbSize+offset+elem]=ptr;
         ptr+=Block[elem];
         if(ptr>hSize) return OVERFLOW;
         }
      ptr = hSize;
      for(elem = BCount-1; elem >=0; elem--)
         {
         if((elem<(BCount-1)) && CBlock[elem]==CBlock[elem+1]) ptr--;
         ptr-=Block[elem];
         if(ptr>=0)EnPos[line*vNumbSize+offset+elem]=ptr;
         else return OVERFLOW;
         }

// Прогоняем подстановку от начала до конца строки
      for(int var = 0; var <= accessVoid; var++)
      	{
         ZeroMemory((LPVOID)Variants, sizeof(int)*hSize);
         ptr = var;
         for(elem = 0; elem < BCount; elem++)// создаем вариант в буфере
            {
            if(((bool)elem) && CBlock[elem]==CBlock[elem-1]) ptr++;
         	for(int elemlen=0; elemlen<Block[elem]; elemlen++)
            	Variants[ptr++]=(int)CBlock[elem];
            }
         for(int cells=0; cells<hSize; cells++)// опр. цвета ячеек
         	if(Variants[cells])
            	{	// указатель на строку доступных цветов
               byte *cptr = AccClr+cells*CCount;
               for(i=0; i<CCount; i++)
                  {// если цвет уже доступен
               	if((int)cptr[i]==Variants[cells]) break;
                  else if(!cptr[i]) {cptr[i] = (byte)Variants[cells]; break;}
                  }
               }
         }

// исключаем недоступные при "скрещивании" цвета
      for(int cells=0; cells<hSize; cells++)
      	{
      	byte *orig = AccessColor+(line*hSize+cells)*CCount;
         byte *test = AccClr+cells*CCount;
         int ocount = -1; while(orig[++ocount]);
         int tcount = -1; while(test[++tcount]);
         for(i=0; i<ocount; i++)
         	{
            bool r=false;
         	for(c=0; c<tcount; c++)
            	if(orig[i]==test[c]) {r=true; break;}
            if(!r) orig[i]=NULL;
            }
         test = orig;
         for(i=0; i<CCount; i++) // уплотняем цвета
         	if(*test) *(orig++)=*(test++);
            else test++;
         while(orig<test) *(orig++)=NULL;
         }
      }
   delete[] AccClr;
   return ALL_OK;
}

bool CALCULATE::CheckColorCell(int adr, byte color)
{
	byte *ac = AccessColor+adr*CCount;
   if(Maps[adr]!=ZEROS && Maps[adr]!=color) return false;
   for(int i=0; i<CCount; i++)if(ac[i]==color) return true;
   return false;
}

bool CALCULATE::CheckCell (int adr, int clrnumb)
{
	byte *ac = AccessColor+((That ? Number+hSize*adr : Number*hSize+adr)*CCount);
   if(Original[adr]!=ZEROS && Original[adr]!=CBlock[clrnumb]) return false;
   for(int i=0; i<CCount; i++)if(ac[i]==CBlock[clrnumb]) return true;
   return false;
}

calcres CALCULATE::RecalcPlace(void)
{ // функция расчета стартовых и стоповых позиций цветного кроссворда
   int i, c, elem, pos, cells;
	bool a;
   for(int row = 0; row < hSize; row++)
   	{
      That = true;
      Number = row;
      c=hNumbSize;
      while (c) if(hData->Digits[(c-1)+row*hNumbSize]) break; // копируем оцифровку в буфер Block
   				  else c--;
		BCount = c;									// заполняем переменную количества блоков
		for(i=0; i<BCount; i++)					// копируем текущую оцифровку в буфер
         {                                // и ее цвет
         Block[i] = (int)hData->Digits[(--c)+row*hNumbSize];
			CBlock[i] = hData->DigitColors[c+row*hNumbSize];
         }
      for(i=0; i<vSize; i++)
      	Variants[i] = Maps[row+hSize*i];
// BCount - кол. цифр
// *Block - оцифровка
// CBlock - цвет оцифровки
// Variants - копия оригинала строки в матрице
// bool CheckAccessColor(int adr, byte color) - проверка цвета на доступность

// Сканируем стартовые позиции
      for(elem = 0; elem<BCount; elem++)// сч. элем.
         {
         for(pos=StPos[row*hNumbSize+elem]; pos<=EnPos[row*hNumbSize+elem]; pos++)
            {
            a=true;
         	for(cells = pos; cells < pos+Block[elem]; cells++)
            	if(!CheckColorCell(row+hSize*cells,CBlock[elem]))
               	{a=false; break;}
            if(a) {
            	if(StPos[row*hNumbSize+elem]<pos)
               	{
                  StPos[row*hNumbSize+elem] = pos;
               	for(int el=elem+1; el<BCount; el++)// обновляем поз. сл. эл-в
                     {
                     int off = CBlock[el-1]==CBlock[el] ?
                     			StPos[row*hNumbSize+el-1]+Block[el-1]+1 :
                              StPos[row*hNumbSize+el-1]+Block[el-1];
                  	StPos[row*hNumbSize+el] = MAX(StPos[row*hNumbSize+el], off);
                     }
                  if(StPos[row*hNumbSize+BCount-1]+Block[BCount-1]>vSize)
                  	return CALCERROR;
                  }
               break;
               }
            }
         if(!a) return CALCERROR;
         }
// Сканируем стоповые позиции
      for(elem = BCount-1; elem>=0; elem--)// сч. элем.
         {
         for(pos=EnPos[row*hNumbSize+elem]; pos>=StPos[row*hNumbSize+elem]; pos--)
            {
            a=true;
         	for(cells = pos; cells < pos+Block[elem]; cells++)
            	if(!CheckColorCell(row+hSize*cells,CBlock[elem]))
               	{a=false; break;}
            if(a) {
            	if(EnPos[row*hNumbSize+elem]>pos)
               	{
                  EnPos[row*hNumbSize+elem] = pos;
               	for(int el=elem-1; el>=0; el--)// обновляем поз. сл. эл-в
                     {
                     int off = CBlock[el+1]==CBlock[el] ?
                     			EnPos[row*hNumbSize+el+1]-Block[el]-1 :
                              EnPos[row*hNumbSize+el+1]-Block[el];
                  	EnPos[row*hNumbSize+el] = MIN(EnPos[row*hNumbSize+el], off);
                     }
                  if(EnPos[row*hNumbSize]<0)
                  	return CALCERROR;
                  }
					break;
               }
            }
         if(!a) return CALCERROR;
         }
      }

// Делаем тоже самое для строк
   for(int line = 0; line < vSize; line++)
   	{
      That = false;
      Number = line;
      c=vNumbSize;
      int offset = hSize*hNumbSize;
      while (c) if(vData->Digits[(c-1)+line*vNumbSize]) break; // копируем оцифровку в буфер Block
   				  else c--;
		BCount = c;									// заполняем переменную количества блоков
		for(i=0; i<BCount; i++)					// копируем текущую оцифровку в буфер
         {                                // и ее цвет
         --c;
         Block[i] = (int)vData->Digits[c+line*vNumbSize];
			CBlock[i] = vData->DigitColors[c+line*vNumbSize];
         }
      for(i=0; i<hSize; i++)
      	Variants[i] = Maps[line*hSize+i];

// Сканируем стартовые позиции
      for(elem = 0; elem<BCount; elem++)// сч. элем.
         {
         for(pos=StPos[line*vNumbSize+offset+elem]; pos<=EnPos[line*vNumbSize+offset+elem]; pos++)
            {
            a=true;
         	for(cells = pos; cells < pos+Block[elem]; cells++)
            	if(!CheckColorCell(line*hSize+cells,CBlock[elem]))
               	{a=false; break;}
            if(a) {
            	if(StPos[line*vNumbSize+offset+elem]<pos)
               	{
                  StPos[line*vNumbSize+offset+elem] = pos;
               	for(int el=elem+1; el<BCount; el++)// обновляем поз. сл. эл-в
                     {
                     int off = CBlock[el-1]==CBlock[el] ?
                     			StPos[line*vNumbSize+offset+el-1]+Block[el-1]+1 :
                              StPos[line*vNumbSize+offset+el-1]+Block[el-1];
                  	StPos[line*vNumbSize+offset+el] = MAX(StPos[line*vNumbSize+offset+el], off);
                     }
                  if(StPos[line*vNumbSize+offset+BCount-1]+Block[BCount-1]>hSize)
                  	return CALCERROR;
                  }
               break;
               }
            }
         if(!a) return CALCERROR;
         }
// Сканируем стоповые позиции
      for(elem = BCount-1; elem>=0; elem--)// сч. элем.
         {
         for(pos=EnPos[line*vNumbSize+offset+elem]; pos>=StPos[line*vNumbSize+offset+elem]; pos--)
            {
            a=true;
         	for(cells = pos; cells < pos+Block[elem]; cells++)
            	if(!CheckColorCell(line*hSize+cells,CBlock[elem]))
               	{a=false; break;}
            if(a) {
            	if(EnPos[line*vNumbSize+offset+elem]>pos)
               	{
                  EnPos[line*vNumbSize+offset+elem] = pos;
               	for(int el=elem-1; el>=0; el--)// обновляем поз. сл. эл-в
                     {
                     int off = CBlock[el+1]==CBlock[el] ?
                     			EnPos[line*vNumbSize+offset+el+1]-Block[el]-1 :
                              EnPos[line*vNumbSize+offset+el+1]-Block[el];
                  	EnPos[line*vNumbSize+offset+el] = MIN(EnPos[line*vNumbSize+offset+el], off);
                     }
                  if(EnPos[line*vNumbSize+offset]<0)
                  	return CALCERROR;
                  }
					break;
               }
            }
         if(!a) return CALCERROR;
         }
      }
	return ALL_OK;
}

bool CALCULATE::BeginAccessColor(void)
{// чл-ф определения доступных цветов ячейки после цикла обсчета
   int elem, i, c, cells;
   int *StPos, *EnPos;
   int SaveCount = 0;
  	for(i = 0; i<MapLen*CCount; i++) if(AccessColor[i]) SaveCount++;
   ZeroMemory(AccessColor, MapLen*CCount);

// обрабатываем столбцы
   for(int row = 0; row < hSize; row++)
   	{
      c=hNumbSize;
      while (c) if(hData->Digits[(c-1)+row*hNumbSize]) break; // копируем оцифровку в буфер Block
   				  else c--;
		BCount = c;									// заполняем переменную количества блоков
		for (i=0; i<BCount; i++)						// копируем текущую оцифровку в буфер
         {
         Block[i] = (int)hData->Digits[(--c)+row*hNumbSize];
			CBlock[i] = hData->DigitColors[c+row*hNumbSize];
         }
      StPos = CALCULATE::StPos + row*hNumbSize;
   	EnPos = CALCULATE::EnPos + row*hNumbSize;

// Прогоняем все элементы
      for(elem = 0; elem < BCount; elem++)
      	{  // от стартовой до финишной позиции
         for(cells=StPos[elem]; cells<EnPos[elem]+Block[elem]; cells++)
            {
	         byte *cptr = AccessColor+(cells*hSize+row)*CCount;
   	      for(i=0; i<CCount; i++)
      	   	{// если цвет уже доступен
         	   if((int)cptr[i]==CBlock[elem]) break;
            	else if(!cptr[i])
	            	{
   	            cptr[i] = CBlock[elem];
      	         break;
         	     }
            	}
            }
         }
      }

// обрабатываем строки
   byte *AccClr = new byte[hSize*CCount];
   for(int line = 0; line < vSize; line++)
   	{
      ZeroMemory((LPVOID)AccClr,hSize*CCount);
      c=vNumbSize;
      while (c) if(vData->Digits[(c-1)+line*vNumbSize]) break; // копируем оцифровку в буфер Block
   				  else c--;
		BCount = c;											// заполняем переменную количества блоков
		for (i=0; i<BCount; i++)						// копируем текущую оцифровку в буфер
         {
         --c;
         Block[i] = (int)vData->Digits[c+line*vNumbSize];
			CBlock[i] = vData->DigitColors[c+line*vNumbSize];
         }
   	StPos = CALCULATE::StPos + line*vNumbSize+hSize*hNumbSize;
		EnPos = CALCULATE::EnPos + line*vNumbSize+hSize*hNumbSize;

// Прогоняем все элементы
      for(elem = 0; elem < BCount; elem++)
      	{  // от стартовой до финишной позиции
         for(cells=StPos[elem]; cells<EnPos[elem]+Block[elem]; cells++)
            {
	         byte *cptr = AccClr+cells*CCount;
   	      for(i=0; i<CCount; i++)
      	   	{// если цвет уже доступен
         	   if((int)cptr[i]==CBlock[elem]) break;
            	else if(!cptr[i])
	            	{
   	            cptr[i] = CBlock[elem];
      	         break;
         	     }
            	}
            }
         }

// исключаем недоступные при "скрещивании" цвета
      for(int cells=0; cells<hSize; cells++)
      	{
      	byte *orig = AccessColor+(line*hSize+cells)*CCount;
         byte *test = AccClr+cells*CCount;
         int ocount = -1; while(orig[++ocount]);
         int tcount = -1; while(test[++tcount]);
         for(i=0; i<ocount; i++)
         	{
            bool r = false;
         	for(c=0; c<tcount; c++)
            	if(orig[i] == test[c]) {r=true; break;}
            if(!r){
            	orig[i]=NULL;
               Hstop[cells]=true;
               Vstop[line]=true;
               }
            }
         test = orig;
         for(i=0; i<CCount; i++) // уплотняем цвета
         	if(*test) *(orig++)=*(test++);
            else test++;
         while(orig<test) *(orig++)=NULL;
         }
      }
   delete[] AccClr;
  	for(i = 0; i<hSize*vSize*CCount; i++) if(AccessColor[i]) SaveCount--;
   return (bool)SaveCount;
}

void CALCULATE::SearchSequenceColor(void *sts)
{
	SAVE_CALC_STATE *scs = (SAVE_CALC_STATE *)sts;
	int *Seq = scs->Sequence;
   int *ptr = new int[vSize];
   int volatile i, x, HX;
	for(HX=0; HX<vSize; HX++){
      int Counter=0;
		for(i = 0; i<hSize; i++){
         if(Maps[HX*hSize+i]==ZEROS) Counter++;
			}
      ptr[HX]=Counter ? Counter : Overflow-1;
      }
   for(i=0; i<vSize; i++){                        // Построение последовательности строк
      int tmp=Overflow;
      for(x=0; x<vSize; x++)
      	if(ptr[x]<tmp){
				tmp=ptr[x];
            Seq[i]=x;
            }
      ptr[Seq[i]]=Overflow;
      }
   delete[] ptr;
}

calcres CALCULATE::FindDecisionColor(void *sts)
{
   Recurce++;
   bool res;
	int X, Y;
   if(!sts) sts = (void *)new SAVE_CALC_STATE(this);
	SAVE_CALC_STATE *scs = (SAVE_CALC_STATE *)sts;
   scs->RecurceSkip=0;
	SearchSequenceColor(scs);
   for(scs->LinePtr=0; scs->LinePtr<vSize; scs->LinePtr++){// Цикл строк
   	scs->Line = scs->Sequence[scs->LinePtr];
      for(scs->Elem=0; scs->Elem<hSize; scs->Elem++){		// Цикл ячеек
        	scs->pos = scs->Line*hSize+scs->Elem;
         if(Maps[scs->pos]!=ZEROS) continue;
      	scs->orig = AccessColor+scs->pos*CCount;
         scs->ocount = -1; while(scs->orig[++scs->ocount]);
         scs->ErrColCount=0;
			for(scs->ColCount=0; scs->ColCount<scs->ocount; scs->ColCount++){
	         Maps[scs->pos]=scs->orig[scs->ColCount];
   	      Hstop[scs->Elem]=true;
      	   Vstop[scs->Line]=true;

  				res = false;
	         Number=scs->Elem;
				res |= ScanColorLine(That = true);
      	   Number=scs->Line;
	   		res |= ScanColorLine(That = false);
				if(res)
		  			do { // цикл прогона всех строк
   		  			res = false;
                  BeginAccessColor();
						X=0; Y=0;
  						while(X<hSize || Y<vSize){
            		   calcerror = false;
  							if(Hstop[Number=X] && X<hSize) res |= ScanColorLine(That = true);
		      			if(calcerror)	break;
   			   		if(Vstop[Number=Y] && Y<vSize) res |= ScanColorLine(That = false);
  	   					if(calcerror) break;
         		      X++; Y++;
							}
	   				if(Semaphore) {
   	         		scs->RestoreData();
	      	        	delete scs;
   	      	      Recurce--;
      	      	   return USERSTOPED;} // Выход, по требованию
	  	   			} while (res);
//	      memcpy(BitMap, Maps, MapLen);
//   	   RedrawWindow(hWnd, &Fild, NULL, RDW_INVALIDATE);
			   if(calcerror){
					scs->ErrColCount++;
               scs->RestoreData();
   	         }
				else if(CheckCompleteColor()) {
         	  	delete (SAVE_CALC_STATE *)scs;
      	      Recurce--;
   	         return ALL_OK; // если анализ окончен
	            }
  				else {
      	      int volatile Acount=0, Bcount=0;
   	         for(int i=0; i<MapLen; i++)
	            	if(Maps[i]==ZEROS) Acount++;
               	else if(Maps[i]!=scs->SaveMaps[i]) Bcount++;
            	if(Acount<DeepRecurce){
						calcres rfd = FindDecisionColor();
         	   	scs = (SAVE_CALC_STATE *)sts;
      	  			if(rfd==ALL_OK){
   	            	delete scs;
	                  Recurce--;
                  	return rfd;}
               	else if(rfd==USERSTOPED){
            	      scs->RestoreData();
         	      	delete scs;
      	            Recurce--;
   	               return rfd;}
	               else if(rfd==CALCERROR){
                     scs->ErrColCount++;
                     }
      	         }
					else scs->RecurceSkip++;
               scs->RestoreData();
					}
            }
			if(scs->ErrColCount==scs->ocount){
           	scs->SaveMaps[scs->pos]=MARKER;
         	scs->SaveHstop[scs->Elem]=true;
      	  	scs->SaveVstop[scs->Line]=true;
				scs->RestoreData();
	     		res = false;
           	Number=scs->pos%hSize;
  				res |= ScanColorLine(That = true);
      	     Number=scs->pos/hSize;
   		  	res |= ScanColorLine(That = false);
	         if(res) {
	      	  	scs->SaveAttr();
   	        	scs->SaveMap();
      	     	}
            }
         }
      }
   delete scs;
	Recurce--;
   if(scs->RecurceSkip)
   	return NOLOGIK;
   return CALCERROR;
}

bool CALCULATE::CheckCompleteColor()
{
	int Count[MAXCOLOR];
   memset(Count, 0, MAXCOLOR*sizeof(int));
   int i;
	for(i=0; i<hSize*hNumbSize; i++)
      if(hData->Digits[i])
   		Count[hData->DigitColors[i]]+=hData->Digits[i];
   for(int i=0; i<MapLen; i++){
   	if(Maps[i]==ZEROS) return false;
      else if(Maps[i]==MARKER) continue;
      else {byte t=Maps[i]; Count[t]--;}
      }
   for(i=0; i<MAXCOLOR; i++)
   	if(Count[i]) return false;
   return true;
}

calcres CALCULATE::CalcColorLogikMain()
{ // Управляющая программа для цветного кроссворда.
   bool cleared = false;
   Maps = BitMap;
   NoClassics = false;
   VarTotal = 0;
	CCount = KROSSFEATURES::Colors;
	VarTotal = SetPriority(true);
begin:
   CompleteCounter = 0;	// сбрасываем счетчик отгаданных ячеек
	// заполняем таблицу начальных позиций элементов и определяем цвета, доступные ячейкам
   if(MakeAccessColor()==OVERFLOW) return OVERFLOW;
	TotalCicle = Count = 0;	// обнуление счетчиков строк и прогонов
   for(int i=0; i<hSize; i++)	Hstop[i] = true;	// сброс отгаданых строк
   for(int i=0; i<vSize; i++)	Vstop[i] = true;

   calcerror = (RecalcPlace()==CALCERROR);
   do { // головной цикл прогона всех строк
      if(calcerror) break;
      Res = false;
      BeginAccessColor();
		int X=0, Y=0;
      while(X<hSize || Y<vSize){
      	if(X<hSize && Hstop[Number=X]) Res |= ScanColorLine(That = true);
         if(calcerror)	break;
         if(Y<vSize && Vstop[Number=Y]) Res |= ScanColorLine(That = false);
         if(calcerror) break;
         X++; Y++;
			}
     	if(calcerror)	break;
      if(Semaphore) return USERSTOPED; // Выход, по требованию
      if(Res) TotalCicle++;	// наращиваем счетчик прогонов (для протокола)
      } while (Res);
  	if(calcerror) if(!cleared && ClearPicturesFromCalc)
      {
      memset(BitMap, ZEROS, MapLen);
      InvalidateRect(hWnd, &Fild, false);
      cleared = true;
		goto begin;
      }
   else return CALCERROR;
   if(Semaphore) return USERSTOPED;
   else if(CheckCompleteColor())
   	return ALL_OK;
   NoClassics=true;
   SendMessage(hWnd, WM_COMMAND, IDC_WORKNOLOGIC, 0);	// посылаем сообщение окну, что
   calcres slv;                                       // подходящего варианта
   byte *SaveMatrix = new byte[MapLen];
   memcpy(SaveMatrix, BitMap, MapLen);
	STATUS("Идет поиск решения.");
   Maps=SaveMatrix;
   Recurce=0;
   DeepRecurce=150;
	while(1){
		slv=FindDecisionColor();
      if(slv==ALL_OK || slv==USERSTOPED) break;
      int coun=0;
      for(int i=0; i<MapLen; i++)
      	if(BitMap[i]!=Maps[i]) {coun++; BitMap[i]=Maps[i]; Complete[CompleteCounter++]=i;}
      if(!coun) DeepRecurce+=30;
      else SendMessage(hWnd, WM_COMMAND, IDC_WORKNOLOGIC, 0);	// посылаем сообщение окну, что
      if(DeepRecurce>=MapLen)
      	DeepRecurce=MapLen;
		}
	if(slv!=CALCERROR)
      for(int i=0; i<MapLen; i++)
      	if(BitMap[i]!=Maps[i]) {BitMap[i]=Maps[i]; Complete[CompleteCounter++]=i;}
   Maps=NULL;
   delete[] SaveMatrix;
	return slv;
}
