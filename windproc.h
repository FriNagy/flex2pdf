
// ========================================================================
//                              WndProc
// ========================================================================

void startmyf(char *filename)
{
		   VpeClosePreview(hDoc);
   		   warprev = 0;
		   strcpy(file2print,curexedir);
		   strcat(file2print,"\\Info\\");
		   strcat(file2print,filename);
		   AutoRender(0);
           VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);

};

void startmyd(char *filename)
{
	VpeClosePreview(hDoc);
	warprev = 0;
	strcpy(file2print, curexedir);
	strcat(file2print, "\\");
	strcat(file2print, filename);
	AutoRender(0);
	VpePreviewDoc(hDoc, NULL, VPE_SHOW_MAXIMIZED);

};

#define UDO_OBJECT_ID 1





LRESULT EXPO WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  //  static HINSTANCE  hInstance;
   static BOOL       no_close;        

   switch (message)
   {
    case WM_CREATE:
       no_close = FALSE;
       hMainWindow = hwnd;
      // hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
       return 0;


   // case WM_PAINT:
 
	//	  if ( mypaintmode ) Paint(hwnd);
 
	//	  break;

  // case WM_MOUSEMOVE:
  //	   mymenmode = 16;
 //		  break;

   case WM_SETFOCUS:
			if ( hDoc) SetFocus(VpeGetWindowHandle(hDoc));
		    return 0;


    case WM_SIZE:
       if (hDoc)   // this is an embedded window
       {
          MoveWindow(VpeGetWindowHandle(hDoc), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
       }
      return 0;

    case WM_CLOSE:
       if (!no_close)      // can't close, because printing?
       {
          if (hDoc) VpeCloseDoc(hDoc);     // because of MAPI problems (process will live on)
          DestroyWindow(hwnd);
          return 1;
       }
       if ( no_close != 99) Msg("Can't close, job is printing!");
       return 0;
              
           
    case WM_DESTROY:
       if (hDoc) VpeCloseDoc(hDoc);     // because of MAPI problems (process will live on)
       PostQuitMessage(0);
       return 0;


       case VPE_DESTROYWINDOW:
      //    Msg("Destroy Window.");

     if (hDoc == (VpeHandle)lParam)
       {
          if ( warprev )  PostQuitMessage(0);
       }
       break;

	   case  VPE_AUTOPAGEBREAK:
		   {
			 	  lasty  =  VpeGet(hDoc, VTOPMARGIN);
				  break;
		   };
                          
/* case VPE_PRINT:
     switch (wParam)
     {
       case PRINT_MSG_SETUPABORT:  // User aborted Setup-Dialog
            break;
       case PRINT_MSG_SETUPSTART:  // Setup-Dialog started
            VpeSetDevDuplex(hDoc, myDuplex ); 
            break;
       case PRINT_MSG_SETUPEND:    // Setup-Dialog ended
           // VpeSetDevDuplex(hDoc, myDuplex);
            break;
       case PRINT_MSG_START:
            break;
     }
    return 0; */


       case VPE_PRINT:
       switch (wParam)
       {
        case PRINT_MSG_SETUPABORT:  // User aborted Setup-Dialog
 //          Msg("Caught event from VPE: Setup aborted.");
	       if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) autoend = FALSE;
 		   if ( autoend ) {
			   mymenmode = 17;
			   VpeSendKey(hDoc,VKEY_CLOSE);
		   };
           break;
           
        case PRINT_MSG_SETUPSTART: // Setup-Dialog started
        	            VpeSetDevDuplex(hDoc, myDuplex );
				 	//		HWND myhwnd;
					//		myhwnd = VpeGetWindowHandle(hDoc);

     //    VpeGetDevice(hDoc,altDev,64);
           break;
           
        case PRINT_MSG_SETUPEND:   // Setup-Dialog ended
  	           if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) autoend = FALSE;
               VpeSetDevDuplex(hDoc, myDuplex);
//         VpeGetDevice(hDoc,myDev,64);
//	     if ( strcmp( myDev , altDev)) {	 
 //	                 VpeSetDevice(hDoc,myDev);
  //                   VpeSetDevDuplex(hDoc, myDuplex);
	//                                   };
           break;
 
		 case PRINT_MSG_START:
		    mymenmode = 1;
	        VpeSetDevDuplex(hDoc, myDuplex );
            no_close++;
           break;

       default:
           no_close--;
 	       if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) autoend = FALSE;
		   if ( autoend ) {
			   mymenmode = 1;
			   VpeSendKey(hDoc,VKEY_CLOSE);
		   };

           break;
       }
       return 0;

