===============================================================================
 F2P (Flex2PDF / FlexPrint) - Referenz
===============================================================================

 Windows Kommandozeilen-Tool zur PDF-Erzeugung und Druckausgabe
 Basis: VPE Library (Virtual Print Engine) von IDEAL Software
 Build: Visual Studio 2026, Release x64

===============================================================================
 KOMMANDOZEILE
===============================================================================

 F2P.exe <eingabe.f2p> [serial] [schalter...]

 Erster Parameter ist immer die Eingabedatei (.f2p, .a2p, .rtf).
 Alle weiteren Parameter beginnen mit - oder /


 Schalter          Beschreibung
 -----------------------------------------------------------------------
 -p:<name.prs>     Drucker-Setup Datei (PRS)
 -k:<setupstr>     Setup-String
 -f:<name>          Ausgabe PDF-Dateiname
 -i:<info>          Info-String
 -s:<verz>          Set-Verzeichnis (Pfad fuer Includes, Bitmaps etc.)
 -h                 Pfade relativ zu HOME-Umgebungsvariable
 -w:<n>             Fenster-Modus (0=unsichtbar, 1=normal, 2=default)
 -t:<n>             Toolbar-Modus (0=ohne, 1=mit Grid)
 -z:<n>             Zoom in Prozent (z.B. -z:100)
 -m:<n>             Menue-Modus
 -v                 Papier-Vorschau
 -g                 Ans Ende springen
 -n                 Nicht speichern
 -u                 Clipboard-Update
 -c                 Aus Clipboard lesen
 -o:<modus>         Direkt-Ausgabe (p=PDF, r=Drucker etc.), ov=mit Vorschau
 -x:<modus>         Shell-Execute Modus
 -q:<modus>         Quick-PDF Modus
 -a:<modus><name>   Archiv/Direkt-PDF (ap=PDF, as=A2P etc.)
 -y:<rect>          Seitenbereich berechnen
 -2                 F2T-Modus
 -dtime:<n>         Debug-Timing: WARNUNG wenn VPE-Aufruf > n Sekunden
                    (ohne -dtime = keine Warnungen)
                    Beispiel: -dtime:2 = Warnung ab 2 Sekunden


 SONDERBEFEHLE (ersetzen die Eingabedatei als 1. Parameter):
 -----------------------------------------------------------------------
 -cash_up[:<port>] [port]
                    Kassenladen oeffnen und F2P sofort beenden.
                    BREAK-Puls + RTS/DTR-Signal auf COM-Port.
                    Default Port: COM3
                    Beispiele:
                      F2P.exe -cash_up:com4
                      F2P.exe -cash_up com4:
                      F2P.exe -cash_up            (-> COM3)

 -cash_anz[:<port>] ["text1"] ["text2"]
                    Kundenanzeige (Bixolon BCD-2000) ansteuern und beenden.
                    Display wird geloescht, optional Text angezeigt.
                    Port-Ermittlung: Cmdline > ENV CASH_ANZ_PORT >
                                     cash_anz.ini > Default COM4
                    Beispiele:
                      F2P.exe -cash_anz:com4 "Willkommen" "Bitte warten"
                      F2P.exe -cash_anz:com4 5       (Block B5 aus INI)
                      F2P.exe -cash_anz:com4          (nur Display loeschen)
                      F2P.exe -cash_anz "Hallo"       (Port aus ENV/INI)


===============================================================================
 SET-PARAMETER (in F2P-Template-Dateien)
