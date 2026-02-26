# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

F2P (Flex2PDF/FlexPrint) is a Windows command-line PDF generation tool written in C++. It converts documents to PDF format using the Virtual Print Engine (VPE) library from IDEAL Software.

- **Language:** C++ (native Windows API)
- **Target:** Windows 10.0+ (Win32 and x64)
- **Core Dependency:** VPE Library (vpep3274.lib for x64, vpep3273.lib for Win32)

## Build Commands

Visual Studio 2026 ist installiert in: `C:\Program Files\Microsoft Visual Studio\18\Community\`

```bash
# MSBuild Pfad
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

# Release x64 (recommended)
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" f2p.sln /p:Configuration=Release /p:Platform=x64

# Release Win32
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" f2p.sln /p:Configuration=Release /p:Platform=Win32

# Debug x64
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" f2p.sln /p:Configuration=Debug /p:Platform=x64
```

Output executables are placed in `.\Release\F2P.exe` with post-build copy to `c:\vdf\f2p` and `c:\dev\f2p`.

## Architecture

### Single-File Design
The application is a monolithic design with `f2p.cpp` (~8,100 lines) containing the main application logic.

### Key Entry Points
- `WinMain()` (line 7439) - Application entry point
- `WndProc()` - Windows message processor (in windproc.h)
- `AutoRender()` (line 4756) - Main document rendering pipeline
- `SComLine()` (line 7039) - Command-line argument parsing

### Core Modules
- **VPE Integration:** `vpiface.h`, `vppiface.h`, `vpecomon.h` - Document creation and rendering
- **Line Rendering:** `LineRender.h` - Drawing and text positioning
- **Resources:** `resource.h`, `colors.h`, `flex2pdf.RC` - UI resources and color definitions
- **Window Handling:** `windproc.h` - Message processing

### Key Functions
| Function | Purpose |
|----------|---------|
| `VpeOpenDoc()` / `VpeCloseDoc()` | Document lifecycle |
| `VpeWrite()`, `VpeWriteBox()` | Text rendering |
| `ExportDocument()` (line 1367) | PDF export |
| `PrintRender()`, `Print_ganz_Teil()` | Print operations |
| `spdfpar()`, `ssetpar()`, `smailpar()` | Parameter handlers |

### Global State
Key globals in f2p.cpp:
- `VpeHandle hDoc` - Current document handle
- `HWND hMainWindow` - Main window handle
- `char file2print[125]` - Input file path
- `char ufname[99]` - Output PDF filename
- `int myDuplex` - Duplex printing mode
- `BOOL directpdf` - Direct PDF generation flag

## Command-Line Usage

```bash
F2P.exe <input.f2p> [serial_key] [additional_params]
```

- Input files are `.f2p` template files (examples in `info/` directory)
- Magic parameter marker `[=)` indicates start of F2P control parameters
- Also accepts RTF files and clipboard input

## Coding Conventions

- Fixed-size char arrays for strings (no STL)
- ANSI string functions (`strcpy`, `strcat`, `sprintf`)
- Path manipulation via `_splitpath()`, `_makepath()`
- VPE handles managed by the VPE DLL
- Control characters (< 14) used for line parsing
- Comments in German/English mix
