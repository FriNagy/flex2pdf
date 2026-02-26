// cash_anz.cpp - Bixolon BCD-2000 via FTDI Virtual COM
// Usage:
//   cash_anz.exe                -> clear display
//   cash_anz.exe "text1"        -> clear + show text1 on line1
//   cash_anz.exe "text1" "text2"-> clear + show 2 lines
//
// COM port selection (highest priority first):
//   1) Environment variable CASH_ANZ_PORT (e.g. COM4)
//   2) cash_anz.ini next to exe: line like  PORT=COM4
//   3) default COM4

#include <windows.h>
#include <cstdio>
#include <string>
#include <vector>

static std::string Trim(std::string s) {
    auto issp = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (!s.empty() && issp((unsigned char)s.front())) s.erase(s.begin());
    while (!s.empty() && issp((unsigned char)s.back())) s.pop_back();
    return s;
}

static std::string GetExeDir() {
    char path[MAX_PATH]{};
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string p(path);
    size_t pos = p.find_last_of("\\/");
    return (pos == std::string::npos) ? std::string(".") : p.substr(0, pos);
}

static bool IsDigitsOnly(const std::string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s) if (c < '0' || c > '9') return false;
    return true;
}

static bool ReadIniBlock(const std::string& exeDir, int n, std::string& out1, std::string& out2) {
    std::string iniPath = exeDir + "\\cash_anz.ini";
    FILE* f = nullptr;
    fopen_s(&f, iniPath.c_str(), "rb");
    if (!f) return false;

    char line[1024];
    std::string key = "B" + std::to_string(n);

    bool found = false;
    while (fgets(line, sizeof(line), f)) {
        std::string s = Trim(line);
        if (s.empty()) continue;
        if (s[0] == '#' || s[0] == ';') continue;

        auto eq = s.find('=');
        if (eq == std::string::npos) continue;

        std::string k = Trim(s.substr(0, eq));
        std::string v = Trim(s.substr(eq + 1));

        for (auto& c : k) c = (char)toupper((unsigned char)c);
        std::string keyUp = key;
        for (auto& c : keyUp) c = (char)toupper((unsigned char)c);

        if (k == keyUp) {
            // split v by '|'
            auto bar = v.find('|');
            if (bar == std::string::npos) {
                out1 = v;
                out2.clear();
            }
            else {
                out1 = Trim(v.substr(0, bar));
                out2 = Trim(v.substr(bar + 1));
            }
            found = true;
            break;
        }
    }
    fclose(f);
    return found;
}


static std::string ReadIniPort(const std::string& exeDir) {
    std::string iniPath = exeDir + "\\cash_anz.ini";
    FILE* f = nullptr;
    fopen_s(&f, iniPath.c_str(), "rb");
    if (!f) return {};

    char line[512];
    std::string port;
    while (fgets(line, sizeof(line), f)) {
        std::string s = Trim(line);
        if (s.empty()) continue;
        if (s[0] == '#' || s[0] == ';') continue;

        // Accept: PORT=COM4  or  COM4
        auto eq = s.find('=');
        if (eq != std::string::npos) {
            std::string key = Trim(s.substr(0, eq));
            std::string val = Trim(s.substr(eq + 1));
            for (auto& c : key) c = (char)toupper((unsigned char)c);
            if (key == "PORT") { port = val; break; }
        }
        else {
            port = s;
            break;
        }
    }
    fclose(f);
    return Trim(port);
}

static std::string GetPortName() {
    // 1) env var
    char buf[128]{};
    DWORD n = GetEnvironmentVariableA("CASH_ANZ_PORT", buf, (DWORD)sizeof(buf));
    if (n > 0 && n < sizeof(buf)) {
        std::string p = Trim(buf);
        if (!p.empty()) return p;
    }

    // 2) ini file next to exe
    std::string exeDir = GetExeDir();
    std::string iniPort = ReadIniPort(exeDir);
    if (!iniPort.empty()) return iniPort;

    // 3) default
    return "COM4";
}

static bool WriteAll(HANDLE h, const void* data, DWORD len) {
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

static bool OpenCom(const std::string& port, HANDLE& out) {
    char full[64];
    // Always use \\.\COMx form for safety (COM10+)
    snprintf(full, sizeof(full), "\\\\.\\%s", port.c_str());

    HANDLE h = CreateFileA(full, GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return false;

    DCB dcb{};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(h, &dcb)) { CloseHandle(h); return false; }

    // Typical defaults; if your BCD DIP settings differ, adjust here.
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

    COMMTIMEOUTS to{};
    to.WriteTotalTimeoutConstant = 150;
    to.WriteTotalTimeoutMultiplier = 0;
    SetCommTimeouts(h, &to);

    PurgeComm(h, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    out = h;
    return true;
}
static void ClearDisplay(HANDLE h) {
    const BYTE initCmd[2] = { 0x1B, 0x40 }; // ESC @
    WriteAll(h, initCmd, 2);
    Sleep(30);

    const BYTE ff = 0x0C;                  // FormFeed (clear)
    WriteAll(h, &ff, 1);
    Sleep(50);
}

int main(int argc, char** argv) {
    std::string text1, text2;

    if (argc == 1) {
        // clear only
    }
    else if (argc == 2) {
        std::string a = argv[1];
        if (IsDigitsOnly(a)) {
            int n = atoi(a.c_str());
            ReadIniBlock(GetExeDir(), n, text1, text2); // wenn nicht gefunden -> bleibt leer (-> nur clear)
        }
        else {
            text1 = a;
            text2.clear();
        }
    }
    else {
        text1 = argv[1];
        text2 = argv[2];
    }



    std::string port = GetPortName();

    HANDLE hCom = NULL;
    if (!OpenCom(port, hCom)) {
        // Return non-zero so DataFlex can detect failure if needed
        // (You can also log to a file here.)
        return 2;
    }

    ClearDisplay(hCom);

    // If no text provided -> pure clear
    if (!text1.empty() || !text2.empty()) {
        std::string msg;
        msg.reserve(text1.size() + text2.size() + 8);
        msg += text1;
        msg += "\r\n";
        msg += text2;
        msg += "\r\n";
        WriteAll(hCom, msg.data(), (DWORD)msg.size());
    }

    CloseHandle(hCom);
    return 0;
}