/*
	   case VPE_OBJECTCLICKED:
		   char s[150];

		   wsprintf(s, "Object #%d was clicked", VpeGetObjectID(hDoc));
		   MessageBox(hMainWindow, s, "CLICK:", MB_OK);
		   return 0;
		   break;

	   case VPE_UDO_PAINT:
		   UDOPaint((VpeHandle)lParam);
		   return 1;		// indicate, that the event has been processed by this application
*/

  case VPE_BEFORE_OPEN_FILE:
         startmyed(file2print);
         return 1;

  case VPE_BEFORE_SAVE_FILE:
	 // VpeSetDocExportType(hDoc, VPE_DOC_TYPE_PDF);
	     VpeSetSaveFileName(hDoc,"*.pdf");
         return 0;

 // case VPE_AFTER_OPEN_FILE:
 //        return 1;

  case VPE_BEFORE_MAIL:
  	      mymenmode = 1;
	  	  if ( filemail() ) return 1;
          return 0;

   case VPE_AFTER_MAIL:
          fileback((int)wParam);
		   mymenmode = 1;

 	       if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) autoend = FALSE;
 	       if ( autoend ) VpeSendKey(hDoc,VKEY_CLOSE);
          return 0;



   case WM_SYSCOMMAND:

	switch (wParam)
       {
           
	case CM_SAVEPDF:
		{    
        	VpeSendKey(hDoc,VKEY_SAVE);
			return 1;
       }
   case ID_SAVE_ONDESKTOP:
	   {
          ExportDocument("1",( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ));
		  goto killminow;
	
	   }
   case ID_SAVE_ONDESKTOPF2P:
	   {
            ExportDocument("2",( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ));
			goto killminow;
   	
	   }
   case ID_SAVE_EIGENEDATEIEN:
	   {
            ExportDocument("3",( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ));
			goto killminow;
        
	   }

	case CM_RPRINT:
		{    
        	VpeSendKey(hDoc,VKEY_PRINT);
			return 1;
       }

	case CM_VORSCHAU:
		   
			goto cm_vorschau;

	};  // end Switch
	break;

   case WM_COMMAND:

	switch (wParam)
       {
           
	case CM_VORSCHAU:
		{   
 cm_vorschau:

		   VpeClosePreview(hDoc);
   		   strcpy(file2print,orif2print);

		   warprev = 0;
		   AutoRender(0);
           VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);
		   SetFocus(VpeGetWindowHandle(hDoc));

           break;
       }

	case CM_EDITSET:
		{  

			  startmyed2(file2print);

/*             char tline[255];
			 
			  FILE *fh;

			 _makepath( tline ,  drive, mdir, ext + 1, ".set" );
			 if ((fh=fopen(tline, "rt")) == NULL) {
				  strcpy(tline,curexedir);
				  strcat(tline,ext + 1);
				  strcat(tline,".set");
				  if ((fh=fopen(tline, "rt")) == NULL) {
 		 		  sprintf(mMsg, "kann Steuerungs-Datei nicht �ffnen...\n\n(%s)\n",tline);
  		          Msg(mMsg);
					  break;
				                                       }; 
			 };
		  fclose(fh);

		  // immer mit Windows editor zu starten

		  bool merkoemconvert = oemconvert;
		  oemconvert = 0;
          startmyed(tline);
		  oemconvert = merkoemconvert; */
          break;

		}
	case CM_EDITMAILSET:
		{  
      
		  if (sumailf() != FALSE)  startmyed(uminfo);
          break;

		}

	case ID_GEHEZU_ERSTESEITE:
		 VpeGotoVisualPage(hDoc, 1);
		break;
	case ID_GEHEZU_SEITEVOR:
		  int n;
		  n = VpeGetVisualPage(hDoc);
          if ( n > 1 ) VpeGotoVisualPage(hDoc, n - 1);
		break;
	case ID_GEHEZU_SEITNACH:
  	 	  int mpcount; 
		  mpcount = VpeGetPageCount(hDoc);

		  n = VpeGetVisualPage(hDoc);
          if ( n < mpcount ) VpeGotoVisualPage(hDoc, n + 1);
		break;
	case ID_GEHEZU_LETZTESEITE:
		  mpcount = VpeGetPageCount(hDoc);
		 VpeGotoVisualPage(hDoc, mpcount);
		break;

	case ID_ANSICHT_GANZESEITE:
		VpeSetScaleMode(hDoc,VSCALE_MODE_FULL_PAGE);
		break;

	case ID_ANSICHT_VOLLEBREITE:
		VpeSetScaleMode(hDoc,VSCALE_MODE_PAGE_WIDTH);
		break;
	case ID_ANSICHT_VERKLEINERN:
     VpeZoomOut(hDoc);
     break;
	case ID_ANSICHT_100:
		 VpeSetScalePercent(hDoc,100);
		 break;
	case  ID_ANSICHT_VERGR:
     VpeZoomIn(hDoc);
     break;

	case ID_ANSICHT_GRIDEIN:
		{
     VpeSetGridMode(hDoc,0);
     VpeSetGridVisible(hDoc,1);
     break;
        }

	case CM_EDITMF:
		{   
     startmyed(file2print);
     break;
        }

	case CM_SAVEPDF:
		{    
        	VpeSendKey(hDoc,VKEY_SAVE);
			return 1;
       }

	case CM_RPRINT:
		{    
        	VpeSendKey(hDoc,VKEY_PRINT);
			return 1;
       }

	case ID_DRUCKERINFO_TEST:
		{    
		   startmyf("test.f2p");
           break;
       }
	case ID_DRUCKERINFO_DEMOSEITE3:
		{    
		   startmyf("Seite3.f2p");
           break;
       }
	case ID_DRUCKERINFO_DEMOSEITE2:
		{    
			startmyf("readme.f2p");
           break;
       }
	case ID_DRUCKERINFO_DEMOSEITE1:
		{    
	        startmyf("cmdline.f2p");
           break;
       }
	case 	ID_TEST_LINEDEMO:
	{
								VpeClosePreview(hDoc);
								warprev = 0;
								LineRender();
								VpePreviewDoc(hDoc, NULL, VPE_SHOW_MAXIMIZED);
								break;
	}
	case ID_DRUCKERINFO_DRUCKERINFO:
		{    
		   VpeClosePreview(hDoc);
   		   warprev = 0;
		   PrintRender();
           VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);
           break;
       }

     case CM_EXIT: {
          PostMessage(hwnd, WM_CLOSE, 0, 0);
          break;
       }

   case ID_SAVES_:
	   {
		      DWORD  fdwMenu = GetMenuState(hMenu, ID_SAVES_ , MF_BYCOMMAND); 
              if ((fdwMenu & MF_CHECKED)) CheckMenuItem(hMenu, ID_SAVES_, MF_UNCHECKED) ;         
			  else  CheckMenuItem(hMenu, ID_SAVES_, MF_CHECKED);

            break;	
	   }
   case ID_SAVE_ONDESKTOP:
	   {
            ExportDocument("1",1);
//            ExportDocument("1",( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ));
			goto killminow;
            break;	
	   }
   case ID_SAVE_ONDESKTOPF2P:
	   {
            ExportDocument("2",mitshellex);
			goto killminow;
            break;	
	   }
   case ID_SAVE_EIGENEDATEIEN:
	   {
            ExportDocument("3",mitshellex);
			goto killminow;
            break;	
	   }
   case ID_SAVE_:
	   {
            ExportDocument("=",mitshellex);
			goto killminow;
            break;	
	   }
   case CM_SAVEPDFS :
	   {
            ExportDocument("=",TRUE );
            break;	
	   }
	   }
	
}
   
 //  // mit gemappte Messages Uncomment und Toolbar Mode ohne Fixed-Message!
 //  // siehe + VPE_FIXED_MESSAGES ;
 //  // Check messages for VPE document:
 //  // ================================
 //  switch (VpeMapMessage(NULL, message))        // we use NULL, because all documents are
 //  {                                             // created with dynamic message mapping
 //   case VPE_DESTROYWINDOW:
 //     //    Msg("Destroy Window.");

 //    if (hDoc == (VpeHandle)lParam)
 //      {
 //         if ( warprev )  PostQuitMessage(0);
 //         // EnableMenuItem(hMenu, CM_PRECISION, MF_ENABLED);
 //      }
 //      break;

	//   case  VPE_AUTOPAGEBREAK:
	//	   {
 //      // Msg("AutoBreak");

	//	break; };
 //  //   case WM_LBUTTONDOWN:
 //  //        MiniDemo(hwnd);	// creates doc, sets global hDoc-variable
 //  //    return 1;
 //                         
 //   case VPE_PRINT:
 //      switch (wParam)
 //      {
 //       case PRINT_MSG_SETUPABORT:  // User aborted Setup-Dialog
 ////          Msg("Caught event from VPE: Setup aborted.");
 //          break;
 //          
 //       case PRINT_MSG_SETUPSTART: // Setup-Dialog started
 ////          Msg("Caught event from VPE: Setup started.\n\nNow you could change some printer properties by code,\nor send ESCAPE-Sequences to the printer.");
 //          break;
 //          
 //       case PRINT_MSG_SETUPEND:   // Setup-Dialog ended
 ////          Msg("Caught event from VPE: Setup successful finished.");
 //          break;
 //          
 //       case PRINT_MSG_START:
 //          no_close++;
 //  //        Msg("Caught event from VPE: Print-Job started.");
 //          break;

 //      default:
 //          no_close--;
 //          break;
 //      }
 //      return 0;

 // case VPE_BEFORE_OPEN_FILE:
 //        startmyed(file2print);
 //        return 1;

 // case VPE_BEFORE_SAVE_FILE:
	//     VpeSetSaveFileName(hDoc,myfname);
 //        return 0;

 //// case VPE_AFTER_OPEN_FILE:
 ////        return 1;

 //  case VPE_BEFORE_MAIL:
	//  	  if ( filemail() ) return 0;
 //         return 0;

 //  case VPE_AFTER_MAIL: 
 //         fileback(wParam);
 //         return 0;

 //      
 //   // case VPE_HELP:
 //   //       break;
 //  }

   return DefWindowProc (hwnd, message, wParam, lParam) ;

