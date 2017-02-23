#include "dialog.h"
#include "palette.h"

class STATUSWND {
   HWND hWnd, hStatus;
   int Hight;
   RECT StatusRect;
   KROSSFEATURES *features;

public:
	HWND GetHandle(){return hStatus;}
   int GetStatusBoxHight(void) {return Hight;}
   void UpdateStatus(void) {UpdateWindow(hStatus);}
   void SetText(int Part, char *Text)
     {
     if(hStatus) SendMessage(hStatus,SB_SETTEXT,(WPARAM)Part,(LPARAM)(LPSTR)Text);
     }
   void Sizing(WPARAM wParam, LPARAM lParam);
   STATUSWND(KROSSFEATURES *feat);
   ~STATUSWND(){
     SendMessage(hStatus, WM_QUIT, NULL, NULL);
     DestroyWindow(hStatus);
     }
   };

class WINMAIN : public KROSSFEATURES, public DIALOGS, public PALETTE
{
	virtual void StopThread()=0;
   MSG  Msg;
   static WINMAIN *This;

   bool init, startk, CreateKross;
	UINT Id[2];

   void Create   (int CmdShow);
   bool RegisterClass (void);
   LRESULT Close (void);

// Члены вызываемой процедкры главного окна
   static LRESULT CALLBACK WndProc(HWND HWnd,UINT Msg,WPARAM WParam,LPARAM LParam);
   LRESULT WProc(HWND HWnd,UINT Msg,WPARAM WParam,LPARAM LParam);

   int Count, Speed,x,y,StartX,StartY;
   WORD MouseFlag;
   bool RButton, LButton;
   UINT Timer;
   int WaitForNextDemo;

	static TBBUTTON button[NUMBBUTTON];

	HWND CreateToolbar (int CmdShow, bool size = false);
   void DestroyToolBar();
   LRESULT ToolBarNotify(LPARAM arg);

	void WINAPI CInitMenu(HMENU hmenu);
	void PasteFromClipboard(void);
	bool CopyToClipboard(void);

                                 //Члены демо режима
   bool BrowsFolders (char *);	//Диалог выбора папки
	LRESULT OpenDemoFolder();
   char *FolderPath;
	HANDLE hDemoFile;
   bool StartDemo();
	void EndDemo();
   bool NextDemo (void);

protected:
   STATUSWND *StatusBox;        //Указатель на объект строки статуса

	virtual void CreateKrossword()      = 0;
   virtual bool Save (bool Quest=true, bool SaveAs=false) = 0;
   virtual void EndCalc(bool)     = 0;
   virtual void CommandProcessing (UINT,WPARAM,LPARAM) = 0;
   virtual void PrepareUndo(void) =0;
   virtual void Undo_(bool) =0;
   virtual void PasteBitmap(byte *) =0;
   void MessageProc();
   void StatusCheck  (void);
   virtual void WindowSizing() = 0;

public:
	WINMAIN(HINSTANCE hInstance, LPSTR lpCmLn, int CmdShow);
};
