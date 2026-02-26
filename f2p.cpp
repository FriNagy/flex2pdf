
/* @package 
        f2p.cpp
        Copyright() * 2007
        Author: FRIGYES NAGY
        Created: FN  19.06.2005 17:09:48
        letzte mit 32 Bit am 01.03.25  / VPE 32.740
		                     goodbye...
		*/


#define STRICT
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable: 4267)  // size_t to int conversion
#pragma warning(disable: 4244)  // __int64/double to int conversion

#include <windows.h>
#include <windowsx.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
// #include <math.h>
#include <direct.h>
#include <ctype.h>
#include <tchar.h>
#include <math.h> 
#include <shlobj.h> 

#include "vpiface.h"
#include "vppiface.h"
#include "vpecomon.h"
#include "colors.h" 
#include "resource.h"
#include "winuser.h"

//  #define vpeDEMO
//  #define mitlogfile  

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//----------------------------------------------------------------------------
// Globals:
//----------------------------------------------------------------------------

static char szAppName[] = "F2P.26";
HWND  hMainWindow;
HMENU hMenu, hpopMenu;
int finlastline, obji = 1;       

int bcolor = 0;
int glodpi = 1;
BOOL mitlog = false, will_save = 0;

BOOL f2tmode   = false;
BOOL oldzwmode = false;

BOOL mitcol = false;
BOOL willback = false;
BOOL directpdf = false;
BOOL wartime = false;
BOOL archmode = false;
BOOL spring2end = false;

int mitclip = 0 ;
int mitdeb  = 1;
int dbg_time_threshold = 0;  // 0 = aus, sonst Schwellwert in Sekunden (-dtime:N)
#define DEB_TIME                    1L   // Zeitangaben mitdrucken
#define DEB_FILE                    8L   // Zwischen-Datei speichern

static bool OpenDrawer_BREAK_Pulse(const char* portName, DWORD breakMs, DWORD pulseMs);  // forward
static int CashAnz_Run(const char *cmdPort, const char *text1, const char *text2);       // forward



// wir haben 3 Zeitmerker pro document
// time0 = start-ausdruck in DF
// time1 = Frei  ( sp�ter ...)
// time9 = ende-ausdruck in DF (direkt vor close_output)

// HWND  hparwindow=NULL;

WNDPROC g_OldVPE;

SYSTEMTIME st;
int myzoomode = 100;
int myDuplex = 1;
int rueckmcode = 0;
int obanzonpage = 0;
int mymwmode = 2;      // mainWindowMode 0 unvisible, 1  
int mypaintmode = 0;   // mainWindowMode 0 unvisible, 1  
int vormycol = -1;
int mymenmode = 1;
int mytbmode = VPE_EMBEDDED + VPE_NO_RULERS + VPE_NO_HELPBUTTON + VPE_GRIDBUTTON +  VPE_NO_OPEN_BUTTON + VPE_NO_PAGESCROLLER + VPE_DOCFILE_READONLY + VPE_FIXED_MESSAGES; //  ;
int xoben  = CW_USEDEFAULT;
int yoben  = CW_USEDEFAULT;
int xbreit = CW_USEDEFAULT;
int yhoch  = CW_USEDEFAULT;
//        char myDev[70];
//	    char altDev[70];	 

#include "vpe_license.h"
char teil2[10] = VPE_LICENSE_KEY2;

// char teil2[10] = "XN2E-T4AD";
// 376896 -                     16384        =   360512 (test mod)
// 32769 Embedded - no -tb 
int mycurspos = 0;   // store und restore setting 
// int mycurdupp = 1;   // store und restore setting 
// int myaktdupp = 1;   // store und restore setting 
double myhoch = 99;
double glasty;    // untere Druckbare Rand
double zeilyres;

int warprev = 0;
int aktline = 1;   // Zeilen-Position in Input-File
// int aktufdoc = 1;
int lastRender = 0;

DWORD  backcol, textcol;
HBRUSH mybrush;

VpeHandle hDoc = 0; 
VpeHandle ParentBookmark = 0;	// used for creating bookmarks
VpeHandle hData;

_int64 time0 = 0, time1 = 0, time9 = 0, start0 ;

char ufname[99]  = "=";   // die pdf DateiName was erstellt wird0
char upname[99]  = "\0";  // die prs DruckeDef. Datei
char uaname[99] = "\0";   // Archiv Datei-Namei
char uminfo[99] = "\0";   // mailInfo (standard txt)
char usetdir[99] = "\0";
char uincldir[99] = "";

char myindis[15];         // Time-Stamp String
char aktulang[5] = "GER";
HWND      hparwindow = NULL;
char myfname[99];         // FileName	die zu speichern 
                          // standard: F2P_%timestamp%.pdf

 char *myRTFline;
 char *mylongline;
 char mMsg[955];
 char mMsgpar[128];
 char file2print[125];
 char orif2print[125];
 char curexedir[155];
 char mmailto[75]; // ab v3 , mailto merken
 char aamode = ' ';

 double lasty, lastx, mymerklastrendy, mymerklastrendx;
 double mymerklasty1, mymerklasty2, mymerklasty3;
 double mymerklastyx, mymerklastyy, mymerklastyz;
 double myy, myx;
	 

 int    pagex, pagey, pagez;

 double mymerky, rendres;  // rendering - reserve

 double czbreit ; // ca. 12 CPI
 double czhoch  ; // ca. 6  LPI
 double globczhoch = 0;
 double globczoffs = 0;
 double lastwribot = 0;

 double kopfab  = 0;
 double fussbis = 0; 
 double indirand = 0;

 bool gPaperView = false;
 bool setglobh = false;
 bool aushome  = false;
 bool nosave = false;
 bool killfirstpage = false;

 int  icopar = -1;
 char mycopar[4];
 char mycoml[4][160];
 char setfdir[128];
 char setfname[64];
 char setdext[7] = "pdf" ; // ohne .

 char hlongline[640];

 char myhmode;
 char nosmode;
 char magicchar;
 char f2magic = ' ';
 char ovorch = ' ';
 char shellexmode = ' ';
 char ypar[12] = " ";
 int wuzezahl;
 int wucharzahl;
 bool colf2t = false;

 bool autoend = false;
 bool mitvorschau = true;
 bool mitshellex = false;
 bool clipupdate = false;

 bool mitdoppchar = true;
 bool blockconvert = true;
 bool oemconvert = false;      // da wird das ganze Block konvertiert, wenn oemcodepage 0 
 int  oemcodepage = 0;
 int  oemcodepageback = 1250;
 int  foemcodepage = 0;
 int  foemcodepageback = 1250;

 
 // bool	printme = false;
// bool	pdfme   = false;
// bool	mailme  = false;
 bool	owncreate = false;
 bool	mypcontrol = false;
 bool	mitRTFchars = false;
 char drive[_MAX_DRIVE];
 char mdir[128];
 char fname[128];
 char ext[21];
 char xppfad[250];

 bool wantbreak = false;

 // ""
 char mySetupStr[99] = "---";         //  [=)
 char SteuBlock [13][256];  //  IUuFfSsBbQq0-
 char SteuBlock2[16][512];  //  P123456789ENLxyz

 const char steustri[31] = "IUuFfSsBbQq0&P123456789ENLxyZ"; // "IUuFfSsBbQq0123456789" HhDd;
 const char steuvars[40] = "0123456789abcdefghijklmnopqrstuvwxyz%$@"; 
 // . Curexedir
 //                                                           pqr Pfad in Dateinamen
 char SteuVar[40][256] =  { "\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0" }; 
 
 FILE *flog;
 FILE *falog;

 void Eincharp(char schaltch);
 void Prozcharp(char schaltch);

 double mypos(char *s);

 int lower(int c)
 {
	 if (c <= 'Z' && c >= 'A') return c + 32;
	 else return c;
 }


// ========================================================================
//                               Msg
// ========================================================================
void Msg(char *s)
{
   MessageBox(hMainWindow, s, szAppName, MB_OK);
}

void Myreg()
{

#ifndef vpeDEMO
//	char teil2[10] = "WK32-RIIB";
	teil2[0] = 'A';
	VpeLicense(hDoc, VPE_LICENSE_KEY, teil2  );
//	teil2[0] = 'Q';
//	VpeLicense(hDoc, "VPE-P2730-5563", teil2);

#endif
};

#include "LineRender.h"
// ========================================================================
//							  GetTmpDir
// ========================================================================
TCHAR *GetTmpDir()
{
	TCHAR *s;

	s = _tgetenv(_T("TMP"));
	if (!s)
		s = _T("c:\\tmp");

	if (!_taccess(s, 2))
	{

		s = _tgetenv(_T("TEMP"));
		if (!s)
			s = _T("c:\\temp");

		if (!_taccess(s, 2))
		{
			s = _tgetenv(_T("ALLUSERSPROFILE"));
			if (!s)
				s = _T("c:");
		}
	}

	return s;
}

int cop2clip(char* s)
{

HGLOBAL hGlobal = GlobalAlloc(GHND, sizeof(DROPFILES) + strlen(s) + 2);

if (!hGlobal) return 0;

DROPFILES* dropfiles = (DROPFILES*)GlobalLock(hGlobal);

if (!dropfiles) {
	GlobalFree(hGlobal);
	return 0;
}

dropfiles->pFiles = sizeof(DROPFILES);
dropfiles->fNC = TRUE;
dropfiles->fWide = FALSE;

memcpy(&dropfiles[1], s, strlen(s));

GlobalUnlock(hGlobal);

if (!OpenClipboard(NULL)) {
	GlobalFree(hGlobal);
	return 0;
}

if (!EmptyClipboard()) {
	CloseClipboard();
	GlobalFree(hGlobal);
	return 0;
}

if (!SetClipboardData(CF_HDROP, hGlobal)) {
	CloseClipboard();
	GlobalFree(hGlobal);
	return 0;
}

// Nach SetClipboardData geh�rt der Speicher dem Clipboard - NICHT freigeben!
CloseClipboard();
return 1;
};


void makemeutf(char *s)
{
	// char zB: \'xxEF1A20...
	BYTE ech;
	int erglauf = 0;
	for (int loci = 4; s[loci];) {
		ech = s[loci++];
		if (ech > 64) ech += 9;  // groesser gleich A
		s[erglauf] = ech << 4;
		ech = s[loci++];
		if (ech > 64) ech += 9;
		s[erglauf++] += (ech & 0xF);
	};
	s[erglauf] = '\0';

}

void makemehex(char *s)
{
	// char zB: \'xxEF1A20...
	BYTE ech;
	int erglauf = 0;
	for (int loci = 4; s[loci];) {
		ech = s[loci++];
		if (ech > 64) ech += 9;  // groesser gleich A
		s[erglauf] = ech << 4;
		ech = s[loci++];
		if (ech > 64) ech += 9;
		s[erglauf++] += (ech & 0xF);
	};
	s[erglauf] = '\0';
}



void mymakepar(char* s, bool replspace)
{

	// sstripar Trennzeichen
		// | trenn kommentar und filename
		// in filename > trennt file-path und interne Name
		// in interne Name ? trennt interne Name und Mime-Type 
		// in MIME-Type ^ trennt Mime-Type und AFRelation 

//	mymakepar(sstripar, 0);


	if (s[0] == '[') replspace = false;
	int mint = 0;
	for (int i = 0; s[i]; i++, mint++) {
		// nbsp if ( replspace && ( (s[i] == ' ') || (s[i] == '\t') ) )  s[i] = '\xA0';  // nbspace

		if ((s[i] == '\\') && (s[i + 1] == '\'')) {

			if (s[i + 2] == 'i') {
				if ((s[i + 3] > '0') && (s[i + 3] < '6')) {
					strcpy(hlongline, "incl???");
					if (s[i + 3] == '1') strcpy(hlongline, SteuBlock2[1]);
					if (s[i + 3] == '2') strcpy(hlongline, SteuBlock2[2]);
					if (s[i + 3] == '3') strcpy(hlongline, SteuBlock2[3]);
					if (s[i + 3] == '4') strcpy(hlongline, SteuBlock2[4]);
					s = hlongline;
					return;
				};
			};


			i = i + 3;
			byte li;

			li = s[i - 1]; // &0x0F;
			if (li > 64) li += 9;
			li <<= 4;
			li += s[i] & 0x0F;
			if (s[i] > '9') {
				if (i == 3) {
					if (s[i] == 'x') {
						if (s[i - 1] == 'u') {
							makemeutf(s);
							goto wandlok;
						};
						if (s[i - 1] == 'x') {
							makemehex(s);
							goto wandlok;
						};

					};
				};
				li += 9;
			};

			s[i] = li;
		};
		s[mint] = s[i];
	};

wandlok:

	s[mint] = '\0';
	//nbspace			   for (int i = 0; i < solang; i++) if ((s[i] == ' ') || (s[i] == '\t'))  s[i] = '\xA0';


}


void mymakeme(char *s, bool replspace)
{

    if (s[0] == '[') replspace = false;
	int mint = 0;
	for (int i = 0; s[i] ; i++, mint++)  {
	// nbsp if ( replspace && ( (s[i] == ' ') || (s[i] == '\t') ) )  s[i] = '\xA0';  // nbspace

		if ((s[i] == '\\') && (s[i + 1] == '\'')) {

			if (s[i + 2] == 'i') {
				if ((s[i + 3] > '0') && (s[i + 3] < '6') ) {
					strcpy(hlongline, "incl???");
					if (s[i + 3] == '1') strcpy(hlongline, SteuBlock2[1]);
					if (s[i + 3] == '2') strcpy(hlongline, SteuBlock2[2]);
					if (s[i + 3] == '3') strcpy(hlongline, SteuBlock2[3]);
					if (s[i + 3] == '4') strcpy(hlongline, SteuBlock2[4]);
					s = hlongline;
					return;
				};
			};


			i = i + 3;
			byte li;

			li = s[i - 1]; // &0x0F;
			if (li > 64) li += 9;
			li <<= 4;
			li += s[i] & 0x0F;
			if (s[i] > '9') {
				if (i == 3)  {
					if (s[i] == 'x') {
						if (s[i - 1] == 'u') {
							makemeutf(s);
							goto wandlok;
						};
						if (s[i - 1] == 'x') {
							makemehex(s);
							goto wandlok;
						};

					};
				};
				li += 9;
			};

			s[i] = li;
		};
		s[mint] = s[i];
	};

wandlok:

	s[mint] = '\0';
	//nbspace			   for (int i = 0; i < solang; i++) if ((s[i] == ' ') || (s[i] == '\t'))  s[i] = '\xA0';


}

int mymakemes(char *s, bool replspace)
{

	if (s[0] == '[') replspace = false;
	int mint = 0;
	for (int i = 0; s[i]; i++, mint++)  {
		//nbspace	if (replspace && ((s[i] == ' ') || (s[i] == '\t')))  s[i] = '\xA0';

		if ((s[i] == '\\') && (s[i + 1] == '\'')) {

			if (s[i + 2] == '%') {
				sprintf(hlongline, s + 3, SteuBlock2[1], SteuBlock2[2], SteuBlock2[3], SteuBlock2[4]);
				return 1;
				};
			if (s[i + 2] == 'v') {
				int wint = s[i + 3] - 48;
				if (wint < 1) wint = 1;
				if (wint > 5) wint = 1;
				strcpy(hlongline, SteuBlock2[wint]);
				return 1;
			};


			i = i + 3;
			byte li;

			li = s[i - 1]; // &0x0F;
			if (li > 64) li += 9;
			li <<= 4;
			li += s[i] & 0x0F;
			if (s[i] > '9') {
				if (i == 3)  {
					if (s[i] == 'x') {
						if (s[i - 1] == 'u') {
							makemeutf(s);
							goto wandlok;
						};
						if (s[i - 1] == 'x') {
							makemehex(s);
							goto wandlok;
						};

					};
				};
				li += 9;
			};

			s[i] = li;
		};
		s[mint] = s[i];
	};
wandlok:
	s[mint] = '\0';
	//nbspace	   for (int i = 0; i < solang; i++) if ((s[i] == ' ') || (s[i] == '\t'))  s[i] = '\xA0';

		 return 0;
}



int myRunP(char *s, int pmode)
{

	STARTUPINFO si;
	PROCESS_INFORMATION lastpi;

	char lstring[256];
	if (s[0] == '\\') {
		strcpy(lstring, s);
	    goto abspfad;
                     };
    if (s[1] == ':') {
	  strcpy(lstring, s);
      goto abspfad;
					 };
	if (s[0] == '^') {
		strcpy(lstring, s + 1);
		goto abspfad;
	};
	strcpy(lstring, curexedir);
    strcat(lstring,s);
abspfad:
	ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    ZeroMemory( &lastpi,  sizeof(lastpi) );
 
	if( !CreateProcess( NULL, // No module name (use command line). 
		lstring,
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,             // Set handle inheritance to FALSE oder TRUE Indexfehler wenn nicht richtig. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &lastpi )         // Pointer to PROCESS_INFORMATION structure.
    ) 
    {
  	 
			sprintf(mMsg, "kann das Programm nicht starten!\n%s\n", lstring);
            return (-1);

    }

	if ( pmode == 0 ) {
		WaitForSingleObject( lastpi.hProcess, INFINITE );
                      };
	DWORD dwCode;
	GetExitCodeProcess(lastpi.hProcess, &dwCode ) ;
    CloseHandle(lastpi.hProcess);
    if ( lastpi.hThread  != 0 ) CloseHandle(lastpi.hThread);
    return(dwCode);

}


void Drop2Clip(TCHAR* efname)
{




	WCHAR *pFullNames = (WCHAR*)malloc(2 * MAX_PATH * sizeof(WCHAR));
	WCHAR *p = pFullNames;
	//		StripEnclosingQuotes(buf);
	//		MakeFullName(buf, 0);


	size_t cSize = strlen(efname) + 1;

	wchar_t wc[MAX_PATH];

	mbstowcs(wc, efname, cSize);

	if (efname[0] != '\\') {         // clipboard verlangt exakte Datei-Namen
		if (efname[1] != ':') {

			char* buffer;
			// Get the current working directory: 
			if ((buffer = _getcwd(NULL, MAX_PATH)) == NULL) {
				Msg("_getcwd error");
				return;
			}
			else
			{
				sprintf(mMsg, "%s\\%s", buffer,efname);
				free(buffer);
			}

			cSize = strlen(mMsg) + 1;
			mbstowcs(wc, mMsg, cSize);

		};
	};

	wcscpy(p, wc);

	int solang = (wcslen(p));
	solang++;
	p[solang] = '\0';
	solang++;
	p[solang] = '\0';

	OpenClipboard(NULL);
	EmptyClipboard();

	DWORD dwDataBytes = sizeof(WCHAR)* (solang);
	DROPFILES df = { sizeof(DROPFILES), { 0, 0 }, 0, TRUE };
	HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DROPFILES)+dwDataBytes);
	char *pGlobal = (char *)GlobalLock(hMem);
	solang = sizeof(DROPFILES);

	CopyMemory(pGlobal, &df, sizeof(DROPFILES));
	CopyMemory(pGlobal + 20, p, dwDataBytes); // that's pGlobal + 20 bytes (the size of DROPFILES);
	GlobalUnlock(hMem);  // MUSS vor SetClipboardData erfolgen!
	SetClipboardData(CF_HDROP, hMem);
	// Nach SetClipboardData geh�rt der Speicher dem Clipboard - NICHT freigeben!
	CloseClipboard();
	free(pFullNames);

}

char *sreplace(char *string, char *oldpiece, char *newpiece) { 

   int str_index, newstr_index, oldpiece_index;
   int new_len, old_len, cpy_len, end;
   char *c; 
   static char newstring[250]; 

   if ((c = (char *) strstr(string, oldpiece)) == NULL)  {
   char *news;
   char *lnewpiece;
   news = _strlwr(_strdup( string ));
   lnewpiece = _strlwr(_strdup( oldpiece ));
   c = (char *) strstr(news, lnewpiece);
   if (c != NULL)  oldpiece_index = c - news;
   free( news );
   free( lnewpiece );
   if (c == NULL)   return string; 
     
                                                        }
   else oldpiece_index = c - string;

   new_len        = strlen(newpiece);
   old_len        = strlen(oldpiece);
   end            = strlen(string)   - old_len;
   


   newstr_index = 0; 
   str_index = 0; 
   while(str_index <= end && c != NULL) 
   { 
// f�r replace all
      /* Copy characters from the left of matched pattern occurence */
      cpy_len = oldpiece_index-str_index;
      strncpy(newstring+newstr_index, string+str_index, cpy_len);
      newstr_index += cpy_len;
      str_index    += cpy_len;

      /* Copy replacement characters instead of matched pattern */
      strcpy(newstring+newstr_index, newpiece);
      newstr_index += new_len;
      str_index    += old_len;

      /* Check for another pattern match */
      if((c = (char *) strstr(string+str_index, oldpiece)) != NULL)
         oldpiece_index = c - string;

   }
  
   /* Copy remaining characters from the right of last matched pattern */    
   strcpy(newstring+newstr_index, string+str_index); 
   return newstring; 
}; 



void spdfpar(char steuchar,TCHAR* sstripar)
{

	// mymakeme(sstripar, 0);

  	int myzugmode = 0;
	int tende = 0;
	int firstz;
	char internename[96];
	char mimetype[60];
	char afrelation[30];
	char lexte[9];

       switch  ( lower( steuchar ) )

      {

 	     case 's' : // Subject 
  		  VpeSetSubject(hDoc, sstripar );
		  break;

         case 'a' : // Author
  		  VpeSetAuthor(hDoc, sstripar );
		  break;

		 case 'e' : // Create  (erstellt)
	      owncreate = true;
  		  VpeSetCreator(hDoc, sstripar );
 	     break;

		 case 'p' : // Password
		  VpeSetEncryption(hDoc,DOC_ENCRYPT_STREAM);
  		  VpeSetProtection(hDoc,PDF_ALLOW_ALL);
  		  VpeSetUserPassword(hDoc, sstripar );
  	     break;

		 case 'o': // Password
		 VpeSetOwnerPassword(hDoc, sstripar);
		 break;

		 case 'l': // Set PDF-A Level

		 VpeSetPDFALevel(hDoc, VPE_PDF_A_LEVEL_1B);
		 VpeAddColorProfile(hDoc,
			 "GTS_PDFA1",
			 "sRGB_IEC61966-2-1",
			 "Custom",
			 "http://www.color.org",
			 "sRGB_IEC61966-2-1",
			 "sRGB_IEC61966-2-1");

		 break;

		 case 'i': // include in PDF
			 // sstripar Trennzeichen
			 // | trenn kommentar und filename
			 // in filename > trennt file-path und interne Name
			 // in interne Name ? trennt interne Name und Mime-Type 
			 // in MIME-Type ^ trennt Mime-Type und AFRelation 

			 // File Name und Kommentar bleiben in sstripar

			 tende = strlen(sstripar) - 1;
			 for (; tende >= 0 && (sstripar[tende] != '|'); tende--);  // suche Trennzeichen 
			 if (tende > 0) {
				 sstripar[tende] = '\0';
				 tende++;
			 };


			 _splitpath(sstripar, afrelation, internename, mimetype, lexte); // C4996

			 _strlwr(lexte);
			 _strlwr(mimetype);

			 strcpy(internename, mimetype);
			 strcat(internename, ".");
			 strcat(internename, lexte);

			 strcpy(mimetype, "text/html");
			 strcpy(afrelation,"Data");

			 if (lexte == "pdf") {

			 };
			 if (lexte == "xml") {
				 strcpy(mimetype, "text/xml");
			 };


			 for (firstz = 0; (sstripar[firstz] != '>') && (sstripar[firstz] != '\0'); firstz++);  // suche Trennzeichen 

			 if (sstripar[firstz] == '>') {
				 sstripar[firstz] = '\0';
				 firstz++;
				 strcpy(internename, sstripar + firstz);

				 for (firstz = 0; (internename[firstz] != '?') && (internename[firstz] != '\0'); firstz++);  // suche Trennzeichen 
				 if (internename[firstz] == '?') {
					 internename[firstz] = '\0';
					 firstz++;
					 strcpy(mimetype, internename + firstz);

					 for (firstz = 0; (mimetype[firstz] != '^') && (mimetype[firstz] != '\0'); firstz++);  // suche Trennzeichen 

					 if (mimetype[firstz] == '^') {
						 mimetype[firstz] = '\0';
						 firstz++;
						 strcpy(afrelation, mimetype + firstz);

					 };
				 };
			 };
			 

			 VpePDFEmbedFile(hDoc, sstripar, internename, sstripar + tende, mimetype, afrelation);
		 break;

		
		 case 'x': // 
				 // MINIMUM, BASIC WL, BASIC, EN 16931, EXTENDED, XRECHNUNG
		


		 VpeSetPDFALevel(hDoc, VPE_PDF_A_LEVEL_3B);

		 VpeAddColorProfile(hDoc,
			 "GTS_PDFA1",
			 "sRGB_IEC61966-2-1",
			 "Custom",
			 "http://www.color.org",
			 "sRGB_IEC61966-2-1",
			 "sRGB_IEC61966-2-1");



		 tende = strlen(sstripar) - 1;
		 for (; tende >= 0 && (sstripar[tende] != '|'); tende--);  // suche Trennzeichen 
		 if (tende > 0) {
			 sstripar[tende] = '\0';
			 tende++;
		 };


		/* _splitpath(sstripar, afrelation, internename, mimetype, lexte); // C4996

		 _strlwr(lexte);
		 _strlwr(mimetype);

		 strcpy(internename, mimetype);
		 strcat(internename, ".");
		 strcat(internename, lexte); */

		 strcpy(mimetype, "MINIMUM");  // echte Mime Type immer: "text/html");
		 strcpy(afrelation, "Alternative");

		 for (firstz = 0; (sstripar[firstz] != '>') && (sstripar[firstz] != '\0'); firstz++);  // suche Trennzeichen 

		 if (sstripar[firstz] == '>') {   // muss bei xml entweder: "factur-x.xml" oder "xrechnung.xml" 
			 sstripar[firstz] = '\0';
			 firstz++;
			 strcpy(internename, sstripar + firstz);

			 for (firstz = 0; (internename[firstz] != '?') && (internename[firstz] != '\0'); firstz++);  // suche Trennzeichen 
			 if (internename[firstz] == '?') {
				 internename[firstz] = '\0';
				 firstz++;

				 strcpy(mimetype, internename + firstz);    // mimetype da 

				 for (firstz = 0; (mimetype[firstz] != '^') && (mimetype[firstz] != '\0'); firstz++);  // suche Trennzeichen 

				 if (mimetype[firstz] == '^') {
					 mimetype[firstz] = '\0';
					 firstz++;
					 strcpy(afrelation, mimetype + firstz);

				 };
			 };
		 };


		 VpePDFEmbedFile(hDoc, sstripar, internename, sstripar + tende, "text/html", afrelation);



		 VpeSetFacturXParams(hDoc, internename, "INVOICE", "1.0", mimetype);

/*		 VpeSetPDFALevel(hDoc, VPE_PDF_A_LEVEL_3B);

		 VpeAddColorProfile(hDoc,
			 "GTS_PDFA1",
			 "sRGB_IEC61966-2-1",
			 "Custom",
			 "http://www.color.org",
			 "sRGB_IEC61966-2-1",
			 "sRGB_IEC61966-2-1");

		 VpePDFEmbedFile(hDoc, "nagy\\factur.xml", "factur-x.xml", "XML Rechnung MINI", "text/html", "Alternative");
		 VpeSetFacturXParams(hDoc, "factur-x.xml", "INVOICE", "1.0", "MINIMUM");
		 // MINIMUM, BASIC WL, BASIC, EN 16931, EXTENDED, XRECHNUNG(see ZUGfERD / Factur - X spec) */

		 break;


		 case 'r' : // Restriction
		  VpeSetEncryption(hDoc,DOC_ENCRYPT_STREAM);
  		  VpeSetProtection(hDoc,atoi(sstripar));
  	     break;

         case 't' : // Title
  		  VpeSetTitle(hDoc,sstripar );
		  break;

         case 'w' : // fastWeb
  		  VpeSetFastWebView(hDoc, true );
		  if (sstripar[0] == '0') VpeSetFastWebView(hDoc, false);
		  break;

		 case 'q' : // PICT Quality
  		  VpeSetDocExportPictureQuality(hDoc, PICEXP_QUALITY_HIGH );
		  if (sstripar[0] == '0') VpeSetDocExportPictureQuality(hDoc, PICEXP_QUALITY_NORMAL);
		  break;

		 case 'd': // DPI
			 VpeSetDocExportPictureResolution(hDoc, atoi(sstripar));
			 break;

 		 case 'b' : // Bookmark
   		     ParentBookmark =  VpeAddBookmark(hDoc, NULL, sstripar);
		   
			 break;

		 case 'c' : // Bookmark colors
			 COLORREF myc;

			 myc = COLOR_BLACK;

		   switch  ( sstripar[0] )
		   {
		    case  '1':  myc =  COLOR_LTGRAY ;   break;
			case  '2':  myc =  COLOR_GRAY ;     break;
			case  '3':  myc =  COLOR_DKGRAY ;   break;
			case  'W':  myc =  COLOR_WHITE ;	break;  
			case  'w':  myc =  COLOR_GRAY ;	    break;  
			case  'B':  myc =  COLOR_BLUE;	    break;  
            case  'b':  myc =  COLOR_DKBLUE ;   break;
			case  'Y': 	myc =  COLOR_YELLOW ;   break;  
            case  'y':  myc =  COLOR_DKYELLOW ; break;
			case  'r':  myc =  COLOR_DKRED ;    break;
			case  'R':  myc =  COLOR_RED ;      break;
			case  'G':  myc =  COLOR_GREEN;	    break;  
			case  'g':  myc =  COLOR_DKGREEN;   break;
			case  'O':  myc =  COLOR_ORANGE;    break;  
			case  'o':  myc =  COLOR_DKORANGE;  break;
			case  'c': 
			case  'C':  myc =  COLOR_CYAN   ;   break;
			case  'p':
			case  'P':  myc =  COLOR_PURPLE ;   break;
			case  'm':
			case  'M':  myc =  COLOR_MAGENTA;   break;
			case  'S':  myc =  COLOR_LTBLUE ;   break;  // Spezial
			case  's':  myc =  COLOR_OLIVE  ;   break;  // Spez.2

		   };

		   VpeSetBookmarkColor(hDoc,myc );
           int inblock;
		   inblock = 0;
		   if ( ( sstripar[1] == 'B' )) inblock =  VBOOKMARK_STYLE_BOLD;
		   if ( ( sstripar[1] == 'I' )) inblock =  VBOOKMARK_STYLE_ITALIC;
		   if ((sstripar[1] > '0') && (sstripar[1] < '8')) inblock = sstripar[1] - 48;
		   VpeSetBookmarkStyle(hDoc, inblock);
		   break;

		 case 'u' : // under Bookmark
			 if (ParentBookmark) VpeAddBookmark(hDoc, ParentBookmark, sstripar);
				else VpeAddBookmark(hDoc, NULL, sstripar);

			 
		   break;

	   }
	
}

void PrintString(char *t2Print)
{

	double  newheight, newwidth;

	VpeRenderPrint(hDoc, 0, 0, t2Print);
	newwidth = VpeGet(hDoc, VRENDERWIDTH);
	newheight = VpeGet(hDoc, VRENDERHEIGHT);

	// double lastwi =  ( fabs ( lastx +  newwidth ));
	double lastwi = lastx + newwidth;


#ifdef mitlogfile
	if (mitlog) {
		sprintf(mMsg, " VPEWrite: x:%#5.2f y:%#5.2f w:%#5.2f h:%#5.2f |%s \n", lastx, lasty - newheight + globczhoch, lastwi, lasty, t2Print);
		fputs(mMsg, flog);
	};
#endif

	// 		VpeWrite(hDoc,	 lastx + width ,  VFREE , lastwi , lasty + globczhoch , t2Print );
	VpeWrite(hDoc, lastx, lasty - newheight + globczhoch, lastwi, lasty + globczhoch, t2Print);

	lastx = lastwi;
};

void sfilepar(char steuchar,TCHAR* sstripar)
{

       switch  ( lower( steuchar ) )

      {
		  

/*  Umgestellt auf SET am 15/02/17
case 'p': // Drucken
		   if (icopar < 3) icopar++;
		   mycopar[icopar] = steuchar;
		   break;

	   case 'e': // Export 
		   if (icopar < 3) icopar++;
		   mycopar[icopar] = steuchar;
		   strcpy(mycoml[icopar], sstripar);
		   break;

	   case 'r': // run After 
		   if (icopar < 3) icopar++;
		   mycopar[icopar] = steuchar;
		   strcpy(mycoml[icopar], sstripar);
		   break;

	   case 'x': // exec sofort
		   int mypar;
		   mypar = 1;
		   if (steuchar == 'X') mypar = 0;
		   myRunP(sstripar, mypar);   
		   break;
*/

 	   //  case 'f' : // formular Name 
       //     if (hMenu != NULL )  EnableMenuItem(hMenu, ID_EDIT_EDITDFFORMULAR, MF_HILITE);
 	   //     VpeSetSubject(hDoc, sstripar );
	   //     break;

	     case 'a' : // File Archiv Mode
			 will_save = 1;



         case 's' : // will-save F2P in der Dateiname wird ersetzt durch
			will_save = 1;
			strcpy(myfname,(sreplace(myfname,"F2P",sstripar)));
 			//   VpeSetSubject(hDoc, sstripar );
		  break;

		 case 'n' : // will-save fixe Datei-Name
			will_save = 1;
			strcpy(myfname,sstripar);

		 case 'u': // nur Umwandeln - Datei-Name

			for (int sloop = 0; myfname[sloop] != '\0'; sloop++ ) {
		    if ( myfname[sloop] == ' ' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '/' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '\\' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '*' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == ':' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '|' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '?' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '>' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '<' ) myfname[sloop] = '_';
		    if ( myfname[sloop] == '"' ) myfname[sloop] = '_';
                                                          	  };
		  break;

		 case 'd' : // will-save directory
		 case 'o' : // will-save ordner
			strcpy(ufname,sstripar);
		  break;

	   }
	
}

