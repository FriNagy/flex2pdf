void PrintBint()
{

	int count = VpeDevEnumPaperBins(hDoc);
	int li;
	char s[256];
	VpeWrite(hDoc, VLEFT, VTOP, VRIGHT, VFREE, "[B U CE ]Installierte PaperBins");
	VpeWrite(hDoc, VLEFT, VTOP, VRIGHT, VFREE, "[BO UO L]");

	for (li = 0; li < count; li++) {
		_itoa(li, s, 10);
		VpeWrite(hDoc, 1, (li * 1) + 4, -3, -1, s);
		VpeGetDevPaperBinID(hDoc, li);
		_itoa(VpeGetDevPaperBinID(hDoc, li), s, 10);
		VpeWriteBox(hDoc, 2, (li * 1) + 4, -2, -0.6, s);
		VpeGetDevPaperBinName(hDoc, li, s, sizeof(s));
		VpeWriteBox(hDoc, 4, (li * 1) + 4, -12, -0.6, s);
	};
}

void PrintDevt()
{

	int count = VpeDevEnum(hDoc);
	int li;
	char s[256];

	VpeWrite(hDoc, VLEFT, VTOP, VRIGHT, VFREE, "[B U CE ]Installierte Drucker");

	VpeSetBkgGradientTriColor(hDoc, TRUE);

	VpeSetBkgMode(hDoc, VBKG_GRD_LINE);

	VpeSetBkgGradientMiddleColorPosition(hDoc, 35);
	VpeSetBkgGradientStartColor(hDoc, COLOR_LTGRAY);
	VpeSetBkgGradientMiddleColor(hDoc, COLOR_WHITE);
	VpeSetBkgGradientEndColor(hDoc, COLOR_DKGRAY);
	VpePrint(hDoc, 1, 1, "Hello World!");


	VpeWrite(hDoc, VLEFT, VTOP, VRIGHT, VFREE, "[BO UO L]");



	for (li = 0; li < count; li++) {

		_itoa(li, s, 10);
		VpeWrite(hDoc, 1, (li * 1) + 4, -3, -0.6, s);
		VpeGetDevEntry(hDoc, li, s, sizeof(s));
		VpeWriteBox(hDoc, 4, (li * 1) + 4, -12, -0.6, s);

	};

}


int PrintRender()
{


	VpeCoord y;

	HCURSOR hOldCursor;

	lasty = 0; lastx = 0;
	globczhoch = 0;
	char tline[1024];

	wantbreak = false;

	if (hDoc) VpeCloseDoc(hDoc);

	hDoc = VpeOpenDoc(hMainWindow, "FlexPrint", mytbmode);

	Myreg();

	hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	//  hDoc = VpeOpenDoc(hMainWindow, "FlexPrint", mytbmode );

	//  VpeSetPageScrollerTracking(hDoc, true);
	VpeSetMinScale(hDoc, 0.50);
	VpeDefineKey(hDoc, VKEY_CLOSE, VK_ESCAPE, 0, 0);
	VpeDefineKey(hDoc, VKEY_PAGE_FIRST, VK_ESCAPE, VK_HOME, 0);
	VpeDefineKey(hDoc, VKEY_PAGE_LAST, VK_ESCAPE, VK_END, 0);
	VpeDefineKey(hDoc, VKEY_PRINT, VK_F12, 0, 0);
	VpeSetPaperView(hDoc, true);

	// bool inifile = false;
	//  if (! SetThreadLocale(0x041b))  Msg("no Thread Locale") ;

	VpeSetFont(hDoc, "Courier New", 10);
	VpeSetScalePercent(hDoc, 100);

	PrintDevt();
	VpePageBreak(hDoc);
	PrintBint();
	VpePageBreak(hDoc);

	VpeGetDevice(hDoc, tline, 64);
	sprintf(mMsg, "Printer: %s\n", tline);
	y = VpePrint(hDoc, 1, 2, mMsg);
	sprintf(mMsg, "PhysPage :  W:%#5.2f H:%#5.2f \n", VpeGetDevPhysPageWidth(hDoc), VpeGetDevPhysPageHeight(hDoc));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "Printable:  W:%#5.2f H:%#5.2f \n", VpeGetDevPrintableWidth(hDoc), VpeGetDevPrintableHeight(hDoc));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "Paper    :  W:%#5.2f H:%#5.2f \n", VpeGetDevPaperWidth(hDoc), VpeGetDevPaperHeight(hDoc));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "Offset   :  X:%#5.2f Y:%#5.2f \n", VpeGetDevPrinterOffsetX(hDoc), VpeGetDevPrinterOffsetY(hDoc));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "MarginL: lx:%#5.2f ly:%#5.2f \n", VpeGet(hDoc, VLEFTMARGIN), VpeGet(hDoc, VTOPMARGIN));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "MarginR: lx:%#5.2f ly:%#5.2f \n", VpeGet(hDoc, VRIGHTMARGIN), VpeGet(hDoc, VBOTTOMMARGIN));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "Duplex: %d\n", VpeGetDevDuplex(hDoc));
	y = VpePrint(hDoc, 1, y, mMsg);
	sprintf(mMsg, "Color: %d\n", VpeGetDevColor(hDoc));
	y = VpePrint(hDoc, 1, y, mMsg);

	return 0;

}