killminow:
			Sleep(100);
			if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) goto killminow;
			VpeSendKey(hDoc,VKEY_CLOSE);  // PostMessage(hwnd, WM_CLOSE, 0, 0);
			return 1;
}

//	mylongline = (char *) malloc( 16384 );  // ca. 1024 * 16 
//
//  FILE *fh;
//
//  if ((fh =fopen(file2print, "rbS")) == NULL)
//   {
//	  sprintf(mMsg, "Fehler bei �ffnen!\nDateiname: %s!",file2print);
//      MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
//      return FALSE;
// };
//
//
//       char *llptstr; 
//
//       flog = fopen("C:\\F2P.LOG", "wb");
//
//      int posback = 0;
//	  int maxread = 16380;
//	      posback = 0;
//
//fseeme:
//
//	  int numread = fread( mylongline + posback , sizeof( char ), maxread - posback , fh );
//	  numread = numread + posback;
//	  
//	  if (numread == maxread ) {
//		  numread-- ;
//		  numread-- ;
//	      posback = 1;
//	                          }
//	  else {
//		  mylongline[numread]   = '\0';
//		  mylongline[numread+1] = '\0';
//		  posback = 0;
//	       };
//
//	  int lastlauf = 0;
//
//	  llptstr = mylongline;
//	  int lauf = 0;
//	  for ( ; lauf < numread; lauf++ ) {
//		  
//
//		  if  ( ( mylongline[lauf] == '\r'))         {             // Steuerungscharacter
//			  if  ( ( mylongline[lauf+1] == '\n'))   {             // Steuerungscharacter
//
//	  		  mylongline[lauf] = '\0'; 
//			  goto neueline;
//       wriwei:
//  		 	  lauf++;
//  		 	  lauf++;
//			  lastlauf = lauf;
//			  llptstr = mylongline + lauf;  // +1
//
//		                                            };
//	                                           };
//	  };
//
//
//	  if (posback) {
//					 // zur�ckpositionieren auf lastlauf - maxread (wenn �berhaupt lastlauf)
//					 if (lastlauf) posback = maxread - lastlauf;
//					 memcpy( mylongline, llptstr, posback );
////		             for ( lauf = 0; lauf < posback; lauf++ )  mylongline[lauf] =  llptstr[lauf];
////					 if  ( fseek( fh, lastlauf - maxread  , SEEK_CUR) != 0 ) {      return -1; }
//					 goto fseeme;
//	               };
//	 
//      fputs( llptstr, flog );
//	  goto nneueline;
//neueline:
//       fputs( llptstr, flog );
//       fputs( "\r\n", flog );
//	   goto wriwei;
//nneueline:
//
//	   fclose(flog);
//
//
//	   exit(0);