void sinclpar(char steuchar, TCHAR* sstripar)
{

	char incfname[512];

	FILE *fh;
//	bool abruchwar = false;


	switch (lower(steuchar))

	{



	case 'f': // include Datei-Name
		if ((strlen(uincldir)) > 1) {
			strcpy(incfname, uincldir);
			strcat(incfname, "\\");
			strcat(incfname, sstripar);
		                      }
		else strcpy(incfname, sstripar);

		// setzt Steuver 1...5

		if ((fh = fopen(incfname, "rt")) != NULL) {
			int ilauf = 1;
			while (fgets(incfname, 512, fh) != NULL) {

				int tende = strlen(incfname) - 1;
					for (; tende >= 0 && incfname[tende] <= 32 && incfname[tende] > 0; tende--);  // strip LF, CR, Blank
				if (tende > 0) {
					incfname[tende +1] = '\0';
					strcpy(SteuBlock2[ilauf++], incfname);
				}
				else strcpy(SteuBlock2[ilauf++], "");;
				if (ilauf > 5) goto lassincl;

			};
		lassincl:
			fclose(fh);
		};
		break;

	case 'w': // weiter mit File
		myhoch = 100;
	case 'i': // weiter mit File
		if (myhoch != 100) myhoch = 101;
		if ((strlen(uincldir)) > 1) {
			strcpy(mMsgpar, uincldir);
			strcat(mMsgpar, "\\");
			strcat(mMsgpar, sstripar);
		}
		else strcpy(mMsgpar, sstripar);

		break;


	case 'd': // include directory
	case 'o': // bzw. ordner
		strcpy(uincldir, sstripar);
		break;

	}

}


// Forward-Declarations fuer cash_anz/cash_up (definiert weiter unten)
static bool CashAnz_IsDigitsOnly(const char *s);
static bool CashAnz_ReadIniBlock(const char *iniPath, int n, char *out1, int max1, char *out2, int max2);
static bool OpenDrawer_BREAK_Pulse(const char* portName, DWORD breakMs, DWORD pulseMs);

void ssetpar(TCHAR* steupar, TCHAR* sstripar)
{

	char tempstr[256];
	strcpy(tempstr, sstripar);

	char steuchar = steupar[0];
	char poschar = steupar[1];


	switch (lower(steuchar))

	{

	case 'd': // Export 
		strcpy(setfdir, sstripar);
		return;

	case 'f': // Export
		strcpy(setfname, sstripar);
		for (int sloop = 0; setfname[sloop] != '\0'; sloop++) {
			if (setfname[sloop] == ' ')  setfname[sloop] = '_';
			if (setfname[sloop] == '/')  setfname[sloop] = '_';
			if (setfname[sloop] == '\\') setfname[sloop] = '_';
			if (setfname[sloop] == '*')  setfname[sloop] = '_';
			if (setfname[sloop] == ':')  setfname[sloop] = '_';
			if (setfname[sloop] == '|')  setfname[sloop] = '_';
			if (setfname[sloop] == '?')  setfname[sloop] = '_';
			if (setfname[sloop] == '>')  setfname[sloop] = '_';
			if (setfname[sloop] == '<')  setfname[sloop] = '_';
			if (setfname[sloop] == '"')  setfname[sloop] = '_';
		};
		return;

	case 't': // Type ohne "." 
		strcpy(setdext, sstripar);
		return;

	case 'a': { // cash_anz: Kundenanzeige  (A=sofort, a=sofort)
		// sstripar = "port,text1,text2" z.B. "com4,Willkommen,Bitte warten"
		char ca_port[32] = "";
		char ca_text1[256] = "";
		char ca_text2[256] = "";
		char ca_buf[512];
		strncpy(ca_buf, sstripar, 511); ca_buf[511] = '\0';
		char *ca_p1 = strchr(ca_buf, ',');
		if (ca_p1) {
			*ca_p1 = '\0';
			strncpy(ca_port, ca_buf, 31); ca_port[31] = '\0';
			char *ca_p2 = strchr(ca_p1 + 1, ',');
			if (ca_p2) {
				*ca_p2 = '\0';
				strncpy(ca_text1, ca_p1 + 1, 255); ca_text1[255] = '\0';
				strncpy(ca_text2, ca_p2 + 1, 255); ca_text2[255] = '\0';
			} else {
				strncpy(ca_text1, ca_p1 + 1, 255); ca_text1[255] = '\0';
			}
		} else {
			strncpy(ca_port, ca_buf, 31); ca_port[31] = '\0';
		}
		// Blocknummer? (nur Ziffern -> Block aus cash_anz.ini lesen)
		if (CashAnz_IsDigitsOnly(ca_text1)) {
			char iniPath[MAX_PATH];
			GetModuleFileName(NULL, iniPath, MAX_PATH);
			char *ls = strrchr(iniPath, '\\');
			if (ls) *(ls + 1) = '\0';
			strcat(iniPath, "cash_anz.ini");
			CashAnz_ReadIniBlock(iniPath, atoi(ca_text1), ca_text1, 255, ca_text2, 255);
		}
		CashAnz_Run(ca_port, ca_text1, ca_text2);
		return;
	}

	case 'u': { // cash_up: Kassenladen oeffnen
		char cu_port[32];
		strncpy(cu_port, sstripar, 31); cu_port[31] = '\0';
		int cu_len = strlen(cu_port);
		if (cu_len > 0 && cu_port[cu_len - 1] == ':') cu_port[cu_len - 1] = '\0';
		if (!cu_port[0]) strcpy(cu_port, "COM3");
		if (!OpenDrawer_BREAK_Pulse(cu_port, 250, 200)) {
			sprintf(mMsg, "cash_up: COM-Port '%s' nicht erreichbar!\n(Error %lu)", cu_port, GetLastError());
			Msg(mMsg);
		}
		return;
	}

	case 'x': // exec sofort
		int mypar;
		mypar = 0;
		if (steuchar == 'X') mypar = 1;
		myRunP(sstripar, mypar);
		return;

	case '?': // Export 

		return;

	}

	int  welche = -1;
	if (poschar == '1') welche = 0;
	if (poschar == '2') welche = 1;
	if (poschar == '3') welche = 2;
	if (poschar == '4') welche = 3;

	if (welche < 0) {
		sprintf(mMsg, "SET - PAR Argument ung�ltig...\n\n(%s)\n(%s)\n", steupar, sstripar);
		Msg(mMsg);
		return;
	                };

	icopar = welche;

	char *pdest;
	int myluaf = 0;

turepl:
	pdest = (char *)strstr(tempstr, "%%"); // eigentlich 3 Zeichen
	if (pdest != NULL) {
		strcpy(tempstr, (sreplace(tempstr, "%%f", setfname)));
		strcpy(tempstr, (sreplace(tempstr, "%%d", setfdir)));
		strcpy(tempstr, (sreplace(tempstr, "%%t", setdext)));
		strcpy(tempstr, (sreplace(tempstr, "%%i", myindis)));
		if (myluaf++ < 3) goto turepl;
	};

	switch (lower(steuchar))

	{

	case 'p': // Drucken
		mycopar[icopar] = steuchar;
		break;

	case 'e': // Export 
		mycopar[icopar] = steuchar;
		strcpy(mycoml[icopar], tempstr);
		break;

	case 'r': // run After 
		mycopar[icopar] = steuchar;
		strcpy(mycoml[icopar], tempstr);
		break;

	}

}

void smailpar(char steuchar,TCHAR* sstripar)
{

       switch  ( lower( steuchar ) )

      {

	    case 's' : // Subject 
  		  VpeSetMailSubject(hDoc, sstripar);
		  break;

         case 'p' : // Protokoll
             strncpy( usetdir , sstripar,98);
		  break;

         case 't' : // Receiver
  		  VpeAddMailReceiver(hDoc,sstripar,1 );
		  break;
		 case 'c' : // Receiver
  		  VpeAddMailReceiver(hDoc, sstripar,2 );
		  break;
         case 'b' : // Receiver
  		  VpeAddMailReceiver(hDoc, sstripar,3 );
		  break;
   
         case 'a' : // Adresse 
  		  VpeSetMailSender(hDoc, sstripar );
		  break;
					   
         case 'z' : // Attachment
  		   VpeAddMailAttachment(hDoc, sstripar, NULL );
		  break;
 
         case 'w' : // Attachment
			  VpeClearMailAttachments(hDoc);
			  // strcpy(fname,sstripar);
  		     	TCHAR dir[164] ;

              _makepath( dir ,  drive, mdir, sstripar , ".pdf" );
    		  VpeAddMailAttachment(hDoc,dir, NULL );
		  break;

        case 'n' : // ClearAttachment
			  VpeClearMailAttachments(hDoc);
		  break;

         case 'm' : // mailen 
          if (sstripar[0] == '0') VpeSetMailWithDialog(hDoc, false);
		                     else VpeSetMailWithDialog(hDoc, true);
		  break;


 	     case 'f' : // Subject 
             if (hMenu != NULL )  EnableMenuItem(hMenu, CM_EDITMAILSET, MF_GRAYED);
 			 VpeSetSubject(hDoc, sstripar );
		  break;


	   }
	
}


void ExportDocument( TCHAR* FName, int mitshell)
{

 	TCHAR dir[164] = "\0";
	mMsg[0] = '\0';

	if ( strlen(FName) > 4 )  strcpy(dir, FName);
	else {

	if (! strcmp( FName , "1")) {
 	   SHGetFolderPath(NULL,  CSIDL_DESKTOPDIRECTORY, 
                             NULL, 
                             0, 
                             mMsg); 
	};
	if (! strcmp( FName , "2")) {
 	   SHGetFolderPath(NULL,  CSIDL_DESKTOPDIRECTORY, 
                             NULL, 
                             0, 
                             mMsg); 
	   strcat(mMsg,"\\f2p");
	};
	if (! strcmp( FName , "3")) {
 	   SHGetFolderPath(NULL,  CSIDL_MYDOCUMENTS, 
                             NULL, 
                             0, 
                             mMsg); 

	};
	 
	if (strlen(mMsg) > 5 ) sprintf(dir, "%s\\%s",mMsg,myfname);

	if (! strcmp( FName , "=")) {
 	               _makepath( dir ,  drive, mdir, fname, ".pdf" );
                                }
	if (! strcmp( FName , "!")) {
 	               sprintf( dir , "%s\\%s", ufname, myfname );
				   will_save = 0;
                                }
 
	if ( strlen(dir) < 3)  wsprintf(dir, _T("%s\\f2p.pdf"), GetTmpDir());
   
	}
   
	if ( ! owncreate ) VpeSetCreator(hDoc, "F2P.exe by f.nagy using VPE 7.40 (64Bit)" );



	if (directpdf) {
		SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY,
			NULL,
			0,
			dir);

		strcat(dir, "\\f2p\\");

		GetLocalTime(&st);
		int i;

		i = st.wMilliseconds;
		i = i + (1000 * st.wSecond);
		i = i / 100;

		strcpy(mMsg, "und");
		switch (st.wMonth)
		{

/*		case 1:  strcpy(mMsg, "I"); break;
		case 2:  strcpy(mMsg, "II"); break;
		case 3:  strcpy(mMsg, "III"); break;
		case 4:  strcpy(mMsg, "IV"); break;
		case 5:  strcpy(mMsg, "V"); break;
		case 6:  strcpy(mMsg, "VI"); break;
		case 7:  strcpy(mMsg, "VII"); break;
		case 8:  strcpy(mMsg, "VIII"); break;
		case 9:  strcpy(mMsg, "IX"); break;
		case 10: strcpy(mMsg, "X"); break;
		case 11: strcpy(mMsg, "XI"); break;
		case 12: strcpy(mMsg, "XII"); break;
*/

		case 1:  strcpy(mMsg, "Jan"); break;
		case 2:  strcpy(mMsg, "Feb"); break;
		case 3:  strcpy(mMsg, "Mar"); break;
		case 4:  strcpy(mMsg, "Apr"); break;
		case 5:  strcpy(mMsg, "May"); break;
		case 6:  strcpy(mMsg, "Jun"); break;
		case 7:  strcpy(mMsg, "Jul"); break;
		case 8:  strcpy(mMsg, "Aug"); break;
		case 9:  strcpy(mMsg, "Sep"); break;
		case 10: strcpy(mMsg, "Oct"); break;
		case 11: strcpy(mMsg, "Nov"); break;
		case 12: strcpy(mMsg, "Dec"); break; 
		};

		char mytindis[17];
		sprintf(mytindis, "%02i%s_%02i%02i_%03i.pdf", st.wDay, mMsg, st.wHour, st.wMinute, i);
		strcat(dir, mytindis);

	};

	rueckmcode = 0;
	_int64 start;
	if (wartime)  QueryPerformanceCounter((LARGE_INTEGER *)&start);
		

    if (VpeWriteDoc(hDoc, dir) )
		{
		if (wartime) {
			_int64 now, freq;
			QueryPerformanceCounter((LARGE_INTEGER *)&now);
			QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
			sprintf(mMsg, "PDF Document successfully created!\nElapsed Time: %.3f seconds", ((double)(now - start) / (double)freq));
			MessageBox(hMainWindow, mMsg, "Message", MB_OK);
  	            	};
//      	MessageBox(hWnd, "", szAppName, MB_OK | MB_ICONINFORMATION);

  if (clipupdate) Drop2Clip(dir);

  rueckmcode = 1;

  if (mitshell) {

	  if (shellexmode == '0') {  // mit warten
		  sprintf(mMsg, "pdf %s", dir);
		  if (ypar[0] > ' ') sprintf(mMsg, "pdf %s -y%s", dir,ypar);
		  myRunP(mMsg, 0);
		  return;
	  };

	  if (shellexmode == '1') {  // ohne warten
		  sprintf(mMsg, "pdf %s", dir);
		  if (ypar[0] > ' ') sprintf(mMsg, "pdf %s -y%s", dir, ypar);
		  myRunP(mMsg, 1);
		  return;
	  };
	  if (shellexmode == '7') {  // ohne warten und beenden
		  sprintf(mMsg, "pdf %s", dir);
		  if (ypar[0] > ' ') sprintf(mMsg, "pdf %s -y%s", dir, ypar);
		  myRunP(mMsg, 1);
		  VpeSendKey(hDoc, VKEY_CLOSE);  // PostMessage(hwnd, WM_CLOSE, 0, 0);

		  return;
	  };



	  ShellExecute(0, NULL, dir, NULL, NULL, SW_SHOWMAXIMIZED);

               };

    VpeWriteStatusbar(hDoc, "VPE/PDF Document created!");

		}
		else
		{
	
		sprintf(mMsg, "Error creating PDF file!\n%s\n\n"
			"Possible reasons:\n"
			" - the file is open in Acrobat Reader\n"
			" - no access rights to the folder\n\n"
			  "(mit Ctrl+ zu tmp on desktop)\n"  , dir);
      Msg(mMsg);
      if (GetAsyncKeyState (VK_CONTROL) < 0 ) {
		  			strcpy(myfname,(sreplace(myfname,"F2P_","TMP~")));
					ExportDocument( "2", mitshell);
	                                          };
		}
//	}
}

 double myposfunc(char *s)    // zB. ( 12 + L / @
 {

  double myferg = 0;
//  double myzwerg = 0;
  char zwiss[5];
  char vpegch;

  boolean nachplus = true;
  char *p;
   while (*s)
   {
   p = (char *)strchr( "+-LRTBlrtbWHXYwhxy=aucMZmzdgo]@?!%)", *s );   // ist in Steuerung Codes
  if ( p != NULL ) {
	   vpegch = *s;
       s++;
	   	   switch  ( vpegch )   
      { 

        case '+' :  nachplus = true; 
			        break;
        case '-' :  nachplus = false; 
			        break;
        case '=' :  
        case '!' :         
        case ']' :  break;

		case ')' :  return myferg;
		default  :
			   zwiss[0] = vpegch;
			   zwiss[1] = '\0';

			   if (vpegch == 'm') {
				   zwiss[1] = *s;
				   s++;
				   zwiss[2] = '\0';
			   };
			   if (nachplus) myferg = myferg + (mypos(zwiss));
			   else myferg = myferg - (mypos(zwiss));
		   }  // end case
                  }
  else {
	   p = (char *)strchr( "0123456789." , *s );   // fangt Zahl an

	   if ( p != NULL ) {
		          int ilauf = 0;
      zaehlweit:
 				  zwiss[ilauf] = *s;
				  s++;
				  ilauf++;
			  	  p = (char *)strchr( "0123456789." , *s );   // ist noch Zahl
				  if ( p != NULL ) goto zaehlweit;
				  zwiss[ilauf] = '\0';
			   if (nachplus) myferg = myferg + (atof(zwiss));
			            else myferg = myferg - (atof(zwiss));

                  	   }
	   else 	  s++;

     };

   }; // end_while

 
  return myferg;


 };

 double mypos(char *s)
{

  char vpegch;
  char vpemgch = ' ';
  char *p;
  bool relpos = false;
nocheinch:
  vpegch = *s;
  if (vpegch == ' ') {
	  s++;
	  goto nocheinch;
  };
  p = (char *)strchr("LRTBlrtbWHXYwhxy=aucMZmzdgo]@?!%(", *s);   // ist in Steuerung Codes
  if ( p != NULL ) {
	   vpegch = *s;
       s++;
	   if (vpegch == 'm') {
		   vpemgch = *s;
		   s++;
	   };
	   if (vpegch == ']') goto nocheinch;
	   if ( vpegch == '(' ) {
		               if (relpos)  return ( -1 * myposfunc(s)) ;
                       return myposfunc(s);
	   };
	   if ( vpegch == '!' ) {
		                      relpos = true;
							  goto nocheinch; 
                             }
                     }
  else return atof(s);


  bool positivewert = true;
  double i = 0;
  double vpei = 0;

  // char operch = ' ';

   while (*s)
   {
	   if (*s == '-') {
		        positivewert = false;
				s++;
	                  };

	   
	   p = (char *)strchr( "0123456789." , *s );   // ist in Steuerung Codes
	   if ( p != NULL ) {
		          i = atof(s);
				  break;
                  	   };
      s++; 
   } 

   
 //  if ( vpegch > ' ' ) {
    
	   switch  ( vpegch )   
      { 

        case '=' : return ( VFREE );
        case '@' : vpei = VpeGet(hDoc, VRENDERWIDTH );    break;
		case '%' : vpei = VpeGet(hDoc, VRENDERHEIGHT);    break;

		case 'W': vpei = VpeGetDevPrintableWidth(hDoc) + VpeGetDevPrinterOffsetX(hDoc);  break;
		case 'H': vpei = VpeGetDevPrintableHeight(hDoc) + VpeGetDevPrinterOffsetY(hDoc); break;

		case 'X': vpei = myx;  break;
		case 'Y': vpei = myy; break;


		//case 'X':
		case 'x':
			vpei = VpeGet(hDoc, VLEFT);                break;
        case 'c' : vpei = VpeGet(hDoc,VRIGHT);         break;
		case 'y':
		// case 'Y':
			vpei = VpeGet(hDoc, VTOP);                 break;
        case 'd' : vpei = VpeGet(hDoc,VBOTTOM);        break;

        case 'a' : vpei = lasty;                       break;
        case 'u' : vpei = lasty + globczhoch ;         break;
        case 'o' : vpei = lasty  - globczhoch ;        break; 
        case 'g' : vpei = globczhoch;                  break;

        case 'k' : vpei = kopfab;                       break;
        case 'f' : vpei = fussbis;                      break;
        case 'i' : vpei = indirand;                     break;
        case 'K' : vpei = VpeGet(hDoc,VTOPMARGIN)    + kopfab ;        break;
        case 'F' : vpei = VpeGet(hDoc,VBOTTOMMARGIN) - fussbis;        break;
        case 'I' : vpei = VpeGet(hDoc,VLEFTMARGIN)   + indirand;       break;

		case 'L' : vpei = VpeGet(hDoc,VLEFTMARGIN);   break;
        case 'T' : vpei = VpeGet(hDoc,VTOPMARGIN);    break;
        case 'R' : vpei = VpeGet(hDoc,VRIGHTMARGIN);  break;
        case 'B' : vpei = VpeGet(hDoc,VBOTTOMMARGIN); break;
	    
        case 'l' : vpei = VpeGetDevPrinterOffsetX(hDoc)  ;  break;
        case 't' : vpei = VpeGetDevPrinterOffsetY(hDoc);  break;
        case 'r' : vpei = VpeGetDevPrintableWidth(hDoc) + VpeGetDevPrinterOffsetX(hDoc) ;  break; 
        case 'b' : vpei = VpeGetDevPrintableHeight(hDoc) + VpeGetDevPrinterOffsetY(hDoc) ; break;
        
	//	case 'W': 
		case 'w': 
			vpei = VpeGetDevPaperWidth(hDoc);  break;
		case 'h': vpei = VpeGetDevPaperHeight(hDoc); break;
        case 'M' : vpei = ( ((VpeGet(hDoc,VRIGHTMARGIN) - VpeGet(hDoc,VLEFTMARGIN) )  / 2 ) + VpeGet(hDoc,VLEFTMARGIN)) ;  break;
        case 'Z' : vpei = ( ((VpeGet(hDoc,VBOTTOMMARGIN) - VpeGet(hDoc,VTOPMARGIN) )  / 2 ) + VpeGet(hDoc,VTOPMARGIN)); break;
     
		case 'm' :
			switch (vpemgch)
			{
			case '1': vpei = mymerklasty1; break;
			case '2': vpei = mymerklasty2; break;
			case '3': vpei = mymerklasty3; break;

			case 'y': vpei = mymerklastrendy; break;
			case 'x': vpei = mymerklastrendx; break;
//			case 's': vpei = mymerklasty3; break;


			case '?':
				vpei = mymerklasty1;
				if (mymerklasty2 > vpei) vpei = mymerklasty2;
				if (mymerklasty3 > vpei) vpei = mymerklasty3;
				break;

			default :
				vpei = mymerky;


			};   // end-switch

			break;

        case 'z' : vpei = ( VpeGetDevPaperHeight(hDoc) / 2 ); break;
	  }; 
		

 //  };

 if ( !relpos ) {
   if ( positivewert ) return ( vpei + i );
   return ( vpei - i );
 }
   if ( positivewert ) return ( -1 * ( vpei + i ) );
   return ( -1 * ( vpei - i ));
  
}


   void xMixRTFFile(VpeCAnsiString file_name)
   {
     FILE *lfh;
//     bool abruchwar = false;
	 if ( ! mitRTFchars ) {
			 strcpy( myRTFline,"RTF in doc nicht FREIgeschaltet... (siehe dos++ in header)");
             return;
	 };

	 if ((lfh=fopen(file_name, "rb")) != NULL) {


	   int myfilel;
	   int myfh = _fileno( lfh );
	   myfilel = _filelength( myfh);
	   if ( myfilel < 32000 ) {
		int eingel;
		eingel = fread( myRTFline, sizeof( char ),  myfilel, lfh ); 
	    if ( eingel != NULL) {
			eingel++;                                  // damit auch NULL dabei
			myRTFline[eingel] = '\0';

     char *pdest;
     int  result = 1 ;
     int  oresult;
	 myfilel = eingel;
	 char zwiline[135];
ersenomal:
     pdest = (char *) strstr(  myRTFline + result ,"%%"  ); // eigentlich 3 Zeichen
     if ( pdest != NULL ) {
 		    result = (int)(pdest - myRTFline);
			int lauf = myfilel;
			oresult = result;

 			  char *pdest2; 
	  		  int inblock = -1;

			  pdest2 = (char *)strchr(steuvars, (lower(myRTFline[oresult + 2])));
   	          if ( pdest2 != NULL ) {
			   inblock = (int)(pdest2 - steuvars );
			                        }
			  else {

           if ( myRTFline[oresult + 2] == '<' ) {  // dump von characterset 129 ... 192
			     inblock = 99;
	   	  		 for (int li = 0; li < 64; li++)  zwiline[li] = li + 128;
			
			  };
           if ( myRTFline[oresult + 2] == '>' ) { // dump von characterset  192 ... 255
		     inblock = 99;
 	  		 for (int li = 0; li < 64; li++) {  
 				  zwiline[li] = li + 192;
			 };
			  };


			       };


        if ( inblock >= 0 ) {

			  if ( inblock < 41 ) strncpy(zwiline,SteuVar[inblock],255);

 	  	      int solazwi = strlen(zwiline);
			  if ( solazwi < 1 ) {
 	  	         strcpy(zwiline,"%%?0?");
				 zwiline[3] = myRTFline[oresult + 2];
 	  	         solazwi = strlen(zwiline);
			  };


			    if (solazwi <= 3 ) {

					myRTFline[result]   = zwiline[0];
					myRTFline[result+1] = zwiline[1];
					myRTFline[result+2] = zwiline[2];

			if (solazwi != 3 ) {
				lauf = oresult + 1;
				if ( solazwi == 2 ) lauf = oresult + 2; 
                 solazwi = 3 - solazwi;
			for ( ; lauf < myfilel; lauf++) {
			myRTFline[lauf] = myRTFline[lauf + solazwi];
				};

			myfilel = myfilel - solazwi;

			};

				}
				else {   
			    solazwi = solazwi - 3;


			for ( ; lauf >= result + 2; lauf--) {
			myRTFline[solazwi + lauf] = myRTFline[lauf];
 			};

			myfilel = myfilel + solazwi;
			solazwi = solazwi + 2;  // sonst (mit 3) ist \0 auch dabei
			for  ( ; solazwi >= 0; solazwi-- ) {
			myRTFline[oresult + solazwi ] = zwiline[solazwi];
			};
				};
			};
			// myRTFline[oresult ] = zwiline[0];

			    result++;
			    result++;

         goto ersenomal; 

	 };
	 

		                                            };
	                         }
	   else {
		    
		     sprintf(mMsg, "RTF Datei zu gross: %s\n",file_name);
			 Msg(mMsg);
	   }

	   fclose( lfh );
	 

                 }

	   else {
	       strcpy(myRTFline, "");

	//	     sprintf(mMsg, "kann RTF Datei nicht finden: %s\n",file_name);
	//		 Msg(mMsg);
	         }


   }

 void myPBreak()
 {
	 
#ifdef mitlogfile
	 if (mitlog) {
		 sprintf(mMsg, "< PageBreak|glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f \n", glasty, fussbis, lasty);
		 fputs(mMsg, flog);
	 };
#endif


	 if (mypcontrol) {
		 int amzp = VpeGetPageCount(hDoc);
	  	 if ( amzp == 1 )   Eincharp('E');
		 else Eincharp('N');
                 	 };
     VpePageBreak(hDoc);

	 if ( mitvorschau )  {

	if ( !warprev )  {
		int amzp = VpeGetPageCount(hDoc);
		if (amzp > 17)  {

			mypaintmode = 1;
			VpePreviewDoc(hDoc, NULL, VPE_SHOW_MAXIMIZED);
			SetFocus(VpeGetWindowHandle(hDoc));
			warprev = 1;
		};
	   VpeDispatchAllMessages(hDoc)  ; //  abruchwar = true;
	}
	//       if  ( abruchwar )  goto ibinfertig;
	    }
	   lasty =  VpeGet(hDoc, VTOPMARGIN);
	   if (mypcontrol)  Eincharp('P');
	   lasty =  VpeGet(hDoc, VTOPMARGIN) + kopfab;
	   if (mypcontrol) {
		       Eincharp('Z');
	   };

	  obanzonpage = 0;

 }


int myPosin (char myChar)
{
  	     char *pdest;                //     const char steustri[28] = "IUuFfSsBbQqP-0123456789ENL"; 
		 pdest = (char *) strchr( steustri , myChar);   // ist in Steuerung Code ES MUSS sein
		 if ( pdest != NULL ) return (int)(pdest - steustri);
		 return -1;
};

void SetxPath(char *spar, char *par5 )
{
	if ((spar[0] != '\\') && (spar[1] != ':'))  {
		strcpy(xppfad, curexedir);
		strcat(xppfad, spar);
	}
	else strcpy(xppfad, spar);

	char *pdest;

	pdest = (char *)strstr(xppfad, "%%"); // eigentlich 3 Zeichen
	if (pdest != NULL) {
		strcpy(xppfad, (sreplace(xppfad, "%%p", SteuVar[25])));
		strcpy(xppfad, (sreplace(xppfad, "%%q", SteuVar[26])));
		strcpy(xppfad, (sreplace(xppfad, "%%r", SteuVar[27])));
		strcpy(xppfad, (sreplace(xppfad, "%%%", aktulang)));
	};

	char tppfad[_MAX_PATH];
	char tdrive[_MAX_DRIVE];
	char tdir[_MAX_DIR];
	char tfname[_MAX_FNAME];
	char t_ext[_MAX_EXT];

	_splitpath(xppfad, tdrive, tdir, tfname, t_ext);  // nur mit ext. zulassen

	if (t_ext[0] != '.')  {
		if (par5[0] != '.')  {
		strcpy(xppfad, " ");
		return;
                         	};
	};

		pdest = (char *)strstr(par5, "^"); // kontrollieren ob Datei existiert bzw. fallback 
	if (pdest != NULL) {
		FILE *tfh;
		if ((tfh = fopen(xppfad, "r")) != NULL) {
			fclose(tfh);
			goto incfok;
		};



		int ilauf = strlen(tdir);
		ilauf--;
		while (ilauf-- > 0) {
			if (tdir[ilauf] == '\\') {
				tdir[ilauf + 1] = '\0';
				ilauf = 0;
			}
		};


		_makepath(tppfad, tdrive, tdir, tfname, t_ext); // C4996

		if ((tfh = fopen(tppfad, "r")) != NULL) {
			fclose(tfh);
			strcpy(xppfad, tppfad);
			goto incfok;
		};

		pdest = (char *)strstr(par5, "^^"); // kontrollieren ob Datei existiert bzw. fallback 
		if (pdest != NULL) {
			int ilauf = strlen(tdir);
			ilauf--;
			while (ilauf-- > 0) {
				if (tdir[ilauf] == '\\') {
					tdir[ilauf + 1] = '\0';
					ilauf = 0;
				}
			};

			_makepath(tppfad, tdrive, tdir, tfname, t_ext); // C4996

			if ((tfh = fopen(tppfad, "r")) != NULL) {
				fclose(tfh);
				strcpy(xppfad, tppfad);
				goto incfok;
			};

			pdest = (char *)strstr(par5, "^^^"); // kontrollieren ob Datei existiert bzw. fallback 
			if (pdest != NULL) {
				int ilauf = strlen(tdir);
				ilauf--;
				while (ilauf-- > 0) {
					if (tdir[ilauf] == '\\') {
						tdir[ilauf + 1] = '\0';
						ilauf = 0;
					}
				};

				_makepath(tppfad, tdrive, tdir, tfname, t_ext); // C4996

				if ((tfh = fopen(tppfad, "r")) != NULL) {
					fclose(tfh);
					strcpy(xppfad, tppfad);
					goto incfok;
				};

			};


		};





	incfok:;

	};


};