#ifdef mitlincl



Hi!

I think VPE has a problem with frameless printing

print a box with :
VpeBox(hDoc, 1.00, 1.00, 20.00, 27.00);
VpeLine(hDoc, 1.00, 1.00, 20.00, 27.00);


If I change the printer setting to frameless(= Randlos)
on my try it with Epsonand HP Printer
VpeBox(hDoc, 1.00, 1.00, 20.00, 27.00);

sprintf(mMsg, "PhysPage :  W:%#5.2f H:%#5.2f \n", VpeGetDevPhysPageWidth(hDoc), VpeGetDevPhysPageHeight(hDoc));
y = VpePrint(hDoc, 1, y, mMsg);
sprintf(mMsg, "Printable:  W:%#5.2f H:%#5.2f \n", VpeGetDevPrintableWidth(hDoc), VpeGetDevPrintableHeight(hDoc));
y = VpePrint(hDoc, 1, y, mMsg);
sprintf(mMsg, "Paper    :  W:%#5.2f H:%#5.2f \n", VpeGetDevPaperWidth(hDoc), VpeGetDevPaperHeight(hDoc));
y = VpePrint(hDoc, 1, y, mMsg);
sprintf(mMsg, "Offset   :  X:%#5.2f Y:%#5.2f \n", VpeGetDevPrinterOffsetX(hDoc), VpeGetDevPrinterOffsetY(hDoc));



void PrintMee(VpeCoord PSize)
{
	char tline[1024];

	VpeSetPenSize(hDoc, PSize);
	VpeRenderWriteBox(hDoc, lastx, lasty, VFREE, VFREE, "M0XYyz");
	sprintf(tline, "Char.Height: %1.4f Box.Height:%2.4f  M0XYyz Pensize: %1.4f", czhoch, VpeGet(hDoc, VRENDERHEIGHT), PSize);
	lasty = VpeWriteBox(hDoc, lastx, lasty, VFREE, VFREE, tline);
	lasty += 0.2;
}

