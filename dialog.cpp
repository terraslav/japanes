#include "dialog.h"

DIALOGS *DIALOGS::This;

// обратно вызываемая функция информационного диалогового окна
bool CALLBACK DIALOGS::DialogProc_info (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	return This->DPI(hwndDlg,Msg,wParam,lParam);
}

bool DIALOGS::DPI (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM)
{
   static HWND hOldFocusWnd;
      switch(Msg)
        {
        case WM_INITDIALOG:
           hOldFocusWnd=SetFocus((HWND) wParam);// установка фокуса
           return FALSE;

        case WM_COMMAND:
           switch(LOWORD(wParam))
               {
               case MY_CREATER:
               case IDCANCEL:
               	SetFocus(hOldFocusWnd);
                  EndDialog(hwndDlg,IDOK);       // восстановление фокуса
               	return TRUE;

               case IDD_WEBADDR:                           //asgrem@bk.ru?subject=Program Matrix
	               ShellExecute(hwndDlg, "open", "http:\/\/my.mail.ru\/bk\/asgrem\/" ,NULL,NULL,SW_MAXIMIZE);
						SetFocus(hOldFocusWnd);
                  EndDialog(hwndDlg,IDOK);
                  return true;
               case IDD_MAILTO:
	               ShellExecute(hwndDlg, "open", "MAILTO:asgrem@bk.ru?subject=Program Matrix" ,NULL,NULL,SW_MAXIMIZE);
						SetFocus(hOldFocusWnd);
                  EndDialog(hwndDlg,IDOK);
               	return true;

               case WM_CTLCOLORDLG:
               case WM_CTLCOLORSTATIC:
                  SetBkColor ((HDC)wParam, RGB (64,128,128));
                  return TRUE;//(BOOL)GetStockObject (RGB (64,128,128));

               default:
                  return FALSE;
               }
        default:
           return FALSE;
        }
}

int DIALOGS::FeaturesDialog (bool NoNew)
{
   feat->Global->SavePalette(true);
   feat->SavehSize = feat->hSize; feat->SavevSize = feat->vSize;
   feat->SavehNumbSize = feat->hNumbSize; feat->SavevNumbSize = feat->vNumbSize;
   if(!NoNew) feat->Global->ResetPalette(true);
	int res = DialogBox(feat->Global->hinst,MAKEINTRESOURCE(IDD_DIALOG_FEATURES),
                                 feat->Global->hWnd,(DLGPROC) DIALOGS::DialogProc);
   if(!res) feat->Global->SavePalette(false);
   return res;
}

BOOL CALLBACK DIALOGS::DialogProc (HWND hwndDlg,UINT Msg,WPARAM wParam,LPARAM)
{
	return This->DProc(hwndDlg,Msg,wParam);
}

BOOL DIALOGS::DProc (HWND hwndDlg,UINT Msg,WPARAM wParam)
{
   static HWND hOldFocusWnd;
   HWND hList;
    switch(Msg)
      {
      case WM_INITDIALOG:

         hList = GetDlgItem(hwndDlg, HORIZONTAL);               // Set Limits
         SendMessage(hList,EM_SETLIMITTEXT,(WPARAM)3,0);
         SetDlgItemInt(hwndDlg,HORIZONTAL,feat->hSize,false);

         hList = GetDlgItem(hwndDlg, VERTICAL);
         SendMessage(hList,EM_SETLIMITTEXT,(WPARAM)3,0);
         SetDlgItemInt(hwndDlg,VERTICAL,feat->vSize,false);

         hList = GetDlgItem(hwndDlg, MAXHORIZ);
         SendMessage(hList,EM_SETLIMITTEXT,(WPARAM)2,0);
         SetDlgItemInt(hwndDlg,MAXHORIZ,feat->vNumbSize,false);

         hList = GetDlgItem(hwndDlg, MAXVERT);
         SendMessage(hList,EM_SETLIMITTEXT,(WPARAM)2,0);
         SetDlgItemInt(hwndDlg,MAXVERT,feat->hNumbSize,false);

         hList = GetDlgItem(hwndDlg,IDC_UPDOWNHORIZONTAL); // ширина
         SendMessage(hList,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)MAXSIZE, (short)5));
         hList = GetDlgItem(hwndDlg,IDC_UPDOWNVERTICAL); // высота
         SendMessage(hList,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)MAXSIZE, (short)5));
         hList = GetDlgItem(hwndDlg,IDC_UPDOWNMAXVERT);// сверху
         SendMessage(hList,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)100, (short)0));
         hList = GetDlgItem(hwndDlg,IDC_UPDOWNMAXHORIZ);// снизу
         SendMessage(hList,UDM_SETRANGE,0,(LPARAM) MAKELONG((short)100, (short)0));



         CheckDlgButton(hwndDlg,CHECK_BOX1,feat->Colors > 1 ? BST_CHECKED:
         							                                    BST_UNCHECKED);