void PrintSetGleich( char *t2Print)
	// [= 12,l
  {

      int sserg = 0;
  	  char *spar;
	  char *par1, *par2, *par3, *par4, *par5;
	  par1 = t2Print + 2;		
	  
	  // sserg = sscanf( t2Print + 3, "%[^,],%[^,],%[^,],%[^,]",par1,par2,par3,par4 );  ( jetzt ohne par4 ! )
	  for (int sscanloop = 2 ; t2Print[sscanloop] != '\0'; sscanloop++ ) {
		  
		  if ( t2Print[sscanloop] == ',' ) {
			     t2Print[sscanloop] = '\0';
			     sserg++;
		 	     if ( sserg == 1 ) par2 = t2Print + sscanloop + 1;
		 	     if ( sserg == 2 ) par3 = t2Print + sscanloop + 1;
		 	     if ( sserg == 3 ) par4 = t2Print + sscanloop + 1;
		 	     if ( sserg == 4 ) par5 = t2Print + sscanloop + 1;
		 	     if ( sserg == 5 ) {
					 spar = t2Print + sscanloop + 1;
					 break;
				                   }
		                                   }

	  };

      sserg++;  


 	  if (sserg > 2 ) {

		  if (!_strnicmp(par1, "pdf", 3)) {
			  spdfpar(par2[0], par3);
			  return;
		  };
		  if (!_strnicmp(par1, "file", 4)) {
			  sfilepar(par2[0], par3);
			  return;
		  };
		  if (!_strnicmp(par1, "incl", 4)) {
			  sinclpar(par2[0], par3);
			  return;
		  };

		  if (!_strnicmp(par1, "set", 3)) {
			  // cash_anz ('a'/'A'): Kommas in par3 wiederherstellen,
			  // da ssetpar "port,text1,text2" als ein String erwartet
			  if ((par2[0] == 'a' || par2[0] == 'A') && sserg > 3) {
				  par3[strlen(par3)] = ',';   // Komma vor par4
				  if (sserg > 4)
					  par4[strlen(par4)] = ',';   // Komma vor par5
			  }
			  ssetpar(par2, par3);
			  return;
		  };
		  
		  if (!_strnicmp(par1, "mail", 4)) {
			  smailpar(par2[0], par3);
			  return;
		  };




            if (!_strnicmp( par3 , "xy",2)) {
					   lastx  = mypos(par1); 
					   lasty  = mypos(par2); 
					   return; 

			};

			if (!_strnicmp(par3, "my", 2)) {
				myx = mypos(par1);
				myy = mypos(par2);
				return;

			};



            if (!_strnicmp( par3 , "kf",2)) {
					   kopfab   = mypos(par1); 
					   fussbis  = mypos(par2); 
					   return; 

			};



            if (!_strnicmp( par3 , "lr",2)) {
					   VpeSet(hDoc,VLEFTMARGIN,mypos(par1)); 
					   VpeSet(hDoc,VRIGHTMARGIN,mypos(par2));
					   return; 
					                        };
            if (!_strnicmp( par3 , "tb",2)) {
			            VpeSet(hDoc,VTOPMARGIN,mypos(par1));
						VpeSet(hDoc,VBOTTOMMARGIN,mypos(par2)); 
					   return; 
					                        };

			if (!_strnicmp(par3, "wh", 2)) {
				VpeSetPageWidth(hDoc, mypos(par1));
				VpeSetPageHeight(hDoc, mypos(par2));
				return;
			};

		   if (!_strnicmp( par3 , "cp",2)) {

		     VpeSetCharset(hDoc,  atoi(par2) );
			 oemconvert = false;
			 oemcodepage = 0;
			 if (  atoi(par1) > 0 ) oemconvert = true;
			 if (  atoi(par1) > 1 ) oemcodepage =  atoi(par1);
			 oemcodepageback = 1250;
			 if ( oemcodepage == 855 ) oemcodepageback = 1251;
			 if ( oemcodepage == 866 ) oemcodepageback = 1251;
			 if ( oemcodepage == 775 ) oemcodepageback = 1257;
			 if ( oemcodepage == 850 ) oemcodepageback = 1252;
			 if ( oemcodepage == 858 ) oemcodepageback = 1252;
//			 if ( sserg == 4 ) oemcodepageback = atoi(par4);
					   return; 
					   };

		   if (!_strnicmp(par3, "cf", 2)) {

				   foemcodepage    = atoi(par1);
				   foemcodepageback = atoi(par2);
			   return;

		   };



               if (!_strnicmp( par3 , "ch",2)) {
 
                   if (strcmp( par1 , "=")) {
					   czbreit  = mypos(par1); 
				   }
				   else {
                      VpeRenderPrint(hDoc,0,0,"X");
                      czbreit  = VpeGet(hDoc, VRENDERWIDTH);
				   };

                   if (strcmp( par2 , "=")) {
					   czhoch   = mypos(par2);  
				   }
				   else {
                      VpeRenderPrint(hDoc,0,0,"X");
					  czhoch = VpeGet(hDoc, VRENDERHEIGHT);   
				   };


				   				      return;

					   };


              // Fehlermeldung wrong Par3

			 if (!_strnicmp(par3, "ct", 2)) {  // current time
 

				 	LARGE_INTEGER mystart;
					mystart.HighPart = atol(par1);
					mystart.LowPart  = atol(par2);
					if (par3[2] == '0') {
						time0 = mystart.QuadPart;
						wartime = true;
				                  	    };
					if (par3[2] == '1') time1 = mystart.QuadPart;
					if (par3[2] == '9') time9 = mystart.QuadPart;
					return;

			 };

			 if (!_strnicmp(par3, "dt", 2)) {  // Delta Time zu time


				 LARGE_INTEGER mystart;
				 mystart.HighPart = atol(par1);
				 mystart.LowPart  = atol(par2);

				 _int64  now, freq;
				 now = mystart.QuadPart;
				 QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
				 sprintf(mMsg, "['Arial' S 8 B C]Time F2P: %.3f sec.;  Info: %s", ((double)(now - time0) / (double)freq), par3);
				 PrintString(mMsg);
				 return;

			 };



	      return;
		   
				                    }


		  if ( sserg == 2 ) {
					  switch  ( lower( par2[0] ) ) {
					  case '?': {
									double zwipos;
									char zwiline[65];

									zwipos = mypos(par1);
									sprintf(zwiline, "%s:%#5.2f", par1, zwipos);
//									sprintf(zwiline, ":%#5.2f",  zwipos);
									PrintString(zwiline);
									break;
					              };
					    case 'x' : lastx    = mypos(par1); break;
						case 'y' : lasty    = mypos(par1); break;
						case 'k' : kopfab   = mypos(par1); break;
						case 'f' : fussbis  = mypos(par1); break;
						case 'h' : czhoch   = mypos(par1); break;
						case 'i':
						//	lasty = mymerky;
						//	myhoch   = mypos(par1); 
							lasty = mymerky + mypos(par1);
							break;

						case 'g' : globczhoch   = mypos(par1); break;
						case 'z' : glasty    = mypos(par1); break;

						case 'e' : 
							VpeSetPaperBin(hDoc, atoi(par1));
				//			VpeSetDevPaperBin(hDoc, atoi(par1));
							break;
						case 'd' : myDuplex = atoi(par1); break;
						case 'o': {

									  VpeSetPageOrientation(hDoc, atoi(par1));
									  break;
						};

						case 's' : VpeSetCornerRadius(hDoc, mypos(par1)); break;


						case 'p' : 
							if (oldzwmode) {
								myPBreak();
								myhoch = VpeGet(hDoc, VTOPMARGIN) - czhoch;  // so wird auch lasty ge�ndert
								break;
							};
							VpeSetCharPlacement(hDoc, mypos(par1)); break;
						case 'a' : VpeSetAutoBreak(hDoc, atoi(par1)); break;
						case 'c' : VpeSetDevCopies(hDoc, atoi(par1)); break;
						case 'l' : VpeSet(hDoc,VLEFTMARGIN,mypos(par1));    break;
					    case 'r' : VpeSet(hDoc,VRIGHTMARGIN,mypos(par1) );  break;
			            case 't' : VpeSet(hDoc,VTOPMARGIN,mypos(par1));     break;
						case 'b' : 
							if (oldzwmode) {
								myPBreak();
								myhoch = VpeGet(hDoc, VTOPMARGIN) - czhoch;  // so wird auch lasty ge�ndert
								break;
							};
							VpeSet(hDoc,VBOTTOMMARGIN,mypos(par1) );
							break;
						case 'u':  {
									   if ((lower(par2[1])) == 'o') {
										   oemcodepage = (atoi(par1)); 
										   break;
									   };
									oemcodepageback = (atoi(par1)); break;

						};
						case 'w': 	wantbreak = true;  break;

						case 'v': 
							VpeSetCornerRadius(hDoc, 0);
							VpeSetBkgGradientRotation(hDoc, 0);
							if (par1[0] == 'H') VpeSetBkgGradientRotation(hDoc, 900);
							if (par1[0] == 'v') VpeSetBkgGradientRotation(hDoc, 1800);
							if (par1[0] == 'h') VpeSetBkgGradientRotation(hDoc, 2700);
							if ((par1[1] >= '0') && (par1[1] <= '9'))  {
								if ((par1[2] >= '0') && (par1[2] <= '9'))  {
									int zwint;
									zwint = (10 * (par1[1] - 48)) + (par1[2] - 48);
									VpeSetBkgGradientMiddleColorPosition(hDoc, zwint);

								};
							};




								break;
						case 'm':
							switch (lower(par1[0])) {
							case 'm': 
								mymerky      = lasty; 
								break;
							case '1': mymerklasty1 = lasty; break;
							case '2': mymerklasty2 = lasty; break;
							case '3': mymerklasty3 = lasty; break;

							case 'a': mymerklasty1 += mymerklastrendy; break;
							case 'b': mymerklasty2 += mymerklastrendy; break;
							case 'c': mymerklasty3 += mymerklastrendy; break;

							case 'x': mymerklastyx = lasty; pagex = VpeGetCurrentPage(hDoc);  break;
							case 'y': mymerklastyy = lasty; pagey = VpeGetCurrentPage(hDoc);  break;
							case 'z': mymerklastyz = lasty; pagez = VpeGetCurrentPage(hDoc);  break;

							case '0':
								mymerklasty1 = 0;
								mymerklasty2 = 0;
								mymerklasty3 = 0;
								break;
							                          };   // end-switch
							break;

						case 'n':
							switch (lower(par1[0])) {
							case 'm': lasty = mymerky ; break;
							case '1': lasty = mymerklasty1; break;
							case '2': lasty = mymerklasty2; break;
							case '3': lasty = mymerklasty3; break;

							case 'x': lasty = mymerklastyx;  break;
							case 'y': lasty = mymerklastyy; VpeGotoPage(hDoc, pagey); break;
							case 'z': lasty = mymerklastyz; VpeGotoPage(hDoc, pagez); break;



							};   // end-switch
							break;




							// case 'i' j: switch (par2[1])
						default : 
							      // Fehlermeldung wrong Case
							      break;
		 			                                 };
					    return;
				                    }
				      // Fehlermeldung wrong Par

};

void InsertExec(char *t2Print)
{
	int sserg = 0;
	char *spar;
	int sscanloop = 2;
	if (t2Print[2] == ']') sscanloop++;
	spar = t2Print + sscanloop;

	int modus = 0; // Standard DOC. Einlesen

	for (; t2Print[sscanloop] != '\0'; sscanloop++) {

			if (t2Print[sscanloop] == ',') {
				t2Print[sscanloop] = '\0';
				sserg++;
				if (t2Print[sscanloop + 1] == '1') modus = 1;
				if (t2Print[sscanloop + 1] == '2') modus = 2;
				if (t2Print[sscanloop + 1] == 'v') modus = 9;
				if (t2Print[sscanloop + 1] == 'V') modus = 9;
				break;
			};
			

		};


//	if (sserg < 1) return;

	SetxPath(spar, ".");

	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	// size_t length_of_arg;  // unused
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	strcpy(szDir, xppfad);

	int wildcard = 0;

	for (sscanloop = 0; szDir[sscanloop] != '\0'; sscanloop++) {
		if (szDir[sscanloop] == '?') wildcard++;
		if (szDir[sscanloop] == '*') wildcard++;
	};

	if (wildcard) {

		char curp_dir[_MAX_PATH];
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];



		_splitpath(szDir, drive, dir, fname, ext); // C4996
		_makepath(curp_dir, drive, dir, "", "");

		hFind = FindFirstFile(szDir, &ffd);

		if (INVALID_HANDLE_VALUE == hFind)
		{

			sprintf(mMsg, "[C RED]Fehler beim lesen: %s", szDir);
			lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
			lasty = lasty + czhoch;
			return;
		}

		// List all the files in the directory with some info about them.

		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{

				if (modus == 9) {
					sprintf(mMsg, "[C BLUE]DIR %s", ffd.cFileName);
					lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
					lasty = lasty + czhoch;
				};





			}
			else
			{
				//			filesize.LowPart = ffd.nFileSizeLow;
				//			filesize.HighPart = ffd.nFileSizeHigh;
				//			_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);

				if (modus == 9) {
					sprintf(mMsg, "[C DKBLUE]    %s", ffd.cFileName);
					lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
					lasty = lasty + czhoch;
					goto weiterf;
				};

				strcpy(fname, curp_dir);
				strcat(fname, ffd.cFileName);

				if (modus == 1) {
					sserg = VpeReadDocPageRange(hDoc, fname, 1, 1);
					if (!sserg)   {
						sprintf(mMsg, "[C RED]VPE ($incl1) Error %d, %s", (VpeGetLastError(hDoc)), szDir);
						lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
					};
					goto weiterf;
				};


				sserg = VpeReadDoc(hDoc, fname);
				if (!sserg)   {
					sprintf(mMsg, "[C RED]VPE ($incl) Error %d, %s", (VpeGetLastError(hDoc)), fname);
					lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
					lasty = lasty + czhoch;
				};

			}
		weiterf:;
		} while (FindNextFile(hFind, &ffd) != 0);


		FindClose(hFind);
		return;
	};

	if (modus == 9) {
		sprintf(mMsg, "[C DKBLUE]Incl:  %s", szDir);
		lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
		lasty = lasty + czhoch;
		return;

	};

	if (modus == 1) {
		sserg = VpeReadDocPageRange(hDoc, szDir,1,1);
		if (!sserg)   {
			sprintf(mMsg, "[C RED]VPE ($incl1) Error %d, %s", (VpeGetLastError(hDoc)), szDir);
			lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
		};

		return;

	};


	sserg = VpeReadDoc(hDoc, szDir);
	if (!sserg)   {
		sprintf(mMsg, "[C RED]VPE ($incl) Error %d, %s", (VpeGetLastError(hDoc)), szDir);
			lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
	           	};
		 

};


void PrintExec(char *t2Print)
{
	int sserg = 0;

	char *spar;
	char *par1, *par2, *par3, *par4, *par5;

	// char longline[1024];

	  int sscanloop = 2;
	  int par5lastbu = 0;
	  if (t2Print[2] == ']' ) sscanloop++;
	  par1 = t2Print + sscanloop;		
	  /*
      sserg = sscanf( t2Print + 2, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^^],150%s",par1,par2,par3,par4,par5,spar );
	  */

	  for (; t2Print[sscanloop] != '\0'; sscanloop++ ) {
		  
		  if ( t2Print[sscanloop] == ',' ) {
			     t2Print[sscanloop] = '\0';
			     sserg++;
		 	     if ( sserg == 1 ) par2 = t2Print + sscanloop + 1;
		 	     if ( sserg == 2 ) par3 = t2Print + sscanloop + 1;
		 	     if ( sserg == 3 ) par4 = t2Print + sscanloop + 1;
		 	     if ( sserg == 4 ) {
					  par5 = t2Print + sscanloop + 1;
				 };
		 	     if ( sserg == 5 ) {
					 spar = t2Print + sscanloop + 1;
					 break;
				                   }
		                                   }

	  };

	  char tumeinbu = ' ';
      sserg++;  
	  double ip1, ip2, ip3, ip4;

	  if ( sserg < 5 ) return;  // Fehlermeldung ?  

 	kleinetp5:
					  par5lastbu = (strlen(par5));
					  par5lastbu--;
				/*	  if (par5[par5lastbu] == 'o') {
						  strcpy(longline, spar);
						  spar = longline;
						  par5[par5lastbu] = ' ';

					  }; */
					  if (par5[par5lastbu] == ' ')  {
					    par5[par5lastbu] = '\0';
						goto kleinetp5;
					  };
	  // ACHTUNG! letztes Zeichen ? (davor zxy? oder + ) 
	  if  ( par5[ par5lastbu ]  == '?' )  {    // objekt Platzbedarf pr�fen, wenn OK drucken, sonst new Page 
  		   par5lastbu--;
            if ( par5lastbu > 1 ) {
				 if  ( par5[ par5lastbu ]  == 'z' )  tumeinbu = 'z';
				 if  ( par5[ par5lastbu ]  == 'y' )  tumeinbu = 'y';
				 if  ( par5[ par5lastbu ]  == 'x' )  tumeinbu = 'x';
				 if (par5[par5lastbu] == '?')  tumeinbu = '?';
				 if (par5[par5lastbu] == '!')  tumeinbu = '!';
				 if (tumeinbu != ' ')  par5lastbu--;
			                      };

					   goto remypbedarf ;  // erste mal springt zu ... und kommt dann zur�ck tricky o. dumm?
				   };

					
naremypbedarf:
				 	setglobh   =  ( par5[ par5lastbu ] == '+' ) ;  
					if ( setglobh )  ( par5[ par5lastbu  ]  = '\0' ) ; 

                      ip1 = mypos(par1);
                      ip2 = mypos(par2);
                      ip3 = mypos(par3);
                      ip4 = mypos(par4);


				if ( sserg == 5 ) {
#ifdef mitlogfile
  if ( mitlog ) {
  sprintf(mMsg, "exec5:%s|x:%#5.2f y:%#5.2f x2:%#5.2f y2:%#5.2f\n",par5,ip1,ip2,ip3,ip4);
  fputs( mMsg, flog );
  };
#endif 
                     if (!_strnicmp( par5 , "rt", 2)) {
					 	          lastwribot = VpeWriteRTF(hDoc,ip1,ip2,ip3,ip4, myRTFline);
					              if (setglobh) myhoch = lastwribot;
					              return ;
					                                  };
                     if (!_strnicmp( par5 , "li", 2)) {
						 VpeLine(hDoc,ip1,ip2,ip3,ip4 );
						 return;
					 };
                     if (!_strnicmp( par5 , "rb", 2)) {
					 	          lastwribot = VpeWriteRTF(hDoc,ip1,ip2,ip3,ip4, myRTFline);
					              if (setglobh) myhoch = lastwribot;
					              return ;
		 			                                  };
					 if (!_strnicmp(par5, "bo", 2)) {
					//	  VpeSetObjectID(hDoc, obji++);

						 VpeBox(hDoc, ip1, ip2, ip3, ip4);
					 };

					 if (!_strnicmp(par5, "uo", 2)) {
						 int myuo;
						 myuo = atoi(par5 + 2);
						 VpeSetObjectID(hDoc, 1024 + myuo);
						 VpeCreateUDO(hDoc, ip1, ip2, ip3, ip4, myuo);
						 VpeSetObjectID(hDoc, 0);
					 };



                     if (!_strnicmp( par5 , "ci", 2)) VpeEllipse(hDoc,ip1,ip2,ip3,ip4 );

                     return;
				                    };

#ifdef mitlogfile

				if (!_strnicmp(spar, "grundaus", 8)) {
				          strncpy(mMsgpar, spar, 125);
				                                    };

		if (mitlog) {
			//	sprintf(mMsg, "exec6:%s|x:%#5.2f y:%#5.2f x2:%#5.2f y2:%#5.2f \n",par5,ip1,ip2,ip3,ip4);
          strncpy(mMsgpar, spar, 125);
	      sprintf(mMsg, "exec6:%s|x:%#5.2f y:%#5.2f x2:%#5.2f y2:%#5.2f :%s\n", par5, ip1, ip2, ip3, ip4, mMsgpar);
				fputs(mMsg, flog);
                    };
#endif 
                
   		  	        if (!_strnicmp( par5 , "te",2)){
						if (mymakemes(spar, 1)) spar = hlongline;
//						mymakeme(spar, 1);
                       lastwribot = VpeWrite(hDoc,ip1,ip2,ip3,ip4,spar );
					   if (setglobh) myhoch = lastwribot;
					   return ;
					   };

  		  	        if (!_strnicmp( par5 , "tb",2)){ 
						if (mymakemes(spar, 1)) spar = hlongline;
				//		mymakeme(spar, 1);
						// VpeSetObjectID(hDoc, 1);
                       lastwribot = VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,spar );

					   if (setglobh) myhoch = lastwribot;
 			  		   return ;
					};

   		  	        if (!_strnicmp( par5 , "rt",2)){
                       lastwribot = VpeWriteRTF(hDoc,ip1,ip2,ip3,ip4,spar );
   					   if (VpeGetLastError(hDoc)) {
						   spar[21] = '\0';
						   sprintf(mMsg, "[C RED]VPE(rt) Error %d, %s",(VpeGetLastError(hDoc)),spar);
						   lastwribot = VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,mMsg );
					   };
   					   if (setglobh) myhoch = lastwribot;
 			  		   return ;
					   };

  		  	        if (!_strnicmp( par5 , "rb",2)){ 
                       lastwribot = VpeWriteBoxRTF(hDoc,ip1,ip2,ip3,ip4,spar );
   					   if (VpeGetLastError(hDoc)) {
						   spar[21] = '\0';
						   sprintf(mMsg, "[C RED]VPE(rb) Error %d, %s",(VpeGetLastError(hDoc)),spar);
						   lastwribot = VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,mMsg );
					   };

					   if (setglobh) myhoch = lastwribot;
 			  		   return ;
					                              };

  		  	        if (!_strnicmp( par5 , "bc",2)){
					//	VpeSetObjectID(hDoc, 2);

						if (mymakemes(spar, 1)) spar = hlongline;
						if ( par5[0] == 'B' ) VpeSetBarcodeParms(hDoc,BCP_BOTTOM, BCP_BOTTOM);
						else VpeSetBarcodeParms(hDoc,BCP_HIDE, BCP_HIDE);
						if ( par5[1] == 'C' ) VpeSetBarcodeAutoChecksum(hDoc,true);
						else VpeSetBarcodeAutoChecksum(hDoc,false);
                        int codetype = atoi(par5 + 2 );
                        if ( codetype > 90 ) {
						if ( codetype == 91 ) VpePDF417(hDoc,ip1,ip2,ip3,ip4,spar);
                        if ( codetype == 92 ) VpeDataMatrix(hDoc,ip1,ip2,ip3,ip4,spar);
                        if ( codetype == 93 ) VpeAztec(hDoc,ip1,ip2,ip3,ip4,spar);
						if (codetype == 96) {
						//	VpeSetQRCodeBorder(hDoc, 0);
							VpeSetQRCodeEccLevel(hDoc, VBAR2D_QRCODE_ECC_LEVEL_M);  // 15% error-corr, standard = nzr 7%
						//	VpeSetQRCodeBorder(hDoc, 0);
							for (int i = 0; spar[i]; i++) 	if (spar[i] == '|')   spar[i] = '\n';  // nbspace
							codetype = 97;
						};
						if ( codetype == 97 ) {
#ifdef mitlogfile
							if (mitlog) {
								sprintf(mMsg, "\nQRCode:%s\n", spar);
								fputs(mMsg, flog);
							};
#endif

							for (int i = 0; spar[i]; i++) 	if (spar[i] == '\xA0')   spar[i] = ' ';  // nbspace
							VpeQRCode(hDoc, ip1, ip2, ip3, ip4, spar);
						};
							                    
						}
 			  	     	else VpeBarcode(hDoc,ip1,ip2,ip3,ip4,codetype,spar,NULL );
					   if (VpeGetLastError(hDoc)) {
						   sprintf(mMsg, "[C RED]VPE(bc) Error %d, %s",(VpeGetLastError(hDoc)),spar);
						   VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,mMsg );
					   };

				//	   VpeSetObjectID(hDoc, 0);

 			  		   return ;
					   };

	  SetxPath(spar, par5);

	  if (!strcmp(xppfad, " ")) {
		  if (setglobh) myhoch = 0;
		  return ;
	  };


#ifdef mitlogfile
  if (mitlog) {
	  sprintf(mMsg, "xppar: %s\n",xppfad);
	  fputs( mMsg, flog );
                };
#endif
 
   		  	        if (!_strnicmp( par5 , "ft",2)){
                       lastwribot = VpeWriteRTFFile(hDoc,ip1,ip2,ip3,ip4,xppfad );
					   if (VpeGetLastError(hDoc)) {
						   sprintf(mMsg, "[C RED]VPE(ft) Error %d, %s",(VpeGetLastError(hDoc)),xppfad);
						   lastwribot = VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,mMsg );
					   };
					   if (setglobh) myhoch = lastwribot;
 			  		   return ;
					   };
   		  	    
					if (!_strnicmp( par5 , "fx",2)){
                           xMixRTFFile(xppfad );
					   	   lastwribot = VpeWriteRTF(hDoc,ip1,ip2,ip3,ip4, myRTFline);
   		 			       if (setglobh) myhoch = lastwribot;
 			  		   return ;
					   };

					if (!_strnicmp( par5 , "fy",2)){  // setzt nur RTF-Text
                       xMixRTFFile(xppfad);
 			  		   return ;
					   };

  		  	        if (!_strnicmp( par5 , "fb",2)){ 
                       lastwribot = VpeWriteBoxRTFFile(hDoc,ip1,ip2,ip3,ip4,xppfad );
   					   if (VpeGetLastError(hDoc)) {
						   sprintf(mMsg, "[C RED]VPE(fb) Error %d, %s",(VpeGetLastError(hDoc)),xppfad);
						   lastwribot = VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,mMsg );
					   };

					   if (setglobh) myhoch = lastwribot;
 			  		   return ;
					                              };

					if ( (strlen(par5)) > 3 ) {
						  char zwembed[125];
						  sprintf(zwembed, "[%s]",par5);
					      lastwribot = VpeWrite(hDoc,ip1,ip2,VFREE,VFREE, zwembed) ;
  					   if (setglobh) myhoch = lastwribot;
						  return;
					};
 

					if (! wartime)  {
				
						VpeRenderPicture(hDoc, ip1, ip2, xppfad); // ,  (atoi(par5)) );
						if (!VpeGetLastError(hDoc))  lastwribot = VpePicture(hDoc, ip1, ip2, ip3, ip4, xppfad); // ,  (atoi(par5)) );
					}
					else  {
					  lastwribot = VpePicture(hDoc, ip1, ip2, ip3, ip4, xppfad);
					  if (!VpeGetLastError(hDoc)) {
						   sprintf(mMsg, "[C RED]VPE(pic) Error %d, %s",(VpeGetLastError(hDoc)),xppfad);
						   lastwribot = VpeWriteBox(hDoc,ip1,ip2,ip3,ip4,mMsg );
					                              };
					}

			  	     if (setglobh) myhoch = lastwribot;
   	                   
                     return;			     
		         

remypbedarf:
                      ip1 = mypos(par1);
                      ip2 = mypos(par2);
                      ip3 = mypos(par3);
                      ip4 = mypos(par4);

					  myhoch = 0;
					 if (!_strnicmp( par5 , "te",2)){
 	  	    	    	 mymakeme(spar, 1);

					   lastwribot = VpeRenderWrite(hDoc,ip1,ip2,ip3,ip4,spar );
					   myhoch = VpeGet(hDoc, VRENDERHEIGHT);
					   goto checkmyhoch ;
					   };

					 if (!_strnicmp(par5, "xf", 2)){
						 xMixRTFFile(xppfad);
						 lastwribot = VpeRenderRTF(hDoc, ip1, ip2, ip3, ip4, myRTFline);
						 myhoch = VpeGet(hDoc, VRENDERHEIGHT);
						 goto checkmyhoch;
					 };



  		  	        if (!_strnicmp( par5 , "tb",2)){ 
						mymakeme(spar, 1);

                       lastwribot = VpeRenderWriteBox(hDoc,ip1,ip2,ip3,ip4,spar );
					   myhoch = VpeGet(hDoc, VRENDERHEIGHT);
					   goto checkmyhoch ;
					};

   		  	        if (!_strnicmp( par5 , "rt",2)){
					//	lastwribot = VpeRenderRTF(hDoc, ip1, ip2, ip3, ip4, myRTFline);
						lastwribot = VpeRenderRTF(hDoc, ip1, ip2, ip3, ip4, spar);
					   myhoch = VpeGet(hDoc, VRENDERHEIGHT);
					   goto checkmyhoch ;
					   };

  		  	        if (!_strnicmp( par5 , "rb",2)){ 
					//	VpeRenderBoxRTF(hDoc, ip1, ip2, ip3, ip4, myRTFline);
						VpeRenderBoxRTF(hDoc, ip1, ip2, ip3, ip4, spar);
					   myhoch = VpeGet(hDoc, VRENDERHEIGHT);
					   goto checkmyhoch ;
					                              };

					if (!_strnicmp(par5, "li", 2)){
						//	VpeRenderBoxRTF(hDoc, ip1, ip2, ip3, ip4, myRTFline);
						// VpeRenderBoxRTF(hDoc, ip1, ip2, ip3, ip4, spar);
						myhoch = czhoch;
						goto checkmyhoch;
					};

				
					SetxPath(spar, par5);

					if (!strcmp(xppfad, " ")) {
						if (setglobh) myhoch = 0;
						return;
					};



#ifdef mitlogfile
  if (mitlog) {
	  sprintf(mMsg, "render par: %s\n",xppfad);
	  fputs( mMsg, flog );
                };
#endif

   		  	        if (!_strnicmp( par5 , "ft",2)){
                       lastwribot = VpeRenderRTFFile(hDoc,ip1,ip2,ip3,ip4,xppfad );
					   myhoch = VpeGet(hDoc, VRENDERHEIGHT);
					   goto checkmyhoch ;
					   };

					if (!_strnicmp(par5, "fx", 2)){
						xMixRTFFile(xppfad);
						lastwribot = VpeRenderRTF(hDoc, ip1, ip2, ip3, ip4, myRTFline);
						myhoch = VpeGet(hDoc, VRENDERHEIGHT);
						goto checkmyhoch;
					};

  		  	        if (!_strnicmp( par5 , "fb",2)){ 
                       lastwribot = VpeRenderBoxRTFFile(hDoc,ip1,ip2,ip3,ip4,xppfad );
					   myhoch = VpeGet(hDoc, VRENDERHEIGHT);
					   goto checkmyhoch ;
					                              };

                     VpeRenderPicture(hDoc,ip3,ip4,  xppfad) ; // ,  (atoi(par5)) );
 				     myhoch = VpeGet(hDoc, VRENDERHEIGHT);
   
checkmyhoch:
       if ( tumeinbu == ' ' ) goto ohnesonds;
#ifdef mitlogfile
	   if (mitlog) {
		   sprintf(mMsg, "+hochpruefbu glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f [yhoch:%#5.2f] %s zeilyres:%#5.2f \n", glasty, fussbis, lasty, myhoch,tumeinbu,zeilyres);
		   fputs(mMsg, flog);
	   };
#endif


	   if ( glasty - fussbis < lasty + myhoch + zeilyres )  wantbreak = true;
	   myhoch = 99;
       if ( wantbreak ) {
		                   wantbreak = false;
						   if (tumeinbu == '!') return;
						   myPBreak();
		                };
	   if (tumeinbu != '?') if (tumeinbu != '!') Eincharp(tumeinbu);
	   goto naremypbedarf ; 

					 
ohnesonds:
	    if ( glasty - fussbis < lasty + myhoch) wantbreak = true;
#ifdef mitlogfile
		if (mitlog) {
			sprintf(mMsg, "+hochpruefl glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f [yhoch:%#5.2f] \n",  glasty , fussbis , lasty , myhoch);
			fputs(mMsg, flog);
		};
#endif


		myhoch = 99;
     	if ( wantbreak ) {
	 		  			   wantbreak = false;
	                       myPBreak();
		                  };

		goto naremypbedarf ; 


}

 


void CalcExec( char *t2Print)
{
      byte sserg = 0;
	  char myf1 = ' ', myf2 = ' ', myf3 = ' '; 
	  char *leer = "0";

  	  char *spar;
	  char *par1, *par2, *par3, *par4, *par5;
	  int sscanloop = 2;
	  if (t2Print[4] == ']') sscanloop = 5;
	  if (t2Print[3] == ']') sscanloop = 4;
	  if (t2Print[2] == ']') sscanloop = 3;
	  if (sscanloop > 3) {   // abfragen wie [?/?] ...
		  myf1 = t2Print[2];  // + oder -
		  myf2 = t2Print[3];  // welche
		  myf3 = t2Print[4];  // y oder x ( y default) 
		  char mytchar;
		  mytchar = t2Print[sscanloop];
		  if (mytchar < ' ') par1 = leer;  // wenn ohne Parameter 0 setzen
		                else par1 = t2Print + sscanloop;

	  }
	  else  par1 = t2Print + sscanloop;		
	  /*
      sserg = sscanf( t2Print + 2, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^^],150%s",par1,par2,par3,par4,par5,spar );
	  */
	  while (par1[0] == ' ') par1 = t2Print + ++sscanloop;

	  for (; t2Print[sscanloop] != '\0'; sscanloop++ ) {
		  
		  if ( t2Print[sscanloop] == ',' ) {
			     t2Print[sscanloop] = '\0';
			     sserg++;
				 while (t2Print[++sscanloop] == ' '); // par2 = t2Print + ++sscanloop + 1;

				 if ( sserg == 1)  par2 = t2Print + sscanloop;
		 	     if ( sserg == 2 ) par3 = t2Print + sscanloop;
		 	     if ( sserg == 3 ) par4 = t2Print + sscanloop;
		 	     if ( sserg == 4 ) par5 = t2Print + sscanloop;
		 	     if ( sserg == 5 ) {
					 spar = t2Print + sscanloop;
					 break;
				                   }
		                                   }

	  };

      sserg++;  


	  if (sserg < 5) {
		  mymerklastrendy = mypos(par1);
		  goto  rechnecalc2;
	  };


	                  double ip1, ip2, ip3, ip4;

                      ip1 = mypos(par1);
                      ip2 = mypos(par2);
                      ip3 = mypos(par3);
                      ip4 = mypos(par4);

					  lastRender = -9;
					  if (!_strnicmp(par5, "te", 2)) {
						  mymakeme(spar, 1);
						  lastRender = VpeRenderWrite(hDoc, ip1, ip2, ip3, ip4, spar);
					  };
  		  	          if (!_strnicmp( par5 , "tb",2)) {
						  mymakeme(spar, 1);
						  lastRender = VpeRenderWriteBox(hDoc, ip1, ip2, ip3, ip4, spar);
					  };

					  if (!_strnicmp(par5, "rt", 2))   {
						  if (_strnicmp(par5, "rtff", 4)) lastRender = VpeRenderRTF(hDoc, ip1, ip2, ip3, ip4, spar);
						  else {  // also rtff formatiere rtfline
							    // spar nach RTFLINE kopiern

							 byte  fmode, fint;
							 fmode = 0;
							 if (par5[4] == '+') fmode = 1;
							 if (par5[4] == '-') fmode = 2;
							 if (par5[4] == '%') fmode = 3; //  ( verkleinern)
							 if (par5[4] == '*') fmode = 4; //  ( vergr�ssern)
							 if (fmode) fint = par5[5] - 48;
	//						 bool mytemp = FALSE;
							 int laufs, laufd;
							 laufs = 0; laufd = 0;
	
							 for (; spar[laufs] != '\0';laufs++) {
								 if (spar[laufs] == '\\') {
									 // if (spar[laufs + 1] == 'p') {
									 //										 for (zlauf = 0; (spar[laufs + zlauf] != '\\') || (spar[laufs + zlauf] != ' ') || (spar[laufs + zlauf] != '\0') || ( zlauf ; zlauf++) {
									 //		 temps[zlauf] = spar[laufs + zlauf];
									 //	 };
									 //	 if (spar[laufs + zlauf] != ' ')

									   
									 if (  fmode && (spar[laufs + 1] == 'f')) {   // fontsize
										 if (spar[laufs + 2] == 's') {
											 if ((spar[laufs + 3] >= '0') && (spar[laufs + 3] <= '9'))  {
												 if ((spar[laufs + 4] >= '0') && (spar[laufs + 4] <= '9'))  {
													 int zwint;
													 zwint = (10 * (spar[laufs + 3] - 48)) + (spar[laufs + 4] - 48);
													 if (fmode == 1) zwint = zwint + fint;
													 if (fmode == 2) zwint = zwint - fint;
													 if (fmode == 3) zwint = zwint - (10 * fint * zwint / 100);
													 if (fmode == 4) zwint = zwint + (10 * fint * zwint / 100);
													 spar[laufs + 3] = (zwint / 10) + 48;
													 spar[laufs + 4] = ( zwint % 10) + 48;
												 };
											                                                            };
										                             };
									                                         };
				/*					 if (spar[laufs + 1] == 'u') {
										 zlauf = 0;
									 nochmzlauf:
										 if ((spar[laufs + 2 + zlauf] >= '0') && (spar[laufs + 2 + zlauf] <= '9'))  {
											 temps[zlauf] = spar[laufs + 2 + zlauf];
											 if (zlauf < 5) {
												 zlauf++;
												 goto nochmzlauf;
											 };
										 };

										 if (zlauf && (spar[laufs + 2 + zlauf] == '?') )  {
											 // unicode Zeichen ;(
											 temps[zlauf] = '\0';
											 int mycode = atoi(temps); 
											 temps[0] = '-';
											// zlauf = mycode;
											 
											 if (mycode == 8222) strcpy(temps, "\\rdblquote ");
											 if (mycode == 9642) strcpy(temps, "\\bullet ");

											 if (mycode == 228) strcpy(temps, "\\'e4");


											 if (temps[0] == '\\') {
												 int loci = 0;
												 for (; temps[loci] != '\0'; loci++) {
													 myRTFline[laufd] = temps[loci];
													 laufd++;
												 };

												 laufs = laufs + zlauf + 2 ;
												 goto nextlooo;
											 };

										                                                   };

										   
									 }; */

								 };

								 myRTFline[laufd] = spar[laufs];
								 laufd++;

								  							 };
							 myRTFline[laufd] = '\0';
							 lastRender = VpeRenderRTF(hDoc, ip1, ip2, ip3, ip4, myRTFline);
						 };
					 };

					 if (!_strnicmp(par5, "rb", 2))  lastRender = VpeRenderBoxRTF(hDoc, ip1, ip2, ip3, ip4, spar);
		 			 if ( lastRender != -9 ) goto rechnecalc;

					 SetxPath(spar, par5);

#ifdef mitlogfile
  if (mitlog) {
	  sprintf(mMsg, "render pfad: %s\n",xppfad);
	  fputs( mMsg, flog );
                };
#endif
  if (!strcmp(xppfad, " ")) {
	  lastRender = 0;
	  mymerklastrendy = 0;
	  mymerklastrendx = 0;
	  goto rechnecalc2;
  };
  
   		  	        if (!_strnicmp( par5 , "ft",2)) lastRender = VpeRenderRTFFile(hDoc,ip1,ip2,ip3,ip4,xppfad );
  		  	        if (!_strnicmp( par5 , "fb",2)) lastRender = VpeRenderBoxRTFFile(hDoc,ip1,ip2,ip3,ip4,xppfad );
					if (lastRender != -9) goto rechnecalc;

					if (myf3 == 'w') VpeRenderPicture(hDoc, VFREE ,ip4 - ip2, xppfad);
					else    VpeRenderPicture(hDoc,ip3 - ip1 ,VFREE,  xppfad) ; // ,  (atoi(par5)) );

	rechnecalc:
					mymerklastrendy = VpeGet(hDoc, VRENDERHEIGHT);
					mymerklastrendx = VpeGet(hDoc, VRENDERWIDTH);

	rechnecalc2:

#ifdef mitlogfile
					if (mitlog) {
						sprintf(mMsg, "lastrender glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f [render:%#5.2f] %s  \n", glasty, fussbis, lasty, mymerklastrendy, t2Print);
						fputs(mMsg, flog);
					};
#endif



					switch (myf1)
					{
					case '\\':
						mymerklasty1 = 0;
						mymerklasty2 = 0;
						mymerklasty3 = 0;
						rendres = mymerklastrendy;

					case '=':
						switch (myf2)
						{
						case '1': mymerklasty1 = mymerklastrendy; break;
						case '2': mymerklasty2 = mymerklastrendy; break;
						case '3': mymerklasty3 = mymerklastrendy; break;
						}
//					myhoch = 0;
//					lasty = lasty - czhoch;

						break;

					case 'a':
					case 'A':
						switch (myf2)
						{
						case '1': mymerklasty1 = lasty; break;
						case '2': mymerklasty2 = lasty; break;
						case '3': mymerklasty3 = lasty; break;
						case 'A':
						case 'a':
						case 'S':
						case 's':
							mymerklasty1 = lasty; 
							mymerklasty2 = 0;
							mymerklasty3 = 0;
							break;
						}
//						lasty = lasty - czhoch;
						break;

					case 'S':
					case 's':
						switch (myf2)
						{
						case 'A':
						case 'a':
						case '1': if (mymerklasty1 > lasty) lasty = mymerklasty1; break;
						case '2': if (mymerklasty2 > lasty) lasty = mymerklasty2 = lasty; break;
						case '3': if (mymerklasty3 > lasty) lasty = mymerklasty3 = lasty; break;
						case '?':
							if (mymerklasty1 > lasty) lasty = mymerklasty1;
							if (mymerklasty2 > lasty) lasty = mymerklasty2;
							if (mymerklasty3 > lasty) lasty = mymerklasty3;
							break;
						}
//						lasty = lasty - czhoch;
						break;

					case '+':
						switch (myf2)
						{
						case '1': mymerklasty1 += mymerklastrendy; break;
						case '2': mymerklasty2 += mymerklastrendy; break;
						case '3': mymerklasty3 += mymerklastrendy; break;
						}
//					myhoch = 0;
//						lasty = lasty - czhoch;
						break;

					case '/':
					case '?':
						switch (myf2)
						{
						case '0':
							myhoch = 0;
							goto pruefepage;
								break;
						case '1':
							myhoch = mymerklasty1;
							goto pruefepage;
							break;
						case '2':
							myhoch = mymerklasty2;
							goto pruefepage;
								break;
						case '3':
							myhoch = mymerklasty3;
							goto pruefepage;
								break;
						case '?':
							myhoch = mymerklasty1;
							if (mymerklasty2 > myhoch) myhoch = mymerklasty2;
							if (mymerklasty3 > myhoch) myhoch = mymerklasty3;
							goto pruefepage;
							break;
						}

					}


					//	myhoch = 0;

					return;


				pruefepage:


#ifdef mitlogfile
					if (mitlog) {
						sprintf(mMsg, "+ ??Page? glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f myhoch:%#5.2f lastrender:%#5.2f  also noch: %#5.2f \n", glasty, fussbis, lasty, myhoch,mymerklastrendy, (glasty - fussbis) - (lasty + myhoch + mymerklastrendy));
						fputs(mMsg, flog);
					};
#endif

					if (myhoch < 90 ) if ( (glasty - fussbis) < (lasty + myhoch + mymerklastrendy))  wantbreak = true;
//					myhoch = 99;
					//	 	myhoch = 0;
					if (wantbreak) {
						wantbreak = false;
						myPBreak();
					};
			

				//	lasty = lasty - czhoch;




}