int LineRender()
{


	VpeCoord y;
	HCURSOR hOldCursor;

	lasty = 0; lastx = 0;
	globczhoch = 0;
	char tline[1024];

	wantbreak = false;

	if (hDoc) VpeCloseDoc(hDoc);

	hDoc = VpeOpenDoc(hMainWindow, "FlexPrint", mytbmode);
	Myreg();


	hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
	//  hDoc = VpeOpenDoc(hMainWindow, "FlexPrint", mytbmode );

	//  VpeSetPageScrollerTracking(hDoc, true);
	VpeSetMinScale(hDoc, 0.50);
	VpeDefineKey(hDoc, VKEY_CLOSE, VK_ESCAPE, 0, 0);
	VpeDefineKey(hDoc, VKEY_PAGE_FIRST, VK_ESCAPE, VK_HOME, 0);
	VpeDefineKey(hDoc, VKEY_PAGE_LAST, VK_ESCAPE, VK_END, 0);
	VpeDefineKey(hDoc, VKEY_PRINT, VK_F12, 0, 0);
	VpeSetPaperView(hDoc, true);

	bool inifile = false;

	VpeSetScalePercent(hDoc, 100);
	
	VpeCoord left = 2;

	VpeCoord top = 2;

	VpeCoord right = 18;

	goto nurf;

	VpeSetFont(hDoc, "Arial", 72);

	VpeCoord ascent = VpeGetFontAscent(hDoc);

	VpeCoord descent = VpeGetFontDescent(hDoc);

	VpeCoord cap_height = VpeGetCharacterHeight(hDoc, "M");



	// top

	VpeLine(hDoc, left, top, right, top);



	// cap height position

	VpeCoord cap_pos = ascent - cap_height;
	VpeSetPenColor(hDoc, COLOR_RED);

	VpeLine(hDoc, left, top + cap_pos, right, top + cap_pos);
	

	// baseline

	VpeCoord baseline = top + ascent;

	VpeSetPenColor(hDoc, COLOR_BLUE);

	VpeLine(hDoc, left, baseline, right, baseline);

	// bottom

	VpeLine(hDoc, left, top + ascent + descent, right, top + ascent + descent);

	// text

	VpePrint(hDoc, left, top, "Üg My Text.");

	VpeCoord x = nRight(hDoc);



	// add-on text, positioned at cap height of main text

	VpeSetFontSize(hDoc, 26);
	VpeCoord SmallCapHeight = VpeGetCharacterHeight(hDoc, "M");
	VpePrintBox(hDoc, x, baseline - cap_height - (VpeGetFontAscent(hDoc) - SmallCapHeight), "Test");

	nurf:


	VpeSetFont(hDoc, "Lucida Console", 10);  

 	cap_height = VpeGetCharacterHeight(hDoc, "X");

 	VpeRenderPrint(hDoc, 0, 0, "X");
		czbreit = VpeGet(hDoc, VRENDERWIDTH);
		czbreit = czbreit - 0.02;

		czhoch = VpeGet(hDoc, VRENDERHEIGHT);

		VpeSetDefOutRect(hDoc, VpeGet(hDoc, VLEFTMARGIN), VpeGet(hDoc, VTOPMARGIN), VpeGet(hDoc, VRIGHTMARGIN), VpeGet(hDoc, VBOTTOMMARGIN));

		globczoffs = 2.54 / 300;
		czhoch = cap_height ;
		VpeSetPenSize(hDoc, 2 * globczoffs);
		VpeSetPenColor(hDoc, COLOR_DKGRAY);
		VpeSetCornerRadius(hDoc, 0);



		lastx = 1;
		lasty = 1;
		globczhoch = czhoch + (8 * globczoffs);


		VpeRenderPrint(hDoc, 0, 0, "X0M");
		czhoch = VpeGet(hDoc, VRENDERHEIGHT);

		VpeSetPenColor(hDoc, COLOR_BLACK);
		VpeSetBkgGradientRotation(hDoc, 900);

		VpeSetBkgMode(hDoc, VBKG_GRD_RECT);

		VpeSetBkgGradientStartColor(hDoc, COLOR_LTGRAY);
		VpeSetBkgGradientEndColor(hDoc, COLOR_DKGRAY);

		lasty = VpeWrite(hDoc, lastx, lasty, -10, -1, "Lucida Console");
		VpeSetBkgMode(hDoc, VBKG_SOLID);


		lasty += 0.5;

		PrintMee(0);
		PrintMee(0.001);
		PrintMee(0.01);
		PrintMee(0.1);

		VpeSetFont(hDoc, "Arial", 10);
		VpeRenderPrint(hDoc, 0, 0, "X0M");
		czhoch = VpeGet(hDoc, VRENDERHEIGHT);
		VpeSetPenColor(hDoc, COLOR_BLUE);
		lasty = VpeWrite(hDoc, lastx, lasty, VFREE, VFREE, "Arial");
		lasty += 0.5;


		PrintMee(0);
		PrintMee(0.001);
		PrintMee(0.01);
		PrintMee(0.1);

		lasty = VpeWrite(hDoc, lastx, lasty, VFREE, VFREE, "Consolas");
		lasty += 0.5;

		VpeSetFont(hDoc, "Consolas", 10);
		VpeRenderPrint(hDoc, 0, 0, "X0M");
		czhoch = VpeGet(hDoc, VRENDERHEIGHT);
		VpeSetPenColor(hDoc, COLOR_RED);
		PrintMee(0);
		PrintMee(0.001);
		PrintMee(0.01);
		PrintMee(0.1);

	
		goto gende;

		VpeSetPenColor(hDoc, COLOR_BLUE);
		sprintf(tline, "czhoch: %1.4f lasty:%2.4f XY Nagy", czhoch, lasty);
		lasty = VpeWriteBox(hDoc, lastx, lasty, VFREE, VFREE, tline);
		lasty = lasty + czhoch;

		VpeSetFont(hDoc, "Consolas", 10);  // da bleibt B immer
		VpeSetBkgMode(hDoc, VBKG_SOLID);

		//	setmee("dos--!a43!fix.92", 0);
		//	if (zuerst) {
		//	Eincharp('0');
		//  	double wuzehoch;

		//VpeSetPenSize(hDoc,0);  
		//   VpeSetFont(hDoc,"Courier",10);  
		//VpeRenderPrint(hDoc,0,0,"X");
		//czbreit  = VpeGet(hDoc, VRENDERWIDTH); 
		// czbreit 0.231
		VpeRenderPrint(hDoc, 0, 0, "X");
		czbreit = VpeGet(hDoc, VRENDERWIDTH);
		czbreit = czbreit - 0.02;

		czhoch = VpeGet(hDoc, VRENDERHEIGHT);
		VpeSetDefOutRect(hDoc, VpeGet(hDoc, VLEFTMARGIN), VpeGet(hDoc, VTOPMARGIN), VpeGet(hDoc, VRIGHTMARGIN), VpeGet(hDoc, VBOTTOMMARGIN));

		globczoffs = 2.54 / 300;
		czhoch = czhoch ;
		VpeSetPenSize(hDoc, 2 * globczoffs);
		VpeSetPenColor(hDoc, COLOR_DKGRAY);
		VpeSetCornerRadius(hDoc, 0);



		lastx = 1;
		globczhoch = czhoch + (8 * globczoffs);

		sprintf(tline, "czhoch: %1.4f lasty:%2.4f XY Nagy", czhoch, lasty);
		VpeWriteBox(hDoc, lastx, lasty, VFREE, lasty + globczhoch, tline);
		lasty = lasty + czhoch;
		sprintf(tline, "czhoch: %1.4f lasty:%2.4f Nagy XY", czhoch, lasty);
		VpeWriteBox(hDoc, lastx, lasty, VFREE, lasty + 2 * globczhoch, tline);
		lasty = lasty + czhoch;
		sprintf(tline, "czhoch: %1.4f lasty:%2.4f XY Nagy", czhoch, lasty);
		VpeWriteBox(hDoc, lastx, lasty, VFREE, lasty + globczhoch, tline);
		lasty = lasty + czhoch;

		sprintf(tline, "czhoch: %1.4f lasty:%2.4f XY Nagy", czhoch, lasty);
		lasty = VpeWriteBox(hDoc, lastx, lasty, VFREE, VFREE, tline);
		lasty = lasty + 2 * globczoffs;


		VpeSetPenColor(hDoc, COLOR_RED);
		sprintf(tline, "czhoch: %1.4f lasty:%2.4f Nagy XY", czhoch, lasty);
		lasty = VpeWriteBox(hDoc, lastx, lasty, VFREE, VFREE, tline);
		lasty = lasty + 2 * globczoffs;

		VpeSetPenColor(hDoc, COLOR_BLUE);
		sprintf(tline, "czhoch: %1.4f lasty:%2.4f XY Nagy", czhoch, lasty);
		lasty = VpeWriteBox(hDoc, lastx, lasty, VFREE, VFREE, tline);
		lasty = lasty + czhoch;

		gende:


		VpePreviewDoc(hDoc, NULL, VPE_SHOW_MAXIMIZED);


	return 0;

}

#else

int LineRender()
{


	
	return 1;
}

#endif