//			EnableWindow(GetDlgItem(hwndDlg, IDD_COLOR),feat->Colors > 1);

         hOldFocusWnd=SetFocus(GetDlgItem(hwndDlg,HORIZONTAL)); //Set Focus

      case WM_COMMAND:
         switch(LOWORD(wParam))
            {
            case IDOK:

               feat->vNumbSize=GetDlgItemInt(hwndDlg,MAXHORIZ,NULL,false);
               feat->hNumbSize=GetDlgItemInt(hwndDlg,MAXVERT,NULL,false);

               feat->hSize=GetDlgItemInt(hwndDlg,HORIZONTAL,NULL,false);
               feat->vSize=GetDlgItemInt(hwndDlg,VERTICAL,NULL,false);

               if(feat->hSize<1 || feat->hSize>MAXSIZE) return Message(0);
               else if(feat->vSize<1 || feat->vSize>MAXSIZE) return Message(1);
               if(feat->hSize < feat->vNumbSize) feat->vNumbSize = feat->hSize;
               if(feat->vSize < feat->hNumbSize) feat->hNumbSize = feat->vSize;

               if(IsDlgButtonChecked(hwndDlg,CHECK_BOX1)==BST_UNCHECKED) feat->Global->ResetPalette(true);
               SetFocus(hOldFocusWnd);                        // восстановление фокуса
               EndDialog(hwndDlg,1);
               return true;

            case IDCANCEL:
               SetFocus(hOldFocusWnd);	//восстановление фокуса
               EndDialog(hwndDlg,0);
               return true;

/*            case IDD_COLOR:
              if(!ColorDialog())
              	{
               CheckDlgButton(hwndDlg,CHECK_BOX1,BST_UNCHECKED);
               EnableWindow(GetDlgItem(hwndDlg, IDD_COLOR),false);
               }
              return true;  */

            case CHECK_BOX1:
				  if(HIWORD(wParam)==BN_CLICKED)
              	if(IsDlgButtonChecked(hwndDlg,CHECK_BOX1)==BST_CHECKED)
               		{
//							EnableWindow(GetDlgItem(hwndDlg, IDD_COLOR),true);
                     feat->Global->ResetPalette(false);
                     }
               else	{
//               		EnableWindow(GetDlgItem(hwndDlg, IDD_COLOR),false);
                     feat->Global->ResetPalette(true);
                     }
              return true;

            case WM_CTLCOLORDLG:
            case WM_CTLCOLORSTATIC:
               SetBkColor ((HDC)wParam, RGB (64,128,128));
               return(BOOL)GetStockObject (RGB (64,128,128));
            default:
                return FALSE;
            }
      default:
        return FALSE;
      }
}

bool DIALOGS::Message(int NumbMessage)
{
  char Pointer[][47]={{"Ширина поля должна лежать в пределах  1 - 254."},
                      {"Высота поля должна лежать в пределах  1 - 254."}};
//                      {"Ширина поля оцифровки должна быть больше нуля."},
//                      {"Высота поля оцифровки должна быть больше нуля."},
//                      {"Неверное соотношение ширины поля и оцифровки. "},
//                      {"Неверное соотношение высоты поля и оцифровки. "}};
  MessageBox(feat->Global->hWnd,Pointer[NumbMessage],"Неверно введены параметры.",
                                                         MB_OK|MB_ICONERROR);
  return true;
}

bool DIALOGS::ColorDialog (void)
{
	int i,col;
   COLORREF *sour, *dist;
	CHOOSECOLOR lpcc;
   COLORREF ClrTbl[MAXCOLOR];
   for(i=0; i<MAXCOLOR; i++)	ClrTbl[i]=feat->ColorTable[i+FIRST];
   lpcc.lStructSize  = (DWORD)sizeof(CHOOSECOLOR);
   lpcc.hwndOwner    = feat->Global->hWnd;
   lpcc.hInstance    = (HWND)feat->Global->hinst;
   lpcc.rgbResult    = VOIDCOLOR;
   lpcc.lpCustColors = (COLORREF *)ClrTbl;
   lpcc.Flags        = CC_FULLOPEN | CC_RGBINIT;// | CC_ENABLETEMPLATEHANDLE;
   lpcc.lCustData    = NULL;
   lpcc.lpfnHook     = NULL;
   lpcc.lpTemplateName=NULL;
	if(ChooseColor(&lpcc))
   	for(i=0; i<MAXCOLOR; i++)
         feat->Global->ColorTable[i+FIRST]=ClrTbl[i];
   dist = sour = feat->Global->ColorTable;
   col = 0;
   for(i=0; i<MAXCOLOR; i++)// "уплотняем" цвета
      if(*++sour != VOIDCOLOR)
      	{
         dist[++col] = *sour;
         if((dist+col) != sour) *sour = VOIDCOLOR;
         }
   feat->Colors = col;
   return (col > 1);
}