void Print_ganz_Teil(char *t2Print, int llen)
{
	double newheight, newwidth;
	VpeRenderPrint(hDoc, 0, 0, t2Print);
	newwidth = VpeGet(hDoc, VRENDERWIDTH);
	newheight = VpeGet(hDoc, VRENDERHEIGHT);

	// double lastwi =  ( fabs ( lastx +  newwidth ));
	double lastwi = lastx + newwidth;



#ifdef mitlogfile
	if (mitlog) {
		sprintf(mMsg, " GTWrite: x:%#5.2f y:%#5.2f w:%#5.2f h:%#5.2f |%s \n", lastx, lasty - newheight + globczhoch, lastwi, lasty, t2Print);
		fputs(mMsg, flog);
	};
#endif

	// 		VpeWrite(hDoc,	 lastx + width ,  VFREE , lastwi , lasty + globczhoch , t2Print );
	//  if (oldzwmode) {
	//  };

	VpeWrite(hDoc, lastx, lasty - newheight + globczhoch, lastwi, lasty + globczhoch, t2Print);

	lastx = lastwi;

	//	if ( czbreit > 0 ) newwidth  =  llen * czbreit ;
	// 	width = width + newwidth ;
};




 void Print_ein_Teil( char *t2Print, int llen)
  {

	 double deltah, newheight,newwidth;
  
#ifdef mitlogfile
  if ( mitlog ) {
	  strncpy(mMsgpar, t2Print, 120);
	  sprintf(mMsg, "~     %s|%i\n", mMsgpar, llen);
      fputs( mMsg, flog );
                 };
#endif    
    
    if ( t2Print[0] == '\0' ) return; 
    obanzonpage++;
   //  if ( t2Print[0] == '^' ) t2Print++; 
     if ( t2Print[0] != '[' ) goto drucknur; 


    //  int sserg;
	//  char par1[11];

	 switch (t2Print[1])

	 {
	 case 'x':
	 case 'X':

		 PrintExec(t2Print);
		 return;
	 case '?':
		 CalcExec(t2Print);
		 myhoch = lasty;  // keine AUTO Zeilenvorschub

		 return;
	 case '$':
		 // if ( obanzonpage < 3 )  killfirstpage = true;  // doch nicht l�schen, ge�ndert am 22.03.2016
		 InsertExec(t2Print);
		 return;

		 /*     case '@' :

		  switch ( t2Print[2] ) {
		  case '0' :
		  hData = VpeChartDataCreate(hDoc, 2, 4);
		  break;
		  case '1' :

		  VpeChartDataAddValue(hDoc, hData, 0, 10);
		  VpeChartDataAddValue(hDoc, hData, 0, 20);
		  VpeChartDataAddValue(hDoc, hData, 0, 30);
		  VpeChartDataAddValue(hDoc, hData, 0, 40);
		  VpeChartDataAddValue(hDoc, hData, 1, 5);
		  VpeChartDataAddValue(hDoc, hData, 1, 10);
		  VpeChartDataAddValue(hDoc, hData, 1, 15);
		  VpeChartDataAddValue(hDoc, hData, 1, 20);
		  VpeChartDataAddLegend(hDoc, hData, "Apples");
		  VpeChartDataAddLegend(hDoc, hData, "Bananas");
		  VpeSetChartXLabelState(hDoc, VCHART_LABEL_USER);
		  VpeChartDataAddXLabel(hDoc, hData, "1. Quarter");
		  VpeChartDataAddXLabel(hDoc, hData, "2. Quarter");
		  VpeChartDataAddXLabel(hDoc, hData, "3. Quarter");
		  VpeChartDataAddXLabel(hDoc, hData, "4. Quarter");

		  break;  // viertel
		  case '8' :
		  VpeSetChartTitle(hDoc, "3-D Bar Chart");
		  VpeChart(hDoc, 1, 1, -18, -18, hData, VCHART_3D_BAR);
		  break;  // achtel
		  case '3' :
		  VpeSetChartTitle(hDoc, "3-3 Bar Chart");
		  VpeChart(hDoc, 1, 1, -18, -18, hData, 3);
		  break;  // achtel
		  case '4' :
		  VpeSetChartTitle(hDoc, "3-4 Bar Chart");
		  VpeChart(hDoc, 1, 1, -18, -18, hData, 4);
		  break;  // achtel
		  case '6' :
		  VpeSetChartTitle(hDoc, "3-6 Bar Chart");
		  VpeChart(hDoc, 1, 1, -18, -18, hData, 6);
		  break;  // achtel

		  };
		  return; */

	 case '^':
		 t2Print[1] = ' ';

	 case ' ':
	 case '[':
		 goto drucknur;

	 case ']':
		 t2Print++;
		 t2Print++;
		 goto drucknur;

	 case '=':
		 PrintSetGleich(t2Print);
		 return;
	 case 'l':

		 lastx = VpeGet(hDoc, VLEFTMARGIN) + indirand;

		 switch (t2Print[2]) {
		 case 'h':
		 case '-':   lasty = lasty - czhoch + (czhoch / 2);   break;  // halbe Linefeed
		 case '+':   lasty = lasty + czhoch + (czhoch / 2);   break;  // ein 1/2 Linefeed

		 case '0':   lasty = lasty - czhoch;                    break;  // kein  Linefeed
		 case 'v':   lasty = lasty - czhoch + (czhoch / 4);   break;  // viertel 
		 case 'a':   lasty = lasty - czhoch + (czhoch / 8);   break;  // achtel
		 case 'z':   lasty = lasty - czhoch + (czhoch / 10);  break;  // zehntel

		 case 'V':   lasty = lasty + (czhoch / 4);   break;  // viertel 
		 case 'A':   lasty = lasty + (czhoch / 8);   break;  // achtel
		 case 'Z':   lasty = lasty + (czhoch / 10);  break;
		 case 'H':   lasty = lasty + (czhoch / 2);   break;  // halbe Linefeed

		 case '?':
			 VpeRenderPrint(hDoc, 0, 0, "X");
			 lasty = lasty - czhoch + VpeGet(hDoc, VRENDERHEIGHT);
			 break;
		 case '!':
			 VpeRenderPrint(hDoc, 0, 0, "X");
			 lasty = lasty - czhoch + (1.1 * VpeGet(hDoc, VRENDERHEIGHT));
			 break;

		 case '<':

			 lasty = lasty + czhoch;
			 char zwiline[65];
			 for (int li = 0; li < 64; li++)  zwiline[li] = li;
			 zwiline[0] = ' '; zwiline[64] = '\0';
			 PrintString(zwiline);
			 for (int li = 0; li < 64; li++)  zwiline[li] = li + 64;
			 lastx = VpeGet(hDoc, VLEFTMARGIN) + indirand;
			 lasty = lasty + czhoch;
			 zwiline[64] = '\0';
			 PrintString(zwiline);
			 break;


		 case '>':

			 lasty = lasty + czhoch;

			 for (int li = 0; li < 64; li++)  zwiline[li] = li + 128;
			 zwiline[64] = '\0';
			 PrintString(zwiline);
			 for (int li = 0; li < 64; li++)  zwiline[li] = li + 192;
			 lastx = VpeGet(hDoc, VLEFTMARGIN) + indirand;
			 lasty = lasty + czhoch;
			 zwiline[64] = '\0';
			 PrintString(zwiline);

			 break;

		 case 'L':  // trippell
			 lasty = lasty + czhoch;
		 case 'l':  // doppel l
			 lasty = lasty + czhoch;
		 default:
			 lasty = lasty + czhoch;
		 };  // end case l




		 if (t2Print[3] == '\0') return;
		 if (t2Print[2] == '\0') return;
		 t2Print = t2Print++;
		 t2Print = t2Print++;
		 break;

	 case 'P':
	 case 'p':
		 switch (t2Print[2])

		 {

		 case 'p':
			 if (!finlastline) {
				 myPBreak();
				 myhoch = lasty;
			 };
			 break;
		 case 'P':
			//  VpeSetAutoBreak(hDoc, AUTO_BREAK_NO_LIMITS			 );


			 VpePageBreak(hDoc);

			 myhoch = VpeGet(hDoc, VTOPMARGIN) - czhoch;  // so wird auch lasty ge�ndert
			 break;
		 case ']':
			 //  VpeSetAutoBreak(hDoc, AUTO_BREAK_NO_LIMITS			 );


			 myPBreak();
			 myhoch = 99;
			// VpePageBreak(hDoc);

			//  myhoch = VpeGet(hDoc, VTOPMARGIN) - czhoch;  // so wird auch lasty ge�ndert
			 break;

		 case '?':
			 char tumeinbu;
			 tumeinbu = t2Print[3];

			 myhoch = mymerklasty1;
			 if (mymerklasty2 > myhoch) myhoch = mymerklasty2;
			 if (mymerklasty3 > myhoch) myhoch = mymerklasty3;

			 if (tumeinbu < '!') goto ohnesonds;

#ifdef mitlogfile
			 if (mitlog) {
				 sprintf(mMsg, "+hochpruefbu glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f [yhoch:%#5.2f] %s \n", glasty, fussbis, lasty, myhoch, tumeinbu);
				 fputs(mMsg, flog);
			 };
#endif


			 if (glasty - fussbis < lasty + myhoch + zeilyres)  wantbreak = true;
			 myhoch = 99;
			 if (wantbreak) {
				 wantbreak = false;
				 myPBreak();
			 };
			 if (tumeinbu != '?') Eincharp(tumeinbu);
			 break;


		 ohnesonds:
			 if (glasty - fussbis < lasty + myhoch) wantbreak = true;
#ifdef mitlogfile
			 if (mitlog) {
				 sprintf(mMsg, "+hochpruefl glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f [yhoch:%#5.2f] \n", glasty, fussbis, lasty, myhoch);
				 fputs(mMsg, flog);
			 };
#endif


			 if (wantbreak) {
				 wantbreak = false;
				 myPBreak();
				 myhoch = lasty;

			 };

			 break;

		 case 'n':
			 if (aktline > 5) {
				 myPBreak();
				 //								   aktufdoc++;
				 myhoch = VpeGet(hDoc, VTOPMARGIN) - czhoch;  // so wird auch lasty ge�ndert

			 };
			 break;

		 case 'w':
			 wantbreak = true;
			 break;

		 case 'b':
			 VpePageBreak(hDoc);
			 //   myhoch = VpeGet(hDoc, VTOPMARGIN) - czhoch;  // so wird auch lasty ge�ndert
			 break;


		 case 's':
		 case 'S':
			 // set [PS  alte Methode, Pensize 
			 break;
		 default:

			 wantbreak = true;
		 }; // end case

		 if (t2Print[3] == '\0') return;
		 if (t2Print[2] == '\0') return;

		 break;

		 //	 default:
	 case 'U': case 'u': case 'F': case 'f':  case 'S': case 's': case 'B': case 'b': case 'Q': case 'q':  case '0': case '&':    // P ist Frei
		 /*  	     char *pdest;        //     const char steustri[28] = "IUuFfSsBbQq0&
				  pdest = (char *) strchr( steustri , t2Print[1] );   // ist in Steuerung Code ES MUSS sein
				  int inblock;
				  inblock = (int)(pdest - steustri);
				  strcpy(spar,SteuBlock[inblock]);  // String nach tline kopiere
				  strcat(spar,t2Print + 2);           // String nach tline kopieren
				  strcpy(t2Print,spar);  // String nach tline kopieren
				  break;
				  */
	 case 'I': case '1': case '2': case '3':  case '4': case '5': case '6': case '7': case '8': case '9': case 'E':  case 'N': case 'L':
	 case 'Z':
		 if (t2Print[2] == '\0') {  // zweichar codes
			 obanzonpage--;

			 Eincharp(t2Print[1]);
			 return;
		 };
		 if (t2Print[2] == ']') {  // zweichar codes
			 obanzonpage--;
			 Eincharp(t2Print[1]);
			 if (t2Print[3] == '\0') return;
			 t2Print = t2Print + 3;
			 break;
		 };
		 char *pdest;
		 pdest = strrchr(t2Print, ']');
		 if (pdest == NULL) {

			 Eincharp(t2Print[1]);
			 t2Print = t2Print + 2;
			 if (t2Print[0] == '^') t2Print = t2Print + 1;
		 };
		 // return;
		 break;
	 case '%':
		 Prozcharp(t2Print[2]);
		 if (t2Print[3] == '\0') return;
		 t2Print = t2Print + 3;
		break;

	case '>':
	         VpeStoreSet(hDoc,++mycurspos);
             VpeStorePos(hDoc);
			 t2Print = t2Print + 2;
             break;

     case '<':
		     VpeUseSet(hDoc,mycurspos);
	         VpeRemoveSet(hDoc,mycurspos--);
             VpeRestorePos(hDoc);
             return;

	 case ':': 
	 case '.': 
    	 char *spar;
         char *par1;
  	     par1 = t2Print + 2;		
	     int sserg = 0;
	     for (int sscanloop = 2 ; t2Print[sscanloop] != '\0'; sscanloop++ ) {
		  
		  if ( t2Print[sscanloop] == ',' ) {
			     t2Print[sscanloop] = '\0';
			     sserg++;
				 spar = t2Print + sscanloop + 1;
				 break;
		                                   }
	       };

 
		 // \' rueckwandeln
		 mymakeme(spar, 0);

		   
  	   if ( sserg > 0 ) {
			 lastx  = mypos(par1);
           if ( t2Print[1] == ':' ) {

		     VpeRenderPrint(hDoc,0,0, spar );
			 newwidth  =  VpeGet(hDoc, VRENDERWIDTH); 
 		     newheight =  VpeGet(hDoc, VRENDERHEIGHT);
 		     deltah = globczhoch - newheight;
		     if (deltah < 0 ) deltah = 0;
  		     VpeWrite(hDoc,	lastx ,lasty + globczhoch - newheight , ( lastx + newwidth) , lasty + globczhoch , spar );
			 lastx  = lastx + newwidth;
			 return;
			                    };

             
          //   VpeSetTextAlignment(hDoc,1);
		     VpeRenderPrint(hDoc,0,0, spar );

  	 	 	 newwidth  =  VpeGet(hDoc, VRENDERWIDTH); 
 		     newheight =  VpeGet(hDoc, VRENDERHEIGHT);
 		     deltah = newheight - globczhoch;
  		     VpeWrite(hDoc,	lastx - newwidth , lasty + globczhoch - newheight ,  lastx ,  lasty + globczhoch , spar );
             return;
		   }
		     break;   // da muss ich weiterdrucken

		   };  // END_CASE 
		   
  	  


drucknur:
	    // VpeSetTextAlignment(hDoc,0);  // Achtung neu 
	    // char t22print[1024];
		// strcpy( t22print,t2Print );

		   mymakeme(t2Print, 1);


		   VpeRenderPrint(hDoc, 0, 0, t2Print);
		   newwidth = VpeGet(hDoc, VRENDERWIDTH);
		   newheight = VpeGet(hDoc, VRENDERHEIGHT);

		   // double lastwi =  ( fabs ( lastx +  newwidth ));
		   double lastwi = lastx + newwidth;



#ifdef mitlogfile
  if (mitlog) {
      sprintf(mMsg, " Write: x:%#5.2f y:%#5.2f w:%#5.2f h:%#5.2f |%s \n", lastx ,  lasty - newheight + globczhoch, lastwi ,  lasty , t2Print );
      fputs( mMsg, flog );
  };
#endif

// 		VpeWrite(hDoc,	 lastx + width ,  VFREE , lastwi , lasty + globczhoch , t2Print );
//  if (oldzwmode) {
//  };

  VpeWrite(hDoc,	 lastx  ,  lasty - newheight + globczhoch  , lastwi , lasty + globczhoch , t2Print );
		
  lastx  = lastwi;

	//	if ( czbreit > 0 ) newwidth  =  llen * czbreit ;
	// 	width = width + newwidth ;
};

  void Eincharp(char schaltch)
  {

 	  char tline[512];
	  int  inblock = 0;

  	     char *pdest;                //     const char steustri[28] = "IUuFfSsBbQqP-0123456789ENL"; 
		 pdest = (char *) strchr( steustri , schaltch );   // ist in Steuerung Code ES MUSS sein
		 if ( pdest == NULL ) {
			 // Fehlermeldung
			 return;
		 };
		 inblock = (int)(pdest - steustri);
 	     if ( inblock > 12 ) {
	             inblock = inblock - 13;
				// tline = SteuBlock2[inblock];
          	     strcpy(tline,SteuBlock2[inblock]);   // String nach tline kopieren
	                    	 }
//		 else  tline = SteuBlock[inblock];  // String nach tline kopieren
		 else  strcpy(tline,SteuBlock[inblock]);  // String nach tline kopieren



#ifdef mitlogfile
  if (mitlog) {
  sprintf(mMsg, "Eincharp:[%c|%s\n",schaltch,tline);
  fputs( mMsg, flog );
  };
#endif
         int tanfang = 0;
  	     int tende = strlen(tline) - 1;

 
             if ( tende < 1 ) return;
  		     int tlauf = 1;
			 for ( ; tlauf < tende ; tlauf++ ) {

				 if ( tline[tlauf] == '[' ) {

  			        tline[tlauf] = '\0';
  			        if ( tline[tlauf-1] == '^') tline[tlauf-1] = '\0';
					if (tlauf - tanfang)  Print_ein_Teil( tline + tanfang , tlauf - tanfang );
					tanfang = tlauf;
  			        tline[tlauf] = '[';
				                            }
         
			   } 

			 if (tlauf > tanfang) Print_ein_Teil( tline + tanfang , tlauf - tanfang );

			 if (f2tmode) {
				 VpeRenderPrint(hDoc, 0, 0, "X");
				 czbreit = VpeGet(hDoc, VRENDERWIDTH);
				 czbreit = czbreit - 0.02;
				 czhoch = VpeGet(hDoc, VRENDERHEIGHT);
	 	     	 };


};

  void Prozcharp(char schaltch)
  {

	  char tline[512];
	  int  inblock = 0;

	  char *pdest;                //     const char steustri[28] = "IUuFfSsBbQqP-0123456789ENL"; 
	  pdest = (char *)strchr(steuvars, schaltch);   // ist in Steuerung Code ES MUSS sein
	  if (pdest == NULL) {
		  // Fehlermeldung
		  return;
	  };
	  inblock = (int)(pdest - steuvars);
      strcpy(tline, SteuVar[inblock]);  // String nach tline kopieren
	  

#ifdef mitlogfile
	  if (mitlog) {
		  sprintf(mMsg, "Prozcharp:[%c|%s\n", schaltch, tline);
		  fputs(mMsg, flog);
	  };
#endif

	  int tanfang = 0;
	  int tende = strlen(tline) - 1;


	  if (tende < 1) return;
	  int tlauf = 1;
	  for (; tlauf < tende; tlauf++) {

		  if (tline[tlauf] == '[') {

			  tline[tlauf] = '\0';
			  if (tline[tlauf - 1] == '^') tline[tlauf - 1] = '\0';
			  if (tlauf - tanfang)  Print_ein_Teil(tline + tanfang, tlauf - tanfang);
			  tanfang = tlauf;
			  tline[tlauf] = '[';
		  }

	  }

	  if (tlauf > tanfang) Print_ein_Teil(tline + tanfang, tlauf - tanfang);

	  if (f2tmode) {
		  VpeRenderPrint(hDoc, 0, 0, "X");
		  czbreit = VpeGet(hDoc, VRENDERWIDTH);
		  czbreit = czbreit - 0.02;
		  czhoch = VpeGet(hDoc, VRENDERHEIGHT);
	  };


  };


/* void Setz_RTF_Teil( char *t2Print)

  {
			 
		     char *pdest;
		 	 pdest = (char *)strchr( steustri , t2Print[0] );
   	         if ( pdest != NULL ) {
			 int inblock = (int)(pdest - steustri );
			 if ( inblock > 12 ) {
				 // Errormeldung
		                     	 }
			     else  strcpy(SteuBlock[inblock]+64,t2Print + 2 );  // String nach tline kopieren
			                 };

 };  // Setz_Wizzairmy_Teil */


  void Setz_OFIX_Teil(char *t2Print)

  {


	  char *pdest;
	  pdest = (char *)strchr(steustri, t2Print[0]);
	  if (pdest != NULL) {
		  int lauf, sola;
		  sola = strlen(t2Print);

		  for (lauf = 0; lauf < sola; lauf++) {
			  if (t2Print[lauf] == '>') t2Print[lauf] = ']';
			  if (t2Print[lauf] == '<') t2Print[lauf] = '[';
			  if (t2Print[lauf] == ';') t2Print[lauf] = ',';
		  };



		  int inblock = (int)(pdest - steustri);
		  if (inblock > 12) {
			  inblock = inblock - 13;
			  strncpy(SteuBlock2[inblock], t2Print + 2,511);   // String nach tline kopieren
		  }
		  else  strncpy(SteuBlock[inblock], t2Print + 2,255);  // String nach tline kopieren
	  };

  };  // Setz_FIX_Teil

void Setz_FIX_Teil( char *t2Print)

  {



			 
		     char *pdest;
		 	 pdest = (char *)strchr( steustri , t2Print[0] );
   	         if ( pdest != NULL ) {
			 int inblock = (int)(pdest - steustri );
			 if ( inblock > 12 ) {
	             inblock = inblock - 13;
          	     strncpy(SteuBlock2[inblock],t2Print + 2,511 );   // String nach tline kopieren
		                     	 }
			     else  strncpy(SteuBlock[inblock],t2Print + 2,255 );  // String nach tline kopieren
			                 };

 };  // Setz_FIX_Teil

void Setz_FIX_Block( char einchar)
{
			Setz_FIX_Teil("F,[ B ]");
			Setz_FIX_Teil("f,[ BO]");
			Setz_FIX_Teil("U,[ U ]");
			Setz_FIX_Teil("u,[ UO]");
			Setz_FIX_Teil("Q,[ I ]");
			Setz_FIX_Teil("q,[ IO]");
			Setz_FIX_Teil("I,[0");

   


 };  // Setz_FIX_Teil


 void Setz_ein_Teil( char *t2Print)
 // wenn wir da sind fangt die Zeile mit [= an, also Steurzeile
 // zB: [==,4,[PS 0 C RED]

  {

#ifdef mitlogfile
  if ( mitlog ) {
  sprintf(mMsg, "Set=: %s\n",t2Print);
  fputs( mMsg, flog );
  };
#endif    
    if ( t2Print[0] == '^' ) t2Print++; 
    if ( t2Print[2] == '=' ) {

		    Setz_FIX_Teil(t2Print+4);
	        return;
			                 };
	if (t2Print[2] == ']') {

		//  old flex2pdf mode:
		// ^[=]=,E,<PS 0 AO><x>1.4;26.3;20;=;ft;all\foot.rtf
		if (t2Print[4] == ',') 	 Setz_OFIX_Teil(t2Print + 5);  // mit wechsel zwischen < [ ...
		               else 	 PrintSetGleich(t2Print + 2);  // ^[=]=set,f,38-1658/08__%%i__1087Z
		return;


	};

	//   if ( t2Print[2] == '~' ) {
	//		    Setz_RTF_Teil(t2Print+4);
	//      return;
	//	                 };

    if ( t2Print[2] == '%' ) {
 			  char *pdest;
			  // char tline[192];  // unused

			  pdest = (char *)strchr(steuvars, (lower(t2Print[4])));
   	          if ( pdest != NULL ) {
			  int inblock = (int)(pdest - steuvars );

            //  for ( int kllauf = 0; kllauf < solang; kllauf++)

  	       	  strncpy(SteuVar[inblock],t2Print + 6,255);
			                  };
			  return;
			  };

  int bissl = strlen(t2Print);
  int lastpoint = 0;
  for ( int ilauf = 1;ilauf < bissl;ilauf++ ) {
     if (t2Print[ilauf] == '[' ) {
		 t2Print[ilauf] = '\0';
		 PrintSetGleich( t2Print + lastpoint );
		 t2Print[ilauf] = '[';
		 lastpoint = ilauf;
	 };

	 };
     
	 PrintSetGleich( t2Print + lastpoint );

 };  // Setz_ein_Teil

 
