# TTI and EP1 Thumbnail Provider for Windows

A Windows Shell Extension that automatically generates thumbnail previews for Teletext files (TTI and EP1 formats) in Windows Explorer.

## Overview

This thumbnail provider reads TTI and EP1 teletext files and renders them as thumbnail icons in Windows Explorer. The renderer parses both text-based TTI format and binary EP1 format, interprets teletext control codes, and generates bitmap images showing the teletext pages with proper colors, graphics, and formatting.

## Features

- **Automatic thumbnail generation** for .tti and .ep1 files in Windows Explorer
- **Multiple format support**:
  - **TTI format**: Text-based teletext intermediate format with escaped control codes
  - **EP1 format**: Binary teletext format (Edit.tf/wxTED/Flair32 format, 968 bytes)
- **Teletext format support**:
  - Standard 40×25 character teletext pages (rows 1-24 displayed)
  - Full color palette (8 colors: black, red, green, yellow, blue, magenta, cyan, white)
  - Alphanumeric and graphics modes
  - Control codes (color selection, background color, graphics modes)
  - Block graphics characters (contiguous and separated)
  - Double height character rendering
  - National character set support (13 languages including English, French, German, Swedish, Italian, Portuguese, Czech, Polish, Turkish, and more)
- **Native Windows integration** using COM-based thumbnail handler
- **Independent implementation** - no dependencies on wxTED or other teletext applications

## Requirements

- Windows 7 or later (tested on Windows 10/11)
- Visual Studio 2019 or later (for building from source)
- Administrator privileges (for installation)
- **teletext2.ttf font** (optional but recommended for proper graphics rendering)

### Font Installation

The renderer can use teletext fonts for accurate display:

**teletext2.ttf** - Normal height characters and graphics
- **Contiguous graphics** (solid blocks): 
  - Characters 0x20-0x3F → U+E680-E69F
  - Characters 0x60-0x7F → U+E6C0-E6DF
- **Separated graphics** (blocks with gaps): 
  - Characters 0x20-0x3F → U+E6A0-E6BF
  - Characters 0x60-0x7F → U+E6E0-E6FF
- **Blast-through range**: Characters 0x40-0x5F display as alphanumeric characters (not graphics)

**teletext4.ttf** - Double height characters
- Contains the same glyphs as teletext2.ttf but designed for double height rendering
- Used when double height control code (0x0D) is active

**Font Loading (Multiple Locations):**