INT_PTR CALLBACK DeleteItemProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HWND hwndOwner;

	// UINT uSent;  // unused
	switch (message)
	{


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return 1;
		case IDOKOPEN:
			EndDialog(hwndDlg, IDOKOPEN);
			return 1;
		case IDOK:
		case IDOKDRUCK:
		case IDC_BUTTON1:
		case IDC_BUTTON2:
		/*	SHELLEXECUTEINFO ShExecInfo;
			ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			ShExecInfo.fMask = NULL;
			ShExecInfo.hwnd = NULL;
			ShExecInfo.lpVerb = "OPEN";
			ShExecInfo.lpFile = "\\dev\\f2p\\f2p.pdf";
			ShExecInfo.lpParameters = NULL;
			ShExecInfo.lpDirectory = NULL;
			ShExecInfo.nShow = SW_MAXIMIZE;
			ShExecInfo.hInstApp = NULL;

			ShellExecuteEx(&ShExecInfo);
  	*/
	
		//	hwndOwner = (HWND)atoi(SteuBlock[6]);

			EndDialog(hwndDlg, wParam);
			return 1;

		case IDOKMAIL:
			EndDialog(hwndDlg, IDOKMAIL);

			return 1;

		case IDCOP2CLIP:
			EndDialog(hwndDlg, IDCOP2CLIP);

	
			return 1;

		//	ShellExecute($df_home_tmp & '\' & $df_par & '_(' & $ts & ').pdf', "", "", "open", @SW_HIDE);
		}

	case WM_SYSCOMMAND:
	{
		switch ((wParam))
		{
		case SC_KEYMENU:
		case SC_MOUSEMENU:
			return 1;

//		case IDM_SYS_LOCATE:
//			myDosRun(1);
//			EndDialog(hwndDlg, IDOK);

		}
		return 0;
	}

	case WM_NOTIFY:
	{
		switch ((lParam))
		{
		case TTN_GETDISPINFO:
		case SC_MOUSEMENU:
			return 1;


		}
		return 0;
	}


	case WM_CTLCOLORDLG:     // F�r den Dialog            
		return (LRESULT)mybrush;

	case WM_CTLCOLORBTN:     // F�r den Dialog            
		return (LRESULT)mybrush;

	case WM_CTLCOLORSTATIC:

//		UINT ID;
//		ID = GetWindowLong((HWND)lParam, GWL_ID);


		hdc = (HDC)wParam;

		SetTextColor(hdc, textcol);  // yourColor is a WORD and it's format is 0x00BBGGRR
		SetBkColor(hdc, backcol);  // yourColor is a WORD and it's format is 0x00BBGGRR 0x008811ff); //

		return   (LRESULT)mybrush;

	case WM_CLOSE:
		EndDialog(hwndDlg, IDCANCEL);
		DestroyWindow(hwndDlg);

	//	SetForegroundWindow(hFlexxWind);
	//	SetFocus(hFlexxWind);

		return 0;

		//	case WM_ACTIVATE:
		//		hFlexxWind = GetForegroundWindow();
		//		return 1;

	case WM_DESTROY:

	/*	DWORD myerg;
		HANDLE hStdIn;

		hStdIn = GetStdHandle(STD_INPUT_HANDLE);
		GetConsoleMode(hStdIn, &myerg);

		if (IsDlgButtonChecked(hwndDlg, IDC_CHECK1)) {
			SetConsoleMode(hStdIn, myerg | ENABLE_QUICK_EDIT_MODE);
		}
		else if (myerg & ENABLE_QUICK_EDIT_MODE) SetConsoleMode(hStdIn, myerg & ~ENABLE_QUICK_EDIT_MODE);
		return TRUE; */



	case WM_INITDIALOG:


		RECT rc, rcDlg, rcOwner;

		hwndOwner = hparwindow;

	//	if ((strlen(SteuBlock[6])) > 1) hwndOwner = (HWND)atoi(SteuBlock[6]);

		if (!IsWindow(hwndOwner)) {
		                 
		// Get the owner window and dialog box rectangles. 

		if ((hwndOwner = GetParent(hwndDlg)) == NULL)
		{
			hwndOwner = GetDesktopWindow();
		}
		}
		GetWindowRect(hwndOwner, &rcOwner);
		GetWindowRect(hwndDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that right and bottom 
		// values represent the width and height, and then offset the owner again 
		// to discard space taken up by the dialog box. 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining space and the owner's 
		// original position. 
	//	GetMenu
		SetWindowPos(hwndDlg,
			HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE);

		textcol = RGB(255, 255, 255); //  (GetSysColor(COLOR_HIGHLIGHTTEXT)); // 23;  // InfoBlock
		backcol = RGB(52, 64, 128); // (GetSysColor(24)); // 24;
	//	backcol = RGB(32, 64, 128); // (GetSysColor(24)); // 24;
//		mybrush = (GetSysColorBrush(24));
//		mybrush = CreateSolidBrush(RGB(60, 80, 160));

		mybrush = CreateSolidBrush(backcol);
		SetDlgItemText(hwndDlg, IDC_BUTTON1, SteuBlock[3]);
		SetDlgItemText(hwndDlg, IDC_BUTTON2, SteuBlock[2]);
		SetDlgItemText(hwndDlg, IDC_STATIC5, SteuBlock[4]);

	//	SetDlgItemText(hwndDlg, IDOKDRUCK, SteuBlock[5]);
	//	SetDlgItemText(hwndDlg, IDOKMAIL, SteuBlock[6]);
		// SetFocus(GetDlgItem(hwndDlg, IDC_EDIT1));

		HMENU hMenu;

		hMenu = GetSystemMenu(hwndDlg, FALSE);

		//	 RemoveMenu(hMenu, 1, MF_BYPOSITION);
		//	 RemoveMenu(hMenu, 0, MF_BYPOSITION);
			 // RemoveMenu(hMenu, 5, MF_BYPOSITION);
			// RemoveMenu(hMenu, 3, MF_BYPOSITION);
			// RemoveMenu(hMenu, 4, MF_BYPOSITION);

		AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
		// strcpy(mMsg, "Info...");
		//	wsprintf(mMsg, "f2p\\scr2bmp.exe %i %s\\tmp\\%s", dmode, dfhome, "fwin.png");


		AppendMenu(hMenu, MF_STRING, IDC_STATIC5, TEXT("zu HOME"));

		return 0;



	};
	return 0;
};