void SetEBlock( char *aktpar)
{
          if (! strncmp( aktpar , "a4",2)) {
               VpeSet(hDoc,VTOPMARGIN,0.8);
		       VpeSet(hDoc,VRIGHTMARGIN,20.2 );
			   VpeSet(hDoc,VBOTTOMMARGIN,29.2 );


			  if ( aktpar[2] == '1' ) {
               VpeSet(hDoc,VLEFTMARGIN,1.4);
			  };
			  if ( aktpar[2] == '2' ) {
               VpeSet(hDoc,VLEFTMARGIN,1.2);
		       VpeSet(hDoc,VRIGHTMARGIN,19.8 );
			  };

			  if (aktpar[2] == '3') {
				  VpeSet(hDoc, VLEFTMARGIN, 1.32);
				  VpeSet(hDoc, VRIGHTMARGIN, 20.5);

			  };


			 if ( aktpar[2] == '7' ) {
               VpeSet(hDoc,VLEFTMARGIN,1.7);
			 };
			 if ( aktpar[2] == '0' ) {
               VpeSet(hDoc,VLEFTMARGIN,1.0);
               VpeSet(hDoc,VTOPMARGIN,0.6);
		       VpeSet(hDoc,VRIGHTMARGIN,20.5 );
			 };
			 if ( aktpar[2] == '-' ) {
               VpeSet(hDoc,VLEFTMARGIN,0.4);
               VpeSet(hDoc,VTOPMARGIN,0.6);
		       VpeSet(hDoc,VRIGHTMARGIN,20.5 );
			 };

			 
	//		VpeSetPageOrientation(hDoc, VORIENT_LANDSCAPE);

			 mypcontrol = ( aktpar[3] == 'a' );
			 return;
		                                      };

		  if (!strncmp(aktpar, "us", 2)) {

			  VpeSetPageFormat(hDoc, VPAPER_LETTER);  // Daten von mein HP mit A4
			  VpeSetPageWidth(hDoc, 21.59);
			  VpeSetPageHeight(hDoc, 27.94);
			  VpeSet(hDoc, VLEFTMARGIN, 1.0);
			  VpeSet(hDoc, VTOPMARGIN, 0.8);
			  VpeSet(hDoc, VRIGHTMARGIN, 20.1);
			  VpeSet(hDoc, VBOTTOMMARGIN, 27.4);




		//	  VpeSet(hDoc, VTOPMARGIN, 0.8);
		//	  VpeSet(hDoc, VRIGHTMARGIN, 20.2);
		//	  VpeSet(hDoc, VBOTTOMMARGIN, 29.2);

			  if (aktpar[2] == '1') {
				  VpeSet(hDoc, VLEFTMARGIN, 1.4);
			  };
			  if (aktpar[2] == '2') {
				  VpeSet(hDoc, VLEFTMARGIN, 1.5);
				  VpeSet(hDoc, VRIGHTMARGIN, 20.1);
			  };
			  if (aktpar[2] == '7') {
				  VpeSet(hDoc, VLEFTMARGIN, 1.7);
			  };
			  if (aktpar[2] == '0') {
				  VpeSet(hDoc, VLEFTMARGIN, 1.0);
				  VpeSet(hDoc, VTOPMARGIN, 0.6);
				  VpeSet(hDoc, VRIGHTMARGIN, 20.5);
			  };
			  if (aktpar[2] == '-') {
				  VpeSet(hDoc, VLEFTMARGIN, 0.4);
				  VpeSet(hDoc, VTOPMARGIN, 0.6);
			  };


			  //		VpeSetPageOrientation(hDoc, VORIENT_LANDSCAPE);

			  mypcontrol = (aktpar[3] == 'a');
			  return;
		  };


           if (! strncmp( aktpar , "doc",3)) {
			 blockconvert = false;
			 oemconvert = true;
			 goto setdopinf;
		   }; 

           if (! strncmp( aktpar , "dos",3)) {
			 blockconvert = true;
			 oemconvert = true;
			 goto setdopinf;
		   };

		   if (! strncmp( aktpar , "wic",3)) {
 		       blockconvert = false;
			   oemconvert = false;
		   };
    
		   if (! strncmp( aktpar , "win",3)) {
			 blockconvert = true;
             oemconvert = false;
	setdopinf:
			 if (aktpar[3] == 'p') aktpar[3] = '+';
			 if (aktpar[4] == 'p') aktpar[4] = '+';

			 mitdoppchar = ( aktpar[3] != '+' );
			 mitRTFchars = ( aktpar[4] == '+' );
			 return;
		   };

          if (! strncmp( aktpar , "lng:",4)) {
			    strncpy(aktulang,aktpar+4,3);
  		   	    return;
                           	  	             };

			     char *pdest;
		 	 pdest = (char *)strchr( aktpar + 4 , 'x');
  	         if ( pdest != NULL ) {
		       int inblock = (int)( pdest - aktpar );
			   aktpar[inblock] = '\0';
			   inblock++;
			   wuzezahl = atoi(aktpar + inblock); 
			 };

        	wucharzahl = atoi(aktpar+4); // Entweder Punkte 6..20 oder Breite 60 ..240
			if (! wucharzahl ) wucharzahl = 92;  
			
	    	globczhoch = 0;
		   if (! strncmp( aktpar , "fix",3)) goto fixfont;
   		   if (! strncmp( aktpar , "pro",3)) goto Arial;
		   if (! strncmp( aktpar , "cou",3)) goto fixfont;
		   if (! strncmp( aktpar , "luc",3)) goto Lucida;
		   if (! strncmp( aktpar , "ari",3)) goto Arial;
   		   if (! strncmp( aktpar , "con",3)) goto Consolas;
		   return;

Arial:
			Setz_FIX_Teil("0,['Arial' S10 BO UO IO]");
			Setz_FIX_Block(' ');
			Setz_FIX_Teil("B,[ S12]");
			Setz_FIX_Teil("b,[ S10]");
			Setz_FIX_Teil("S,[ S9 ]");
			Setz_FIX_Teil("s,[ S10]");

			return;

	fixfont:
			if (wucharzahl > 141)  goto Luci6;
			if (wucharzahl > 129)  goto Con7;
			if (wucharzahl > 123)  goto Luci7;
			if (wucharzahl > 112)  goto Con8;
			if (wucharzahl > 111)  goto Luci8;
			if (wucharzahl > 100)  goto Con9;
			if (wucharzahl > 99 )  goto Luci9;
			if (wucharzahl > 90)   goto Con99;
			goto Luci100;


		Consolas:
			if (wucharzahl > 129) goto Con7;
			if (wucharzahl > 112) goto Con8;
			if (wucharzahl > 100) goto Con9;
			goto Con99;

	    Lucida:
			if (wucharzahl > 141)  goto Luci6;
			if (wucharzahl > 123)  goto Luci7;
			if (wucharzahl > 111)  goto Luci8;
			if (wucharzahl > 99 )  goto Luci9;
			goto Luci100;


return;


Con7:
   VpeSetFont(hDoc, "Consolas", 7);  // da bleibt B immer
   Setz_FIX_Teil("0,['Consolas' S7 BO UO IO]");
   Setz_FIX_Teil("B,[ S9 B]");
   Setz_FIX_Teil("b,[ S7 BO]");
   Setz_FIX_Teil("S,[ S6]");
   Setz_FIX_Teil("s,[ S6]");
   Setz_FIX_Block(' ');
   globczoffs = 0.0086;

   // da kein RTF
return;



Con8:
VpeSetFont(hDoc, "Consolas", 8);  // da bleibt B immer
Setz_FIX_Teil("0,['Consolas' S8 BO UO IO]");
Setz_FIX_Teil("B,[ S9 B]");
Setz_FIX_Teil("b,[ S8 BO]");
Setz_FIX_Teil("S,[ S6]");
Setz_FIX_Teil("s,[ S8]");
Setz_FIX_Block(' ');
globczoffs = 0.017;
// da kein RTF
return;

Con9:
VpeSetFont(hDoc, "Consolas", 9);  // da bleibt B immer
Setz_FIX_Teil("0,['Consolas' S9 BO UO IO]");
Setz_FIX_Teil("B,[ S10 B]");
Setz_FIX_Teil("b,[ S9 BO]");
Setz_FIX_Teil("S,[ S8]");
Setz_FIX_Teil("s,[ S9]");
Setz_FIX_Block(' ');
// da kein RTF
 globczoffs = 0.025;
return;

Con99:

VpeSetFont(hDoc, "Consolas", 10);  // da bleibt B immer
Setz_FIX_Teil("0,['Consolas' S10 BO UO IO]");
Setz_FIX_Teil("B,[ S12 B ]");
Setz_FIX_Teil("b,[ S10 BO ]");
Setz_FIX_Teil("S,[ S8]");
Setz_FIX_Teil("s,[ S10]");
Setz_FIX_Block(' ');
globczoffs = 0.02;
// globczoffs = 0.05;
return;

Luci6:  // kleinste
VpeSetFont(hDoc, "Lucida Console", 6);  // da bleibt B immer
Setz_FIX_Teil("0,['Lucida Console' S6 BO UO IO]");
Setz_FIX_Teil("B,[ S8 B]");
Setz_FIX_Teil("b,[ S6 BO]");
Setz_FIX_Teil("S,[ S5]");
Setz_FIX_Teil("s,[ S6]");
Setz_FIX_Block(' ');
// da kein RTF
globczoffs = 0.02;
return;

 Luci7:
 	  VpeSetFont(hDoc, "Lucida Console", 7);  // da bleibt B immer
	  Setz_FIX_Teil("0,[ 'Lucida Console' S7 BO UO IO]");
	  Setz_FIX_Teil("B,[ 'Arial' S9 B ]");
	  Setz_FIX_Teil("b,[ 'Lucida Console' S7 BO]");
	  Setz_FIX_Teil("S,[ S6]");
	  Setz_FIX_Teil("s,[ S7]");
	  Setz_FIX_Block(' ');
	  // da kein RTF
	  globczoffs = 0.03;
	  return;
  
  Luci8:
	 VpeSetFont(hDoc, "Lucida Console", 8);  // da bleibt B immer
	  Setz_FIX_Teil("0,['Lucida Console' S8 BO UO IO]");
	  Setz_FIX_Teil("B,[ S10 B]");
	  Setz_FIX_Teil("b,[ S8 BO]");
	  Setz_FIX_Teil("S,[ S7]");
	  Setz_FIX_Teil("s,[ S8]"); 
	  Setz_FIX_Block(' ');
	  globczoffs = 0.04;

	  // da kein RTF
	  return;

  
 Luci9:
  	  VpeSetFont(hDoc, "Lucida Console", 9);  // da bleibt B immer
	  Setz_FIX_Teil("0,['Lucida Console' S9 BO UO IO]");
	  Setz_FIX_Teil("B,[ S10 B]");
	  Setz_FIX_Teil("b,[ S9 BO]");
	  Setz_FIX_Teil("S,[ S8]");
	  Setz_FIX_Teil("s,[ S9]");
	  Setz_FIX_Block(' ');
	  // da kein RTF
	  globczoffs = 0.06;
	  return;

  Luci100:
	  VpeSetFont(hDoc, "Lucida Console", 10);  // da bleibt B immer
	  Setz_FIX_Teil("0,['Lucida Console' S10 BO UO IO]");
	  Setz_FIX_Teil("B,[ S12 B]");
	  Setz_FIX_Teil("b,[ S10 BO]");
	  Setz_FIX_Teil("S,[ S8]");
	  Setz_FIX_Teil("s,[ S10]");
	  Setz_FIX_Block(' ');
	  // da kein RTF
	  globczoffs = 0.08;
	  return;


};


int setmyprs( char *line )
  {
	         char tline[128];
			 int zweitepar = 0;
	 

              // drei m�glichkeiten: 1 define mit PRS File
			    if ( upname[0] == '\0') {
                   if ( (strlen(line)) < 3 ) {
					    zweitepar = myPosin(magicchar);
						strcpy(tline,"prn");
						if ( zweitepar >= 0 )  {
  						   char par2[7];
 			         	  _itoa(zweitepar,par2,10);
					 	  strcat(tline,par2);
					             	};
                        zweitepar = 1;  
				
				                }
				   else 	sscanf( line + 3, "%[^,],%i",tline, &zweitepar );

				 		                }
				   else {
                sscanf( upname, "%[^,],%i",tline, &zweitepar );
                if ( zweitepar <= 0 ) zweitepar = 1;
				        }

				if ( tline[0] ==  '=' ) { // ist als Device
						if (dbg_time_threshold > 0) {
							_int64 dbg_start, dbg_end, dbg_freq;
							QueryPerformanceFrequency((LARGE_INTEGER *)&dbg_freq);
							QueryPerformanceCounter((LARGE_INTEGER *)&dbg_start);
							int erg = VpeSetDevice(hDoc, tline + 1);
							QueryPerformanceCounter((LARGE_INTEGER *)&dbg_end);
							if ((dbg_end - dbg_start) / dbg_freq > dbg_time_threshold) {
								sprintf(mMsg, "WARNUNG: VpeSetDevice dauerte %.1f Sek!\nDrucker: %s", (double)(dbg_end - dbg_start) / (double)dbg_freq, tline + 1);
								Msg(mMsg);
							}
						} else {
							int erg = VpeSetDevice(hDoc, tline + 1);
						}
						goto setprifo;
				                        };   

				    char *p;
   				    p = strchr( tline, '.' );   // ist mit PRS
					
					if ( p == NULL ) strcat(tline,".PRS");
				// 	else VpeSetPrintOptions(hDoc, PRINT_ALL + PRINT_NO_AUTO_PAGE_DIMS);

					p = strchr( tline, '\\' );   // ist mit Pfad
					if ( p == NULL ) {
 	                char htline[128];

					if ( aushome ) {
					if (! GetEnvironmentVariable("HOME", htline, 99) ) return -2;
						  strcat(htline,"\\");
						  strcat(htline,tline);
			   			  strcpy(tline,htline);

					}
					else {
			   				  strcpy(htline,curexedir);
							  strcat(htline,tline);
			   				  strcpy(tline,htline);
					                 };

					                 };


		//     VpeSetPrintOptions(hDoc, PRINT_ALL + PRINT_NO_AUTO_PAGE_DIMS);
		     int setret;
			 if (dbg_time_threshold > 0) {
				 _int64 dbg_start, dbg_end, dbg_freq;
				 QueryPerformanceFrequency((LARGE_INTEGER *)&dbg_freq);
				 QueryPerformanceCounter((LARGE_INTEGER *)&dbg_start);
				 setret = VpeSetupPrinter(hDoc, tline, zweitepar );
				 QueryPerformanceCounter((LARGE_INTEGER *)&dbg_end);
				 if ((dbg_end - dbg_start) / dbg_freq > dbg_time_threshold) {
					 sprintf(mMsg, "WARNUNG: VpeSetupPrinter dauerte %.1f Sek!\nPRS: %s", (double)(dbg_end - dbg_start) / (double)dbg_freq, tline);
					 Msg(mMsg);
				 }
			 } else {
				 setret = VpeSetupPrinter(hDoc, tline, zweitepar );
			 }
		     if ( setret ) {
				 if ( setret == 1 )   return 0;   // cancel button
				 if ( setret == 4 )  {
				 	  char Msg[255];
			     	  sprintf(Msg, "Setup Datei fehlerhaft:\n>%s",tline);
			          MessageBox(hMainWindow, Msg, "Leider", MB_OK);
                      return -1;
				 };

	//	 	 VpeSetPageFormat(hDoc, VpeGetDevPaperFormat(hDoc));
	//		 VpeSetPrintOptions(hDoc, PRINT_ALL);
			                  };
setprifo:
			 if (dbg_time_threshold > 0) {
				 _int64 dbg_start3, dbg_end3, dbg_freq3;
				 QueryPerformanceFrequency((LARGE_INTEGER *)&dbg_freq3);
				 QueryPerformanceCounter((LARGE_INTEGER *)&dbg_start3);
				 VpeCoord myco = VpeGetDevPaperWidth(hDoc);
				 VpeSetPageWidth(hDoc, myco );
				 myco = VpeGetDevPaperHeight(hDoc);
				 VpeSetPageHeight(hDoc, myco);
				 VpeSetPageOrientation(hDoc, VpeGetDevOrientation(hDoc));
				 QueryPerformanceCounter((LARGE_INTEGER *)&dbg_end3);
				 if ((dbg_end3 - dbg_start3) / dbg_freq3 > dbg_time_threshold) {
					 Msg("WARNUNG: VpeGetDev* (Drucker-Abfrage) dauerte lang!");
				 }
			 } else {
				 VpeCoord myco = VpeGetDevPaperWidth(hDoc);
				 VpeSetPageWidth(hDoc, myco );
				 myco = VpeGetDevPaperHeight(hDoc);
				 VpeSetPageHeight(hDoc, myco);
				 VpeSetPageOrientation(hDoc, VpeGetDevOrientation(hDoc));
			 }


			 return 1;
};

void setmee( char *SetStri,int zuerst)
  {

      char par1[15];


	  // default a4 Full

		if (zuerst) {
  	  	     VpeSetPageFormat(hDoc,VPAPER_A4);  // Daten von mein HP mit A4
			 VpeSetPageWidth (hDoc, 20.997 );     
             VpeSetPageHeight(hDoc, 29.697);
             VpeSet(hDoc,VLEFTMARGIN,0.5);
             VpeSet(hDoc,VTOPMARGIN,0.5);
		     VpeSet(hDoc,VRIGHTMARGIN, 20.574  ); // 20.151 + 0.423 );
			 VpeSet(hDoc,VBOTTOMMARGIN,29.198  );

	//		VpeSetDevOrientation(hDoc, VORIENT_LANDSCAPE);
		     VpeSetCharset(hDoc,  1 );  // Standard Win
             oemconvert = true;
			 oemcodepage = 0;
             mitdoppchar = true;
			 mitRTFchars = false;
			 strcpy(aktulang,"GER");
//			if ( aktline > 5 ) { 
//			                       myPBreak();
//								   aktufdoc++;
//							       lasty = VpeGet(hDoc, VTOPMARGIN); 
//			  		     		 };


			mypcontrol = false;
			
/*			if (zuerst >= 10) {  // erste Zeile ohne [=)
				zuerst = zuerst - 10;
			    if (zuerst < 20) zuerst = 10;
			    if (zuerst > 240) zuerst = 10;
				char par2[15];
				_itoa(zuerst,par2,10);
				strcpy(par1,SetStri);
				strcat(par1,par2);
			    SetEBlock(par1);
			    //	return;
 			}; */

//			if ( printme ) {
//			  if (    setmyprs("") < 1 ) exit ( 99 ) ;
//			};


		};

	  wuzezahl = 0;
//	  SetStri[25] = '\0';
	  int gutechar = 0;
	  for ( int slauf = 0 ; SetStri[slauf] != '\0'; slauf++ ) {
		if (  SetStri[slauf] > '!' ) {
				if ( SetStri[slauf] == ';') break;
				if ( SetStri[slauf] == '^') {
					                          zuerst = 1;
											  continue;
				};
				par1[gutechar] = lower (SetStri[slauf]);
				gutechar++;
		 } else {
			if ( gutechar) {
				par1[gutechar] = '\0';
				SetEBlock(par1);
				gutechar = 0;
			};

		};
	  };

	if ( gutechar) {
				par1[gutechar] = '\0';
				SetEBlock(par1);
	 	    	};	        

 if ( zuerst ) {
//	Eincharp('0');
//  	double wuzehoch;

	VpeSetPenSize(hDoc,0);  
 //   VpeSetFont(hDoc,"Courier",10);  
	//VpeRenderPrint(hDoc,0,0,"X");
	//czbreit  = VpeGet(hDoc, VRENDERWIDTH); 
	// czbreit 0.231
    VpeRenderPrint(hDoc,0,0,"X");
	czbreit  = VpeGet(hDoc, VRENDERWIDTH); 
	czbreit = czbreit - 0.02;
	czhoch = VpeGet(hDoc, VRENDERHEIGHT);
//	if (globczoffs > 0) VpeSetTextAlignment(hDoc, 5);

	// czbreit 0.2132


  /*  VpeRenderPrint(hDoc,0,0,"XX");
	czbreit  = VpeGet(hDoc, VRENDERWIDTH); 
	czbreit = czbreit / 2; 

	if (f2tmode) {
		VpeSetPenSize(hDoc, 0);
		globczoffs = 0.01;
		if (globczoffs > 0) {
			  VpeSetTextAlignment(hDoc,5);

			if ( colf2t ) {
			// 2,54 / 300 =  0.008467

	// 		 globczoffs = 0.008467;
	//		 globczoffs = 0.03;
			 // globczoffs = 0;
			 czhoch = czhoch + (2 * globczoffs);
			 VpeSetPenSize(hDoc, 0.001);
			 VpeSetPenColor(hDoc, COLOR_WHITE);
	//		 VpeSetCornerRadius(hDoc, 0);

		// 	VpeSetPen(hDoc,  globczoffs, 0, COLOR_WHITE);
		                  }
			else czhoch = czhoch + (2 * globczoffs);

		};
	};  */

	// 	czhoch = czhoch + 0.05; (zb bei Luc)

//	if ( wuzezahl ) {
	//		wuzezahl++;
	//	    wuzezahl++;
	//		wuzezahl++;
	//			wuzehoch = ( VpeGet(hDoc, VBOTTOMMARGIN) - VpeGet(hDoc, VTOPMARGIN) ) / wuzezahl;
	//	if (wuzehoch > czhoch) {
	//		czhoch = wuzehoch;
	//	};
	// };
	//if ( wucharzahl ) {  /7 sp�ter vielleicht

	//  double echtsobre = wucharzahl * czbreit ;

	//	wuzehoch = ( VpeGet(hDoc, VRIGHTMARGIN) - VpeGet(hDoc, VLEFTMARGIN) ) ; // da ist es die Berite
	//	if ( echtsobre < wuzehoch ) {
	//		        echtsobre =  wuzehoch - echtsobre;  // der differenz
	//				echtsobre = 3 * echtsobre / 4 ;
	// 	            wuzehoch = ( VpeGet(hDoc, VLEFTMARGIN));
 //                   VpeSet(hDoc,VLEFTMARGIN, wuzehoch + echtsobre);
	//	}; 
	//	  
	//	};
  	// VpeSetDevice(hDoc, "");
     
	VpeSetDefOutRect( hDoc,VpeGet(hDoc, VLEFTMARGIN), VpeGet(hDoc, VTOPMARGIN),VpeGet(hDoc, VRIGHTMARGIN), VpeGet(hDoc, VBOTTOMMARGIN) );

	};



 };
	

 void PrinteLine( char *t2Print)
 
  {

#ifdef mitlogfile
  if ( mitlog ) {
  sprintf(mMsg, "PrinteLine=: %s\n",t2Print);
  fputs( mMsg, flog );
  };
#endif    

     int ganz_ende = strlen(t2Print) - 1;
// 	       for ( ;  (ganz_ende >= 0 ) && ( t2Print[ganz_ende] < 32 )  && ( t2Print[ganz_ende] > 0); ganz_ende--);
//         t2Print[ganz_ende+1] = '\0';
		 
         int lauf   = 0;
         int anfang = 0;
	

         int zegrenze = ganz_ende - 2;



 eincharweiter:
    lauf++;
      
	if ( t2Print[lauf] == '[' ) {   // Steuerzeichen fangt an

//		if ( line[lauf+1] == '[' ) goto eincharweiter;   // doppel [[
		if ( t2Print[lauf-1] == '[' ) goto eincharweiter;   // doppel [[
		t2Print[lauf] = '\0';    // Block VORHER schliessen
		Print_ein_Teil( t2Print + anfang,lauf - anfang );
		t2Print[lauf] = '[';      // neuanfang
        anfang = lauf  ;
			                        	
			
		};  // mit Trennzeichen

		if ( lauf <= zegrenze ) goto eincharweiter;
		                     else { 
	                if ( lauf < ganz_ende) {
				     lauf = ganz_ende;
				 	 Print_ein_Teil( t2Print + anfang, lauf - anfang );
					                       }
                           };

 };  // vPrinteLine

 bool toggle = false;


 


 LRESULT CALLBACK NewVPEProc(HWND hwnd, UINT message,
	 WPARAM wParam, LPARAM lParam)
 {

   if ( message ==  WM_RBUTTONUP ) {
	   POINT point;
	   point.x = LOWORD ( lParam);
	   point.y = HIWORD ( lParam);
	   ClientToScreen(hMainWindow ,&point);
	   HMENU mypop = GetSubMenu(hpopMenu,0);

	   TrackPopupMenu(mypop, TPM_RIGHTBUTTON, point.x, point.y,0,hMainWindow,NULL);

	   return(0);
   };

 

   if ( message !=  WM_VSCROLL ) goto noprue;

    if ( wParam == SB_LINEDOWN ) {
      int mpcount; 
	  mpcount = VpeGetPageCount(hDoc);
  	  if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) {
		  ShowWindow(hMainWindow,SW_SHOWMAXIMIZED );
 		  VpeGotoVisualPage(hDoc, mpcount);
		  VpeSendKey(hDoc,VKEY_FULL_PAGE );
		  return 0;
	  };

      if ( mpcount < 2 ) goto noprue;

	  if ( GetAsyncKeyState(VK_CONTROL) & 0x80000000 ) { 
 		  VpeGotoVisualPage(hDoc, mpcount);
		  return 0;
	  };
	
//	    VpeSendKey(hDoc,VKEY_PAGE_LAST);
//	    VpeSendKey(hDoc,VKEY_SCROLL_PAGE_DOWN);

   int n = VpeGetVisualPage(hDoc);
   if ( n < mpcount ) VpeGotoVisualPage(hDoc, n + 1);
	  else    VpeGotoVisualPage(hDoc, 1);
	       return 0;
};
if ( wParam == SB_LINEUP ) {
      int mpcount; 
		  mpcount = VpeGetPageCount(hDoc);
  	  if ( GetAsyncKeyState(VK_SHIFT) & 0x80000000 ) {
		  ShowWindow(hMainWindow,SW_SHOWMAXIMIZED );
 		  VpeGotoVisualPage(hDoc,1);
		  VpeSendKey(hDoc,VKEY_FULL_PAGE );
		  return 0;
	  };

      if ( mpcount < 2 ) goto noprue;
  	  if ( GetAsyncKeyState(VK_CONTROL) & 0x80000000 ) {
 		  VpeGotoVisualPage(hDoc, 1);
		  return 0;
	  };

	  int n = VpeGetVisualPage(hDoc);
      if ( n > 1 ) VpeGotoVisualPage(hDoc, n - 1);
	   else    VpeGotoVisualPage(hDoc, mpcount);

//	    VpeSendKey(hDoc,VKEY_PAGE_FIRST);
//	    VpeSendKey(hDoc,VKEY_SCROLL_PAGE_UP);
	       return 0;
};

noprue:
  return CallWindowProc (g_OldVPE, hwnd, message, wParam, lParam);
;
}


 static int myDuptest()
{
      hDoc = VpeOpenDoc(hMainWindow, "Test", VPE_EMBEDDED + VPE_NO_RULERS + VPE_NO_PAGESCROLLER + VPE_FIXED_MESSAGES );
      // Open DOC and set  VpeDevice to standard printer, (my standard printer has no duplex)

	  VpeSetPrintOptions(hDoc, PRINT_ALL + PRINT_NO_AUTO_PAGE_DIMS);
	  // I try with and without this, the same 

	  myDuplex = VDUP_VERTICAL; // myDuplex is a global int variable

 	  VpeGetDevice(hDoc,mMsg,64);
  	  VpePrint(hDoc, 1 , 2, mMsg);
	  sprintf(mMsg, "Duplex: %d\n",VpeGetDevDuplex(hDoc)); 
	  VpePrint(hDoc, 1 , 4, mMsg);
      VpePageBreak(hDoc);
  	  VpePrint(hDoc, 1 , 4, "[ S 24 ]Page ... 2");
 	  VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);

	  VpePrintDoc(hDoc,1);  
	  // Print with Dialog, now I can change to my duplex Printer
	  // but the output is simple

	  return 1;
}

int AutoRender(int online)
{

	FILE *fh;
	bool abruchwar = false;

	_int64 now, freq, start;
	QueryPerformanceCounter((LARGE_INTEGER *)&start);

	if (online < 1) {
		if (hDoc) VpeCloseDoc(hDoc);
		if (dbg_time_threshold > 0) {
			_int64 dbg_start, dbg_end, dbg_freq;
			QueryPerformanceFrequency((LARGE_INTEGER *)&dbg_freq);
			QueryPerformanceCounter((LARGE_INTEGER *)&dbg_start);
			hDoc = VpeOpenDoc(hMainWindow, "FlexPrint", mytbmode);
			QueryPerformanceCounter((LARGE_INTEGER *)&dbg_end);
			if ((dbg_end - dbg_start) / dbg_freq > dbg_time_threshold) {
				sprintf(mMsg, "WARNUNG: VpeOpenDoc dauerte %.1f Sekunden!", (double)(dbg_end - dbg_start) / (double)dbg_freq);
				Msg(mMsg);
			}
		} else {
			hDoc = VpeOpenDoc(hMainWindow, "FlexPrint", mytbmode);
		}
		Myreg();
	};




	if ((strlen(upname)) > 4) {
		int prs_result;
		if (dbg_time_threshold > 0) {
			_int64 dbg_start, dbg_end, dbg_freq;
			QueryPerformanceFrequency((LARGE_INTEGER *)&dbg_freq);
			QueryPerformanceCounter((LARGE_INTEGER *)&dbg_start);
			prs_result = setmyprs("");
			QueryPerformanceCounter((LARGE_INTEGER *)&dbg_end);
			if ((dbg_end - dbg_start) / dbg_freq > dbg_time_threshold) {
				sprintf(mMsg, "WARNUNG: setmyprs dauerte %.1f Sekunden!\nPRS-Datei: %s", (double)(dbg_end - dbg_start) / (double)dbg_freq, upname);
				Msg(mMsg);
			}
		} else {
			prs_result = setmyprs("");
		}
		if (prs_result < 1) exit(99);  // printer setzen wenn definiert
	}
		
	_splitpath(file2print, drive, mdir, fname, ext);

	f2tmode = true;                       
	archmode = false;
	f2magic = '0';                        // schnelle Textconverter

	if (!_strnicmp(ext, ".F2", 3)) f2magic = lower(ext[3]);
	if (!_strnicmp(ext, ".A2", 3)) {
		archmode = true;
		f2magic = lower(ext[3]);
	};
	oldzwmode = (!_strnicmp(ext, ".ZW", 3));

	if (f2magic == 'p') f2tmode = false;  // da ohne textmode
	if (oldzwmode)      f2tmode = false;  // da ohne textmode


// also f2tmode == schnelle textkonverter 
//                  abh�ngig von f2magic "0" = plaint text, "t,w,.." color-line durch /t ' ' nbsp
// ist f2tmode falsch (.f2p) werden die volle f2p Befehle unterst�tzt
// nicht vergessen auch bei olzwmode

	will_save = 0;

	char zwilines[33][255];  //  P123456789ENLxyz
	int  zwilcount = 0;


#ifdef mitlogfile
	mitlog = false;
	if (hMenu != NULL) {
		DWORD  fdwMenu = GetMenuState(hMenu, ID_SAVES_, MF_BYCOMMAND);
		if ((fdwMenu & MF_CHECKED)) mitlog = true;
		if (mitlog)  flog = fopen(".\\f2p.log", "wt");
	};

//	if (!mitlog) {
//		mitlog = true;
//		flog = fopen(".\\f2p.log", "wt");
//	};

#endif



	lasty = 0; lastx = 0;
	globczhoch = 0;


	// VpeEnableMouseScaling(hDoc, false);
	//   VpeSetPageScrollerTracking(hDoc, true);
	VpeSetMinScale(hDoc, 0.50);
	VpeSetPDFVersion(hDoc, VPE_PDF_ACROBAT_5);
	VpeSetDocExportPictureQuality(hDoc, PICEXP_QUALITY_HIGH);
	VpeDefineKey(hDoc, VKEY_CLOSE, VK_ESCAPE, 0, 0);
	VpeDefineKey(hDoc, VKEY_PAGE_FIRST, VK_HOME, VK_CONTROL, 0);
	VpeDefineKey(hDoc, VKEY_PAGE_LAST, VK_END, VK_CONTROL, 0);
	VpeDefineKey(hDoc, VKEY_PRINT, VK_F12, 0, 0);
	VpeDefineKey(hDoc, VKEY_OPEN, VK_F2, 0, 0);

	VpeSetPaperView(hDoc, gPaperView);

 //	VpeSetDocExportType(hDoc, VPE_DOC_TYPE_PDF);

	//	  VpeSetPreviewCtrl(hDoc, 1);
	//  VpeSetPrintOptions(hDoc, PRINT_ALL );
	// VpeSetPrintOptions(hDoc, PRINT_ALL + PRINT_NOABORTDLG );

	// VpeSetPreviewWithScrollers(hDoc, false );

// 	SCROLLBARINFO
	VpeSetPrintOptions(hDoc, PRINT_ALL + PRINT_NO_AUTO_PAGE_DIMS);
	VpeSetBkgMode(hDoc, VBKG_SOLID);
	if (myzoomode < 10) VpeSetScaleMode(hDoc, myzoomode);
	else VpeSetScalePercent(hDoc, myzoomode);

	//	  VpeSetScaleStep(hDoc, 0.5 );
	// 	bool inifile   = false;
	//  if (! SetThreadLocale(0x041b))  Msg("nein") ;


	char *line;

	char *pdest;
	int  anfang, ganz_ende;


	for (anfang = 0; anfang < 12; anfang++) {
		SteuBlock[anfang][0] = '\0';
		SteuBlock[anfang][64] = '\0';
		SteuBlock2[anfang][0] = '\0';
	};
	for (; anfang < 15; anfang++) {
		SteuBlock2[anfang][0] = '\0';
	};

	vormycol = -1;
	
	//=== ZW neu


	if (!oldzwmode) goto keinparmehr2;

	int zweitepar;
	char tline[512];
	char tline2[21];
	double dlpar1, dlpar2;
	double	 grightm, gbottomm, gleftm, gtopm;
	int sovpar;

	strcpy(tline, curexedir);
		strcat(tline, ext + 1);
		strcat(tline, ".set");
	//	_makepath(tline, drive, curexedir, ext + 1, ".set");

		if ((fh = fopen(tline, "rt")) == NULL) {
			oldzwmode = false;
			goto keinparmehr2;
		};


		setmee("cou.93 a41", 1);
		Setz_FIX_Teil("P,^[pP");


	line = zwilines[1];
neuparl:

 if (fgets(line, 1024, fh) == NULL)   goto keinparmehr;


 int tende = strlen(line);
 if (line[tende - 1] == '\n') line[tende - 1] = '\0';

 		 // keine leer

 switch (lower(line[1]))

 {
 case 'p': // Printer
	 // drei m�glichkeiten: 1 define mit PRS File
	 if (upname[0] == '\0') {
		 sscanf(line + 3, "%[^,],%i", tline, &zweitepar);
	 }
	 else {
		 sscanf(upname, "%[^,],%i", tline, &zweitepar);
		 if (zweitepar <= 0) zweitepar = 1;
	 }

	 if (tline[0] == '=') { // ist mit Pfad

		 VpeSetDevice(hDoc, tline + 1);
		 goto setprifo;


	 };

	 char *p;
	 p = strchr(tline, '.');   // ist mit PRS

	 if (p == NULL) {
		 strcat(tline, ".PRS");

		 p = strchr(tline, '\\');   // ist mit Pfad
		 if (p == NULL) {
			 strcpy(line, curexedir);
			 strcat(line, tline);
			 strcpy(tline, line);
		 };

	 };

	 VpeSetPrintOptions(hDoc, PRINT_ALL + PRINT_NO_AUTO_PAGE_DIMS);
	 p = strchr(tline, '!');
	 if (p == NULL) {
		 int setret = VpeSetupPrinter(hDoc, tline, zweitepar);
		 if (setret ) {
			 if (setret == 1)   return FALSE;   // cancel button
			 if (setret == 4)  {
				 char Msg[255];
				 sprintf(Msg, "Setup Datei fehlerhaft:\n>%s", tline);
				 MessageBox(hMainWindow, Msg, "Leider", MB_OK);
				 return FALSE;
			 };

		 };
		 // 	 VpeSetPageFormat(hDoc, VpeGetDevPaperFormat(hDoc));
		 //	 VpeSetPrintOptions(hDoc, PRINT_ALL);
	 };
 setprifo:
	 VpeSetPageWidth(hDoc, VpeGetDevPaperWidth(hDoc));
	 VpeSetPageHeight(hDoc, VpeGetDevPaperHeight(hDoc));
	 VpeSetPageOrientation(hDoc, VpeGetDevOrientation(hDoc));

	 goto neuparl;


 case 'r': // Margin bzw. Rand x y ( Standard 200 200 )
	 sscanf(line + 3, "%[^,],%s", tline, tline2);
	 dlpar1 = mypos(tline);
	 dlpar2 = mypos(tline2);


	 //  VpeSetOutRect(hDoc,anfang, zweitepar, VpeGetDevPaperWidth(hDoc) - anfang , VpeGetDevPaperHeight(hDoc) - zweitepar);
	 //   VpeSetDefOutRectSP(hDoc,anfang, zweitepar, VpeGetDevPaperWidth(hDoc) - anfang , VpeGetDevPaperHeight(hDoc) - zweitepar);
	 //	 }
	 grightm = dlpar1;
	 gbottomm = dlpar2;

	 VpeSet(hDoc, VRIGHTMARGIN, grightm);
	 VpeSet(hDoc, VBOTTOMMARGIN, gbottomm);
	 goto neuparl;

 case 'l': // linke bzw untere Rand wenn definiert Achte auf Reihenfolge Printer; Rechte Rand; Linke Rand
	 sscanf(line + 3, "%[^,],%s", tline, tline2);
	 dlpar1 = mypos(tline);
	 dlpar2 = mypos(tline2);
	 gleftm = dlpar1;
	 gtopm = dlpar2;
	 VpeSet(hDoc, VLEFTMARGIN, gleftm);
	 VpeSet(hDoc, VTOPMARGIN, gtopm);

	 goto neuparl;

 case 'f':  // font
	 sscanf(line + 3, "%[^,], %i", tline, &zweitepar);
	 VpeSetFont(hDoc, tline, zweitepar);
	 goto neuparl;

 case 'c':  // Charset

	 sovpar = sscanf(line + 3, "%[^,], %i,%[^',']", tline, &zweitepar, tline2);
	 VpeSetCharset(hDoc, zweitepar);
	 zweitepar = atoi(tline);
	 if (zweitepar > 0) oemconvert = true;
	 if (zweitepar > 1) oemcodepage = zweitepar;
	 oemcodepageback = 1250;
	 if (oemcodepage == 855) oemcodepageback = 1251;
	 if (oemcodepage == 866) oemcodepageback = 1251;
	 if (oemcodepage == 775) oemcodepageback = 1257;
	 if (oemcodepage == 850) oemcodepageback = 1252;
	 if (oemcodepage == 858) oemcodepageback = 1252;
	 if (sovpar == 3)  oemcodepageback = atoi(tline2);
	 goto neuparl;
 case 'h':  // Header
	 sscanf(line + 3, "%[^,], %i", tline, &zweitepar);
	 VpeDefineHeader(hDoc, 0.5, 0.5, (VpeGetDevPaperWidth(hDoc) - 0.5), 1.2, tline);
	 goto neuparl;
 case 's':  // Charsize
	 sscanf(line + 3, "%[^,], %i", tline, &zweitepar);
	 if (zweitepar != 0) {
		 czbreit = atoi(tline);
		 czhoch = zweitepar;
	 }
	 goto neuparl;
 case '[':  // Steuerung darf als letztes
	 pdest = (char *)strchr(steustri, line[2]);
	 if (pdest != NULL) {
		 int inblock = (int)(pdest - steustri);
		 if (inblock > 12) {
			 inblock = inblock - 13;
			 strcpy(SteuBlock2[inblock], line + 3);   // String nach tline kopieren
		 }
		 else  strcpy(SteuBlock[inblock], line + 3);  // String nach tline kopieren
#ifdef mitlogfile
		 if (mitlog) {

			 sprintf(mMsg, "steub-:%d|%s|%s\n", inblock, SteuBlock[inblock], line);
			 fputs(mMsg, flog);
		 };
#endif

	 };

	 goto neuparl;

 }

 goto neuparl;


keinparmehr:

 fclose(fh);

keinparmehr2:



 if (!_strnicmp(ext, ".VPE", 4)) {


	 zweitepar = VpeReadDoc(hDoc, file2print);
	 if (!zweitepar)   {
		 sprintf(mMsg, "[C RED]VPE ($incl) Error %d, %s", (VpeGetLastError(hDoc)), file2print);
		 lastwribot = VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), lasty, mMsg);
		 lasty = lasty + czhoch;
	 };





	 if (mitvorschau)  {
		 if (!warprev)  {
			 mypaintmode = 1;
			 VpePreviewDoc(hDoc, NULL, VPE_SHOW_MAXIMIZED);
			 SetFocus(VpeGetWindowHandle(hDoc));
			 warprev = 1;
		 }
		 if (VpeDispatchAllMessages(hDoc)) abruchwar = true;

	 };




 if (mitvorschau)  {
	 // einige WM_Msg umlenken
	 HWND myhandle;
	 myhandle = GetWindow(VpeGetWindowHandle(hDoc), GW_CHILD);  // Achtung! das ist erste Child von das Parent-Window
//	 g_OldVPE = (WNDPROC)SetWindowLong(myhandle, GWL_WNDPROC, (LONG)NewVPEProc);
	//  g_OldVPE = (WNDPROC)SetWindowLongPtr(myhandle, GWLP_WNDPROC, (LONG)NewVPEProc);
 };


	 return 1;
 };

 VpeSetDevScalePercent(hDoc, 102);