===============================================================================

 Format: [=set,<steuerzeichen>,<wert>]

 Steuerzeichen   Beschreibung
 -----------------------------------------------------------------------
 d               Verzeichnis fuer Export setzen
 f               Dateiname fuer Export setzen
 t               Dateityp (Extension ohne Punkt)
 x / X           Programm sofort ausfuehren (x=mit Warten, X=ohne Warten)
 a / A           Kundenanzeige (cash_anz) sofort ansteuern
                 Format: [=set,A,port,text1,text2]
                 Beispiele:
                   [=set,A,com4,Willkommen,Bitte warten]
                   [=set,A,com4,Betrag: 12.50]
                   [=set,A,com4,1]                (Block B1 aus cash_anz.ini)
                   [=set,A,com4]                  (nur Display loeschen)
                   [=set,A,,Hallo]                (Port aus ENV/INI/Default)
 u / U           Kassenladen (cash_up) sofort oeffnen
                 Format: [=set,U,port]
                 Beispiele:
                   [=set,U,COM4]
                   [=set,U,COM3]
                 Default: COM3

 1..4            End-Programm Slot (nach Rendering ausgefuehrt):
                   p/P  = Drucken (p=ohne Dialog, P=mit Dialog)
                   e/E  = Export (e=direkt, E=mit Rueckfrage)
                   r/R  = Programm ausfuehren (r=mit Warten, R=ohne)


===============================================================================
 CASH_ANZ.INI (neben F2P.exe)
===============================================================================

 Optionale Konfigurationsdatei fuer die Kundenanzeige.

 # Port-Definition
 PORT=COM4

 # Text-Bloecke (aufrufbar per Blocknummer)
 B1=Willkommen|Bitte warten
 B2=Vielen Dank|Auf Wiedersehen
 B5=Sonderangebot|Heute -20%

 Aufruf per Blocknummer:
   F2P.exe -cash_anz:com4 1      -> zeigt Block B1
   [=set,A,com4,1]               -> zeigt Block B1 (wie Cmdline)


===============================================================================
 INTERNE FUNKTIONEN (Referenz fuer Entwickler)
===============================================================================

 cash_up - Kassenladen oeffnen:
   OpenDrawer_BREAK_Pulse(port, breakMs, pulseMs)
     - Oeffnet COM-Port, sendet BREAK-Puls + RTS/DTR-Signal
     - breakMs: BREAK-Dauer (Standard 250ms)
     - pulseMs: RTS/DTR-Puls (Standard 200ms)
     - Aufruf von: Cmdline (-cash_up), ssetpar ([=set,U,port])
     - Fehlermeldung: MessageBox bei Port nicht erreichbar

 cash_anz - Kundenanzeige:
   CashAnz_Run(cmdPort, text1, text2)
     - Zentrale Funktion, aufgerufen von Cmdline und SET-Parameter
     - Port-Kaskade: cmdPort > ENV > INI > Default COM4
     - Sendet ESC @ (Init) + FormFeed (Clear) + Text
     - Fehlermeldung: MessageBox bei Port nicht erreichbar

   CashAnz_OpenCom(port, handle)  - COM-Port oeffnen (9600,8,N,1)
   CashAnz_ClearDisplay(handle)   - Display loeschen (ESC @ + FF)
   CashAnz_WriteAll(handle, data)  - Bytes senden
   CashAnz_ReadIniPort(ini, buf)   - PORT= aus INI lesen
   CashAnz_ReadIniBlock(ini, n)    - B<n>= Block aus INI lesen
   CashAnz_GetPort(cmd, buf)       - Port-Kaskade auswerten

 Debug-Timing:
   Globale Variable: dbg_time_threshold (0=aus, sonst Sekunden)
   5 ueberwachte Stellen: VpeSetDevice, VpeSetupPrinter,
                           VpeGetDev*, VpeOpenDoc, setmyprs
   Aktivierung nur per -dtime:N auf der Kommandozeile


===============================================================================
 FEHLERBEHANDLUNG
===============================================================================

 COM-Port nicht erreichbar:
   -> MessageBox mit Portname und Windows-Fehlernummer
   -> Exit-Code 2 (bei Cmdline-Aufruf)

 Eingabedatei nicht gefunden:
   -> MessageBox "Fehler bei Oeffnen!"
   -> Exit-Code 7

 Drucker-Setup fehlerhaft:
   -> MessageBox "Setup Datei fehlerhaft"

 VPE-Aufruf zu langsam (nur mit -dtime:N):
   -> MessageBox "WARNUNG: <Funktion> dauerte X.X Sek!"


===============================================================================
