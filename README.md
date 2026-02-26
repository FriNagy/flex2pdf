# F2P (Flex2PDF)

A Windows command-line tool for generating PDF documents from template files using the Virtual Print Engine (VPE) library.

**Author:** Frigyes Nagy
**Created:** June 2005
**Latest Version:** 25

## Features

- PDF generation from `.f2p` template files
- Rich Text Format (RTF) support
- 2D barcode generation
- Clickable objects and charts
- Duplex printing support
- Direct PDF output or print preview
- Clipboard input processing
- Email integration

## Requirements

- Windows 10 or later
- Visual Studio 2017+ (for building)
- VPE Library DLL (`vpep3274.dll` for x64, `vpep3273.dll` for Win32)

## Building

Open `f2p.sln` in Visual Studio and build, or use MSBuild:

```bash
# Release x64
msbuild f2p.sln /p:Configuration=Release /p:Platform=x64

# Release Win32
msbuild f2p.sln /p:Configuration=Release /p:Platform=Win32
```

The compiled executable is placed in `.\Release\F2P.exe`.

## Usage

```bash
F2P.exe <input.f2p> [options]
```

### Examples

```bash
# Basic PDF generation
F2P.exe document.f2p

# With license key
F2P.exe document.f2p A64-03463_45
```

### Input Files

Template files use the `.f2p` extension. Example templates are provided in the `info/` directory:
- `bgexample.f2p` - Background example
- `Dialog1.f2p` - Dialog template
- `test.f2p` - Test template

### Output

PDF files are generated with the naming pattern `F2P_MMDD_HHMM_###.pdf` by default, or a custom path can be specified via parameters.

## Project Structure

```
f2p/
├── f2p.cpp           # Main application source
├── f2p.sln           # Visual Studio solution
├── F2P.vcxproj       # Project file
├── vpiface.h         # VPE interface headers
├── vppiface.h        # VPE print interface
├── vpecomon.h        # VPE common definitions
├── colors.h          # Color definitions
├── resource.h        # Resource IDs
├── LineRender.h      # Line rendering
├── windproc.h        # Window procedure
├── flex2pdf.RC       # Resources
├── info/             # Example templates
└── Release/          # Build output
    ├── F2P.exe
    └── vpep3274.dll
```

## License

This project uses the Virtual Print Engine (VPE) library from IDEAL Software.