//=== End ZW

  if ((fh=fopen(file2print, "rbS")) == NULL)
   {
	  sprintf(mMsg, "Fehler bei �ffnen!\nDateiname: %s!",file2print);
      MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
	  exit(7);
      //return false;
   };


  int maxread;
  fseek(fh, 0, SEEK_END);
  maxread = ftell(fh);
  if (maxread > 64000 ) maxread = 48000;
  fseek(fh, 0, SEEK_END);
  fseek(fh, 0, SEEK_SET);   // r�ckspulen


       wantbreak = false;
	   BOOL informff = false;
       finlastline = 0;
	   aktline  = 1;

		if ( oldzwmode ) goto setmitzwok;

		if ( fgets( tline, 216, fh ) == NULL)   goto ibinfertig;   // hole erste Zeile

		if (!f2tmode) { 

			if (!strncmp(tline, "<@F5>", 5)) { // alte Archiv Format
				for (int lauf = 0; lauf < 8; lauf++)	fgets(tline, 216, fh);   // erste 8 Zeilen �berlesen
				strcpy(tline, "[=) dos++ Arial");
			};

			if (!strncmp(tline, "[=)", 3)) {
				setmee(tline + 3, 1);       // f2p Doc handle selbst
				goto setmitzwok;
			};

		};

			if (mySetupStr[0] == '-') {

				if (mySetupStr[1] == '-') {
					strcpy(mySetupStr, "dos--!a43a!fix.92");
					if (f2magic == 't') mySetupStr[1] = 'd';
					if (f2magic == 'w') mySetupStr[1] = 'w';

				};
			// 	dev\nagy\tmp6.f2t - kdos--!a40a!con.## - v
				if (mySetupStr[1] == 'a') strcpy(mySetupStr, "win--!a43a!fix.92");
				if (mySetupStr[1] == 'd') strcpy(mySetupStr, "dos--!a43a!fix.##");
				if (mySetupStr[1] == 'w') strcpy(mySetupStr, "win--!a43a!fix.##");
			};
			int solang = strlen(tline);
			solang--;

			if (f2magic != '0') {  // bei f2t 
				colf2t = true;
				solang--;
				solang--;
			                    };

			if (solang < 60) solang = 61;
			if (solang > 210) solang = 210;


			fseek( fh, 0 , SEEK_SET); // sonst r�ckspulen
			char solangs[7];
			_itoa(solang, solangs, 10);
			setmee((sreplace(mySetupStr, "##", solangs)), 1);


         	
	     

	setmitzwok:

	// VpeSetDevCopies(hDoc, 2);

	if ( mitRTFchars ) {
      myRTFline = (char *) malloc( 33024 );  // 1024 zeichen �berhang f. ersetzen ca, 16 Zeilen mit 64 byte
 			           }
	else {
	   myRTFline = (char *) malloc( 128 );  // 1024 zeichen �berhang f. ersetzen ca, 16 Zeilen mit 64 byte
 	     };
 	 myRTFline[0] = '\0';

	  lasty  =  VpeGet(hDoc, VTOPMARGIN);
      glasty =  VpeGet(hDoc, VBOTTOMMARGIN);

 vorfseeme:  // neu Anfang nach Include ) mylongline  NEU

      int posback = 0;
 
	  bool bininRTF = false;
	  bool mitleerz = false;
  	  int anzparag = 0;
	  mylongline = (char *) malloc( maxread + 256 );  // ca. 1024 * 32 

fseeme:

	  int numread = fread( mylongline + posback , sizeof( char ), maxread - posback , fh );
	  numread = numread + posback;

	  mylongline[numread]   = '\0';
	  if (blockconvert) {
	  if (oemconvert) {
	   if (!oemcodepage)    OemToChar( mylongline + posback , mylongline + posback );
	  }
	  };

	  if (numread == maxread ) {
		  numread-- ;
		  numread-- ;
	      posback = 1;
	                          }
	  else {
		  mylongline[numread+1] = '\0';
		  posback = 0;
	       };


	  int lastlauf = 0;
	  bool Steucharins = false;  // nicht ganz am Anfang
	  int warSteu  = 0;

	  line = mylongline;
	  int lauf = 0;
	  for ( ; lauf < numread; lauf++ ) {
 		  if  (  mylongline[lauf] == '[')           {
			  if (Steucharins) continue;
			  if (mitdoppchar)  {
				  if  ( lauf && ( mylongline[lauf-1] == '^')) warSteu = lauf;
				  if ( lauf > ( lastlauf + 1 ) ) Steucharins = true;

			                    }
			          else {
						  warSteu = lauf;
						  if ( lauf > lastlauf ) Steucharins = true;
					  };
		                                             };
 		  if  ( ( mylongline[lauf] == '\f'))         {
	   		      mylongline[lauf] = '\0'; 
				  informff = true;
				  if ( lastlauf && (lauf != lastlauf) ) goto spring1line;  // Zeichen vorher nicht gedruckt
				  if ( line[0]  == '\0' ) line++;
				  informff = false;
				  if ( posback ) myPBreak();
					  else if ( lauf < numread) myPBreak();



		  };

		  
//		  if ((mylongline[lauf] == '\r'))         {             // Zeilen-Trenner CR/LF
//			  if ((mylongline[lauf + 1] == '\n'))   {
//				  mylongline[lauf] = '\0';


		  
		  if  ( ( mylongline[lauf] == '\n'))         {             // Zeilen-Trenner CR/LF
			  mylongline[lauf] = '\0';                             // neu auch nur LF
			  lauf--;
			  if ((mylongline[lauf] == '\r'))  {
				  mylongline[lauf] = '\0';
			  };

			  goto spring1line;
naechsteZeil:
			  Steucharins = false;
 	          warSteu  = 0;
			  if ( informff ) {
				  informff  = false;
				  wantbreak = true;
			  };
  		 	  lauf++;
			  lastlauf = lauf+1;
			  line = mylongline + lauf+1;  

		                                           
	                                           };
	  };  // for loop


	  if (posback) {
					 // zur�ckpositionieren auf lastlauf - maxread (wenn �berhaupt lastlauf)
					 if (lastlauf) posback = maxread - lastlauf;
					 memcpy( mylongline, line, posback );
//		             for ( lauf = 0; lauf < posback; lauf++ )  mylongline[lauf] =  line[lauf];
//					 if  ( fseek( fh, lastlauf - maxread  , SEEK_CUR) != 0 ) {      return -1; }
					 goto fseeme;

	              };

	  // sonst haben wir unseren letzten Block

      finlastline = 1;


spring1line:  // neue Zeile da


	//   if (oemconvert) OemToChar( line , line );

		if ( !blockconvert ) {
		if ( oemcodepage ) {
		 	if (oemconvert) {

			//	WCHAR wuline[4100];
			//	MultiByteToWideChar(oemcodepage, 0, line, 2048, wuline, sizeof(wuline) / sizeof(wuline[0]));

				int sola = strlen(line); 
//				int nSize = MultiByteToWideChar(oemcodepage, MB_ERR_INVALID_CHARS, line, -1, NULL, 0);
				int nSize = MultiByteToWideChar(oemcodepage, 0, line, -1, NULL, 0);  // auf 0 umgestellt in Vers. 87
				if (nSize != 0) { 

				WCHAR * pBuffer = new WCHAR[nSize];
				MultiByteToWideChar(oemcodepage, 0, line, -1, pBuffer, nSize);

	
				int nUtfSize = WideCharToMultiByte(CP_UTF8, 0, pBuffer, -1, NULL, 0, NULL, NULL);

				if (nUtfSize != 0)
				{

					char * pDest = new char[nUtfSize];

					WideCharToMultiByte(oemcodepageback, 0, pBuffer, -1, pDest, nUtfSize, NULL, NULL);

					int neula = strlen(pDest);

					if (sola == neula) strcpy(line, pDest);
					else {
						BOOL bUsed = false;
						Msg("CharErr");
					};

					delete pDest;

				};
				delete[] pBuffer;




   //  WideCharToMultiByte( oemcodepageback , 0, wuline, -1, line, 4096, NULL, NULL );  // statt 1250
				}

  	                       }
		} else if (oemconvert) OemToChar( line , line );
		};



       
       aktline++;

#ifdef mitlogfile
	   if (mitlog) {
		//   strncpy(mMsgpar, t2Print, 120);
	    // da Debug-Punkt auf Line setzen
		   sprintf(mMsg, "*%04i ", aktline);
		   fputs(mMsg, flog);
		   fputs(line, flog);
		   fputs("\n", flog);
	   };
#endif

	   if (f2tmode)  {

		   int solang = strlen(line);
		   if (colf2t) {

			   char myendch, myvendch;
			   solang = solang - 1;
			   if (line[1] == '\t') {   // zweite Zeichen Tab
				   if (solang < 5) {    // Kurz
					   myvendch = line[0]; // n _ t

					   //				   tt___ Einschalten Fett
					   //				   tt__ Einschalten Gross
					   //				   _t__ Ausschalten


					   //				   if (line[solang] != ' ')     goto noleerz;   // letzte Zwei Leerzeichen
					   //				   if (line[solang - 1] != ' ') goto noleerz;
					   if (myvendch == '\t')    Eincharp('B');
					   if (myvendch == ' ')     Eincharp('0');
					   if (myvendch == '\xA0')  Eincharp('F');
					   myvendch = line[2]; // n _ t
					   //if (myvendch == '\t')    lasty = lasty + (czhoch / 2);
					   if (myvendch == ' ') {
						   lasty = lasty + (czhoch / 4); 
						   myvendch = line[2]; // n _ t
						   if (myvendch == ' ')  lasty = lasty + (czhoch / 4);
					   };
					   //	   if (myvendch == '\xA0')  Eincharp('F');

					   if (finlastline) goto ibinfertig;
		 		  	   if ( zwilcount ) goto naechstezwiline;
					   goto naechsteZeil; // ZeileFertig;


			// 		   goto  leerZeile;
				   };

				   if (line[0] == '^') {
					   line[1] = '[';
					   goto standline;
				   };
			   };


		   // noleerz:  // unused label

			   if (solang > 1)  myendch = line[solang];
			   else myendch = '-';
			   if (solang > 1)  myvendch = line[solang - 1];
			   else myvendch = '-';
			   byte mycol = 0;




			   if (myendch == '\t') {  // lastchar
				   mycol = 1;
				   if (myvendch == '\t') mycol = 2;  
				   if (myvendch == '\xA0') mycol = 7;
			   }

			   else {
				   if (myendch == '\xA0') {  // lastchar
					   mycol = 4;
					   if (myvendch == '\t') mycol = 5;
					   if (myvendch == '\xA0') mycol = 8;  // 8ter gibt nicht
				   }
				   else
				   {
					   if (myvendch == '\t')   mycol = 3;
					   if (myvendch == '\xA0')    mycol = 6;
				   }
			   };

			   if (mycol) line[solang - 1] = '\0';


			   if (mycol != vormycol) {

				   //			   if (vormycol == )
				   switch (mycol)  {

				   case 0:
					   VpeSetBkgColor(hDoc, COLOR_WHITE);
					   VpeSetTextColor(hDoc, COLOR_BLACK);
//					   VpeSetPenColor(hDoc, COLOR_WHITE);
					   mitcol = false;
					   break;
				   case 1:  // grau
					   //  VpeSetColor(hDoc, RGB(210, 210, 210));
					   //VpeSetPenColor(hDoc, RGB(210, 210, 210));
					   mitcol = true;
					   VpeSetBkgColor(hDoc, RGB(210, 210, 210));
					   VpeSetTextColor(hDoc, COLOR_BLACK);
					   break;
				   case 2:  // line 2 
					   //				   VpeSetBkgColor(hDoc,  0xD3E8F5);
					   //				   VpeSetTextColor(hDoc, 0x102864);
					   mitcol = true;
					   VpeSetBkgColor(hDoc, RGB(0xD3, 0xE8, 0xF5));
					   VpeSetTextColor(hDoc, RGB(0x10, 0x28, 0x64));
					   break;
				   case 3:  // Header 
					   mitcol = true;
					   VpeSetBkgColor(hDoc, RGB(0x33, 0x4C, 0x95));
					   VpeSetTextColor(hDoc, COLOR_WHITE);
					   break;
				   case 4:  // T! Spezial 
					   mitcol = true;
					   VpeSetBkgColor(hDoc, RGB(0xFF, 0xFF, 0x00));
					   VpeSetTextColor(hDoc, RGB(0xFF, 0x40, 0x00));
					   break;

				   case 5:  // Rot 
					   mitcol = true;
					   VpeSetBkgColor(hDoc, RGB(221, 0, 0));
					   VpeSetTextColor(hDoc, RGB(255, 255, 0));
					   break;

				   case 6:  // Gr�n 
					   mitcol = true;
					   VpeSetBkgColor(hDoc, RGB(0, 144, 0));
					   VpeSetTextColor(hDoc, COLOR_WHITE);
					   break;

				   case 7:  // BLACK 
					   mitcol = true;
					   VpeSetBkgColor(hDoc, COLOR_BLACK);
					   VpeSetTextColor(hDoc, COLOR_WHITE);
					   break;

				   case 8:  // T!T  
					   VpeSetBkgColor(hDoc, COLOR_WHITE);
					   VpeSetTextColor(hDoc, COLOR_BLACK);
					   mitcol = false;
					   VpeSetPenColor(hDoc, COLOR_BLACK);

					   break;

				   };

				   vormycol = mycol;

			   };


		   };

		   lastx = VpeGet(hDoc, VLEFTMARGIN) + indirand;

		   if ( mypcontrol ) {
			   if (glasty - fussbis < lasty + czhoch) wantbreak = true; // mypcontrol ;
	                  	     };

		   globczhoch = czhoch;  // Grundlinie bleibt im ganzen Line  lasty + globczhoch

		   if (wantbreak) {
			   wantbreak = false;
			   if (!finlastline)  myPBreak();
		                  };

// !erse		   for (int i = 0; i < solang; i++) if ((line[i] == ' ') || (line[i] == '\t'))  line[i] = '\xA0';
		   
	//	   if (!colf2t) {
	//		   VpeWrite(hDoc, lastx, lasty, VFREE, lasty + globczhoch, line);
	//		   goto leerZeile;
	//	   };

		//   lasty = VpeWriteBox(hDoc, lastx, lasty, VFREE, lasty + globczhoch + (2 * globczoffs), line);
		//   VpeSetTextAlignment(hDoc,0);

		   double vlasty;
		   VpeSetPenSize(hDoc, 0);

		   vlasty = lasty;
		   lasty = lasty + globczoffs;

		   lasty = VpeWrite(hDoc, lastx, lasty, VFREE, lasty + czhoch, line);
		   if (mitcol) {
			   VpeBox(hDoc, VLEFT, vlasty, VRIGHT, vlasty + globczoffs);
			   VpeBox(hDoc, VLEFT, lasty,  VRIGHT,  lasty + globczoffs);
		   };

		   if (colf2t) lasty = lasty + globczoffs;

//		   VpeWriteBox(hDoc, lastx, lasty , VFREE, lasty + globczhoch , line);
		   if (finlastline) goto ibinfertig;

		   goto naechsteZeil; // ZeileFertig;


//		   goto leerZeile;

	   };   // Ende F2T_mode
	  



	   if ( bininRTF )     goto RTFSammle;
       if (line[2] == '=') {
            if (line[1] == '[') {
               if (line[0] == '^') {
				          line = line++; // erste ^ �berlesen
						  goto auchistgleich;
			                       };
//			                    goto standline;
		   };

	   };

	//   if (line[1] == '~') goto auchistgleich;

	   if (line[1] != '=') goto standline;

       if (line[0] == '[') {   // [=
auchistgleich:		
	if (line[2] == ')') {  // [=)  Startseqeuenz
		if ((line[3] == '&') && (line[4] == '&') && (line[5] == '&')) {
			willback = true;
		//	line = mylongline + lauf + 1;
		//	memcpy(myRTFline, mylongline + lauf + 1, 300);
			goto ibinfertig;
		};
	       setmee(line+3,0);
		   goto naechsteZeil;
	                    }  // [=)  Startseqeuenz

	double pruepos;
	int abpos;

	if ( line[2] == '<' ) {  // nur wenn kleiner gleich

		for (abpos = 3; (line[abpos] != ' ') && ( abpos < 10) ; abpos++);
		line[abpos] = '\0';
		pruepos = (atof(line + 3));
		if (lasty <= pruepos) {
		//	line = line + abpos + 1;
			ganz_ende = lauf - lastlauf;
			anfang = abpos + 1;
			goto nstandline;

		};

		goto naechsteZeil;



	};
	if (line[2] == '>') {  // nur wenn gr�sssergleich
		for (abpos = 3; (line[abpos] != ' ') && (abpos < 10); abpos++);
		line[abpos] = '\0';
		pruepos = (atof(line + 3));
		if (lasty >= pruepos) {
		//	line = line + abpos + 1;

			ganz_ende = lauf - lastlauf;
			anfang = abpos + 1;
			goto nstandline;

		};

		goto naechsteZeil;

	};



	if (line[2] == '/') {  // [=/  Kommentar
		goto naechsteZeil;
	}  // [=)  Startseqeuenz


 	nextegle:

       if ( mitdoppchar ) {
   	         pdest = strstr( line  , "|^[" );   // mehrere Bl�cke d�rfen in eine Zeile sein (aber mit more getrennt)
             if ( pdest != NULL ) {
               int atblock = (int)( pdest  -  line  ) ;
	  		   line[atblock] = '\0';
  			    Setz_ein_Teil( line );
			   strcpy(line,line + atblock + 2);  // more und ^ �berspingen
			 goto nextegle;
			          };
	                    };

	         pdest = strstr( line  , "|[" );   // mehrere Bl�cke d�rfen in eine Zeile sein (aber mit more getrennt)
             if ( pdest != NULL ) {
               int atblock = (int)( pdest  -  line  ) ;
	  		   line[atblock] = '\0';
  			    Setz_ein_Teil( line );
			   strcpy(line,line + atblock +1);  // more �berspingen
 			 goto nextegle;
			          };
	   

 		      Setz_ein_Teil( line );
		      goto naechsteZeil;

			     };  // [=   Ende

       // RTF Start
        if (line[0] != '\\') goto standline;
                        //  <=T startet das RTF Block dahinter die Zeilenbeschereibung
                        // ,... ] f�r nachkommende Texte bis ... Leerzeile bzw. /=
		      // erste Linie Merken
       if (line[2] == '?') {
		          int reszeilen = (atoi(line + 3));
				  if ( reszeilen ) {
                         if ( glasty - fussbis  <  lasty + ( reszeilen * czhoch) ) {  // hat kein Platz
							 myPBreak();
 						                                                           };
				                   };
		          goto naechsteZeil;
	                       };

	   if (line[2] == '~') { 
		            mitleerz = true;
					line[2] = '=';
	                       };
	   if (line[2] == '=') {
			   zwilcount = 1;
		       line[2] = 'T';
                  	   };
       if (line[2] != 'T') goto standline;

	       int abpos = 3;
		   zeilyres = globczhoch;
		nochpluss:
			abpos++;
			if ( line[abpos] == '+') {
				                 zeilyres = zeilyres + globczhoch;  
								 goto nochpluss;
			                         }
			if ( line[abpos] == '-') {
				zeilyres = zeilyres - globczhoch;  // da ist die Zeile beschrieben 
								 goto nochpluss;
			                         }
	//		if ( ! zwilcount ) strncpy(SteuBlock2[15],line+abpos,300);  // an z kopieren
	//		anzparag = 0;
			bininRTF = true; // mitRTFchars;
 		    goto naechsteZeil;
    RTFSammle:
	  //   if ( ! anzparag ) {
			 
               if ( zwilcount ) {

				    if (!strncmp("/==",line,3)) {  // letzte line also Pr�fen
					 	 endezezaehl:
		 				 zwilcount--;

						 double myplatz = glasty - fussbis - lasty - zeilyres;

//                        if ( glasty - fussbis  <  lasty + ( zwilcount * czhoch) + zeilyres ) {  // hat kein Platz
//                        if ( glasty - fussbis - lasty - zeilyres  <   ( zwilcount * czhoch) ) {  // hat kein Platz

						 int springbis = -1;
						 int platzbis  = -1;
                         if ( myplatz  < ( 1.5 * zwilcount * czhoch ) ) {     // hat kein Platz zuerst GROBe Pr�fung (alle Zeilen als voll gez�hlt)

							 double plabedarf = 0;
							 int iilauf;
							 for ( iilauf = 0; iilauf < zwilcount; iilauf++ ) {
										int solang = strlen(zwilines[iilauf]); 
										if (solang) {
											solang--;
										//	solang--;
											for ( int i2lauf = 0; i2lauf < solang; i2lauf++ ) {
												if (zwilines[iilauf][i2lauf] == '[' ) {
													if (zwilines[iilauf][i2lauf+1] == 'l' ) {
			  switch ( zwilines[iilauf][i2lauf+2] ) {
			  case '-' :   plabedarf = plabedarf - czhoch + ( czhoch / 2 );   break;  // halbe Linefeed
			  case '+' :   plabedarf = plabedarf + czhoch + ( czhoch / 2 );   break;  // ein 1/2 Linefeed
			  case '0' :   plabedarf = plabedarf - czhoch;                    break;  // kein  Linefeed
			  case 'v' :   plabedarf = plabedarf - czhoch + ( czhoch / 4 );   break;  // viertel 
			  case 'a' :   plabedarf = plabedarf - czhoch + ( czhoch / 8 );   break;  // achtel
			  case 'z' :   plabedarf = plabedarf - czhoch + ( czhoch / 10 );  break;
//			  case '?' :
// 			  case '!' : in default
			  case 'L' :  // trippell
			          plabedarf = plabedarf +  czhoch ;
			  case 'l' :  // doppel l
			          plabedarf = plabedarf +  czhoch ;
			  default:
			          plabedarf = plabedarf +  czhoch ;
			                           };  // end case l

										          }  // zweite Zeichen l
												};    // fangt mit [
											};  // String durchlaufen

										};   // kein Leerstring
                    plabedarf = plabedarf +  czhoch ;  // Standard Zeilenvorschub
			        if ( myplatz  < plabedarf )  {
						  springbis = iilauf - 1;
						  break;
					};
                                            };  // Bedarf ermitteln Zeilen Loop
							
 			 	  if ( springbis < 2 ) springbis = -1;

				  if ( myplatz  > plabedarf )  goto ichhabedochpl;




							 if ( mitleerz ) {                              // vielleicht irgendwo eine Leerzeile
							        for (    ; springbis > 1; springbis-- ) {
										if (!(strlen(zwilines[springbis])))  break;
												                           };
									    if ( springbis < 2 ) springbis = -1;
										     }
							 else springbis = -1;

				 if ( springbis < 2 )   myPBreak();
						              };							 
						        



ichhabedochpl:
						 for ( int ivar = 0; ivar < zwilcount; ivar++ ) {
										            if ( ivar == springbis)  myPBreak();
													else { 
					   				                    line = zwilines[ivar];
													    goto druckmispool;
						  naechstezwiline: ;
													};
 										 
								                                        };
						 zwilcount = 0;
						 mitleerz = false;
                         bininRTF = false;
             			 anzparag = 0;
             	  	 	 zeilyres = 0;
						 strcpy(line,"");
			             goto naechsteZeil;

							                  };

	                             
		        		                                                  
				    strcpy(zwilines[zwilcount-1],line);      
					zwilcount++;
					if (zwilcount < 33 ) goto naechsteZeil;  
					goto endezezaehl;
 	  		                    };
			                  
								/*

  		    if ( line[1] != '=' )  {   // RTF zweite Line
						   sprintf(line, "[C RED]VPE (no=) %s",line);
						   Print_ein_Teil( line + anfang, lauf - anfang );
                           bininRTF = false;
						   goto naechsteZeil;
	                                };

			strncpy(SteuBlock2[14],line+2,300);  // da ist die RTF druck beschrieben (ohne OEM konvertierung) // y
			char rtfmod = line[0];        
			strcpy(myRTFline,SteuBlock[0] + 64);  // RTF-Initstring kopieren
            anzparag++;
 		    goto naechsteZeil;
                                      };

	 if (! strncmp(line , "/=",2)) {       // Fertig

             bininRTF = false;
 			 anzparag = 0;
			 if ( glasty - fussbis < lasty + czhoch) wantbreak = true;
			 PrinteLine(SteuBlock2[14]);
  	  	 	 zeilyres = 0;
			 goto naechsteZeil;
      		                            };

	  	  anzparag++;
		  if ( anzparag > 50 ) {
			  // Fehlermeldung
             bininRTF = false;
		     goto naechsteZeil;

		  };

		  int bissl = (strlen(line));
		  int lastline  = 0;

	  for ( int llauf = 0; llauf < bissl; llauf++ ) {
			  if ( line[llauf] == '[' ) {

				   line[llauf] = '\0';
  				   strcat(myRTFline,line + lastline);
				   lastline = llauf + 1;
			       if ( line[llauf] == '[' ) goto nextllauf;

   				   char *pdest;                
		           pdest = (char *) strchr(  steustri , line[lastline] );   // RTF-Steuerungscode

				   if ( pdest != NULL ) {
 		                int inblock = (int)(pdest - steustri);
			            if  (inblock < 13) strcat(myRTFline,SteuBlock[inblock] + 64);        // bis 
						else strcat(myRTFline,SteuVar[inblock-13]);                            

			                            }
				   else {
					   switch (line[lastline])
					   {
					   case 't' : strcat(myRTFline,"\\tab "); break;
					   case 'l' : strcat(myRTFline,"\\par "); break;
					   case 'p' : strcat(myRTFline,"\\bullet "); break;
					   case 'C' : VpeClearAllTabs(hDoc); break;

				        };
				        };



  	               llauf++; 
		           lastline = llauf + 1;
                      			  
  nextllauf:
		 			   llauf++; 

                       			  };
		  }; // for 


  
	      strcat(myRTFline,line + lastline);
	      strcat(myRTFline,"\\par ");
  	      goto naechsteZeil;  */

druckmispool:
	  lastx  =  VpeGet(hDoc, VLEFTMARGIN) + indirand;
      anfang = 0;
      globczhoch = czhoch;  // Grundlinie bleibt im ganzen Line  lasty + globczhoch
      ganz_ende = (strlen(line));
	  if ( ! ganz_ende  ) goto leerZeile;
	  if ( line[0] == '^' ) if ( line[1] == '[' )  anfang++;
	  goto standline2;

standline:   // unsere Standard-Line


	  lastx  =  VpeGet(hDoc, VLEFTMARGIN) + indirand;
      anfang = 0;

	  if (mypcontrol ) {
		  if (line[0] == '[') {
			  if (line[1] == '?') goto nopcheck;
			  if (line[1] == 'x') goto nopcheck;
		  };
		  if (glasty - fussbis < lasty + czhoch) wantbreak = true; // mypcontrol ;

#ifdef mitlogfile
		  if (mitlog) {
			  sprintf(mMsg, "+ Page? glasty:%#5.2f fussbis:%#5.2f lasty:%#5.2f czhoch:%#5.2f  also noch: %#5.2f \n", glasty, fussbis, lasty, czhoch,  (glasty - fussbis) - (lasty + czhoch));
			  fputs(mMsg, flog);
		  };
#endif
	  nopcheck: ;
	  };
       globczhoch = czhoch;  // Grundlinie bleibt im ganzen Line  lasty + globczhoch

				if ( wantbreak ) {
			  		   wantbreak = false;
                       if ( !finlastline ) {
	                           myPBreak();
			                               };
		                  };


	    ganz_ende = lauf - lastlauf;
		anfang = 0;

		if ( ! ganz_ende  ) goto leerZeile;
		if (line[0] == '^') if (line[1] == '[') {
			anfang++;
			if (line[2] == ']') {
				anfang++;             // Direktdruck ab v.87 
				                      // bei ^[][ BO ] umstelellen auf ^[[[ BO ] ( sonst [ start embedded)
				if (line[3] == '[') (line[2] = '[');
				          	else anfang++;
				Print_ganz_Teil(line + anfang, ganz_ende - anfang);
						
				goto leerZeile;
			};
		};
		if ( ! Steucharins)   goto druckZeile;

//	    ganz_ende = strlen(line);
 //      int zegrenze = ganz_ende;
// 		zegrenze--;
nstandline:
		int lilauf =  warSteu - lastlauf ;
		if ( ! warSteu ) lilauf = ganz_ende;

 standline2:
		// wir suchen den zweiten [ !!
		if ( zwilcount ) lilauf = 2;
	    if (!mitdoppchar) goto singlecloop;  
		                      
//		lilauf = 2;

 while ( lilauf < ganz_ende ) { 
      
	if ( line[lilauf] == '[' ) {   // Steuerzeichen fangt an

	    if ( line[lilauf-1] == '^' ) { 
		line[lilauf-1] = '\0' ;
		Print_ein_Teil( line + anfang,lilauf - anfang );
		anfang = lilauf  ;
 		                             };  
	};
	 lilauf++;

 }; // while
	goto druckZeile;

singlecloop:

 while ( lilauf < ganz_ende ) { 
      
	if ( line[lilauf] == '[' ) {   // Steuerzeichen fangt an

		if ( line[lilauf-1] == '[' ) {
			                      lilauf++;
			                      continue;   // doppel [[ �berspringen
		};
		line[lilauf] = '\0' ;    // Block VORHER schliessen
		Print_ein_Teil( line + anfang, lilauf - anfang );
		line[lilauf] = '['  ;    // Block VORHER schliessen
        anfang = lilauf   ;
 		                       };  
	                         
	 lilauf++;

 }; // while

 
druckZeile:
	
	 	 Print_ein_Teil( line + anfang, ganz_ende - anfang );

leerZeile: 
    if ( finlastline ) goto ibinfertig;

 		if ( myhoch != 99 ) {
//			lasty = myhoch + czhoch;
			if (myhoch > 99) {
				// include wechsel
				fclose(fh);

				if ((fh = fopen(mMsgpar, "rbS")) == NULL)
				{
					sprintf(mMsg, "Fehler bei �ffnen!\nInclude-Dateiname: %s!", mMsgpar);
					MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
					exit(7);
					//return false;
				};

				free(mylongline);

		// 		maxread = ftell(fh);

				fseek(fh, 0, SEEK_END);
				maxread = ftell(fh);
				if (maxread > 64000) maxread = 48000;
				fseek(fh, 0, SEEK_END);
				fseek(fh, 0, SEEK_SET);   // r�ckspulen

				wantbreak = false;
				informff = false;
				finlastline = 0;
				aktline = 1;
				myhoch = 99;

				goto vorfseeme;

				
			};
			lasty = myhoch ;
			myhoch = 99;
		                   }
		else  lasty = lasty + czhoch;
		

		if ( zwilcount ) goto naechstezwiline;
        goto naechsteZeil; // ZeileFertig;


