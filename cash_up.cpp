#include <windows.h>
#include <stdio.h>
#include <string.h>

static void PrintErr(const char* what) {
    printf("%s failed: %lu\n", what, GetLastError());
}

static bool OpenDrawer_BREAK_Pulse(const char* portName, DWORD breakMs, DWORD pulseMs)
{
    char full[64];
    snprintf(full, sizeof(full), "\\\\.\\%s", portName);

    HANDLE hCom = CreateFileA(full, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hCom == INVALID_HANDLE_VALUE) { PrintErr("CreateFile"); return false; }

    // Handshake aus, definierter State
    DCB dcb{}; dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(hCom, &dcb)) { PrintErr("GetCommState"); CloseHandle(hCom); return false; }

    dcb.BaudRate = CBR_9600;  // meist egal, aber definieren
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (!SetCommState(hCom, &dcb)) { PrintErr("SetCommState"); CloseHandle(hCom); return false; }

    PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    // Startzustand
    EscapeCommFunction(hCom, CLRRTS);
    EscapeCommFunction(hCom, CLRDTR);
    ClearCommBreak(hCom);
    Sleep(30);

    // 1) BREAK setzen
    if (!SetCommBreak(hCom)) PrintErr("SetCommBreak");
    Sleep(breakMs);

    // 2) BREAK l�sen
    if (!ClearCommBreak(hCom)) PrintErr("ClearCommBreak");
    Sleep(20);

    // 3) Leitungs-Puls (hier beide; wenn du willst nur RTS oder nur DTR)
    if (!EscapeCommFunction(hCom, SETRTS)) PrintErr("SETRTS");
    if (!EscapeCommFunction(hCom, SETDTR)) PrintErr("SETDTR");
    Sleep(pulseMs);
    EscapeCommFunction(hCom, CLRRTS);
    EscapeCommFunction(hCom, CLRDTR);

    CloseHandle(hCom);
    return true;
}

static void ShowHelp(const char* exe)
{
    printf("cash_up - Kassenladen oeffnen via COM-Port\n\n");
    printf("Syntax: %s [port] [break_ms] [pulse_ms]\n\n", exe);
    printf("Parameter:\n");
    printf("  port      COM-Port (Standard: COM3)\n");
    printf("  break_ms  BREAK-Dauer in ms (Standard: 250)\n");
    printf("  pulse_ms  RTS/DTR-Puls in ms (Standard: 200)\n\n");
    printf("Beispiele:\n");
    printf("  %s              -> COM3, 250ms, 200ms\n", exe);
    printf("  %s COM4         -> COM4, 250ms, 200ms\n", exe);
    printf("  %s COM4 300 150 -> COM4, 300ms, 150ms\n", exe);
}

int main(int argc, char** argv)
{
    if (argc >= 2 && (strcmp(argv[1], "-?") == 0 || strcmp(argv[1], "/?") == 0)) {
        ShowHelp(argv[0]);
        return 0;
    }

    const char* port = (argc >= 2) ? argv[1] : "COM3";
    DWORD breakMs = (argc >= 3) ? (DWORD)strtoul(argv[2], nullptr, 10) : 250;
    DWORD pulseMs = (argc >= 4) ? (DWORD)strtoul(argv[3], nullptr, 10) : 200;

    bool ok = OpenDrawer_BREAK_Pulse(port, breakMs, pulseMs);
    printf(ok ? "OK\n" : "FAIL\n");
    return ok ? 0 : 1;
}