The renderer searches for fonts in this order:
1. **DLL directory** (e.g., `C:\Program Files\wxTED\`) - checked first
2. **Windows Fonts folder** (e.g., `C:\Windows\Fonts\`) - checked if not found in DLL directory

This allows fonts to be:
- Installed system-wide for use by other applications (via Windows Fonts folder)
- OR kept private with the DLL (via DLL directory)
- The renderer works with either location

**Installation Options:**

**Option 1 - System-Wide (Recommended for wxTED):**
- Install fonts to `C:\Windows\Fonts\`
- Fonts available to all applications
- Used by wxTED editor and thumbnail provider

**Option 2 - Private Installation:**
- Place fonts in same directory as `TTIThumbnailProvider.dll`
- Fonts only used by thumbnail provider
- More portable, easier to uninstall

**Note:** The fonts are loaded temporarily for each thumbnail generation and do not need to be "active" in the system for thumbnails to work.

## Building from Source

### Using Visual Studio

1. Open `TTIThumbnailProvider.vcxproj` in Visual Studio
2. Select your desired configuration (Debug/Release) and platform (Win32/x64)
3. Build the solution (Build → Build Solution or press F7)
4. The DLL will be created in the Debug or Release folder

### Project Structure

```
TTIThumbnailProvider/
├── TTIThumbnailProvider.h        # Main thumbnail provider interface
├── TTIThumbnailProvider.cpp      # Thumbnail provider implementation
├── TeletextRenderer.h            # Teletext parser and renderer interface
├── TeletextRenderer.cpp          # Teletext parsing and rendering logic
├── DllMain.cpp                   # DLL entry point and COM registration
├── TTIThumbnailProvider.def      # Module definition file
├── TTIThumbnailProvider.vcxproj  # Visual Studio project file
├── Install.bat                   # Installation script
├── Uninstall.bat                 # Uninstallation script
└── README.md                     # This file
```

## Installation

### Method 1: Using wxTED Installer (Easiest - Recommended)

The simplest way to install the TTI/EP1 thumbnail provider is through the **wxTED installer**:

1. Download wxTED from [github.com/peterkvt80/wxted](https://github.com/peterkvt80/wxted)
2. Run the wxTED installer (setup.exe)
3. During installation, select the **"TTI Thumbnail Provider"** component
4. The installer will automatically:
   - Install the thumbnail provider DLL
   - Install the teletext2.ttf and teletext4.ttf fonts
   - Register the thumbnail handler for .tti and .ep1 files
   - Configure Windows Explorer integration

**Advantages:**
- ✅ Fully automated installation
- ✅ Includes wxTED editor for creating and editing teletext files
- ✅ Fonts and DLL installed to correct locations
- ✅ Proper Windows registration and integration
- ✅ Easy uninstallation through Windows Settings

### Method 2: Manual Installation Using Script

1. Build the project to create `TTIThumbnailProvider.dll`
2. Copy the following files to a permanent location:
   - `TTIThumbnailProvider.dll`
   - `Install.bat`
   - `teletext2.ttf` (optional - can be in DLL directory OR Windows Fonts folder)
   - `teletext4.ttf` (optional - can be in DLL directory OR Windows Fonts folder)
   - **Important**: Do not delete the DLL after installation - Windows needs it to generate thumbnails
3. Right-click `Install.bat` and select "Run as administrator"
4. The script will:
   - Register the thumbnail provider
   - Clear the thumbnail cache
   - Restart Windows Explorer

**Font Installation:**
- **Option A**: Copy `teletext2.ttf` and `teletext4.ttf` to the same directory as the DLL
- **Option B**: Install fonts system-wide to `C:\Windows\Fonts\` (right-click font → Install)
- **Option C**: Both (fonts in both locations work fine)
- If fonts are missing from both locations, thumbnails will use Courier New (no graphics)

### Method 3: Manual Registration (Advanced Users)

1. Open Command Prompt as Administrator
2. Navigate to the directory containing the DLL
3. Run: `regsvr32 TTIThumbnailProvider.dll`
4. Clear thumbnail cache: `del /f /s /q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db"`
5. Restart Windows Explorer

## Uninstallation

### Using wxTED Uninstaller (Recommended)

If you installed via wxTED installer:
1. Open Windows Settings → Apps
2. Find "wxTED" in the list
3. Click Uninstall
4. The thumbnail provider will be automatically removed along with wxTED

### Using the Uninstallation Script

1. Right-click `Uninstall.bat` and select "Run as administrator"
2. The script will unregister the handler and clean up

### Manual Uninstallation

1. Open Command Prompt as Administrator
2. Navigate to the directory containing the DLL
3. Run: `regsvr32 /u TTIThumbnailProvider.dll`

## TTI File Format Support

The renderer supports the standard TTI (Teletext Intermediate) format with proper 7-bit encoding.

### TTI Format Encoding Rules

1. **7-bit data**: All characters use 7-bit encoding (parity bit is set to 0)
2. **Control character encoding**: Characters below 0x20 are encoded as:
   - ESC character (0x1B, byte value 27)
   - Followed by the control code + 0x40
   - Example: Control code 0x01 (Red) → bytes `1B 41` (ESC + 'A')
3. **Row format**: `OL,<row>,<40 characters>` where row is 0-24
4. **Missing rows**: Rows not present in the file are treated as 40 spaces
5. **Thumbnail rendering**: Only rows 1-23 are displayed (row 0 is header, row 24 is footer)

**Note:** In text editors, ESC (0x1B) may display as `[`, `^[`, or not at all, but the actual byte in the file is always 0x1B.

### Supported Commands

- `PN,` - Page Number (marks page boundaries)
- `OL,<row>,<data>` - Output Line with row number and character data
- `FL,<row>,<data>` - Fastext Line (treated as output line)

### Supported Control Codes

Control codes in OL lines are encoded as ESC (0x1B) + letter:

- ESC + `@` (0x1B 0x40) - Black alphanumeric (0x00) *extended*
- ESC + `A` (0x1B 0x41) - Red alphanumeric (0x01)
- ESC + `B` (0x1B 0x42) - Green alphanumeric (0x02)
- ESC + `C` (0x1B 0x43) - Yellow alphanumeric (0x03)
- ESC + `D` (0x1B 0x44) - Blue alphanumeric (0x04)
- ESC + `E` (0x1B 0x45) - Magenta alphanumeric (0x05)
- ESC + `F` (0x1B 0x46) - Cyan alphanumeric (0x06)
- ESC + `G` (0x1B 0x47) - White alphanumeric (0x07)
- ESC + `P` (0x1B 0x50) - Black graphics (0x10) *extended*
- ESC + `Q` (0x1B 0x51) - Red graphics (0x11)
- ESC + `R` (0x1B 0x52) - Green graphics (0x12)
- ESC + `S` (0x1B 0x53) - Yellow graphics (0x13)
- ESC + `T` (0x1B 0x54) - Blue graphics (0x14)
- ESC + `U` (0x1B 0x55) - Magenta graphics (0x15)
- ESC + `V` (0x1B 0x56) - Cyan graphics (0x16)
- ESC + `W` (0x1B 0x57) - White graphics (0x17)
- ESC + `Y` (0x1B 0x59) - Contiguous graphics (0x19)
- ESC + `Z` (0x1B 0x5A) - Separated graphics (0x1A)
- ESC + `\` (0x1B 0x5C) - Black background (0x1C)
- ESC + `]` (0x1B 0x5D) - New background (0x1D)
- ESC + `L` (0x1B 0x4C) - Normal height (0x0C)
- ESC + `M` (0x1B 0x4D) - Double height (0x0D)

**Note:** Black (0x00/0x10) is not part of the original teletext standard but is supported by most modern devices.

### Character Support

- Standard ASCII characters
- Teletext block graphics (automatically converted to Unicode glyphs in teletext2.ttf)
- Proper handling of graphics/alphanumeric mode switching
- Special character mappings:
  - `~` (0x7E) → U+00F7 (division sign ÷)
  - 0x7F → U+E65F (special teletext character in teletext2.ttf)

## Technical Details

### COM Implementation

The thumbnail provider implements these COM interfaces:
- `IInitializeWithStream` - Receives the file stream
- `IThumbnailProvider` - Generates the thumbnail bitmap
- `IClassFactory` - Creates provider instances

### Rendering Process

**For TTI files:**
1. **Stream Reading**: Reads the entire TTI file into memory
2. **Parsing**: Parses TTI format line by line
   - Strips parity bit (bit 7) from all characters
   - Decodes ESC sequences: 0x1B followed by (code + 0x40)
   - Parses PS command to extract national character set
   - Handles header row special case (8 leading space positions on row 0)
   - Processes control codes for colors and modes
3. **Page Building**: Constructs a 40×25 character grid with color attributes
4. **Rendering**: Converts the character grid to a bitmap with proper scaling
5. **Return**: Provides the HBITMAP to Windows Explorer

**For EP1 files:**
1. **Stream Reading**: Reads the fixed 968-byte EP1 file
2. **Validation**: Verifies header signature (FE 01 09)
3. **Parsing**: Reads 24 rows × 40 characters (960 bytes)
   - Strips parity bit (bit 7) from all characters
   - Control codes stored as raw bytes (no ESC sequences)
   - ESC character (0x1B) replaced with space (0x20)
4. **Page Building**: Constructs a 40×25 character grid (row 0 initialized to spaces)
5. **Rendering**: Converts the character grid to a bitmap with proper scaling
6. **Return**: Provides the HBITMAP to Windows Explorer

### Graphics Character Encoding

Graphics characters use specific code ranges to encode a 2×3 pixel grid:

**Code Ranges:**
- **0x20-0x3F**: Graphics patterns 0x00-0x1F (uses bits 0-4)
- **0x40-0x5F**: Blast-through range (displays as alphanumeric characters, not graphics)
- **0x60-0x7F**: Graphics patterns 0x20-0x3F (uses bits 0-5)

**Bit Pattern (2×3 grid):**
```
Bit:  5 4 3 2 1 0
Pos:  ┌─┬─┐
      │0│1│  (top row)
      ├─┼─┤
      │2│3│  (middle row)
      ├─┼─┤
      │4│5│  (bottom row)
      └─┴─┘