ibinfertig:


   	 if (mypcontrol) {
		 int amzp = VpeGetPageCount(hDoc); 
	  	 if ( amzp == 1 )   Eincharp('E');
                 	 };


   if  ( !abruchwar ) {
   	   // Block 9 wenn definiert = Gesamtseiten
	   // achtung '9' ist zu SteuBlock[16] zugeordnet ( 17 aber mit 0 start)



	   if ( obanzonpage < 1 ) {                  // kein Text auf letzte Seite
 			int amzp = VpeGetPageCount(hDoc);   
			if ( ( amzp > 1 ) && ( !f2tmode) ) {                    // wenn mehrseitig, letzte Seite l�schen 
				VpeGotoPage(hDoc, amzp);
				VpeRemovePage(hDoc);

			};
	   };


	   if (killfirstpage ) {                  // kein Text auf letzte Seite
		   int amzp = VpeGetPageCount(hDoc);
		   if (amzp > 1) {                    // wenn mehrseitig, letzte Seite l�schen 
			   VpeGotoPage(hDoc, 0);
			   VpeRemovePage(hDoc);

		   };
	   };


	   int zlauf = 1;

  	    if ( strlen(SteuBlock2[8]) > 7 ) {   // erste Seite wenn 8-er definiert
		    char tline[512]; 
			char lint[512];
			int amzp = VpeGetPageCount(hDoc);
  	  	    char buffer[65];
			strcpy(tline, SteuBlock2[8]);
			VpeGotoPage(hDoc, zlauf-1);
	        strcpy( lint, tline );
			_itoa_s( zlauf, buffer, 7, 10 );
			strcpy(lint,(sreplace(lint,"%a",buffer)));
			_itoa_s( amzp, buffer, 7, 10 );
			strcpy(SteuBlock2[8],(sreplace(lint,"%g",buffer)));
			Eincharp('8');
	 		zlauf++;  
		};

	   
	   if ( strlen(SteuBlock2[9]) > 7 ) {
		   char tline[512];
		   char lint[512];
		   int amzp = VpeGetPageCount(hDoc);
  	  	    char buffer[65];

            strcpy( tline, SteuBlock2[9] );

  	        for ( ; zlauf <= amzp; zlauf++) {
	       
			VpeGotoPage(hDoc, zlauf);

			strcpy( lint, tline );
			_itoa_s( zlauf, buffer, 7, 10 );
			strcpy(lint,(sreplace(lint,"%a",buffer)));
			_itoa_s( amzp, buffer, 7, 10 );
			strcpy(SteuBlock2[9],(sreplace(lint,"%g",buffer)));
			Eincharp('9');
	 		};  
		};
		
	   if (willback) {
		   obanzonpage = 0;
		   willback = false; 
		   lasty = VpeGet(hDoc, VTOPMARGIN);
		   for (anfang = 0; anfang < 12; anfang++) {
			   SteuBlock[anfang][0] = '\0';
			   SteuBlock[anfang][64] = '\0';
			   SteuBlock2[anfang][0] = '\0';
		   };
		   for (; anfang < 15; anfang++) {
			   SteuBlock2[anfang][0] = '\0';
		   };
		   
		   //		   memcpy(mylongline + lauf + 1, myRTFline, 300);
		   myPBreak();

		   goto naechsteZeil;
	   };




    fclose(fh);

	   
	free(mylongline);


// #ifdef mitlogfile

	   if   ( wartime )  { 
			QueryPerformanceCounter((LARGE_INTEGER *)&now);
			QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

			sprintf(mMsg, "['Arial' S 8 B C RED PAO]Elapsed Time  -FLEX: %.3f", ((double)(time9 - time0) / (double)freq));
			if (time1) {
				sprintf(mMsgpar, " -T1: %.3f", ((double)(time9 - time1) / (double)freq));
				strcat(mMsg, mMsgpar);
			};

			if (time9) {
				double stime = ((double)(start0 - time9 ) / (double)freq);
				if (stime < 5) {
				sprintf(mMsgpar, " -start exe: %.3f", stime);
				strcat(mMsg, mMsgpar);
				};
				stime = ((double)(start0 - start) / (double)freq);
				if (stime < 5) {
					sprintf(mMsgpar, " ( + ini: %.3f )", stime);
					strcat(mMsg, mMsgpar);
				};
			};


			sprintf(mMsgpar, " -F2P: %.3f", ((double)(now - start) / (double)freq));
			strcat(mMsg, mMsgpar);


			if (time9) {
					double stime = ((double)(start0 - time9) / (double)freq);
				if (stime < 5) {
					sprintf(mMsgpar, " GESAMT: %.3f sec.", ((double)(now - time0) / (double)freq));
					strcat(mMsg, mMsgpar);
				}
				else            {
					sprintf(mMsgpar, " GESAMT: %.3f sec.", ((double)(time9 - time0 + now - start0 ) / (double)freq));
					strcat(mMsg, mMsgpar);

				};
						};

	       VpeGotoPage(hDoc, 1);
 	 	   VpePrint(hDoc, VpeGet(hDoc, VLEFTMARGIN), VpeGet(hDoc, VTOPMARGIN), mMsg);
		   
	   };

	   free( myRTFline );

	

	   if (online < 1) {

	   if ( mitvorschau )  {
     if ( !warprev )  {
       mypaintmode = 1;
 	   VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);
       SetFocus(VpeGetWindowHandle(hDoc));
	   warprev = 1;
 	 }
	if  ( VpeDispatchAllMessages(hDoc) ) abruchwar = true;

   };
	   };

   };


  if ( mitvorschau )  {
 // einige WM_Msg umlenken
  HWND myhandle;
  myhandle = GetWindow( VpeGetWindowHandle (hDoc),GW_CHILD);  // Achtung! das ist erste Child von das Parent-Window
 //  g_OldVPE = (WNDPROC)SetWindowLong(myhandle, GWL_WNDPROC, (LONG)NewVPEProc);
 //   g_OldVPE = (WNDPROC)SetWindowLongPtr(myhandle, GWLP_WNDPROC, (LONG_PTR)NewVPEProc);
   g_OldVPE = reinterpret_cast<WNDPROC>(SetWindowLongPtr(myhandle, GWLP_WNDPROC, (LONG_PTR)NewVPEProc));

  };

#ifdef mitlogfile

   if (mitlog) {
   	     fputs( mMsg, flog );
  	     fclose(flog);
   };
#endif


         if  ( ! abruchwar )  {
		 if (nosave)    will_save = 0;
      if (will_save) {
		  ExportDocument("!",0);
	  };
		

	  int endlaufi;
	  if (nosave) icopar = -1;
	  if (icopar < 0) goto ohneendprog;





	  for (endlaufi = 0; endlaufi < 4;  endlaufi++) {

/*		  int  icopar = 0;
		  char mycopar[4];
		  char mycoml[4][128];  */

		  switch (mycopar[endlaufi])
		  {
		  case 'p': 
			  
			  VpePrintDoc(hDoc, 0);       break;
		  case 'P':     
			  VpePrintDoc(hDoc, 1);       break;
		  case 'E':
			  sprintf(mMsg, "Export Doc...\n%s\n", mycoml[endlaufi]);
			  if (MessageBox(hMainWindow, mMsg, szAppName, MB_YESNO) != IDYES) {
				  break;
			  };

		  case 'e':
			  if (!(VpeWriteDoc(hDoc, mycoml[endlaufi]))) rueckmcode = 0;

			  break;
		  case 'r':
			  myRunP(mycoml[endlaufi], 0);       break;  // mit warten

		  case 'R':
			  myRunP(mycoml[endlaufi], 1);       break;  // ohne warten

		  };


	  };

  ohneendprog:;

		 };



		return !abruchwar;

}




void Paint(HWND hwnd)
{

   PAINTSTRUCT ps;
   HDC hdc;
   RECT rectClient;

   hdc = BeginPaint(hwnd, &ps);
   GetClientRect(hwnd, &rectClient);
   FillRect(hdc, &rectClient,(HBRUSH) (COLOR_WINDOW+1));
   EndPaint(hwnd, &ps);

}

void startmyed(char *filename)
{
	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    char lstring[255];

	strcpy(lstring,"tx.exe ");
	if (oemconvert)	strcpy(lstring,"ty.exe ");
	strcat(lstring, "/s diff " );
	strcat(lstring,filename);

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );


	if( !CreateProcess( NULL, // No module name (use command line). 
		lstring,
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        false,            // Set handle inheritance to false. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        NULL,             // Use parent's starting directory. 
        &si,              // Pointer to STARTUPINFO structure.
        &pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
    {
  	 
      Msg("kann Editor nicht starten");
      
    }
	else ( WaitForInputIdle (pi.hProcess,5000) ) ;

};

void startmyed2(char *filename)
{
    char lstring[255];
	strcpy(lstring,filename);
	strcat(lstring," -s -f");
	strcat(lstring,curexedir);
	strcat(lstring,"f2p.exe");
    startmyed(lstring);
    
};


void fileback( int ipari)
{
   if ( usetdir[0] != '\0') {

 	
	  FILE *mlog;

	  if (( mlog = fopen(usetdir, "wt")) != NULL)   {
  
   sprintf(mMsg, "Mail: %s\n%i\n%i\n", fname, ipari , (VpeGetLastError(hDoc)));
   fputs( mMsg, mlog );
   fclose(mlog);
	
                                                   }
	  
	 }
   ;
};



int sumailf()
{
    FILE *fh;

		   if ( uminfo[0] == '\0') {

              _makepath( uminfo ,  drive, mdir, ext + 1, "txt" );
			  if ((fh=fopen(uminfo, "rt")) != NULL) goto fhopenda;
				  strcpy(uminfo,curexedir);
				  strcat(uminfo,ext + 1);
				  strcat(uminfo,".txt");
				  if ((fh=fopen(uminfo, "rt")) != NULL) goto fhopenda;
				  strcpy(uminfo,curexedir);
				  strcat(uminfo,"smail.txt");
				  if ((fh=fopen(uminfo, "rt")) != NULL) goto fhopenda;

      _makepath( uminfo ,  drive, mdir, ext + 1, "txt" );
	  sprintf(mMsg, "Aut.Mail Steuerungsdatei fehlt:\n<%s>",uminfo);
      MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
      return false;
				  
			                                        }


   if ((fh=fopen(uminfo, "rt")) == NULL)
   {
	  sprintf(mMsg, "Mail Include Datei fehlt:\n<%s>",uminfo);
      MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
      return false;
   }


   fhopenda:
    fclose(fh);
    return true;

};


int filemail()
{
    FILE *fh;
    char tline[512]; 
    char line[1024];

	 // Steuerung durch die Datei uminfo
	foemcodepage = 0;
	foemcodepageback = 0;

    char mailt[10120];
	int appmailt = 0; 
	DWORD attsize  = 0;



    WCHAR wuline[2048]; 


/*	 appmailt = VpeGetMAPIType(hDoc);
	if ( appmailt != 1 )   MessageBox(hMainWindow, "Extended MAPI ist nicht installiert", "Leider", MB_OK);


    VpeClearMailAttachments(hDoc)  ;
    VpeAddMailReceiver(hDoc,"f.nagy@aon.at",1 );
	VpeSetMailText(hDoc, "<html><body><font size=2 color=red face=Arial><span style='font-size:10.0pt;font-family:Arial;color:red'>HTML Mail Body</font></body></html>");

	if ( aushome ) {

	VpeSetMailSubject(hDoc,"VPE Test...  (Dialog OFF)");
    VpeSetMailWithDialog(hDoc, false);
	}
	else {
	VpeSetMailSubject(hDoc,"VPE Test...  (Dialog ON)");
    VpeSetMailWithDialog(hDoc, true);
	};

    return false; */


//	appmailt = 0;

   if (! sumailf() ) return 0;
   if ((fh=fopen(uminfo, "rt")) == NULL)
   {
	  sprintf(tline, "Mail Include Datei fehlt:\n<%s>",uminfo);
      MessageBox(hMainWindow, tline, "Leider", MB_OK);
      return false;
   }


	 bool dochblat = false;

     FILE *fmail;
     FILE *fbody;
	 
	 char xcomline[512] = " ";


	 char tempfname[199];
	 char bodyfname[199];
	 

	while ( fgets( line, 1024, fh ) != NULL) {

		int tende = strlen(line) - 1;
		for (; tende >= 0 && line[tende] <= 32 && line[tende] > 0; tende--);  // strip LF, CR, Blank
		line[tende + 1] = '\0';

		if (foemcodepage)     {
			if (foemcodepageback) {

				if (foemcodepageback != foemcodepage) {
					MultiByteToWideChar(foemcodepage, 0, line, 1024, wuline, sizeof(wuline) / sizeof(wuline[0]));
					WideCharToMultiByte(foemcodepageback, 0, wuline, -1, line, 2048, NULL, NULL);  // statt 1250
				}

			}
			else 		  OemToChar(line, line);
		};

			 
			
	if ( line[0] == '-' ) {
       switch  ( lower( line[1] ) )

      {
	    case 'x' : // execprogr, umstellen auf BLAT oder was anderes
			       // 
           dochblat = true;
		   if ((line[2]) == '!') strncpy(xcomline, line + 3,512);
  		   wsprintf(tempfname, _T("%s\\of~%s.tmp"), GetTmpDir(), myindis);
           fmail = fopen(tempfname, "wt");
		   break;

	    case 's' : // Subject 
          VpeSetMailSubject(hDoc, line + 3 );
		  break;
         case 'p' : // Protokoll
            strncpy( usetdir , line + 3,98);
		  break;
         case 't' : // Receiver
		  strncpy( fname , line + 3,98);
 		  VpeAddMailReceiver(hDoc,line + 3,1 );
		  break;
         case 'c' : // Copy
  		  VpeAddMailReceiver(hDoc, line + 3,2 );
		  break;
         case 'b' : // Receiver blind copy
  		  VpeAddMailReceiver(hDoc, line + 3,3 );
		  break;  
         case 'a' : // Adresse 
  		  VpeSetMailSender(hDoc, line + 3 );
		  break;
		 case ':' : // Parameter durchschleifen 
 		  if ( dochblat) {
			  fputs( line + 3 , fmail );
			  fputs( "\r\n" , fmail );
                     	  };
		  break;

		 case '?': 

			 fputs(line + 3, fmail);
			 sprintf(line, "oemconvert oemcp: %i oemcpback: %i", foemcodepage, foemcodepageback);
			 fputs(line , fmail);
			 fputs("\r\n", fmail);
			 goto nosammle;
			 break;

		 case '=' : // oem charset zu �ndern ? 
		  PrintSetGleich(line + 3 );
          goto nosammle;
		  break;
					   
         case 'z' : // Attachment
  		   VpeAddMailAttachment(hDoc, line + 3, NULL );
		  break;
 
		  /*
		  es ist nur bei die Attachment eine Unterschied ob mit oder ohne Blat
		  die Blat Befehle m�ssen mit : durchgeschliefen werden
		  (-::-to        "f.nagy@aon.at"  ab -:: kommt alles in of~ )

		  bei standard Mail
		  */

         case 'w' : // Attachment



		  VpeWriteDoc(hDoc, line + 3);
		  if  ( line[2] == '=' ) {
			  VpeClearMailAttachments(hDoc);
    		  VpeAddMailAttachment(hDoc, line + 3, NULL );

/*			  HANDLE hFile;
			  hFile = CreateFileA (line + 3,
				  GENERIC_READ ,
				  0,
				  NULL,
				  CREATE_ALWAYS,
				  FILE_ATTRIBUTE_NORMAL,
				  NULL);
			
			  if (hFile != INVALID_HANDLE_VALUE)
			  {
				  attsize = GetFileSize(hFile, NULL);
				  CloseHandle(hFile);

			  };  */

			  FILE* fp = fopen(line + 3,"r");
			  if (fp) {
				  fseek(fp, 0, SEEK_END);
				  attsize += ftell(fp);
				  fclose(fp);
			  }


		//	  sprintf(mMsg, "Mail Attachment: %s\n\n Size: %i Byte", line + 3, attsize);

		//	  MessageBox(hMainWindow, mMsg, szAppName, MB_YESNO);

		
		                         };
		  break;

		 case 'n' : // ClearAttachment
			  VpeClearMailAttachments(hDoc);
		  break;


         case 'm' : // mailen 

	   if (appmailt == 2 )  {   // File noch nicht closed

 		  if ( dochblat) {
			  fputs("-bodyF " , fmail);
			  fputs(bodyfname , fmail);
			  fputs("\r\n"    , fmail);
        		         }  
		  else  VpeSetMailText(hDoc, mailt);
			   appmailt = 0;
			                };

	  if ( dochblat) {
			fclose( fmail );
			fclose( fbody );

        if (line[3] == '1')	 {
	 		sprintf(mMsg, "Mail to: %s\nsend with (%s).\n\nAttachment: %i kB",fname,"blat.exe",int(attsize/1000));
			if ( MessageBox(hMainWindow, mMsg, szAppName, MB_YESNO) !=  IDYES) {

				return 0; 
			};

			if (attsize > 0) {
				if (attsize < 16000) {

					sprintf(mMsg, "Achtung, Attention!!!\n\nAttachment unter 16 kB! (%i kB)", int(attsize / 1000));
					if (MessageBox(hMainWindow, mMsg, szAppName, MB_YESNO) != IDYES) {

						return 0;


					}
				}
			}


			if (GetAsyncKeyState(VK_CONTROL) < 0) {
				strcpy(line, "blat.exe -of ");
				if ((strlen(xcomline)) > 7) {
					strcpy(line, xcomline);
					strcat(line, " ");
				};
				strcat(line, tempfname);
				sprintf(mMsg, "cmdline:\n\n %s", line);
				MessageBox(hMainWindow, mMsg, szAppName, MB_OK);
				if (MessageBox(hMainWindow, mMsg, szAppName, MB_YESNO) != IDYES) return 0;
		     	myRunP(line, 0);
			    MessageBox(hMainWindow, "Fertig", szAppName, MB_OK);
				  return 0;
    			};

		                };

			strcpy(line, "blat.exe -of ");
			if ((strlen(xcomline)) > 7) {
				strcpy(line, xcomline);
				strcat(line, " ");
			};
			strcat(line, tempfname);

			myRunP(line,0);
			return 0;

		  }  else {

        if (line[3] == '0')	 VpeSetMailWithDialog(hDoc, false);
		                else VpeSetMailWithDialog(hDoc, true);
	  }
		  break;

         case '+' : // Text - Start 
		  appmailt = 1;
		  break;
					   

         case '-' : // Text-Ende
	     if (appmailt == 2 ) {

 		  if ( dochblat) {
			  fputs("-bodyF ", fmail);
			  fputs(bodyfname, fmail);
			  fputs("\r\n", fmail);
		  }  else  VpeSetMailText(hDoc, mailt);

		  appmailt = 0;
 		                  };

		  break;

					};  // and switch
	}; // and Zeile startet mit -
   
			 if (appmailt == 2 ) {

	  if ( dochblat)  {
			  fputs( line  , fbody );
			  fputs( "\r\n" , fbody );
	                  }
	             else {
		  strcat(mailt,line);
		  strcat(mailt,"\r\n");
	                  };
			 };

			 if (appmailt == 1 ) {
				  appmailt = 2;

	  if ( dochblat) {
  				 	wsprintf(bodyfname, _T("%s\\ob~%s.tmp"), GetTmpDir(), myindis);
                    fbody = fopen(bodyfname, "wt");
	                  }
	  else 	  mailt[0] = '\0';
			 };

   nosammle: ;
	               };

  fclose(fh);
  if ( dochblat)  fclose( fmail );


  return 1;

}


#include "windproc.h"


void MyCmlHelp()
{

   hDoc = VpeOpenDoc(hMainWindow, "F2P Hilfe", mytbmode);

   Myreg();

   VpeSetBkgMode(hDoc, VBKG_SOLID);
   VpeSetBkgColor(hDoc, COLOR_WHITE);
   
   VpeSetPenSize(hDoc, 0.03);
   VpePrint(hDoc, 1.20, 0.5, "['Arial' S 12 C Blue]F2P (Flex to PDF) text to Print/PDF konverter...");
   VpeSetAutoBreak(hDoc,  AUTO_BREAK_ON );
   
   VpeSetBkgColor(hDoc, COLOR_LTGRAY);
   VpeWriteBoxRTFFile(hDoc, 2.00 , 2.00, 19.00, VFREE, "info\\syntax.rtf"); 

   VpeSetBkgMode(hDoc, VBKG_SOLID);
   VpeSetBkgColor(hDoc, COLOR_WHITE);
 //  VpeSetObjectID(hDoc, 2);

 //  VpeCreateUDO(hDoc, 3.00, 3.00, -3.00, -3.00, UDO_OBJECT_ID);

   VpeWriteBox(hDoc, 10.00, 10.00, VFREE, VFREE, "[N 'Arial' S 8 SH C RED]Programm mixed by f.nagy v. 2026a\nPrint Engine Copyright Ideal-Software");
   VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);
 

};

  #define MONITOR_CENTER   0x0001        // center rect to monitor 
  #define MONITOR_CLIP     0x0000        // clip rect to monitor 
  #define MONITOR_WORKAREA 0x0002        // use monitor work area 
  #define MONITOR_AREA     0x0000        // use monitor entire area 


void ClipOrCenterRectToMonitor(LPRECT prc, UINT flags)
  {
      HMONITOR hMonitor;
      MONITORINFO mi;
      RECT        rc;
      int         w = prc->right  - prc->left;
      int         h = prc->bottom - prc->top;
  
      // 
      // get the nearest monitor to the passed rect. 
      // 
      hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);
  
      // 
      // get the work area or entire monitor rect. 
      // 
      mi.cbSize = sizeof(mi);
      GetMonitorInfo(hMonitor, &mi);
  
      if (flags & MONITOR_WORKAREA) {
          rc =  mi.rcWork;
          prc->left   = rc.left ;
          prc->top    = rc.top  ;
		  int zwi =  ( rc.right - rc.left );
		   zwi = 0.8 * zwi;
		   if ( zwi > 1080 ) zwi = 1080;

          prc->right  = rc.right;
		  prc->left   = rc.right - zwi ;

        //  prc->right  = rc.left  + zwi ;
          prc->bottom = rc.bottom;

		  return;
	  }
      else
          rc = mi.rcMonitor;
  
      // 
      // center or clip the passed rect to the monitor rect 
      // 
      if (flags & MONITOR_CENTER)
      {
          prc->left   = rc.left + (rc.right  - rc.left - w) / 2;
          prc->top    = rc.top  + (rc.bottom - rc.top  - h) / 2;
          prc->right  = prc->left + w;
          prc->bottom = prc->top  + h;
      }
      else
      {
          prc->left   = max(rc.left, min(rc.right-w,  prc->left));
          prc->top    = max(rc.top,  min(rc.bottom-h, prc->top));
          prc->right  = prc->left + w;
          prc->bottom = prc->top  + h;
      }
  }

void wCalcRect(char *whand)
{
	HWND hparwindowt = (HWND)(intptr_t)_atoi64(whand + 1);

	if (IsWindow(hparwindowt)) {

		RECT  rcOwner;
		hparwindow = hparwindowt;
		
		GetWindowRect(hparwindow, &rcOwner);
		//	 ClipOrCenterRectToMonitor(&rcOwner, 2);

		xoben = rcOwner.left;
		xbreit = (rcOwner.right - rcOwner.left);
		yoben = rcOwner.top;
		yhoch = (rcOwner.bottom - rcOwner.top);

	}

}



void CalcRect(char meinch)
  {
      HMONITOR hMonitor;
      MONITORINFO mi;
	  RECT rc;
      LPRECT        prc;
      prc = &rc;
 
   switch (meinch) {
   case 'L':
  	  prc->left    = GetSystemMetrics(SM_XVIRTUALSCREEN) + 1 ;
      prc->right   = prc->left + 1;
  	  prc->bottom  = 200;
      prc->top     = 100;
	  break;

    case 'R':
	  prc->left    = GetSystemMetrics(SM_CXVIRTUALSCREEN) - 2 ;
      prc->right   = prc->left + 1;
  	  prc->bottom  = 1;
      prc->top     = 0;
	  break;
	default:

  	  prc->left    = 1;
      prc->right   = 1;
  	  prc->bottom  = 1;
      prc->top     = 0;


     };



   if ( meinch == 'r' ) {

  	  prc->left    = 1;
      prc->right   = 1;
  	  prc->bottom  = 1;
      prc->top     = 0;

                       };

	 hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);
	 mi.cbSize = sizeof(mi);
     GetMonitorInfo(hMonitor, &mi);

	  
     prc = &mi.rcWork;

		           xbreit =  prc->right - prc->left - 2;
                   yoben  =  prc->top ;	   yhoch  = ( prc->bottom - prc->top )  ; 

	  if ( (lower(meinch)) == 'r' ) {
				   if ( meinch == 'r' ) xbreit = xbreit / 1.8 ;
				   xoben  = ( prc->right - xbreit ) ;   

				   return;
  
			};

	  if ( (lower(meinch)) == 'l' ) { 
				   if ( meinch == 'l' ) xbreit = xbreit / 1.8 ;
				   xoben  =  prc->left;     
				   return;
  
			};

	   xoben  =  prc->left;     
			
  }


#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

#define NT_OK(Status)          (((NTSTATUS)(Status)) >= 0)
#define STATUS_UNSUCCESSFUL    ((NTSTATUS)0xC0000001L)

int hash_SHA256(const BYTE* pSrc, int nLenSrc, CHAR* pDst)
{
	NTSTATUS                status = STATUS_UNSUCCESSFUL;


	BCRYPT_ALG_HANDLE       hAlg = NULL;
	BCRYPT_HASH_HANDLE      hHash = NULL;
	DWORD                   cbData = 0, cbHash = 0, cbHashObject = 0;
	PBYTE                   pbHashObject = NULL;
	PBYTE                   pbHash = NULL;
	int back = 0;

	//open an algorithm handle
	if (!NT_OK(status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0)))	{
		printf("**** Error 0x%x returned by BCryptOpenAlgorithmProvider\n", status);
		goto Cleanup;
	}

	//calculate the size of the buffer to hold the hash object
	if (!NT_OK(status = BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, (PBYTE)&cbHashObject, sizeof(DWORD), &cbData, 0)))	{
		printf("**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	//allocate the hash object on the heap
	pbHashObject = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHashObject);
	if (NULL == pbHashObject)	{
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	//calculate the length of the hash
	if (!NT_OK(status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&cbHash, sizeof(DWORD), &cbData, 0)))	{
		printf("**** Error 0x%x returned by BCryptGetProperty\n", status);
		goto Cleanup;
	}

	//allocate the hash buffer on the heap
	pbHash = (PBYTE)HeapAlloc(GetProcessHeap(), 0, cbHash);
	if (NULL == pbHash)	{
		printf("**** memory allocation failed\n");
		goto Cleanup;
	}

	//create a hash
	if (!NT_OK(status = BCryptCreateHash(hAlg, &hHash, pbHashObject, cbHashObject, NULL, 0, 0)))	{
		printf("**** Error 0x%x returned by BCryptCreateHash\n", status);
		goto Cleanup;
	}

	//hash some data
	if (!NT_OK(status = BCryptHashData(hHash, (PBYTE)pSrc, nLenSrc, 0))) {
		printf("**** Error 0x%x returned by BCryptHashData\n", status);
		goto Cleanup;
	}

	//close the hash
	if (!NT_OK(status = BCryptFinishHash(hHash, pbHash, cbHash, 0)))	{
		printf("**** Error 0x%x returned by BCryptFinishHash\n", status);
		goto Cleanup;
	};

	DWORD i = 0;
	for (; i < cbHash; i++) pDst[i] = pbHash[i];
	pDst[i] = '\0';

	back = 1;

Cleanup:
	if (hAlg)			BCryptCloseAlgorithmProvider(hAlg, 0);
	if (hHash)			BCryptDestroyHash(hHash);
	if (pbHashObject)	HeapFree(GetProcessHeap(), 0, pbHashObject);
	if (pbHash)			HeapFree(GetProcessHeap(), 0, pbHash);
	return back;

}


// --- cash_up: Kassenladen oeffnen via COM-Port ---
static bool OpenDrawer_BREAK_Pulse(const char* portName, DWORD breakMs, DWORD pulseMs)
{
    char full[64];
    sprintf(full, "\\\\.\\%s", portName);

    HANDLE hCom = CreateFileA(full, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hCom == INVALID_HANDLE_VALUE) return false;

    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(hCom, &dcb)) { CloseHandle(hCom); return false; }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(hCom, &dcb)) { CloseHandle(hCom); return false; }

    PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    EscapeCommFunction(hCom, CLRRTS);
    EscapeCommFunction(hCom, CLRDTR);
    ClearCommBreak(hCom);
    Sleep(30);

    SetCommBreak(hCom);
    Sleep(breakMs);

    ClearCommBreak(hCom);
    Sleep(20);

    EscapeCommFunction(hCom, SETRTS);
    EscapeCommFunction(hCom, SETDTR);
    Sleep(pulseMs);
    EscapeCommFunction(hCom, CLRRTS);
    EscapeCommFunction(hCom, CLRDTR);

    CloseHandle(hCom);
    return true;
}

// --- cash_anz: Bixolon BCD-2000 Kundenanzeige via COM-Port ---
static bool CashAnz_WriteAll(HANDLE h, const void* data, DWORD len) {
    const BYTE* p = (const BYTE*)data;
    while (len) {
        DWORD w = 0;
        if (!WriteFile(h, p, len, &w, NULL)) return false;
        if (w == 0) return false;
        p += w;
        len -= w;
    }
    return true;
}

static bool CashAnz_OpenCom(const char* port, HANDLE *out) {
    char full[64];
    sprintf(full, "\\\\.\\%s", port);

    HANDLE h = CreateFileA(full, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return false;

    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(h, &dcb)) { CloseHandle(h); return false; }

    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(h, &dcb)) { CloseHandle(h); return false; }

    COMMTIMEOUTS to;
    memset(&to, 0, sizeof(to));
    to.WriteTotalTimeoutConstant = 150;
    to.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(h, &to);

    PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    *out = h;
    return true;
}

static void CashAnz_ClearDisplay(HANDLE h) {
    const BYTE initCmd[2] = { 0x1B, 0x40 }; // ESC @
    CashAnz_WriteAll(h, initCmd, 2);
    Sleep(30);
    const BYTE ff = 0x0C;                    // FormFeed (clear)
    CashAnz_WriteAll(h, &ff, 1);
    Sleep(50);
}

static bool CashAnz_IsDigitsOnly(const char *s) {
    if (!s || !*s) return false;
    for (const char *p = s; *p; p++)
        if (*p < '0' || *p > '9') return false;
    return true;
}

// INI: PORT=COMx lesen
static bool CashAnz_ReadIniPort(const char *iniPath, char *portOut, int portMax) {
    FILE *f = fopen(iniPath, "r");
    if (!f) return false;
    char line[512];
    bool found = false;
    while (fgets(line, sizeof(line), f)) {
        // trim
        char *s = line;
        while (*s == ' ' || *s == '\t') s++;
        int len = strlen(s);
        while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' || s[len-1] == ' ')) s[--len] = '\0';
        if (!*s || *s == '#' || *s == ';') continue;
        char *eq = strchr(s, '=');
        if (eq) {
            *eq = '\0';
            char *key = s;
            char *val = eq + 1;
            while (*val == ' ' || *val == '\t') val++;
            // trim key
            int klen = strlen(key);
            while (klen > 0 && (key[klen-1] == ' ' || key[klen-1] == '\t')) key[--klen] = '\0';
            if (!_stricmp(key, "PORT")) {
                strncpy(portOut, val, portMax - 1); portOut[portMax-1] = '\0';
                found = true;
                break;
            }
        }
    }
    fclose(f);
    return found;
}

// INI: B<n>=text1|text2 lesen
static bool CashAnz_ReadIniBlock(const char *iniPath, int n, char *out1, int max1, char *out2, int max2) {
    FILE *f = fopen(iniPath, "r");
    if (!f) return false;
    char key[16];
    sprintf(key, "B%d", n);
    char line[1024];
    bool found = false;
    while (fgets(line, sizeof(line), f)) {
        char *s = line;
        while (*s == ' ' || *s == '\t') s++;
        int len = strlen(s);
        while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r' || s[len-1] == ' ')) s[--len] = '\0';
        if (!*s || *s == '#' || *s == ';') continue;
        char *eq = strchr(s, '=');
        if (!eq) continue;
        *eq = '\0';
        char *k = s;
        char *v = eq + 1;
        while (*v == ' ' || *v == '\t') v++;
        int klen = strlen(k);
        while (klen > 0 && (k[klen-1] == ' ' || k[klen-1] == '\t')) k[--klen] = '\0';
        if (!_stricmp(k, key)) {
            char *bar = strchr(v, '|');
            if (bar) {
                *bar = '\0';
                strncpy(out1, v, max1 - 1); out1[max1-1] = '\0';
                char *v2 = bar + 1;
                while (*v2 == ' ' || *v2 == '\t') v2++;
                strncpy(out2, v2, max2 - 1); out2[max2-1] = '\0';
            } else {
                strncpy(out1, v, max1 - 1); out1[max1-1] = '\0';
                out2[0] = '\0';
            }
            // trim trailing spaces
            int l1 = strlen(out1);
            while (l1 > 0 && (out1[l1-1] == ' ' || out1[l1-1] == '\t')) out1[--l1] = '\0';
            int l2 = strlen(out2);
            while (l2 > 0 && (out2[l2-1] == ' ' || out2[l2-1] == '\t')) out2[--l2] = '\0';
            found = true;
            break;
        }
    }
    fclose(f);
    return found;
}

// Port ermitteln: Kommandozeile > ENV > INI > Default
static void CashAnz_GetPort(const char *cmdPort, char *portOut, int portMax) {
    // 1) Kommandozeile
    if (cmdPort && *cmdPort) {
        strncpy(portOut, cmdPort, portMax - 1); portOut[portMax-1] = '\0';
        return;
    }
    // 2) Environment
    char buf[64];
    if (GetEnvironmentVariable("CASH_ANZ_PORT", buf, sizeof(buf)) > 0) {
        strncpy(portOut, buf, portMax - 1); portOut[portMax-1] = '\0';
        return;
    }
    // 3) INI neben EXE
    char iniPath[MAX_PATH];
    GetModuleFileName(NULL, iniPath, MAX_PATH);
    char *lastSlash = strrchr(iniPath, '\\');
    if (lastSlash) *(lastSlash + 1) = '\0';
    strcat(iniPath, "cash_anz.ini");
    if (CashAnz_ReadIniPort(iniPath, portOut, portMax)) return;
    // 4) Default
    strcpy(portOut, "COM4");
}

// Hauptfunktion: Display ansteuern
static int CashAnz_Run(const char *cmdPort, const char *text1, const char *text2) {
    char port[32];
    CashAnz_GetPort(cmdPort, port, sizeof(port));

    HANDLE hCom = NULL;
    if (!CashAnz_OpenCom(port, &hCom)) {
        char errmsg[128];
        sprintf(errmsg, "cash_anz: COM-Port '%s' nicht erreichbar!\n(Error %lu)", port, GetLastError());
        MessageBox(NULL, errmsg, "F2P - cash_anz Fehler", MB_OK | MB_ICONERROR);
        return 2;
    }

    CashAnz_ClearDisplay(hCom);

    if ((text1 && *text1) || (text2 && *text2)) {
        char msg[512];
        sprintf(msg, "%s\r\n%s\r\n", text1 ? text1 : "", text2 ? text2 : "");
        CashAnz_WriteAll(hCom, msg, (DWORD)strlen(msg));
    }

    CloseHandle(hCom);
    return 0;
}

void SComLine( char *t2Print)
{
  	// comline -
	// if ( t2Print[0] != '-' ) {
	//		                        return;
	// };

	  char meinch = t2Print[1];
	  t2Print++;
	  t2Print++;
  	  if ( t2Print[0] == ':' ) t2Print++;

	  switch (lower(meinch))

      {
         case 'p':
         	strncpy( upname , t2Print,98);
            break;
         case 'k':
         	strncpy( mySetupStr , t2Print,98);
            break;
         case 'f':
            strncpy( ufname , t2Print ,98);
            break;
         case 'i':
            strncpy( uminfo , t2Print,98);
            break;
         case 's':
            strncpy( usetdir , t2Print,98);
            break;
         case 'c':  // aus Clipboard
		    mitclip = 1;
			aamode = (lower(t2Print[0]));
			break;
		 case '2':  
			 f2tmode = true;
			 f2magic = (lower(t2Print[2]));
			 break;
		 case 'w':
            mymwmode = atoi( t2Print );
            break;
         case 'h':
              aushome  = true;
			  myhmode = t2Print[2];
            break;
		 case 'n':
			 nosave = true;
			 nosmode = t2Print[2];
			 break;
		 case 'v':
             gPaperView  = true;
             break;
     	 case 'y':
			 if ((t2Print[0] == 'w') || (t2Print[0] == 'W')) {
				 strcpy(ypar, t2Print);
				 wCalcRect(t2Print);
				 break;
			 };
			 CalcRect(t2Print[0]);
            break;
        case 'z':
            myzoomode = atoi( t2Print );
            break;
		 case 'm':
            mymenmode = atoi( t2Print );
            break;
        case 't':
            mytbmode = atoi( t2Print );
			if (mytbmode == 1) mytbmode = (VPE_GRIDBUTTON + VPE_NO_RULERS );
				               
			if (  ! mytbmode  ) mytbmode = ( VPE_NO_TOOLBAR +  VPE_NO_RULERS ) ;
            mytbmode |= (  VPE_EMBEDDED + VPE_DOCFILE_READONLY + VPE_NO_PAGESCROLLER + VPE_FIXED_MESSAGES ) ;
            break; 

         case 'o':
			mymenmode = 0; 
			mitvorschau = FALSE;
			mytbmode = (VPE_NO_TOOLBAR + VPE_NO_RULERS + VPE_EMBEDDED + VPE_FIXED_MESSAGES + VPE_NO_PAGESCROLLER + VPE_DOCFILE_READONLY);
			ovorch = (lower(t2Print[0]));
            if  ( (lower(t2Print[1])) == 'v' ) {
							mymenmode = 1; 
				            mitvorschau = TRUE;
				            autoend = TRUE;
			};
			
            break;

         case 'x':
            mitshellex  = true;
//			_ascii_towlower(t2Print[0])
			shellexmode = (lower(t2Print[0]));
			break;

		 case 'g':  // to end
			 spring2end   = true;
			 break;


		 case 'q':  // quick-pdf
			 shellexmode = (lower(t2Print[0]));
			 break; 

		 case 'u':
			 clipupdate = true;
			 break;
		 case 'a':
              // wird am Desktop\F2P immer eine neue Datei angelegt
			  directpdf = true;             
			  aamode = (lower(t2Print[0]));
			  if (aamode == 'p' ) t2Print++;
			  if (aamode == 'a')  t2Print++;
			  if (aamode == 's')  t2Print++;
			  if (aamode == 'l')  t2Print++;  // PDF Archiv Format

			  strncpy(uaname, t2Print, 98); //

			  if ((strlen(uaname)) > 4) {  // " entfernen
				  directpdf = false;
				  int zloop = 0;
				  char mychar;
				  for (int sloop = 0; uaname[sloop] != '\0'; sloop++) {
					  mychar = uaname[sloop];

					  if (uaname[sloop] == '/') mychar = '_';
					  if (uaname[sloop] == '*') mychar = '_';
					 // if (uaname[sloop] == ':') myfname[sloop] = '_';
					  if (uaname[sloop] == '|') mychar = '_';
					  if (uaname[sloop] == '?') mychar = '_';
					  if (uaname[sloop] == '>') mychar = '_';
					  if (uaname[sloop] == '<') mychar = '_'; 
					 // if (uaname[sloop] == '"') mychar = ' ';
					  if (mychar != '"') uaname[zloop++] = mychar;
				  };
                  
				  uaname[zloop] = '\0';

				  if (mychar == '~')  {
					  uaname[zloop - 1] = '\0';
					  strcat(uaname, myindis);
				                       };
				  zloop = strlen(uaname);
				  if (uaname[zloop - 4] != '.') {
					  if ((aamode == 'p') || (aamode == 'l'))	  strcat(uaname, ".pdf");
                                                             else strcat(uaname, ".a2p");
				  }; 


// rueberkopieren
				  if ((aamode == 'p') || (aamode == 'l')) {
					  will_save = true;
					  break; // die werden am Ende exportiert
				  };
				  FILE *fh;
				  FILE *fho;

				  if ((fh = fopen(file2print, "rbS")) == NULL)
				  {
					  sprintf(mMsg, "Fehler bei �ffnen!\nDateiname: %s!", file2print);
					  MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
					  exit(7);
					  //return false;
				  };

				  if ((fho = fopen(uaname, "wbS")) == NULL)
				  {
					  sprintf(mMsg, "Fehler bei �ffnen!\nArchiv Datei: %s!", uaname);
					  MessageBox(hMainWindow, mMsg, "Leider", MB_OK);
					  exit(7);
					  //return false;
				  };

				  bool changeincl = false;
				  char mylongl[10240];
				  while (fgets(mylongl, 10240, fh) != 0) {
					 //  int sola = strlen(mylongl);
					 // if (mylongl[sola] == '\r') mylongl[sola] == '\0';

					  if (mylongl[0] == '[') {
						  if (!strncmp(mylongl, "[=/|",   4)) goto lasses;   // Kommentar [=/|[=%,u,ATU524536068493
						  if (!strncmp(mylongl, "[==!",   4)) goto lasses;   //           [==!!___G^H^4^
						  if (!strncmp(mylongl, "[=set",  5)) goto lasses;   // set       [=set,d,t:\df\pdf\history\rechnung
						  if (!strncmp(mylongl, "[=incl", 6)) {
							  char incfname[399], vorpar[7];
							  sscanf(mylongl + 7, "%[^,],%s", vorpar, incfname);
//							  strcpy(incfname, mylongl + 9);
							  FILE *fhi;
							  if ((fhi = fopen(incfname, "rt")) != NULL) {
								  int ilauf = 1;
								  changeincl = true;
								  while (fgets(incfname, 512, fhi) != NULL) {

									  int tende = strlen(incfname) - 1;
									  for (; tende >= 0 && incfname[tende] <= 32 && incfname[tende] > 0; tende--);  // strip LF, CR, Blank
									  if (tende > 0) {
										  incfname[tende + 1] = '\0';
										  strcpy(SteuBlock2[ilauf++], incfname);
									  }
									  else strcpy(SteuBlock2[ilauf++], "");;
									  if (ilauf > 5) goto lassincl;

								  };
							  lassincl:
								  fclose(fhi);
								  goto lasses;


						  };
					  };

					  };



// [=incl,f,.\lastqr.txt                                       
// [xl+0.02,9.2,-3,-3,BC97,\'v1



					  if (changeincl) {
					  for (int i = 0; mylongl[i]; i++)  {

						  if ((mylongl[i] == '\\') && (mylongl[i + 1] == '\'')) {

							  if (mylongl[i + 2] == 'v') {
		 			  			      if ((mylongl[i + 3] > '0') && (mylongl[i + 3] < '6')) {

									  if (mylongl[i + 3] == '1') fputs(SteuBlock2[1], fho);
									  if (mylongl[i + 3] == '2') fputs(SteuBlock2[2], fho);
									  if (mylongl[i + 3] == '3') fputs(SteuBlock2[3], fho);
									  if (mylongl[i + 3] == '4') fputs(SteuBlock2[4], fho);
									  i = i + 4;
								                                                             };
							  };
						  };

							  fputc(mylongl[i], fho);
					  };

							  goto lasses;

						  };


					  fputs(mylongl, fho);
				  lasses:				  ;
				  };


				  fclose(fh);
				  fclose(fho);

		//		  if (aamode != 's')  break;

				  break; // sec nicht erstellen


			/*	  if ((fh = fopen(uaname, "rbS")) != NULL) {

					  int maxread;
					  fseek(fh, 0, SEEK_END);
					  maxread = ftell(fh);
					  if (maxread > 64000) maxread = 48000;
					  fseek(fh, 0, SEEK_END);
					  fseek(fh, 0, SEEK_SET);   // r�ckspulen

					  mylongline = (char *)malloc(maxread + 2);  // ca. 1024 * 32 

					  int numread = fread(mylongline, sizeof(char), maxread, fh);
					  char sTurnOverValue[64];
					  if (!hash_SHA256((BYTE *)mylongline, maxread, sTurnOverValue)) ;

					//   ToBase64Crypto(sTurnOverValue, 8, Sig_vor_Beleg, 16);

					  fclose(fh);

					  strcat(uaname, ".sec");

					  char checks[64], nochecks[32] = "\0", tchecks[4];
					  int checkscou = 0;
					  
					  if ((fho = fopen(uaname, "wbS")) != NULL)
					  {
				//		  fprintf(fho, "Hash-256: ");
						  for (int i = 0; i < 32; i++) {
							  fprintf(fho, "%02x", (BYTE)sTurnOverValue[i]);

							  if (sTurnOverValue[i] >= '0') {
								  if (sTurnOverValue[i] <= '9') {
									  checks[checkscou++] = sTurnOverValue[i];
									  goto goodchar;
								                                };
								  if (sTurnOverValue[i] >= 'A') {
									  if (sTurnOverValue[i] <= 'Z') {
										  checks[checkscou++] = sTurnOverValue[i];
										  goto goodchar;
									  };
  								  }
								  if (sTurnOverValue[i] >= 'a') {
									  if (sTurnOverValue[i] <= 'z') {
										  checks[checkscou++] = sTurnOverValue[i];
										  goto goodchar;
									  };
								  };
							  };

							  if (i < 16) {
								  sprintf(tchecks, "%02x", (BYTE)sTurnOverValue[i]);
								  strcat(nochecks,tchecks);
							              };

 goodchar: ;

						  };

						  checks[checkscou] = '\0';
						  strcat(checks,nochecks);
						  checks[16] = '\0';

						  fputs("\n", fho);
						  fprintf(fho, "%d", maxread);
						  fputs("\n", fho);

						  fprintf(fho, "%s", checks);
						  fputs("\n", fho);

					  };

					  fclose(fho);


				  }; */


		//		  fclose(fh);
                  

			  };


			break;


         case 'd':
             if (!strncmp(t2Print, "time", 4)) {
                 char *val = t2Print + 4;
                 if (*val == ':') val++;
                 dbg_time_threshold = atoi(val);
                 if (dbg_time_threshold < 1) dbg_time_threshold = 2;
             }
             break;

         default:
	 		sprintf(mMsg, "Commandline Argument ung�ltig...\n\n(%s)\n",t2Print);
	        Msg(mMsg);

      };   // case
};

int ScanLine( char *t2Print)    // ist nur gedacht bei Clipboard einlesen 
  {

 	  char *par1;
	  par1 = t2Print ;		
	  
	  for (int lauf = 0 ; ; lauf++ ) {
		  
		  if ( t2Print[lauf] < 14 ) {             // Steuerungscharacter

	//		  if ( t2Print[lauf] == '\t' ) {      // Tabulator zulassen
	//			  t2Print[lauf] = ' ';
	//			  continue;
	//		  };


 		      if (! strncmp( par1 , "[=)",3))        {   // Magic - Parameter ab da starte F2P
				  if ((strcmp( file2print,"-c-" )))  {   // oder wenn nur als Steuerung merken als Magic-String f�r den druck 
					  			  t2Print[lauf] = '\0';
								  strcpy(mySetupStr,par1+3);
								  return 0;
				                                     };
				  return lauf;
                                   				    };

			  t2Print[lauf] = '\0';  


pruaufchweitr:
			  if ( t2Print[lauf + 1] < 14 )  {
				  lauf++;
				  if ( t2Print[lauf] == '\0' ) break;  // String ist Fertig
				  if ( t2Print[lauf] == 3 )    break;  // oder EOF 
				  goto pruaufchweitr;
			  };


			  SComLine(par1);
			  mitclip = lauf + 1;
			  par1 = t2Print + lauf + 1 ;		
				                   }
		                                   }
  SComLine(par1);
  mitclip = 0;
  return 0;
};


// ========================================================================
//                              WinMain
// ========================================================================
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine , int nCmdShow)
{


   HWND        hwnd = 0 ;
   WNDCLASS    wndclass ;

   QueryPerformanceCounter((LARGE_INTEGER *)&start0);

   GetLocalTime (&st);
   int i;

   i = st.wMilliseconds;
   i = i + ( 1000 * st.wSecond );
   i = i / 100;
 
   char tempfname[99];
   sprintf(myindis, "%02i%02i_%02i%02i_%03i",st.wMonth,st.wDay,st.wHour,st.wMinute,i);

   strcpy(myfname, "F2P_");
   strcat(myfname, myindis);
   strcat(myfname, ".pdf");

 
    if    ( __argc == 1 ) {
		           strcpy(file2print,"-?");
			//	   mitclip = 1;
                           }
	                else {
		// cash_up: Kassenladen oeffnen und beenden
		if (!strncmp(__argv[1], "-cash_up", 8)) {
			char *port = __argv[1] + 8;
			if (*port == ':') port++;
			// Port auch aus argv[2] nehmen wenn leer
			if (*port == '\0' && __argc > 2) port = __argv[2];
			if (*port == '\0') port = "COM3";
			// trailing : entfernen (COM4: -> COM4)
			char portbuf[32];
			strncpy(portbuf, port, 31); portbuf[31] = '\0';
			int plen = strlen(portbuf);
			if (plen > 0 && portbuf[plen-1] == ':') portbuf[plen-1] = '\0';
			if (!OpenDrawer_BREAK_Pulse(portbuf, 250, 200)) {
				char errmsg[128];
				sprintf(errmsg, "cash_up: COM-Port '%s' nicht erreichbar!\n(Error %lu)", portbuf, GetLastError());
				MessageBox(NULL, errmsg, "F2P - cash_up Fehler", MB_OK | MB_ICONERROR);
				exit(2);
			}
			exit(0);
		}
		// cash_anz: Kundenanzeige ansteuern und beenden
		if (!strncmp(__argv[1], "-cash_anz", 9)) {
			char *port = __argv[1] + 9;
			if (*port == ':') port++;
			// trailing : entfernen
			char portbuf[32] = "";
			if (*port) {
				strncpy(portbuf, port, 31); portbuf[31] = '\0';
				int plen = strlen(portbuf);
				if (plen > 0 && portbuf[plen-1] == ':') portbuf[plen-1] = '\0';
			}
			// Text-Argumente: argv[2] und argv[3]
			char text1[256] = "";
			char text2[256] = "";
			if (__argc > 2) {
				// Blocknummer aus INI? (nur Ziffern)
				if (CashAnz_IsDigitsOnly(__argv[2])) {
					char iniPath[MAX_PATH];
					GetModuleFileName(NULL, iniPath, MAX_PATH);
					char *ls = strrchr(iniPath, '\\');
					if (ls) *(ls + 1) = '\0';
					strcat(iniPath, "cash_anz.ini");
					CashAnz_ReadIniBlock(iniPath, atoi(__argv[2]), text1, 255, text2, 255);
				} else {
					strncpy(text1, __argv[2], 255); text1[255] = '\0';
					if (__argc > 3) { strncpy(text2, __argv[3], 255); text2[255] = '\0'; }
				}
			}
			exit(CashAnz_Run(portbuf, text1, text2));
		}
		(strcpy(file2print,__argv[1]));
	}



	GetModuleFileName(NULL, curexedir, 155);
	// ohne Setverz. Curexedir f�r alle Includes Bitmaps usw.
	_splitpath(curexedir, drive, mdir, fname, ext);
	// if ((strlen(fname)) > 5) oldf2pexe = TRUE;
	_makepath(curexedir, drive, mdir, "", "");

	int frageende = 0;


	// if (i == 2) {
	if  (file2print[0] == '?')  {   // Schalter f. DFPRINT Mode arg1 = !?!

			//	PComLine(__argv[i]);
			INT_PTR diaerge;
			strcpy(file2print, file2print + 1);

			strcpy(SteuBlock[1], file2print);
			DWORD ftyp = GetFileAttributes(file2print);

			if (ftyp == INVALID_FILE_ATTRIBUTES) {
				sprintf(mMsg, "Invalid Input File...\n\n(%s)\n", file2print);
				Msg(mMsg);

			};


			if (__argc > 2) strcpy(SteuBlock[5], __argv[2]);

			char htline[128];

			if (!GetEnvironmentVariable("HOME", htline, 99)) strcpy(htline, GetTmpDir());
			strcat(htline, "\\tmp\\");

			 ftyp = GetFileAttributes(htline);
			//	if (ftyp == INVALID_FILE_ATTRIBUTES)
			//		return false;  //something is wrong with your path!

			if (ftyp & FILE_ATTRIBUTE_DIRECTORY) {

				if ((strlen(SteuBlock[5])) > 5) strcat(htline, SteuBlock[5]);
				else strcat(htline, "dfprint");


				char myday[7] = "__";

				i = 0;
			nachstline:
				  
				    strncpy(ufname, htline, 98);
					if (i > 0) {
						strcat(ufname, "(");
						_itoa(i, myday, 10);
						strcat(ufname, myday);
						strcat(ufname, ")");

					};
					strcat(ufname, ".PDF");

					if ((_access(ufname, 0)) != -1) 
					 {
						i++;
						if (i < 99) goto nachstline;
					};

				//	if (FileAttributes & FILE_ATTRIBUTE_NORMAL) {
				//		printf("Normal ");
				//		FileAttributes &= ~FILE_ATTRIBUTE_NORMAL;
				//	}

/*
				char indite[21];


				switch (st.wDayOfWeek)
				{
				case 0: strcpy(myday, "So"); break;
				case 1: strcpy(myday, "Mo"); break;
				case 2: strcpy(myday, "Di"); break;
				case 3: strcpy(myday, "Mi"); break;
				case 4: strcpy(myday, "Do"); break;
				case 5: strcpy(myday, "Fr"); break;
				case 6: strcpy(myday, "Sa"); break;
				};

/*				int seci = st.wMilliseconds;
				seci = 2 * seci / 3;
				int seci2 = seci / 26;
				myday[0] = seci2 + 97;
				myday[1] = (seci - (25 * seci2)) + 97;

				i = st.wMilliseconds;
				i = i + (1000 * st.wSecond);
				i = i / 100;

				sprintf(indite, "_(%s%02i%02i_%03i).PDF", myday, st.wHour, st.wMinute,i);
				                 
				strcat(htline, indite);
				*/
			//	strcat(htline, myday);
			//	strcat(htline, ".PDF");

			}
			else {

				sprintf(mMsg, "Directory fehlt...\n\n(%s)\n", htline);
				Msg(mMsg);

				return 1;    // this is not a directory!
			};


		//	strncpy(ufname, htline, 98);


 		    _splitpath(ufname, drive, mdir, fname, ext);  // Fullname
			_makepath(SteuBlock[2], drive, mdir, "", "");       // Ordner

		//	strupr(fname);
			_strupr(ext);

			strcpy(SteuBlock[3], fname);   // Datei
			strcpy(SteuBlock[4], ext);     // Erweiterung

			aktline = __argc;
			aktline--;  // Anzahl Parameter !?! �berspringen

			if (__argc > 4) strcpy(mmailto, __argv[4]);
			if (__argc > 3) strcpy(SteuBlock[6], __argv[3]);


		//	if (__argc > 5) strcpy(SteuBlock[8], __argv[5]);
		//	if (__argc > 6) strcpy(SteuBlock[9], __argv[6]);

			if ((strlen(SteuBlock[6])) > 1) {

				HWND hparwindowt = (HWND)(intptr_t)_atoi64(SteuBlock[6]);

				if (IsWindow(hparwindowt)) 	hparwindow = hparwindowt;

			};


//			diaerge = DialogBox(hInstance,	MAKEINTRESOURCE(IDD_DDIALOG2),	hparwindow, DeleteItemProc);

			diaerge = DialogBoxParamA(hInstance, MAKEINTRESOURCE(IDD_DDIALOG2), hparwindow, (DLGPROC) DeleteItemProc, 0L);

			if (diaerge < 0) {
				Msg("kann Dialog nicht laden");
				return 1;

			};
			if (diaerge == IDCANCEL  ) return 0;


			if (diaerge == IDC_BUTTON1) {
				// klick auf Filename

			};

			if (diaerge == IDOK) {
				strcpy(file2print,"F2P\\info\\dialog1.f2p");
				SComLine("-m0");
				SComLine("-t0");

			};

			if (diaerge == IDC_BUTTON2) {
				SComLine("-or");
				frageende = IDC_BUTTON2;
			}

			if (diaerge == IDOKDRUCK) {
				SComLine("-op");
			};
			if (diaerge == IDOKOPEN) {
				SComLine("-or");
				frageende = IDOKOPEN;
			};
			if (diaerge == IDOKMAIL) {
				SComLine("-or");
				frageende = IDOKMAIL;
			};

			if (diaerge == IDCOP2CLIP ) {
				SComLine("-or");
				frageende = IDCOP2CLIP;
			};

			goto nachfrage;
		};
	



//--- Ini Steuerung eventuell-sp�ter
/*  if (__argc == 2) {   // nur Dateiname als argument

	  _splitpath(file2print, drive, mdir, fname, ext);

	  if ((strlen(ext)) < 1) {
		  strcpy(ext, ".f2p");
	  };

	  ext[0] = '@';

	  char setname[64];
	  char setchar;

	  setchar = (fname[(strlen(fname)) - 1]);
	  setchar = (lower(setchar));
	  strcpy(setname, "ini\\");
	  strcat(setname, ext);

	  GetModuleFileName(NULL, curexedir, 255);
	  // ohne Setverz. Curexedir f�r alle Includes Bitmaps usw.
	  _splitpath(curexedir, drive, mdir, fname, ext);
	  _makepath(tempfname, drive, mdir, setname, ".f2s");

	  // wsprintf(tempfname, _T("%s\\cliptmp.f2p"), GetTmpDir());
	  // wsprintf(tempfname, _T("%s\\cliptmp.f2p"), GetTmpDir());
//	  if ((lfh = fopen(file_name, "rb")) != NULL) {
	   if ( (flog = fopen(tempfname, "rS")) != NULL) {


	    char fline[1032];
	  

		// zu ersetzen Drirectorys
		// .H\ = ( set Home )
		// .T\ = ( GetTmpDir )
		// .V\ = ( set DFROOT wenn nicht gesetzt .E\.. )

		// .E\ = ( aktuelle Execute Directory )
		// .D\ = ( aktuelle Dateiname Directory )
	
		// %F = ( aktuelle File-Name )
		// %T =  myindis
		// %1 ... %9 =  aufruf parameter durchschleifen

 	    while (fgets(fline, 1024, flog) != NULL) {

	    if (fline[0] == '!') {
			  if ((fline[1] == '!') || (fline[1] == setchar)) {
				  int tende = strlen(fline) - 1;
				  for (; tende >= 0 && fline[tende] <= 32 && fline[tende] > 0; tende--);  // strip LF, CR, Blank
				  fline[tende + 1] = '\0';
				  
				  // mit xX execute neues Programm, mit > setze Cmdline f. f2p
				  if ( (fline[2] == 'x') || (fline[2] == 'X') || (fline[2] == '>') )  {






				                       };

				                    			  }
	
		};

                                             	  };
		  };

	   //	  if (!strncmp(fline, "[=)", 3)) setmee(fline + 3, 1);
//	  _makepath(curexedir, drive, mdir, "", "");



  }; */


  for (i = 2; i < __argc; i++)
	{
		char meinch;
		meinch = __argv[i][0];

		if (meinch != '/') {
		if ( meinch != '-' ) {
		 		sprintf(mMsg, "Commandline Argument ung�ltig...\n\n(%s)\n",__argv[i]);
		        Msg(mMsg);
				continue;
		}
		}

      SComLine(__argv[i]);
	}; //for

	if ( mitclip ) {
     HGLOBAL   hglb; 
     LPTSTR    lptstr; 
	 if (aamode == '-') exit(rueckmcode);

	 if (aamode == '?') {

		 sprintf(mMsg, "Commandline Argument ung�ltig...\n\n(%s)\n", __argv[1]);
		 Msg(mMsg);


		 exit(rueckmcode);
	 };
   if (IsClipboardFormatAvailable(CF_OEMTEXT)) { 
          if (OpenClipboard(hwnd)) { 
   
          hglb = GetClipboardData(CF_OEMTEXT); 
          if (hglb != NULL) 
          { 
              lptstr = (LPTSTR)GlobalLock(hglb); 
              if (lptstr != NULL) 
              { 

                 if ( ScanLine(lptstr) ) {  // sind nur Parameter?	

				 if (mitclip) {
				 	wsprintf(tempfname, _T("%s\\cliptmp.f2p"), GetTmpDir());
                    flog = fopen(tempfname, "wt");
				    fputs( lptstr + mitclip, flog );
                    fclose(flog);				  
					strcpy(file2print,tempfname);
				             };

			                             };
                   GlobalUnlock(hglb); 
              } 
          } 
          CloseClipboard(); 
		   }
	}
	};
 


//4   VpeRegisterCtl3D(hInstance);
  
nachfrage:

   hMenu = NULL;

   if ( mymenmode == 1 ) {
	   hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(MENU_1));
       if ( hMenu != NULL )  CheckMenuItem(hMenu, ID_SAVES_, MF_UNCHECKED);
	   };

   hpopMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
 //   if ( hMenu != NULL )  CheckMenuItem(hMenu, ID_SAVES_, MF_UNCHECKED);
   
   if (!hPrevInstance)
   {
      wndclass.style         = CS_HREDRAW | CS_VREDRAW;
      wndclass.lpfnWndProc   = WndProc ;
      wndclass.cbClsExtra    = 0 ;
      wndclass.cbWndExtra    = 0 ;
      wndclass.hInstance     = hInstance ;
	  wndclass.hIcon         = LoadIcon (hInstance, ( MAKEINTRESOURCE( 102 )) );
      wndclass.hCursor       = NULL; //LoadCursor (NULL, IDC_ARROW) ;
      wndclass.hbrBackground = GetStockBrush (NULL_BRUSH) ;
      wndclass.lpszMenuName  = NULL;
      wndclass.lpszClassName = szAppName;
      RegisterClass(&wndclass) ;
   }

   char Apptitle[164];
   strcpy( Apptitle, szAppName );
   strcat( Apptitle, "  (");
   strcat( Apptitle, file2print);
   strcat( Apptitle, ")");
   hwnd = CreateWindow(szAppName, Apptitle,
                       WS_OVERLAPPEDWINDOW  | WS_CLIPCHILDREN | WS_MAXIMIZE,
					   xoben,yoben,
					   xbreit,yhoch,
					  // CW_USEDEFAULT, CW_USEDEFAULT,
                      //  CW_USEDEFAULT, CW_USEDEFAULT,
                      //  NULL, hMenu, hInstance, NULL) ;
					  hparwindow, hMenu, hInstance, NULL);
     if ( ! mitvorschau ) goto ohnevor;

     if (!_strnicmp( file2print,"-?",2)) {
		   mypaintmode = 1;
           ShowWindow (hwnd, SW_SHOWMAXIMIZED);
		   MyCmlHelp();
           goto zeigmi;
	   };

   if ( mymwmode & 1 )  mypaintmode = 1;
   int showmode=nCmdShow;
   if ( mymwmode & 4 ) showmode=SW_SHOWMAXIMIZED;
   if ( mymwmode & 8 ) showmode=SW_SHOWMINIMIZED;

   ShowWindow (hwnd, showmode);
   UpdateWindow (hwnd) ;


ohnevor:
    
   VpeSetPictureCacheSize(1024);
	   // ohne Setverz. Curexedir f�r alle Includes Bitmaps usw.
   if ( usetdir[0] != '\0') {

     // sonst  dir f�r alle Includes Bitmaps usw.
     _splitpath( usetdir, drive, mdir, fname, ext );
     _makepath ( curexedir, drive, mdir, fname, "" );   
 
		 		            }


    strcpy(usetdir,"\0");  // ab dem usetdir dir f�r R�ckmeldung
	 
    strcpy(orif2print,file2print);
    magicchar = orif2print[(strlen(orif2print)) - 5];

// 	hMainWindow =  GetDesktopWindow();

	rueckmcode = 1;

//	if (LineRender()) {
    if (AutoRender(0)) {

//	if (myDuptest()) {

 
   if ( mitvorschau )  {
       mypaintmode=1;

       if ( !warprev )  {
       VpePreviewDoc(hDoc, NULL , VPE_SHOW_MAXIMIZED);
	                    };
	 //  hMainWindow = VpeGetWindowHandle(hDoc);
      SetFocus(VpeGetWindowHandle(hDoc));
	  HWND myhw;
	  myhw = VpeGetWindowHandle(hDoc);
	 // if ( (strlen(upname)) > 4 ) {
	        // if (  setmyprs("") < 1 ) exit ( 99 );
		//	                      };
	  if ( ovorch > ' ' ) {

	  if ( (ovorch == 'd') || ( ovorch == 'p')) {
		                         VpeSendKey(hDoc,VKEY_PRINT);
	  };
	  if   (ovorch == 'r') VpeSendKey(hDoc,VKEY_SAVE);
	  if   (ovorch == 'm') VpeSendKey(hDoc,VKEY_MAIL);
	  if   (ovorch <= '9') {
	  if   (ovorch >= '0') {
		                 int ili = ovorch;
						 ili = ili - 47;
                         Sleep(ili*500);
						 ovorch = 'o';
	                       };
	                       };



	  if (ovorch == 'e') {
		  ExportDocument(sreplace(file2print, ".F2P", ".VPE"), 0);
		  remove(orif2print);
		  ovorch = 'o';
	  };

	  if (ovorch == 'x') {
		  ExportDocument(sreplace(file2print, ".F2P", ".PDF"), 0);
		  remove(orif2print);
		  ovorch = 'o';
	  };


	  if   (ovorch == 'o')  VpeSendKey(hDoc,VKEY_CLOSE);
	  
                        };
   }
   else {

      switch  ( ovorch )

      {
         case 'p':
         case 'd':
		   if ( (strlen(upname)) > 4 ) {
	       //  if (  setmyprs("") < 1 ) exit ( 99 );
		     ovorch = 'p';  // nicht nochmal fragen
		                               };

		   if (ovorch == 'd') VpePrintDoc(hDoc,1);
		                 else VpePrintDoc(hDoc,0);

             break;

         case 'r':


			 if ( (strlen(ufname)) > 2 ) {

		       char *p;
		       p = strchr( ufname, '.' );   // ist mit PRS
		       if ( p == NULL ) {
			        strcat(ufname, myindis);
                    strcat(ufname, ".pdf");
			                    };

			                             };
  		       ExportDocument(ufname,mitshellex);

			  break;

		 case 'e':
			 ExportDocument(sreplace(file2print, ".F2P", ".VPE"), 0);
			 remove(orif2print); 
			 break;
		 case 'x':
			 ExportDocument(sreplace(file2print, ".F2P", ".PDF"), 0);
			 remove(orif2print);
			 break;

		 case 'm':
		   if (filemail()) {
			           i = VpeMailDoc(hDoc);
					   fileback(i);
		                  };
             break;

			};   // switch


	  if (frageende == IDOKOPEN) {
		  ShellExecute(hparwindow, "OPEN", ufname , NULL, NULL, SW_SHOW);
	  };
	  if (frageende == IDOKMAIL) {
		  cop2clip(ufname);
		  strcpy(mMsg, "mailto:");
		  strcat(mMsg, mmailto);

		  ShellExecute(NULL, "OPEN", mMsg, NULL, NULL, SW_SHOW);
	  };

	  if (frageende == IDCOP2CLIP)    cop2clip(ufname);

	  if (frageende == IDC_BUTTON2) {

		  _splitpath(ufname, drive, mdir, fname, ext);  // Fullname
		  _makepath(SteuBlock[2], drive, mdir, "", "");       // Ordner

		  strcpy(mMsg, "/e,");
		  strcat(mMsg, SteuBlock[2]);
		  strcat(mMsg, "\\,/select,");
		  strcat(mMsg, ufname);


//		  ShellExecute(hparwindow, "OPEN", "explorer", "/e,C:\\dev\f2p, /select, C:\\dev\\f2p\\f2p.exe", NULL, SW_SHOW);
		  ShellExecute(hparwindow, "OPEN", "explorer", mMsg , NULL, SW_SHOW);
	  };

        PostQuitMessage(0);
   }; 
   };

zeigmi:

  MSG    msg ;

  if (spring2end) {
	 VpeSendKey(hDoc, VKEY_PAGE_LAST);
  };




  while (GetMessage (&msg, NULL, 0, 0))
   {


	  if (msg.message == WM_KEYUP) {

		  int mpcount, gotop;
		  char myein = (char)msg.wParam;
		   


		  switch (myein)
			  
		  {

		  case '1':
		  case '2':
		  case '3':
		  case '4':
		  case '5':
		  case '6':
		  case '7':
		  case '8':
		  case '9':
			  gotop = myein - 48;

			  mpcount = VpeGetPageCount(hDoc);
			  if (mpcount < gotop) gotop = mpcount;
			  VpeGotoVisualPage(hDoc, gotop);
			  break;

		  case 'T':
		  case 'A':

			  VpeGotoVisualPage(hDoc, 0);
			  break;

		  case 'E':
		  case 'B':

			  mpcount = VpeGetPageCount(hDoc);
			  VpeGotoVisualPage(hDoc, mpcount);
			  break;

		  case 'F':
			  //	  case ID_ANSICHT_GANZESEITE:
			  VpeSetScaleMode(hDoc, VSCALE_MODE_FULL_PAGE);
			  goto habeschon;


			  break;

		  case 'W':
             //	  case ID_ANSICHT_VOLLEBREITE:
			  VpeSetScaleMode(hDoc, VSCALE_MODE_PAGE_WIDTH);
			  goto habeschon;
			  break;

		  case VK_SUBTRACT:
			  VpeZoomOut(hDoc);
			  break;
		  case '0':
			  VpeSetScalePercent(hDoc, 100);
			  break;
		  case  VK_ADD :
			  VpeZoomIn(hDoc);

break;			  

		  case  VK_SNAPSHOT:
			  VpeSendKey(hDoc, VKEY_PRINT);
		//	  goto habeschon;
			  break;

		  }

		  goto habeschon;



	  };

           TranslateMessage (&msg) ;
           DispatchMessage (&msg) ;
	   habeschon:;

   }


//4   VpeUnregisterCtl3D(hInstance);

  VpeCloseDoc(hDoc);


 // return msg.wParam;

 // if (mitlog) goto startneu;
    exit( rueckmcode );

}
 
// cmdline Argumente f. Debugger 
// c:\dev\nagy\mail9.f2p -I:c:\dev\nagy\body9.f2p -om
// c:\dev\bon.f2p - a"c:\dfhome\testa~"
/*
[=set, d, C:\DEV\NAGY
[=set, f, A21_12 / 34\"test
[=set, t, pdf
[=set, e1, %%d\%%f.%%t

[=set, e2, C:\dev\umi\lastf2p.html
[= / aufrufen von BAT Dateien
[=set, r3, ^cmd / C c : \dev\fsend C : \dev\umi\lastf2p.html
[=set, P4,
*/