```

For example:
- 0x20 (bits 00000) = empty/space
- 0x3F (bits 11111) = left column + top-right
- 0x60 (bits 100000) = bottom-right only
- 0x7F (bits 111111) = full block

The renderer maps these patterns to Unicode glyphs in teletext2.ttf:
- **Contiguous graphics** (solid blocks): 
  - Characters 0x20-0x3F → U+E680-E69F
  - Characters 0x60-0x7F → U+E6C0-E6DF
- **Separated graphics** (blocks with gaps): 
  - Characters 0x20-0x3F → U+E6A0-E6BF
  - Characters 0x60-0x7F → U+E6E0-E6FF
- **Blast-through** (0x40-0x5F): Displays as regular alphanumeric characters

### Color Palette

The renderer uses the standard teletext color palette:
- Black: RGB(0, 0, 0)
- Red: RGB(255, 0, 0)
- Green: RGB(0, 255, 0)
- Yellow: RGB(255, 255, 0)
- Blue: RGB(0, 0, 255)
- Magenta: RGB(255, 0, 255)
- Cyan: RGB(0, 255, 255)
- White: RGB(255, 255, 255)

### Aspect Ratio

Thumbnails are rendered in 4:3 aspect ratio, matching traditional teletext displays.

## Troubleshooting

### Thumbnails Not Appearing

1. **Clear the thumbnail cache**:
   ```
   del /f /s /q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db"
   ```

2. **Restart Windows Explorer**:
   ```
   taskkill /f /im explorer.exe
   start explorer.exe
   ```

3. **Verify registration**:
   - Check if the DLL is registered: Look for the CLSID in Registry Editor under `HKEY_CLASSES_ROOT\CLSID`
   - Verify the .tti extension handler: Check `HKEY_CLASSES_ROOT\.tti\shellex`

4. **Check file associations**:
   - Ensure .tti files are properly recognized by Windows
   - The file extension should be associated with a file type

5. **Enable thumbnails in Windows**:
   - Open File Explorer Options
   - Go to the View tab
   - Uncheck "Always show icons, never thumbnails"

### DLL Registration Fails

- Ensure you're running as Administrator
- Check that the DLL is the correct architecture (32-bit or 64-bit) for your system
- Verify all dependencies are present (should be minimal - just Windows system DLLs)

### Incorrect Rendering

- Verify the TTI file format is correct
- Check if the file contains valid teletext control codes
- The renderer expects standard TTI format with OL/FL commands

## Customization

### Changing the CLSID

If you need to change the CLSID (for example, to avoid conflicts):

1. Generate a new GUID using `guidgen.exe` or an online tool
2. Update `CLSID_TTIThumbnailProvider` in `TTIThumbnailProvider.h`
3. Rebuild the project

### Modifying Colors

To adjust the color palette, edit the `GetColorRef()` function in `TeletextRenderer.cpp`.

### Adjusting Rendering Quality

You can modify font rendering parameters in `RenderToBitmap()`:
- Font face (currently "Courier New")
- Font quality (currently NONANTIALIASED_QUALITY)
- Cell dimensions

## Known Limitations

- Only renders the first page of multi-page TTI files
- **Requires teletext2.ttf and teletext4.ttf fonts for proper display** (see FONT_INFO.md)
- Does not support:
  - Flashing text
  - Concealed text
  - Held graphics mode (partially implemented)
  - DRCS (Dynamically Redefinable Character Sets)
- Without the fonts, graphics and double height will not render correctly (falls back to Courier New)

## License

This is independent code, not derived from wxTED. You may use it according to your own licensing terms.

## Version History

- **1.0** - Initial release
  - Basic TTI parsing
  - Teletext rendering with color support
  - Windows thumbnail integration

## Credits

Created as an independent thumbnail provider for TTI (Teletext) files.

## Further Information

For more information about the TTI file format and teletext in general:
- Teletext specifications are based on ETSI standards
- TTI format is commonly used by teletext editing applications
- The format is a text-based representation of teletext pages

## Support

For issues or questions:
1. Check the Troubleshooting section above
2. Verify your TTI files are in the correct format
3. Enable Windows Event Viewer to check for error messages from the shell extension
